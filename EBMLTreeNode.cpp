#include <string.h>
#include <stdio.h>
#include <iostream>

#include "EBMLTreeNode.h"

/**
 * Creates a new tree node of given element.
 * @param elementDefinition shared pointer to element definition (do not free after use!)
 */
EBMLTreeNode::EBMLTreeNode(EBMLElement *elementDefinition) {
    this->elementDefinition = elementDefinition;
}

EBMLTreeNode::~EBMLTreeNode() {
    // delete all children
    std::vector<EBMLTreeNode*>::iterator it = childrenNodes.begin();
    while (it != childrenNodes.end()) {
        delete *it;
        it++;
    };
    
    // delete own content
    delete binaryContent;
}

/**
 * Sets element payload by saving given pointer and length. Pointer is saved directly,
 * data is not being copied, so do not change/delete it afterwards. Pointer may
 * be manipulated or freed by this class, so do not rely on it after calling
 * this method.
 * @param binaryContent pointer to binary payload
 * @param contentLength length of data at pointer
 */
void EBMLTreeNode::setBinaryContent(unsigned char *binaryContent, unsigned long long contentLength) {
    this->binaryContent = binaryContent;
    this->contentLength = contentLength;
}

/**
 * Swaps byte order.
 * @param original start of memory region to change
 * @param len length of memory region to change
 */
void EBMLTreeNode::changeEndianness(unsigned char* original, unsigned long long len) {
    unsigned char *out = new unsigned char[len];
    for (unsigned long long i=0; i<len; i++) {
        out[len - i - 1] = original[i];
    }
    
    memcpy(original, out, len);
    
    delete out;
}

/**
 * Sets the given signed integer as payload. Automatically converts from Little
 * Endian to Big Endian.
 * @param x integer to set
 */
void EBMLTreeNode::setIntegerContent(long long x) {
    const unsigned char len = sizeof(x);
    
    // content has to be copied as memory area of x will change after return
    unsigned char *out = new unsigned char[len];
    memcpy(out, &x, len);
    
    // encoding has to be BE but we are LE
    changeEndianness(out, len);
    
    setBinaryContent(out, len);
}

/**
 * Sets the given string as payload. Saves pointer directly, do not
 * modify/delete afterwards! String may be freed by this function later on.
 * @param s string to set
 */
void EBMLTreeNode::setString(char *s) {
    setBinaryContent((unsigned char*) s, strlen(s));
}

/**
 * Adds the given node as child in hierarchy. Node may be manipulated by this
 * class later on. EBML only allows either child nodes or payload content so
 * adding nodes will void binary content upon serialization, avoid setting both!
 * Children will be freed on this element's destruction.
 * @param node node to add as child
 */
void EBMLTreeNode::addChildNode(EBMLTreeNode *node) {
    childrenNodes.push_back(node);
}

/**
 * Combines payload with this element's frame. Run serialize() before to compile
 * children to payload. Returned pointer is yours, so free it after use!
 * @return pointer to binary
 */
unsigned char* EBMLTreeNode::getOuterContent() {
    unsigned char* outerContent = new unsigned char[getOuterSize()];
    
    // outer content consists of 3 parts:
    
    // 1. element sequence
    memcpy(outerContent, elementDefinition->getSequence(), elementDefinition->getSequenceLength());
    
    // 2. inner content size
    unsigned char *innerContentSize = encodeDataSize(contentLength);
    memcpy(outerContent + elementDefinition->getSequenceLength(), innerContentSize, EBML_DATA_SIZE);
    delete innerContentSize;
    
    // 3. inner content
    memcpy(outerContent + elementDefinition->getSequenceLength() + EBML_DATA_SIZE, binaryContent, contentLength);
    
    return outerContent;
}

/**
 * Calculates this element's frame + payload size.
 * @return total (frame + payload) size
 */
unsigned long long EBMLTreeNode::getOuterSize() {
    return elementDefinition->getSequenceLength() + EBML_DATA_SIZE + contentLength;
}

/**
 * Encodes given size for use as an EBML data size attribute.
 * Currently fixed to encode as 56 bit integer (automatically converts endianness).
 * @param size value to encode (size of some other content)
 * @return pointer to EBML encoded data size attribute; pointer is yours, so free after use
 */
unsigned char* EBMLTreeNode::encodeDataSize(unsigned long long size) {
    // size length can be variable according to spec but we simply always use
    // 64 bits (56 bits payload) to keep one uniform encoding
    
    // it's unlikely that we encounter more than 72PB but warn anyway
    if (size & 0xFF00000000000000) {
        std::cerr << "ERROR: tried to encode higher data size than 72 PB which is unsupported by Matroska, file will be corrupt\n";
    }
    
    // keep only lower 7 bytes
    //              1122334455667788
    size = size & 0x00FFFFFFFFFFFFFF;
    
    // copy LE value
    unsigned char* out = new unsigned char[EBML_DATA_SIZE];
    memcpy(out, &size, EBML_DATA_SIZE);
    
    // change LE to BE
    changeEndianness(out, EBML_DATA_SIZE);
    
    // first byte encodes length of data size and should be 0x01 for 56 bit
    // payload according to specs
    out[0] = 0x01;
    
    return out;
}

/**
 * Recursively compiles all children of EBML hierarchy to current element's
 * payload. Call will void any binaryContent without warning if child nodes exist!
 */
void EBMLTreeNode::serialize() {
    // no need to serialize of current node does not have child nodes
    if (childrenNodes.size() == 0) {
        return;
    }
    
    // recursively serialize all child nodes and calculate total data size
    long long totalInnerSize = 0;
    std::vector<EBMLTreeNode*>::iterator it;
    it = childrenNodes.begin();
    while (it != childrenNodes.end()) {
        (*it)->serialize();
        totalInnerSize += (*it)->getOuterSize();
        
        it++;
    }
    
    // combine all child nodes into binary payload data
    if (binaryContent != 0) {
        delete binaryContent;
    }
    long long currentPos = 0;
    binaryContent = new unsigned char[totalInnerSize];
    it = childrenNodes.begin();
    while (it != childrenNodes.end()) {
        long long childSize = (*it)->getOuterSize();
        unsigned char *childContent = (*it)->getOuterContent();
        
        memcpy(binaryContent + currentPos, childContent, childSize);
        
        delete childContent;
        
        currentPos += childSize;
        it++;
    }
    
    // update binary size
    contentLength = totalInnerSize;
}