#ifndef ENCODINGRINGBUFFER_H
#define	ENCODINGRINGBUFFER_H

#include <vector>

#include <QObject>
#include <QMutex>

#include "TimedPacket.h"
#include "QueueStats.h"

class EncodingRingBuffer : public QObject {
    Q_OBJECT
    
    public:
        EncodingRingBuffer();
        void configure(unsigned long, unsigned long);
        bool addPacket(TimedPacket*);
        TimedPacket* popFirst();
        TimedPacket* popFirstIfExpectedIndexOrMinSize(unsigned long long, unsigned long);
        bool containsZeroLengthPacket();
        void removeZeroLengthPackets();
        void clear();
        QueueStats* getStats();
    
    private:
        std::vector<TimedPacket*> packets;
        unsigned long maxPacketsInBuffer;
        unsigned long maxMillisInBuffer;
        
        QMutex mutex;
};

#endif	/* ENCODINGRINGBUFFER_H */

