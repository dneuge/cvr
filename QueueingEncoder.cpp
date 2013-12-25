#include "QueueingEncoder.h"

/**
 * Returns a consistent timestamp to use for tagging simultaneous audio packets
 * and video frames.
 * @return timestamp
 */
long QueueingEncoder::getTimestamp() {
    
}

/**
 * Adds one audio packet identified by timestamp.
 * @param timestamp timestamp to tag audio packet
 * @param data array of bytes to store
 */
void QueueingEncoder::addAudioPacket(long timestamp, char* data) {

}

/**
 * Adds one video frame identified by timestamp.
 * @param timestamp timestamp to tag video frame
 * @param data array of raw video frame bytes to store
 */
void QueueingEncoder::addVideoFrame(long timestamp, char* data) {

}

/**
 * Starts encoder threads.
 * @param numJPEGEncoders number of JPEG encoders to run simultaneously
 */
void QueueingEncoder::startThreads(int numJPEGEncoders) {

}
