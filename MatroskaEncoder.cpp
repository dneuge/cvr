#include "MatroskaEncoder.h"

MatroskaEncoder::MatroskaEncoder(const char *fileName) {
    // open file for output
    fh = fopen(fileName, "wb+");
    success = (ferror(fh) == 0);
    
    // get Matroska EBML definitions
    elementDefinitions = new MatroskaEBMLElementDefinitions();
    
    // start building EBML tree
    EBMLTreeNode *node;
    rootNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBML"));
    //rootNode->setIntegerContent(255);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLVersion"));
    node->setIntegerContent(1);
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLReadVersion"));
    node->setIntegerContent(1);
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLMaxIDLength"));
    node->setIntegerContent(4);
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLMaxSizeLength"));
    node->setIntegerContent(8);
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocType"));
    node->setString("matroska");
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocTypeVersion"));
    node->setIntegerContent(1);
    rootNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocTypeReadVersion"));
    node->setIntegerContent(1);
    rootNode->addChildNode(node);
}

void MatroskaEncoder::writeFileHeader() {
    rootNode->serialize();
    fwrite(rootNode->getOuterContent(), rootNode->getOuterSize(), 1, fh);
}

void MatroskaEncoder::closeFile() {
    fflush(fh);
    fclose(fh);
}