#ifndef MATROSKAENCODER_H
#define	MATROSKAENCODER_H

#include <stdio.h>

#include "link/TimedPacket.h"

#include "EBMLTreeNode.h"
#include "MatroskaEBMLElementDefinitions.h"
#include "MatroskaCuePoint.h"

#define TRACK_NUMBER_VIDEO 1
#define TRACK_NUMBER_AUDIO 2

class MatroskaEncoder {
    public:
        MatroskaEncoder(const char*);
        ~MatroskaEncoder();
        bool isSuccess();
        void writeFileHeader();
        void addVideoFrame(TimedPacket*);
        void addAudioPacket(TimedPacket*);
        void consumeVideoFrame(TimedPacket*);
        void consumeAudioPacket(TimedPacket*);
        void closeFile();
    
    private:
        FILE *fh;
        bool success;
        EBMLTreeNode *rootNode;
        MatroskaEBMLElementDefinitions *elementDefinitions;
        std::vector<unsigned char*> usedIDs; // keeps random EBML IDs so we can avoid collisions
        std::vector<MatroskaCuePoint*> cuePoints;
        
        // used for finishing the file
        unsigned long long fileOffsetCueReference;
        unsigned long long fileOffsetSegmentSize;
        unsigned long long fileOffsetSegmentPayload;
        
        // used during recording
        unsigned long long fileOffsetCluster;
        unsigned long long fileOffsetClusterSize;
        unsigned long long fileOffsetClusterPayload;
        unsigned long long initialTimestamp;
        unsigned long long relativeTimestampRecording;
        unsigned long long clusterTimestampRelativeToRecording;
        
        // signalling termination
        bool audioStreamTerminated;
        bool videoStreamTerminated;
        
        void freePacket(TimedPacket*);
        unsigned char* generateUniqueRandomID();
        bool checkIDAlreadyUsed(unsigned char*);
        void finalizeCluster(unsigned long long);
        void checkAndHandleEndOfRecording();
        void startNewCluster();
        void writeSimpleBlock(TimedPacket*, unsigned int, unsigned char);
};

#endif	/* MATROSKAENCODER_H */

