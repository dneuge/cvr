#include "VideoSurface.h"

VideoSurface::VideoSurface(QWidget* parent, QImage** image, QMutex* frameDrawMutex) : QAbstractVideoSurface(parent) {
    this->parent = parent;
    this->image = image;
    this->frameDrawMutex = frameDrawMutex;
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const {
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32
            //<< QVideoFrame::Format_ARGB32
            //<< QVideoFrame::Format_ARGB32_Premultiplied
            //<< QVideoFrame::Format_RGB565
            //<< QVideoFrame::Format_RGB555
            ;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoSurface::start(const QVideoSurfaceFormat &format) {
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();
    
    std::cout << "VideoSurface::start\n"; // DEBUG
    
    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        frameDrawMutex->lock();
        
        //this->imageFormat = imageFormat;
        //imageSize = size;
        sourceRect = format.viewport();
        
        QAbstractVideoSurface::start(format);
        
        parent->updateGeometry();
        
        targetRect = QRect(QPoint(0, 0), (*image)->size());
        targetRect.moveCenter(parent->rect().center());

        frameDrawMutex->unlock();
    } else {
        return false;
    }
    
    return true;
}

void VideoSurface::newFrame() {
    QVideoFrame frame;
    present(frame);
}

bool VideoSurface::present(const QVideoFrame &frame) {
    Q_UNUSED(frame);
    
    parent->repaint(targetRect);
    
    return true;
}

void VideoSurface::paint(QPainter *painter) {
    frameDrawMutex->lock();
    
    const QTransform oldTransform = painter->transform();
    
    targetRect = QRect(QPoint(0, 0), (*image)->size());
    targetRect.moveCenter(parent->rect().center());
    
    painter->drawImage(targetRect, **image, sourceRect);
    painter->setTransform(oldTransform);
    
    frameDrawMutex->unlock();
}
