#ifndef EBMLTREENODE_H
#define	EBMLTREENODE_H

#include <vector>

#include "EBMLElement.h"

class EBMLTreeNode {
    public:
        EBMLTreeNode(EBMLElement*);
        ~EBMLTreeNode();
        void setBinaryContent(unsigned char*, unsigned long long);
        void setIntegerContent(long long);
        void setString(char*);
        void addChildNode(EBMLTreeNode*);
        void serialize();
        unsigned char* getOuterContent();
        unsigned long long getOuterSize();
    
    private:
        EBMLElement *elementDefinition;
        std::vector<EBMLTreeNode*> childrenNodes;
        
        unsigned int contentLength;
        unsigned char *binaryContent;
        
        static const unsigned char EBML_DATA_SIZE = 8; // 8 bytes encode one size (0.5 header + 7.5 payload)
        
        void changeEndianness(unsigned char*, unsigned long long);
        unsigned char* encodeDataSize(unsigned long long size);
};

#endif	/* EBMLTREENODE_H */

