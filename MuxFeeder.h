#ifndef MUXFEEDER_H
#define	MUXFEEDER_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "EncodingRingBuffer.h"
#include "MatroskaEncoder.h"

#define INACTIVITY_SLEEP_MILLIS 50
#define MIN_VIDEO_WAIT_SIZE 10
#define MIN_AUDIO_WAIT_SIZE 20

class MuxFeeder : public QObject {
    Q_OBJECT
    
    public:
        MuxFeeder(EncodingRingBuffer*, EncodingRingBuffer*);
        bool setContainerEncoder(MatroskaEncoder*);
    
    private:
        QThread thread;
        QMutex mutex;
        
        EncodingRingBuffer *audioQueue;
        EncodingRingBuffer *videoQueue;
        MatroskaEncoder *containerEncoder;
        
        unsigned long long expectedAudioIndex;
        unsigned long long expectedVideoIndex;
        
        TimedPacket *currentAudioPacket;
        TimedPacket *currentVideoFrame;
        
        bool audioStreamTerminated;
        bool videoStreamTerminated;
        
        void msleep(unsigned long);
        
    signals:
        void startEncoding();
    
    private slots:
        void feedEncoder();
};

#endif	/* MUXFEEDER_H */

