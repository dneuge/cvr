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
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

#include "DataCallback.h"
#include "VideoSurface.h"

// see: http://www.fourcc.org/yuv.php
#define FOURCC_UYVY_PLANAR 0x59565955

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    public:
        RenderingTest();
        DataCallback *dataCallback;
    
    public slots:
        void showXvFrame();
    
    protected:
        void paintEvent(QPaintEvent *event);
    
    private:
        VideoSurface *videoSurface;
        QImage *image;
        unsigned char *rawImage;
        unsigned long rawImageLength;
        QMutex frameDrawMutex;
        QThread dataCallbackThread;
        
        Display *display;
        unsigned int grabbedXvPort;
        GC x11GC;
        WId windowId;
};

#endif	/* RENDERINGTEST_H */

