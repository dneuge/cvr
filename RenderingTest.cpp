#include "RenderingTest.h"

RenderingTest::RenderingTest()
{
    // enhance performance by setting widget content unmanaged
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_PaintOnScreen, true);
    
    // create image and shared buffer
    image = new QImage(1280, 720, QImage::Format_RGB32);
    rawImageLength = 1280/2 * 720 * 4; // UYVY: 4 bytes per 2 pixels
    rawImage = new unsigned char[rawImageLength]; 
    
    /*
    videoSurface = new VideoSurface(this, &image, &frameDrawMutex);
    QVideoSurfaceFormat format = QVideoSurfaceFormat(QSize(1280, 720), QVideoFrame::Format_RGB32);
    videoSurface->start(format);
    */
    
    // get X11 information
    display = x11Info().display();
    windowId = winId();
    
    // setup X11 Garbage Collector
    x11GC = XCreateGC(display, winId(), 0, 0);
    
    // get adaptor infos
    unsigned int numAdaptors = 0;
    XvAdaptorInfo *adaptorInfo;
    
    if (XvQueryAdaptors(display, DefaultRootWindow(display), &numAdaptors, &adaptorInfo) != Success) {
        std::cout << "failed to query XVideo adaptors\n";
        return;
    }
    
    if (numAdaptors <= 0) {
        std::cout << "no XVideo adaptors available\n";
        return;
    }
    
    // it appears we can grab any adaptor we want, try to grab one now
    grabbedXvPort = -1;
    bool success = false;
    for (unsigned int i = 0; i < numAdaptors; i++) {
        unsigned int xvPort = adaptorInfo->base_id + i;
        if (XvGrabPort(display, xvPort, CurrentTime) == Success) {
            printf("grabbed Xv port %u\n", i);
            grabbedXvPort = xvPort;
            success = true;
            break;
        }
    }
    
    XFree(adaptorInfo);
    
    if (!success) {
        std::cout << "failed to grab any Xv port\n";
        return;
    }
    
    // check if pixel format (UYVY packed) is understood by Xv adaptor
    int numImageFormats = 0;
    XvImageFormatValues *xvImageFormatValues = XvListImageFormats(display, grabbedXvPort, &numImageFormats);
    if (numImageFormats <= 0) {
        std::cout << "no image formats listed for Xv port\n";
        return;
    }
    
    success = false;
    for (int i=0; i < numImageFormats; i++) {
        if (xvImageFormatValues[i].id == FOURCC_UYVY_PLANAR) {
            success = true;
        }
    }
    
    XFree(xvImageFormatValues);
    
    if (!success) {
        std::cout << "required FourCC pixel format is not supported by Xv adaptor\n";
        return;
    }
    
    // setup capturing thread
    dataCallback = new DataCallback(&image, rawImage, rawImageLength, &frameDrawMutex);
    dataCallback->moveToThread(&dataCallbackThread);
    
    // link capture to trigger output update
    //connect(dataCallback, SIGNAL(imageUpdated()), videoSurface, SLOT(paintEvent));
    connect(dataCallback, SIGNAL(imageUpdated()), this, SLOT(showXvFrame()));
    
    // setup and start audio sink thread
    pulseAudioSink = new PulseAudioSink();
    pulseAudioSink->moveToThread(&pulseAudioSinkThread);
    pulseAudioSinkThread.start();
    
    // link audio input signal to output slot
    connect(dataCallback, SIGNAL(audioArrived(char*, unsigned long)), pulseAudioSink, SLOT(playAudio(char*, unsigned long)));
    
    // start capturing
    dataCallbackThread.start();
}

void RenderingTest::showXvFrame() {
    frameDrawMutex.lock();
    
    XvImage *xvImage = XvCreateImage(display, grabbedXvPort, FOURCC_UYVY_PLANAR, (char*) rawImage, 1280, 720);
    windowId = winId();
    x11GC = XCreateGC(display, winId(), 0, 0);
    XvPutImage(display, grabbedXvPort, windowId, x11GC, xvImage, 0, 0, 1280, 720, 0, 0, width(), height());
    XFreeGC(display, x11GC);
    XFree(xvImage);
    
    frameDrawMutex.unlock();
}

void RenderingTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    /*
    QPainter *painter = new QPainter(this);
    
    videoSurface->paint(painter);
    
    painter->end();
    */
    /*
    frameDrawMutex.lock();
    
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
    painter.end();
    
    frameDrawMutex.unlock();
    */
}
