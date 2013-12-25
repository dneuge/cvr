#ifndef MATROSKAENCODER_H
#define	MATROSKAENCODER_H

#include <stdio.h>

#include "EBMLTreeNode.h"
#include "MatroskaEBMLElementDefinitions.h"

class MatroskaEncoder {
    public:
        MatroskaEncoder(const char*);
        bool isSuccess();
        void writeFileHeader();
        //addVideoFrame();
        //addAudioPacket();
        void closeFile();
    
    private:
        FILE *fh;
        bool success;
        EBMLTreeNode *rootNode;
        MatroskaEBMLElementDefinitions *elementDefinitions;
};

#endif	/* MATROSKAENCODER_H */

