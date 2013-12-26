#ifndef EBMLTREENODE_H
#define	EBMLTREENODE_H

#include <vector>

#include "EBMLElement.h"

#define EBML_FRAMED_ID_LENGTH 4

// we use a fixed length of 8 bytes to encode one size (1 header + 7 payload)
#define EBML_DATA_SIZE 8

class EBMLTreeNode {
    public:
        EBMLTreeNode();
        EBMLTreeNode(EBMLElement*);
        ~EBMLTreeNode();
        void setBinaryContent(unsigned char*, unsigned long long);
        void copyBinaryContent(unsigned char*, unsigned long long);
        void copyWithChangedEndianness(unsigned char*, long long);
        void setIntegerContent(long long);
        void setFloatContent(double);
        void copyString(const char*);
        void addChildNode(EBMLTreeNode*);
        void serialize();
        void updateOffsets(unsigned long long);
        unsigned char* getBinaryContent();
        unsigned long long getBinarySize();
        unsigned char* getOuterContent();
        unsigned long long getOuterSize();
        unsigned long long getAbsoluteOffset();
        unsigned long long getAbsolutePayloadOffset();
        unsigned long long getAbsoluteDataSizeOffset();
        unsigned long long getRelativeOffset(EBMLTreeNode*);
        static unsigned char* createRandomID();
        static unsigned char* encodeDataSize(unsigned long long size);
        static void changeEndianness(unsigned char*, unsigned long long);
        
    private:
        EBMLElement *elementDefinition;
        std::vector<EBMLTreeNode*> childrenNodes;
        
        unsigned long long contentLength;
        unsigned char *binaryContent;
        unsigned long long absoluteOffset;
};

#endif	/* EBMLTREENODE_H */

