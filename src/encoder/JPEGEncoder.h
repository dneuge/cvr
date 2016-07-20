#ifndef JPEGENCODER_H
#define	JPEGENCODER_H

#include <iostream>
#include <stdio.h>

#include <QObject>
#include <QThread>

#include "EncodingRingBuffer.h"

#define INACTIVITY_SLEEP_MILLIS 50
#define JPEG_QUALITY 90

class JPEGEncoder : public QObject
{
    Q_OBJECT
    
    public:
        JPEGEncoder(EncodingRingBuffer*, EncodingRingBuffer*);
    
    private:
        EncodingRingBuffer *inputQueue;
        EncodingRingBuffer *outputQueue;
        
        QThread thread;
        
        void msleep(unsigned long);
        unsigned long encodeImage(char*, unsigned long long, char**);
    
    signals:
        void startEncoding();
    
    private slots:
        void encodeBetweenQueues();
};

#endif	/* JPEGENCODER_H */

