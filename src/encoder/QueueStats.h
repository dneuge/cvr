#ifndef QUEUESTATS_H
#define	QUEUESTATS_H

class QueueStats {
    public:
        unsigned long latestPacketTimestamp;
        unsigned long oldestPacketTimestamp;
        unsigned long maxPackets;
        unsigned long numPackets;
        unsigned long maxMillis;
        unsigned long totalBytes;
};

#endif	/* QUEUESTATS_H */

