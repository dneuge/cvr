#ifndef TIMEDPACKET_H
#define	TIMEDPACKET_H

class TimedPacket {
    public:
        TimedPacket(unsigned long long, unsigned long long, char*, unsigned long long);
        
        unsigned long long index;
        unsigned long long timestampMillis;
        char* data;
        unsigned long long dataLength;
};

#endif	/* TIMEDPACKET_H */

