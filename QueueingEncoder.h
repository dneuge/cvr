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
        MuxFeeder *muxFeeder;
        void signalEndOfRecording();
        
    private:
        std::vector<JPEGEncoder*> jpegEncoders;
        
        EncodingRingBuffer rawFrameQueue;
        EncodingRingBuffer encodedFrameQueue;
        EncodingRingBuffer audioQueue;
        
        QMutex mutex;
        unsigned long long nextAudioIndex;
        unsigned long long nextVideoIndex;
};


#endif	/* QUEUEINGENCODER_H */

