#ifndef VIDEOSURFACE_H
#define	VIDEOSURFACE_H

#include <iostream>

#include <QtGui>
#include <QWidget>
#include <QMutex>
#include <QImage>
//#include <QBuffer>
//#include <QAudioOutput>
#include <QThread>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

#include "link/DataCallback.h"

class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
    
    public:
        VideoSurface(QWidget*, QImage**, QMutex*);
        QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType) const;
        bool present(const QVideoFrame&);
        void paint(QPainter*);
        bool start(const QVideoSurfaceFormat&);
        
    public slots:
        void newFrame();
    
    protected:
    
    private:
        QWidget *parent;
        QImage **image;
        QMutex *frameDrawMutex;
        QRect sourceRect;
        QRect targetRect;
};

#endif	/* VIDEOSURFACE_H */

