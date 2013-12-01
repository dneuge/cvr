#ifndef PULSEAUDIOSINK_H
#define	PULSEAUDIOSINK_H

#include <iostream>
#include <stdio.h>

#include <QObject>
#include <QThread>

#include <pulse/simple.h>

#define APP_NAME "HDMIViewer"
#define STREAM_DESCRIPTION "HDMI Audio Input"

class PulseAudioSink : public QObject
{
    Q_OBJECT
    
    public:
        PulseAudioSink();
    
    public slots:
        void playAudio(char* rawAudio, unsigned long byteLength);
        
    private:
        pa_simple *paSimple;
};

#endif	/* PULSEAUDIOSINK_H */

