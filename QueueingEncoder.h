#ifndef QUEUEINGENCODER_H
#define	QUEUEINGENCODER_H

#include <iostream>
#include <stdio.h>
#include <list>

#include <QObject>
#include <QThread>
#include <QMutex>

#include "JPEGEncoder.h"
#include "TimedPacket.h"

class QueueingEncoder : public QObject
{
    Q_OBJECT
    
    public:
        void addVideoFrame(long, char*);
        void addAudioPacket(long, char*);
        long getTimestamp();
        void startThreads(int);
        
    private:
        QMutex jpegEncoderMutex;
        bool jpegEncoderReadiness[];
        unsigned int numJPEGEncoders;
        JPEGEncoder jpegEncoderInstances[];
        QThread jpegEncoderThreads[];
        
        QMutex queueMutex;
        std::list<TimedPacket> rawVideoFrames;
        std::list<TimedPacket> JPEGFrames;
        std::list<TimedPacket> audioPackets;
};


#endif	/* QUEUEINGENCODER_H */

