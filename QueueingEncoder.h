#ifndef QUEUEINGENCODER_H
#define	QUEUEINGENCODER_H

#include <vector>

#include <QObject>

#include "JPEGEncoder.h"
#include "TimedPacket.h"
#include "DelayedReceptionCallback.h"
#include "EncodingRingBuffer.h"
#include "MuxFeeder.h"

class QueueingEncoder : public QObject, public DelayedReceptionCallback {
    Q_OBJECT
    
    public:
        QueueingEncoder(int);
        virtual void dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame);
        
    private:
        std::vector<JPEGEncoder*> jpegEncoders;
        
        MuxFeeder muxFeeder;
        
        EncodingRingBuffer rawFrameQueue;
        EncodingRingBuffer encodedFrameQueue;
        EncodingRingBuffer audioQueue;
};


#endif	/* QUEUEINGENCODER_H */

