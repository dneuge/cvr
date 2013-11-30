#include "RenderingTest.h"

RenderingTest::RenderingTest()
{
    // try to enhance performance
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_PaintOnScreen, true);
    
    image = new QImage(1280, 720, QImage::Format_RGB32);
    
    dataCallback = new DataCallback(&image, &frameDrawMutex);
    dataCallback->moveToThread(&dataCallbackThread);
    
    connect(dataCallback, SIGNAL(imageUpdated()), this, SLOT(update()));
    
    dataCallbackThread.start();
}

void RenderingTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    frameDrawMutex.lock();
    
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
    painter.end();
    
    /*
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    
    //painter.drawRect(QRect(0, 0, 200, 200));
    
    for (int y=0; y<256; y++) {
        for (int x=0; x<256; x++) {
            painter.setPen(QColor::fromRgb(x, y, z));
            painter.drawPoint(x, y);
        }
    }
    */
    
    frameDrawMutex.unlock();
}
