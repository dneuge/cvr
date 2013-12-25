#ifndef MATROSKAENCODER_H
#define	MATROSKAENCODER_H

#include <stdio.h>

#include "EBMLTreeNode.h"
#include "MatroskaEBMLElementDefinitions.h"

#define TRACK_NUMBER_VIDEO 1
#define TRACK_NUMBER_AUDIO 2

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
        std::vector<unsigned char*> usedIDs; // keeps random EBML IDs so we can avoid collisions
        
        unsigned char* generateUniqueRandomID();
        bool checkIDAlreadyUsed(unsigned char*);
};

#endif	/* MATROSKAENCODER_H */

