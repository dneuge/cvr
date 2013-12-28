#include "EncodingRingBuffer.h"

#include <stdio.h>

EncodingRingBuffer::EncodingRingBuffer() {
    maxMillisInBuffer = 0;
    maxPacketsInBuffer = 0;
}

void EncodingRingBuffer::configure(unsigned long maxMillisInBuffer, unsigned long maxPacketsInBuffer) {
    mutex.lock();
    
    this->maxMillisInBuffer = maxMillisInBuffer;
    this->maxPacketsInBuffer = maxPacketsInBuffer;
    
    mutex.unlock();
}

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
