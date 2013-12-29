#include "MuxFeeder.h"

#include <QWaitCondition>

MuxFeeder::MuxFeeder(EncodingRingBuffer *audioQueue, EncodingRingBuffer *videoQueue) {
    this->audioQueue = audioQueue;
    this->videoQueue = videoQueue;
    containerEncoder = 0;
    currentAudioPacket = 0;
    currentVideoFrame = 0;
    expectedAudioIndex = 0;
    expectedVideoIndex = 0;
    audioStreamTerminated = false;
    videoStreamTerminated = false;
    
    // move to own thread
    moveToThread(&thread);
    thread.start();
    
    // tell thread to start working
    connect(this, SIGNAL(startEncoding()), this, SLOT(feedEncoder()));
    emit startEncoding();
}

bool MuxFeeder::setContainerEncoder(MatroskaEncoder *newContainerEncoder) {
    bool success = false;
    
    printf("setting container encoder\n"); // DEBUG
    
    mutex.lock();
    
    // only set container encoder if none is already in use
    if (containerEncoder == 0) {
        containerEncoder = newContainerEncoder;
        success = true;
    }
    
    mutex.unlock();
    
    printf("new container encoder has been registered\n"); // DEBUG
    
    return success;
}


void MuxFeeder::msleep(unsigned long millis) {
    QMutex sleepMutex;
    sleepMutex.lock();
    
    QWaitCondition wc;
    wc.wait(&sleepMutex, millis);
    
    sleepMutex.unlock();
}

void MuxFeeder::feedEncoder() {
    printf("mux feeder started\n"); // DEBUG
    
    while (true) {
        mutex.lock();
        
        // only continue if we got an encoder, else sleep and try again
        if (containerEncoder == 0) {
            printf("MuxFeeder: no container encoder set\n"); // DEBUG
            
            mutex.unlock();
            msleep(INACTIVITY_SLEEP_MILLIS);
            continue;
        }
        
        printf("MuxFeeder iteration\n"); // DEBUG
        
        // get audio packet if we got none
        // NOTE: we can pop it unconditionally as audio packets will always be in order
        if (currentAudioPacket == 0) {
            currentAudioPacket = audioQueue->popFirst();
        }
        
        // get video frame if we got none
        if (currentVideoFrame == 0) {
            // if a zero length frame resides in queue, we are told to end the
            // recording, so we can simply flush the queue
            // FIXME: this will likely be dropping content off the end of a recording, fix or ignore it? (maybe just add a delay between end of capture and zero frame insertion)
            if (videoQueue->containsZeroLengthPacket()) {
                printf("MuxFeeder: termination upcoming in video stream, popping unconditionally\n");
                currentVideoFrame = videoQueue->popFirst();
            } else {
                currentVideoFrame = videoQueue->popFirstIfExpectedIndexOrMinSize(expectedVideoIndex, MIN_VIDEO_WAIT_SIZE);
            }
        }
        
        // discard outdated packets
        // FIXME: consider overflow of 64 bit index numbers? (how long would a recording have to be to make this an issue?)
        if ((currentAudioPacket != 0) && (currentAudioPacket->index < expectedAudioIndex)) {
            printf("MuxFeeder dropping audio packet with outdated index %lld\n", currentAudioPacket->index); // DEBUG
            
            delete currentAudioPacket->data;
            delete currentAudioPacket;
            
            currentAudioPacket = 0;
        }
        if ((currentVideoFrame != 0) && (currentVideoFrame->index < expectedVideoIndex)) {
            printf("MuxFeeder dropping video frame with outdated index %lld\n", currentVideoFrame->index); // DEBUG
            
            delete currentVideoFrame->data;
            delete currentVideoFrame;
            
            currentVideoFrame = 0;
        }
        
        // wait if either stream data is missing (not possible to do the comparisons below)
        if ((currentAudioPacket == 0) || (currentVideoFrame == 0)) {
            printf("MuxFeeder is missing packet type to continue\n"); // DEBUG
            
            mutex.unlock();
            msleep(INACTIVITY_SLEEP_MILLIS);
            continue;
        }
        
        // check if that packets are terminating streams (zero data length)
        audioStreamTerminated |= (currentAudioPacket->dataLength == 0);
        videoStreamTerminated |= (currentVideoFrame->dataLength == 0);
        
        // check if both streams were terminated
        if (audioStreamTerminated && videoStreamTerminated) {
            printf("MuxFeeder: both streams are terminated\n"); // DEBUG
            
            // flush final packets to container encoder which will shutdown upon reception
            containerEncoder->addAudioPacket(currentAudioPacket);
            containerEncoder->addVideoFrame(currentVideoFrame);
            currentAudioPacket = 0;
            currentVideoFrame = 0;
            
            // detach encoder
            containerEncoder = 0;
            
            // reset state for next recording
            audioStreamTerminated = false;
            videoStreamTerminated = false;
        } else if (audioStreamTerminated) {
            printf("MuxFeeder: audio stream has been terminated, only forwarding video frames\n"); // DEBUG
            
            // audio stream already terminated, only forward video frames to keep terminating packet waiting
            expectedVideoIndex = currentVideoFrame->index + 1;
            containerEncoder->addVideoFrame(currentVideoFrame);
            currentVideoFrame = 0;
        } else if (videoStreamTerminated) {
            printf("MuxFeeder: video stream has been terminated, only forwarding audio packets\n"); // DEBUG
            
            // video stream already terminated, only forward audio packets to keep terminating packet waiting
            expectedAudioIndex = currentAudioPacket->index + 1;
            containerEncoder->addAudioPacket(currentAudioPacket);
            currentAudioPacket = 0;
        } else {
            printf("MuxFeeder: forwarding packet\n"); // DEBUG
            
            // check if audio precedes video frame
            bool audioPrecedesVideo = (currentAudioPacket->timestampMillis < currentVideoFrame->timestampMillis);

            // forward audio or video frame first?
            if (audioPrecedesVideo) {
                // forward audio packet to container encoder
                expectedAudioIndex = currentAudioPacket->index + 1;
                containerEncoder->addAudioPacket(currentAudioPacket);
                currentAudioPacket = 0;
            } else {
                // forward video frame to container encoder
                expectedVideoIndex = currentVideoFrame->index + 1;
                containerEncoder->addVideoFrame(currentVideoFrame);
                currentVideoFrame = 0;
            }
        }
        
        mutex.unlock();
    }
}


