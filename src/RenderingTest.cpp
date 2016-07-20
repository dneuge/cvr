#include "RenderingTest.h"

// includes needed to display information
#include <time.h>
#include <sys/vfs.h>

RenderingTest::RenderingTest(QueueingEncoder *queueingEncoder)
{
    this->queueingEncoder = queueingEncoder;
    infoDisplayMode = 0;

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
    
    XLockDisplay(display);
    
    // configure keep alive to avoid screen saver/dimming
    screenKeepAliveActive = false;
    screenKeepAliveCountDown = 0;
    screenKeepAliveModulo = 1200; // 1200 = 20 seconds @ 60 fps, 40 seconds @ 30 fps
    screenKeepAliveKey = XKeysymToKeycode(display, XK_Shift_L);
    
    // try to find and set VSync/buffering attributes
    int numAttributes;
    XvAttribute *attributes = XvQueryPortAttributes(display, grabbedXvPort, &numAttributes);
    for (int i=0; i<numAttributes; i++) {
        XvAttribute *attribute = &attributes[i];
        std::cout << "found XVideo attribute: " << attribute->name << "\n";
        
        if (strcmp(attribute->name, "XV_SYNC_TO_VBLANK") == 0) {
            std::cout << "trying to enable VSYNC\n";
            
            Atom atom = XInternAtom(display, attribute->name, False);
            if (atom == BadAlloc) {
                std::cout << "atom for VSYNC was BadAlloc\n";
            } else if (atom == BadAtom) {
                std::cout << "atom for VSYNC was BadAtom\n";
            } else if (atom == BadValue) {
                std::cout << "atom for VSYNC was BadValue\n";
            }
            
            
            int res = XvSetPortAttribute(display, grabbedXvPort, atom, 1);
            if (res != Success) {
                std::cout << "failed to enable VSYNC: ";
                if (res == XvBadExtension) {
                    std::cout << "XvBadExtension";
                } else if (res == XvBadAlloc) {
                    std::cout << "XvBadAlloc";
                } else {
                    std::cout << "unknown return code from XvSetPortAttribute";
                }
                std::cout << "\n";
            }
        /*
        } else if (strcmp(attribute->name, "XV_BRIGHTNESS") == 0) {
            std::cout << "trying to set brightness\n";
            
            Atom atom = XInternAtom(display, "XV_BRIGHTNESS", False);
            
            int res = XvSetPortAttribute(display, grabbedXvPort, atom, 50);
            if (res != Success) {
                std::cout << "failed to set brightness: ";
                if (res == XvBadExtension) {
                    std::cout << "XvBadExtension";
                } else if (res == XvBadAlloc) {
                    std::cout << "XvBadAlloc";
                } else {
                    std::cout << "unknown return code from XvSetPortAttribute";
                }
                std::cout << "\n";
            }
        */
        } else if (strcmp(attribute->name, "XV_ITURBT_709") == 0) {
            std::cout << "trying to set dynamic range\n";
            
            Atom atom = XInternAtom(display, attribute->name, False);
            
            int res = XvSetPortAttribute(display, grabbedXvPort, atom, 0);
            if (res != Success) {
                std::cout << "failed to set dynamic range: ";
                if (res == XvBadExtension) {
                    std::cout << "XvBadExtension";
                } else if (res == XvBadAlloc) {
                    std::cout << "XvBadAlloc";
                } else {
                    std::cout << "unknown return code from XvSetPortAttribute";
                }
                std::cout << "\n";
            }
        }
    }
    
    XFree(attributes);
    
    XUnlockDisplay(display);
    
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
    connect(queueingEncoder, SIGNAL(stateChanged()), this, SLOT(repaint()), Qt::QueuedConnection);
    
    // shortcuts for cycling of information display
    QAction *actionCycleInfo = new QAction(this);
    actionCycleInfo->setShortcut(QKeySequence(Qt::Key_Q));
    connect(actionCycleInfo, SIGNAL(triggered()), this, SLOT(cycleInfoDisplay()));
    addAction(actionCycleInfo);
}

