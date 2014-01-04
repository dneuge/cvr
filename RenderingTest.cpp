#include "RenderingTest.h"

// includes needed to display information
#include <time.h>
#include <sys/vfs.h>

RenderingTest::RenderingTest(QueueingEncoder *queueingEncoder)
{
    this->queueingEncoder = queueingEncoder;
    
    // initialize dimensions
    updateDimensions();
    
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
    
    // trigger window repaint every second
    QTimer *repaintTimer = new QTimer(this);
    repaintTimer->setInterval(1000);
    repaintTimer->start();
    
    connect(repaintTimer, SIGNAL(timeout()), this, SLOT(repaint()));
    
    // also trigger repaint if encoding state changes
    connect(queueingEncoder, SIGNAL(stateChanged()), this, SLOT(repaint()));
}

void RenderingTest::showXvFrame() {
    frameDrawMutex.lock();
    
    XvImage *xvImage = XvCreateImage(display, grabbedXvPort, FOURCC_UYVY_PLANAR, (char*) rawImage, 1280, 720);
    windowId = winId();
    x11GC = XCreateGC(display, winId(), 0, 0);
    XvPutImage(display, grabbedXvPort, windowId, x11GC, xvImage, 0, 0, 1280, 720, targetX, targetY, targetWidth, targetHeight);
    XFreeGC(display, x11GC);
    XFree(xvImage);
    
    frameDrawMutex.unlock();
}

void RenderingTest::updateDimensions() {
    windowWidth = width();
    windowHeight = height();
    
    //bool landscape = windowWidth > windowHeight;
    
    targetWidth  = windowWidth;
    targetHeight = (double) targetWidth / 16 * 9;
    
    targetX = 0;
    targetY = (windowHeight - targetHeight) / 2;
}

void RenderingTest::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    
    updateDimensions();
}

QString RenderingTest::formatBytesAsQString(double bytes) {
    char timesDivided = 0;
    char *prefixes = " kMGTPE";
    while ((bytes >= 1024.0) && (timesDivided < 7)) {
        bytes /= 1024.0;
        timesDivided++;
    }
    
    char *freeSpaceString = new char[255];
    snprintf(freeSpaceString, 255, "%.1lf %cB", bytes, prefixes[timesDivided]);
    QString freeSpaceQString(freeSpaceString);
    
    delete[] freeSpaceString;
    //delete[] prefixes;
    
    return freeSpaceQString;
}

void RenderingTest::paintInfo(QPainter *painter) {
    // get current time
    time_t currentTime;
    struct tm currentLocalTime;
    char *timeString = new char[255];
    time(&currentTime);
    localtime_r(&currentTime);
    strftime(timeString, 255, "%H:%M:%S", &currentLocalTime);
    QString timeQString(timeString);
    delete timeString;
    
    // get free disk space
    struct statfs64 stat;
    statfs64(".", &stat);
    double freeSpace = stat.f_bavail * stat.f_bsize;
    
    // draw
    painter->setPen(Qt::white);
    long width;
    long offsetX;
    
    //  - draw clock
    QFont font("Source Sans Pro");
    font.setPixelSize(24);
    painter->setFont(font);
    painter->drawText(0, 0, windowWidth, 50, Qt::AlignCenter, timeQString, 0);
    
    //  - draw info about disk space
    width = 100;
    font.setPixelSize(14);
    painter->setFont(font);
    painter->drawText(windowWidth - width - 10, 20, width, 50, Qt::AlignRight, formatBytesAsQString(freeSpace).append(" free"), 0);
    
    //  - draw state info
    EncoderState encoderState = queueingEncoder->getState();
    switch (encoderState) {
        case IDLE:      painter->drawText(windowWidth - width - 10, 0, width, 50, Qt::AlignRight, tr("Idle"), 0);
                        break;
                        
        case TIMESHIFT: painter->drawText(windowWidth - width - 10, 0, width, 50, Qt::AlignRight, tr("Timeshift"), 0);
                        break;
                        
        case RECORDING: painter->drawText(windowWidth - width - 10, 0, width, 50, Qt::AlignRight, tr("Recording"), 0);
                        break;
                        
        case STOPPING:  painter->drawText(windowWidth - width - 10, 0, width, 50, Qt::AlignRight, tr("Stopping"), 0);
                        break;
    }
}

void RenderingTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    // paint everything outside Xv area black
    
    QPainter *painter = new QPainter(this);
    
    // area above video
    if (targetY > 0) {
        painter->fillRect(0, 0, windowWidth, targetY, Qt::black);
    }
    
    // area below video
    int yEndVideo = targetY + targetHeight;
    if (yEndVideo < windowHeight) {
        painter->fillRect(0, yEndVideo, windowWidth, windowHeight - yEndVideo, Qt::black);
    }
    
    // paint additional info
    paintInfo(painter);
    
    painter->end();
}

void RenderingTest::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::RightButton) {
        emit doubleClickedRight();
    } else {
        emit doubleClicked();
    }
}
