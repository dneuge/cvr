#ifndef EBMLTREENODE_H
#define	EBMLTREENODE_H

#include <vector>

#include "EBMLElement.h"

#define EBML_FRAMED_ID_LENGTH 4

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
        unsigned char* getBinaryContent();
        unsigned long long getBinarySize();
        unsigned char* getOuterContent();
        unsigned long long getOuterSize();
        static unsigned char* createRandomID();
        
    private:
        EBMLElement *elementDefinition;
        std::vector<EBMLTreeNode*> childrenNodes;
        
        unsigned int contentLength;
        unsigned char *binaryContent;
        
        static const unsigned char EBML_DATA_SIZE = 8; // 8 bytes encode one size (1 header + 7 payload)
        
        void changeEndianness(unsigned char*, unsigned long long);
        unsigned char* encodeDataSize(unsigned long long size);
};

#endif	/* EBMLTREENODE_H */

