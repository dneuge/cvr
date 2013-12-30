#include "QueueingEncoder.h"

#include <iostream>
#include <math.h>

QueueingEncoder::QueueingEncoder(int numEncoderThreads) {
    unsigned long timeshiftMillis = 30000;
    unsigned long maxRawFrames = 30;
    nextAudioIndex = 0;
    nextVideoIndex = 0;
    
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
    
    printf("signalling end of recording\n");
    
    timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    unsigned long long currentTimeMillis = currentTime.tv_sec * 1000 + lround((double) (currentTime.tv_nsec / 100000) / 10.0);
    
    mutex.lock();
    TimedPacket *audioPacket = new TimedPacket(nextAudioIndex++, currentTimeMillis, 0, 0);
    TimedPacket *videoFrame = new TimedPacket(nextVideoIndex++, currentTimeMillis, 0, 0);
    mutex.unlock();
    
    // add directly to output queues (skip encoders)
    audioQueue.addPacket(audioPacket);
    encodedFrameQueue.addPacket(videoFrame);
}

void QueueingEncoder::dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame) {
    // packets/frames are reindexed at this point for a few reasons:
    //  - skipped frames (frame division) would look like dropped frames to muxer if we wouldn't reindex
    //  - being able to add EOR signalling packets with a unique index number
    
    if (audioPacket != 0) {
        mutex.lock();
        audioPacket->index = nextAudioIndex++;
        mutex.unlock();
        
        if (!audioQueue.addPacket(audioPacket)) {
            std::cerr << "failed to add audio packet to ring buffer\n";
            
            // free memory or we will leak
            delete audioPacket->data;
            delete audioPacket;
        }
    }
    
    // TODO: frame division
    if (videoFrame != 0) {
        mutex.lock();
        videoFrame->index = nextVideoIndex++;
        mutex.unlock();
        
        if (!rawFrameQueue.addPacket(videoFrame)) {
            std::cerr << "failed to add video frame to raw image ring buffer\n";
            
            // free memory or we will leak
            delete videoFrame->data;
            delete videoFrame;
        }
    }
}
