#include "QueueingEncoder.h"

#include <iostream>
#include <math.h>

QueueingEncoder::QueueingEncoder(int numEncoderThreads) {
    unsigned long timeshiftMillis = 30000;
    unsigned long maxRawFrames = 30;
    nextAudioIndex = 0;
    nextVideoIndex = 0;
    frameDivisionCounter = 0;
    frameDivisionModulo = 1;
    
    audioQueue.configure(timeshiftMillis, 0);
    rawFrameQueue.configure(0, maxRawFrames);
    encodedFrameQueue.configure(timeshiftMillis, 0);
    
    for (int i=0; i<numEncoderThreads; i++) {
        jpegEncoders.push_back(new JPEGEncoder(&rawFrameQueue, &encodedFrameQueue));
    }
    
    muxFeeder = new MuxFeeder(&audioQueue, &encodedFrameQueue);
}

/**
 * Frame rate division can be used to reduce memory used (both in RAM and on
 * disk for a recording) and to increase encoding performance on slower computers
 * by encoding only every nth frame.
 * @param modulo use first of every n frames (minimum 1)
 */
void QueueingEncoder::setFrameDivisionModulo(unsigned char modulo) {
    if (modulo < 1) {
        std::cerr << "invalid modulo value for frame division, modulo has to be at least 1\n";
        return;
    }
    
    printf("frame division: first of %d frames will be recorded\n", modulo); // DEBUG
    
    mutex.lock();
    frameDivisionModulo = modulo;
    mutex.unlock();
}

void QueueingEncoder::signalEndOfRecording() {
    printf("signaling end of recording requested\n"); // DEBUG
    
    if (!muxFeeder->isContainerEncoderRegistered()) {
        printf("no recording running, not signaling EOR (no container encoder registered)\n"); // DEBUG
        return;
    }
    
    timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    unsigned long long currentTimeMillis = currentTime.tv_sec * 1000 + lround((double) (currentTime.tv_nsec / 100000) / 10.0);
    
    TimedPacket *audioPacket = 0;
    TimedPacket *videoFrame = 0;
    
    mutex.lock();
    if (!audioQueue.containsZeroLengthPacket()) {
        audioPacket = new TimedPacket(nextAudioIndex++, currentTimeMillis, 0, 0);
    }
    if (!encodedFrameQueue.containsZeroLengthPacket()) {
        videoFrame = new TimedPacket(nextVideoIndex++, currentTimeMillis, 0, 0);
    }
    mutex.unlock();
    
    // add directly to output queues (skip encoders)
    if (audioPacket != 0) {
        audioQueue.addPacket(audioPacket);
    } else {
        printf("audio queue already contained EOR termination packet, no signal added\n"); // DEBUG
    }
    
    if (videoFrame != 0) {
        encodedFrameQueue.addPacket(videoFrame);
    } else {
        printf("video queue already contained EOR termination packet, no signal added\n"); // DEBUG
    }
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
    
    if (videoFrame != 0) {
        mutex.lock();
        
        // check for frame division
        bool skipFrame = (frameDivisionCounter % frameDivisionModulo != 0);
        
        // increment counter restricted to range defined by modulo
        frameDivisionCounter = ++frameDivisionCounter % frameDivisionModulo; // TODO: somehow silence compiler warning which is invalid (initialized by only constructor)
        
        // increment index only if not skipped
        if (!skipFrame) {
            videoFrame->index = nextVideoIndex++;
        }
        
        mutex.unlock();
        
        // add to frame queue only if not skipped
        if (!skipFrame) {
            if (!rawFrameQueue.addPacket(videoFrame)) {
                std::cerr << "failed to add video frame to raw image ring buffer\n";
                skipFrame = true;
            }
        }
        
        // free memory if frame was skipped
        if (skipFrame) {
            delete videoFrame->data;
            delete videoFrame;
        }
    }
}
