
#include "DummyReceptionCallback.h"

#include <stdio.h>

DummyReceptionCallback::DummyReceptionCallback() {
    millisLastAudioPacket = 0;
    millisLastVideoFrame = 0;
}

void DummyReceptionCallback::dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame) {
    if (audioPacket != 0) {
        unsigned long long millisSinceLastAudioPacket = audioPacket->timestampMillis - millisLastAudioPacket;
        double audioPacketsPerSecond = (double) 1000.0 / millisSinceLastAudioPacket;
        
        printf("received audio: index %lld @ %lld (%lld ms since last call, %.2f packets per second), size %lld\n", audioPacket->index, audioPacket->timestampMillis, millisSinceLastAudioPacket, audioPacketsPerSecond, audioPacket->dataLength);
        
        millisLastAudioPacket = audioPacket->timestampMillis;
        
        delete audioPacket->data;
        delete audioPacket;
    }
    
    if (videoFrame != 0) {
        unsigned long long millisSinceLastVideoFrame = videoFrame->timestampMillis - millisLastVideoFrame;
        double framesPerSecond = (double) 1000.0 / millisSinceLastVideoFrame;
        
        printf("received video: index %lld @ %lld (%lld ms since last call, %.2f frames per second), size %lld\n", videoFrame->index, videoFrame->timestampMillis, millisSinceLastVideoFrame, framesPerSecond, videoFrame->dataLength);
        
        millisLastVideoFrame = videoFrame->timestampMillis;
        
        delete videoFrame->data;
        delete videoFrame;
    }
}
