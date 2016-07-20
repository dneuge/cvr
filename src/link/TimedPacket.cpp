#include "TimedPacket.h"

/**
 * Convenience constructor simply assigning all supplied values to attributes.
 * @param index current packet index
 * @param timestampMillis (abstract) timestamp in milliseconds to measure time difference between two packets
 * @param data pointer to payload data
 * @param dataLength length of payload data
 */
TimedPacket::TimedPacket(unsigned long long index, unsigned long long timestampMillis, char* data, unsigned long long dataLength) {
    this->index = index;
    this->timestampMillis = timestampMillis;
    this->data = data;
    this->dataLength = dataLength;
}
