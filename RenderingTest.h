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

#include "DataCallback.h"
#include "VideoSurface.h"

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    public:
        RenderingTest();
        DataCallback *dataCallback;
    
    protected:
        void paintEvent(QPaintEvent *event);
    
    private:
        VideoSurface *videoSurface;
        QImage *image;
        QMutex frameDrawMutex;
        QThread dataCallbackThread;
};

#endif	/* RENDERINGTEST_H */

