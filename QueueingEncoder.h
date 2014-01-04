#ifndef QUEUEINGENCODER_H
#define	QUEUEINGENCODER_H

#include <vector>

#include <QObject>

#include "JPEGEncoder.h"
#include "TimedPacket.h"
#include "DelayedReceptionCallback.h"
#include "EncodingRingBuffer.h"
#include "MuxFeeder.h"

enum EncoderState {
    IDLE,       // when encoding is suspended
    TIMESHIFT,  // when encoding is active but no recording runs
    RECORDING,  // when encoding is active and recording runs
    STOPPING    // when End Of Record has been signalled but recording still runs
};

class QueueingEncoder : public QObject, public DelayedReceptionCallback {
    Q_OBJECT
    
    public:
        QueueingEncoder(int);
        virtual void dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame);
        MuxFeeder *muxFeeder;
        void signalEndOfRecording();
        void setFrameDivisionModulo(unsigned char);
        void removeEORFromQueues();
        void clearQueues();
        bool startRecording(char*);
        EncoderState getState();
        
    private:
        EncoderState state;
        
        std::vector<JPEGEncoder*> jpegEncoders;
        
        EncodingRingBuffer rawFrameQueue;
        EncodingRingBuffer encodedFrameQueue;
        EncodingRingBuffer audioQueue;
        
        QMutex mutex;
        unsigned long long nextAudioIndex;
        unsigned long long nextVideoIndex;
        
        unsigned char frameDivisionModulo;
        unsigned char frameDivisionCounter;
        
        void setState(EncoderState);
        
    signals:
        void stateChanged();

    private slots:
        void stateChangeRecordingStarted();
        void stateChangeRecordingStopped();
};


#endif	/* QUEUEINGENCODER_H */

