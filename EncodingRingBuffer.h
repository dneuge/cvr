#ifndef ENCODINGRINGBUFFER_H
#define	ENCODINGRINGBUFFER_H

#include <vector>

#include <QObject>
#include <QMutex>

#include "TimedPacket.h"

class EncodingRingBuffer : public QObject {
    Q_OBJECT
    
    public:
        EncodingRingBuffer();
        void configure(unsigned long, unsigned long);
        bool addPacket(TimedPacket*);
        TimedPacket* popFirst();
    
    private:
        std::vector<TimedPacket*> packets;
        unsigned long maxPacketsInBuffer;
        unsigned long maxMillisInBuffer;
        
        QMutex mutex;
};

#endif	/* ENCODINGRINGBUFFER_H */
