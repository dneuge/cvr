#include "RenderingTest.h"

RenderingTest::RenderingTest()
{
    // try to enhance performance
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_PaintOnScreen, true);
    
    image = new QImage(1280, 720, QImage::Format_RGB32);
    
    videoSurface = new VideoSurface(this, &image, &frameDrawMutex);
    QVideoSurfaceFormat format = QVideoSurfaceFormat(QSize(1280, 720), QVideoFrame::Format_RGB32);
    videoSurface->start(format);
    
    dataCallback = new DataCallback(&image, &frameDrawMutex);
    dataCallback->moveToThread(&dataCallbackThread);
    
    connect(dataCallback, SIGNAL(imageUpdated()), videoSurface, SLOT(newFrame()));
    
    dataCallbackThread.start();
}

void RenderingTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter *painter = new QPainter(this);
    
    videoSurface->paint(painter);
    
    painter->end();
    
    /*
    frameDrawMutex.lock();
    
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
    painter.end();
    
    frameDrawMutex.unlock();
    */
}