void RenderingTest::cycleInfoDisplay() {
    mutex.lock();
    infoDisplayMode = ++infoDisplayMode % 3;
    mutex.unlock();
    
    emit repaint();
}

void RenderingTest::toggleScreenKeepAlive(bool active) {
    // keep alive happens during frame draw so we need to lock the draw mutex
    frameDrawMutex.lock();
    
    std::cout << "screen keep alive changed: " << (active ? "on" : "off") << "\n";
    screenKeepAliveCountDown = 0; // resets counter
    screenKeepAliveActive = active;
    
    frameDrawMutex.unlock();
}

void RenderingTest::showXvFrame() {
    frameDrawMutex.lock();
    XLockDisplay(display);
    
    XvImage *xvImage = XvCreateImage(display, grabbedXvPort, FOURCC_UYVY_PLANAR, (char*) rawImage, 1280, 720);
    windowId = winId();
    x11GC = XCreateGC(display, winId(), 0, 0);
    XvPutImage(display, grabbedXvPort, windowId, x11GC, xvImage, 0, 0, 1280, 720, targetX, targetY, targetWidth, targetHeight);
    XFreeGC(display, x11GC);
    XFree(xvImage);
    
    // send key strokes to X server via test extension every n frames to prevent
    // screen saver and screen dimming from triggering
    if (screenKeepAliveActive) {
        screenKeepAliveCountDown = ++screenKeepAliveCountDown % screenKeepAliveModulo;
        if (screenKeepAliveCountDown == 0) {
            XTestFakeKeyEvent(display, screenKeepAliveKey, True, 0);
            XTestFakeKeyEvent(display, screenKeepAliveKey, False, 0);
        }
    }
    
    //XSync(display, False);
    XFlush(display);
    
    XUnlockDisplay(display);
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

void RenderingTest::paintInfo(QPainter *painter, unsigned char infoDisplayMode) {
    // get current time
    time_t currentTime;
    struct tm currentLocalTime;
    char *timeString = new char[255];
    time(&currentTime);
    localtime_r(&currentTime, &currentLocalTime);
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
    
    // draw statistics
    if (infoDisplayMode == 2) {
        std::map<ContentType,QueueStats*>* stats = queueingEncoder->getQueueStats();
        std::map<ContentType,QueueStats*>::iterator it = stats->begin();

        while (it != stats->end()) {
            // paint
            paintStats(painter, (*it).first, (*it).second, encoderState);

            // free after use
            delete (*it).second;

            it++;
        }

        delete stats;
    }
}

void RenderingTest::paintStats(QPainter *painter, ContentType contentType, QueueStats *stats, EncoderState encoderState) {
    QString name;
    long offsetY = -1;
    
    switch (contentType) {
        case AUDIO:         offsetY =  0; name.append("Audio Queue"); break;
        case VIDEO_RAW:     offsetY = 20; name.append("Raw Video Queue"); break;
        case VIDEO_ENCODED: offsetY = 40; name.append("JPEG Video Queue"); break;
    }
    
    // skip unsupported content types
    if (offsetY < 0) {
        return;
    }
    
    // setup painter
    QFont font("Source Sans Pro");
    font.setPixelSize(14);
    painter->setFont(font);
    painter->setPen(Qt::white);
    
    // - queue name
    long offsetX = 5;
    painter->drawText(offsetX, offsetY, 120, 15, Qt::AlignLeft, QString(name).append(":"), 0);
    offsetX += 130;
    
    // - total bytes
    painter->drawText(offsetX, offsetY, 50, 15, Qt::AlignRight, formatBytesAsQString(stats->totalBytes), 0);
    offsetX += 55;
    
    // - divider
    painter->drawLine(offsetX, offsetY, offsetX, offsetY + 20);
    offsetX += 5;
    
    // - seconds
    unsigned long millisInBuffer = stats->latestPacketTimestamp - stats->oldestPacketTimestamp;
    char *secondsInBuffer = new char[256];
    snprintf(secondsInBuffer, 255, "%.1lf", (double) millisInBuffer / 1000.0);
    QString secondsInBufferQString(secondsInBuffer);
    delete[] secondsInBuffer;
    
    painter->drawText(offsetX, offsetY, 40, 15, Qt::AlignRight, secondsInBufferQString.append(" s"), 0);
    offsetX += 45;
    
    // - divider
    painter->drawLine(offsetX, offsetY, offsetX, offsetY + 20);
    offsetX += 5;
    
    // - packets
    char *packetsInBuffer = new char[256];
    snprintf(packetsInBuffer, 255, "%ld", stats->numPackets);
    QString packetsInBufferQString(packetsInBuffer);
    delete[] packetsInBuffer;
    
    painter->drawText(offsetX, offsetY, 50, 15, Qt::AlignRight, packetsInBufferQString.append((contentType == AUDIO) ? " p" : " f"), 0);
    offsetX += 55;
    
    // - divider
    painter->drawLine(offsetX, offsetY, offsetX, offsetY + 20);
    
    // - percentage filled
    double percentageUsed = -1.0;
    double percentageUsedMillis = -1.0;
    double percentageUsedPackets = -1.0;
    char *percentageUsedString = new char[256];
    
    if (stats->maxMillis != 0) {
        percentageUsedMillis = (stats->latestPacketTimestamp - stats->oldestPacketTimestamp) / (double) stats->maxMillis * 100.0;
    }
    
    if (stats->maxPackets != 0) {
        percentageUsedPackets = (double) stats->numPackets / stats->maxPackets * 100.0;
    }
    
    percentageUsed = percentageUsedPackets;
    if (percentageUsedMillis > percentageUsedPackets) {
        percentageUsed = percentageUsedMillis;
    }
    
    if (percentageUsed < -0.5) {
        strcpy(percentageUsedString, "n/a");
    } else if (percentageUsed < 0.01) {
        strcpy(percentageUsedString, "0 %");
    } else if (percentageUsed > 99.99) {
        strcpy(percentageUsedString, "100 %");
    } else {
        snprintf(percentageUsedString, 255, "%.0lf %%", percentageUsed);
    }
    
    QString percentageUsedQString(percentageUsedString);
    delete[] percentageUsedString;
    
    painter->drawText(offsetX, offsetY, 40, 15, Qt::AlignRight, percentageUsedQString, 0);
    offsetX += 45;
    
    // info about size per second makes only sense during timeshift
    if (encoderState == TIMESHIFT) {
        // - divider
        painter->drawLine(offsetX, offsetY, offsetX, offsetY + 20);
        offsetX += 5;

        // - packets/second (in buffer)
        QString ppsInBufferQString;
        if ((millisInBuffer == 0) || (stats->numPackets == 0)) {
            ppsInBufferQString.append("n/a");
        } else {
            double ppsInBuffer = (double) stats->numPackets / millisInBuffer * 1000.0;
            char *ppsInBufferString = new char[256];
            snprintf(ppsInBufferString, 255, "%.1lf", ppsInBuffer);
            ppsInBufferQString.append(ppsInBufferString);
            delete[] ppsInBufferString;
        }

        painter->drawText(offsetX, offsetY, 60, 15, Qt::AlignRight, ppsInBufferQString.append((contentType == AUDIO) ? " p/s" : " f/s"), 0);
        offsetX += 65;

        // - divider
        painter->drawLine(offsetX, offsetY, offsetX, offsetY + 20);
        offsetX += 5;

        // - bytes/second (in buffer)
        QString bpsInBufferQString;
        if ((millisInBuffer == 0) || (stats->numPackets == 0)) {
            bpsInBufferQString.append("n/a");
        } else {
            bpsInBufferQString = formatBytesAsQString((double) stats->totalBytes / millisInBuffer * 1000.0).append("/s");
        }
        painter->drawText(offsetX, offsetY, 60, 15, Qt::AlignRight, bpsInBufferQString, 0);
        offsetX += 65;
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
    mutex.lock();
    unsigned char currentInfoDisplayMode = infoDisplayMode;
    mutex.unlock();
    
    if (currentInfoDisplayMode != 0) {
        paintInfo(painter, currentInfoDisplayMode);
    }
    
    painter->end();
}

void RenderingTest::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::RightButton) {
        emit doubleClickedRight();
    } else {
        emit doubleClicked();
    }
}
