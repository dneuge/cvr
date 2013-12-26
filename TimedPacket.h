#ifndef TIMEDPACKET_H
#define	TIMEDPACKET_H

class TimedPacket {
    public:
        unsigned long timestamp;
        char* data;
        unsigned long long dataLength;
};

#endif	/* TIMEDPACKET_H */

