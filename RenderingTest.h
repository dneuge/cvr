#ifndef RENDERINGTEST_H
#define	RENDERINGTEST_H

#include <iostream>

#include <QtGui>
#include <QWidget>
#include <QMutex>
#include <QImage>
//#include <QBuffer>
//#include <QAudioOutput>
#include <QThread>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

#include "DataCallback.h"
#include "VideoSurface.h"
#include "PulseAudioSink.h"
#include "QueueingEncoder.h"

// see: http://www.fourcc.org/yuv.php
#define FOURCC_UYVY_PLANAR 0x59565955

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    public:
        RenderingTest(QueueingEncoder*);
        DataCallback *dataCallback;
        PulseAudioSink *pulseAudioSink;
    
    public slots:
        void showXvFrame();
        void updateDimensions();
        void toggleScreenKeepAlive(bool);
    
    protected:
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent*);
        void mouseDoubleClickEvent(QMouseEvent*);
    
    private:
        VideoSurface *videoSurface;
        QImage *image;
        unsigned char *rawImage;
        unsigned long rawImageLength;
        QMutex frameDrawMutex;
        QThread dataCallbackThread;
        QThread pulseAudioSinkThread;
        
        Display *display;
        unsigned int grabbedXvPort;
        GC x11GC;
        WId windowId;
        
        int windowWidth;
        int windowHeight;
        int targetX;
        int targetY;
        unsigned int targetWidth;
        unsigned int targetHeight;
        
        QueueingEncoder *queueingEncoder;
        
        QMutex mutex;
        unsigned char infoDisplayMode;
        
        bool screenKeepAliveActive;
        KeyCode screenKeepAliveKey;
        unsigned int screenKeepAliveModulo;
        unsigned int screenKeepAliveCountDown;
        
        void paintInfo(QPainter*, unsigned char);
        void paintStats(QPainter*, ContentType, QueueStats*, EncoderState);
        QString formatBytesAsQString(double);
        
    signals:
        void doubleClicked();
        void doubleClickedRight();
        
    private slots:
        void cycleInfoDisplay();
};

#endif	/* RENDERINGTEST_H */

