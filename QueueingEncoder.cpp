#include "QueueingEncoder.h"

#include <iostream>

QueueingEncoder::QueueingEncoder(int numEncoderThreads) {
    unsigned long timeshiftMillis = 30000;
    unsigned long maxRawFrames = 30;
    
    audioQueue.configure(timeshiftMillis, 0);
    rawFrameQueue.configure(0, maxRawFrames);
    encodedFrameQueue.configure(timeshiftMillis, 0);
    
    for (int i=0; i<numEncoderThreads; i++) {
        jpegEncoders.push_back(new JPEGEncoder(&rawFrameQueue, &encodedFrameQueue));
    }
}

void QueueingEncoder::dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame) {
    if (audioPacket != 0) {
        if (!audioQueue.addPacket(audioPacket)) {
            std::cerr << "failed to add audio packet to ring buffer\n";
            
            // free memory or we will leak
            delete audioPacket->data;
            delete audioPacket;
        }
    }
    
    if (videoFrame != 0) {
        if (!rawFrameQueue.addPacket(videoFrame)) {
            std::cerr << "failed to add video frame to raw image ring buffer\n";
            
            // free memory or we will leak
            delete videoFrame->data;
            delete videoFrame;
        }
    }
}
