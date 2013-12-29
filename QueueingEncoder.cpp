#include "QueueingEncoder.h"

#include <iostream>
#include <math.h>

QueueingEncoder::QueueingEncoder(int numEncoderThreads) {
    unsigned long timeshiftMillis = 30000;
    unsigned long maxRawFrames = 30;
    nextExpectedAudioIndex = 0;
    nextExpectedVideoIndex = 0;
    
    audioQueue.configure(timeshiftMillis, 0);
    rawFrameQueue.configure(0, maxRawFrames);
    encodedFrameQueue.configure(timeshiftMillis, 0);
    
    for (int i=0; i<numEncoderThreads; i++) {
        jpegEncoders.push_back(new JPEGEncoder(&rawFrameQueue, &encodedFrameQueue));
    }
    
    muxFeeder = new MuxFeeder(&audioQueue, &encodedFrameQueue);
}

void QueueingEncoder::signalEndOfRecording() {
    // FIXME: maybe insert conditionally? (recording has to be running/no other termination packets have to be in queues already)
    // QUESTION: use expected indices to override received indices and thus avoid potential index overflow issues?
    
    printf("signalling end of recording\n");
    
    mutex.lock();
    
    timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    unsigned long long currentTimeMillis = currentTime.tv_sec * 1000 + lround((double) (currentTime.tv_nsec / 100000) / 10.0);
    
    TimedPacket *audioPacket = new TimedPacket(nextExpectedAudioIndex, currentTimeMillis, 0, 0);
    TimedPacket *videoFrame = new TimedPacket(nextExpectedVideoIndex, currentTimeMillis, 0, 0);
    
    // add directly to output queues (skip encoders)
    audioQueue.addPacket(audioPacket);
    encodedFrameQueue.addPacket(videoFrame);
    
    mutex.unlock();
}

void QueueingEncoder::dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame) {
    if (audioPacket != 0) {
        mutex.lock();
        nextExpectedAudioIndex = audioPacket->index + 1;
        mutex.unlock();
        
        if (!audioQueue.addPacket(audioPacket)) {
            std::cerr << "failed to add audio packet to ring buffer\n";
            
            // free memory or we will leak
            delete audioPacket->data;
            delete audioPacket;
        }
    }
    
    if (videoFrame != 0) {
        mutex.lock();
        nextExpectedVideoIndex = videoFrame->index + 1;
        mutex.unlock();
        
        if (!rawFrameQueue.addPacket(videoFrame)) {
            std::cerr << "failed to add video frame to raw image ring buffer\n";
            
            // free memory or we will leak
            delete videoFrame->data;
            delete videoFrame;
        }
    }
}
