#include "EncodingRingBuffer.h"

#include <stdio.h>
#include <bits/stl_vector.h>

EncodingRingBuffer::EncodingRingBuffer() {
    maxMillisInBuffer = 0;
    maxPacketsInBuffer = 0;
}

/**
 * Configure maximum ring buffer size.
 * @param maxMillisInBuffer maximum amount of time to span in milliseconds (0 to disable criterion)
 * @param maxPacketsInBuffer maximum number of packets to keep (0 to disable criterion)
 */
void EncodingRingBuffer::configure(unsigned long maxMillisInBuffer, unsigned long maxPacketsInBuffer) {
    mutex.lock();
    
    this->maxMillisInBuffer = maxMillisInBuffer;
    this->maxPacketsInBuffer = maxPacketsInBuffer;
    
    mutex.unlock();
}

/**
 * Pops the oldest packet from queue unconditionally.
 * @return pointer to first element, 0 if queue is empty
 */
TimedPacket* EncodingRingBuffer::popFirst() {
    mutex.lock();
    
    TimedPacket* oldestPacket = 0;
    if (!packets.empty()) {
        oldestPacket = packets.front();
        packets.erase(packets.begin());
    }
    
    mutex.unlock();
    
    return oldestPacket;
}

/**
 * Pops the oldest packet from queue conditionally if it either has the expected
 * index number or at minimum minSizeToWaitFor other packets wait in queue.
 * @param maxExpectedIndex oldest packet in queue is expected to have at most the given index number
 * @param minSizeToWaitFor minimum number of other packets waiting in queue to be able to skip index condition
 * @return pointer to first element if conditions applied, else 0 (also on empty queue)
 */
TimedPacket* EncodingRingBuffer::popFirstIfExpectedIndexOrMinSize(unsigned long long maxExpectedIndex, unsigned long minSizeToWaitFor) {
    TimedPacket* oldestPacket = 0;
    
    mutex.lock();
    
    if (!packets.empty()) {
        oldestPacket = packets.front();
        
        if ((oldestPacket->index > maxExpectedIndex) && (packets.size() <= minSizeToWaitFor)) {
            // unexpected packet index and not enough packets in queue to skip
            // don't dequeue packet yet
            oldestPacket = 0;
        } else {
            // expected index or allowed to skip, dequeue packet
            packets.erase(packets.begin());
        }
    }
    
    mutex.unlock();
    
    return oldestPacket;
}

/**
 * Checks if the ring buffer currently contains a packet with zero dataLength
 * at any queue position. Meaning of such packets is to stop a recording.
 * @return packet with dataLength == 0 in queue?
 */
bool EncodingRingBuffer::containsZeroLengthPacket() {
    bool containsZeroLengthPacket = false;
    
    mutex.lock();
    
    std::vector<TimedPacket*>::iterator it = packets.begin();
    while (it != packets.end()) {
        if ((*it)->dataLength == 0) {
            containsZeroLengthPacket = true;
            break;
        }
        
        it++;
    }
    
    mutex.unlock();
    
    return containsZeroLengthPacket;
}

/**
 * Adds the given packet in ring buffer, ordered by index number.
 * @param packet packet to queue
 * @return success? (should only fail on null packet pointer)
 */
bool EncodingRingBuffer::addPacket(TimedPacket *packet) {
    // don't process null packets
    if (packet == 0) {
        return false;
    }
    
    mutex.lock();
    
    // check if limits apply to drop the oldest packet
    bool removeOldestPacket = false;
    if (!packets.empty()) {
        TimedPacket *oldestPacket = packets.front();
        
        // check for maximum amount of time to keep in buffer
        removeOldestPacket |= (maxMillisInBuffer > 0) && (packet->timestampMillis - oldestPacket->timestampMillis > maxMillisInBuffer);
        
        // check for maximum number of packets to keep in buffer
        removeOldestPacket |= (maxPacketsInBuffer > 0) && (packets.size() >= maxPacketsInBuffer);
    }
    
    // drop oldest packet if necessary
    if (removeOldestPacket) {
        TimedPacket *oldestPacket = packets.front();
        packets.erase(packets.begin());
        
        //printf("dropping packet index %lld\n", oldestPacket->index); // DEBUG
        
        delete oldestPacket->data;
        delete oldestPacket;
    }
    
    // find new packet position in buffer by index
    // we search in reverse since will mean less iterations when time-shifting
    std::vector<TimedPacket*>::reverse_iterator it = packets.rbegin();
    while (it != packets.rend()) {
        if ((*it)->index < packet->index) {
            break;
        }

        it++;
    };
    packets.insert(it.base(), packet);
    
    /*
    // DEBUG
    {
        unsigned long long size = 0;
        std::vector<TimedPacket*>::iterator it = packets.begin();
        while (it != packets.end()) {
            size += (*it)->dataLength;
            it++;
        }
        printf("%lld bytes in buffer, %d packets, avg size %lld\n", size, packets.size(), (size / packets.size()));
    }
    */
    
    mutex.unlock();
    
    return true;
}
