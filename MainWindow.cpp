#include "MainWindow.h"

#include <time.h>
#include <stdio.h>

#include <QtGui>
#include <QAction>
#include <QKeySequence>

#include "DummyReceptionCallback.h"
#include "QueueingEncoder.h"

MainWindow::MainWindow(QApplication *application)
{
    // DEBUG: add dummy callback to print reception info
    //renderingTest->dataCallback->registerDelayedReceptionCallback(new DummyReceptionCallback());
    
    encoder = new QueueingEncoder(2);
    encoder->setFrameDivisionModulo(2); // encode every nth frame
    
    renderingTest = new RenderingTest(encoder);
    renderingTest->dataCallback->registerDelayedReceptionCallback(encoder);
    
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    
    mainLayout->addWidget(renderingTest, 0, 0);
    
    setLayout(mainLayout);
    
    resize(1280, 720);
    
    // hot-key SPACE toggles capturing (used to resync)
    QAction *actionToggleCapture = new QAction(this);
    actionToggleCapture->setShortcut(QKeySequence(Qt::Key_Space));
    connect(actionToggleCapture, SIGNAL(triggered()), renderingTest->dataCallback, SLOT(toggleCapture()));
    addAction(actionToggleCapture);
    
    // hot-key ESC quits application
    QAction *actionQuit = new QAction(this);
    actionQuit->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(actionQuit, SIGNAL(triggered()), application, SLOT(quit()));
    addAction(actionQuit);
    
    // left double click toggles full screen display
    connect(renderingTest, SIGNAL(doubleClicked()), this, SLOT(toggleRealFullscreen()));
    
    // right double click toggles maximized borderless window ("fake full screen")
    connect(renderingTest, SIGNAL(doubleClickedRight()), this, SLOT(toggleFakeFullscreen()));
    
    // hot-key RETURN starts new recording
    QAction *actionStartRecording = new QAction(this);
    actionStartRecording->setShortcut(QKeySequence(Qt::Key_Return));
    connect(actionStartRecording, SIGNAL(triggered()), this, SLOT(startRecording()));
    addAction(actionStartRecording);
    
    // hot-key BACKSPACE stops recording
    QAction *actionStopRecording = new QAction(this);
    actionStopRecording->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(actionStopRecording, SIGNAL(triggered()), this, SLOT(stopRecording()));
    addAction(actionStopRecording);
}

void MainWindow::startRecording() {
    time_t currentTime;
    char fileName[256];
    
    time(&currentTime);
    struct tm *currentLocalTime = localtime(&currentTime);
    
    strftime(fileName, 256, "cvr_recording_%F_%H-%M-%S.mkv", currentLocalTime);
    
    //delete currentLocalTime;
    
    bool success = encoder->startRecording(fileName);
    
    if (!success) {
        printf("failed to start recording to %s\n", fileName);
    }
}

void MainWindow::stopRecording() {
    encoder->signalEndOfRecording();
}


void MainWindow::toggleFullscreen(bool maximizeInstead) {
    Qt::WindowFlags flags = windowFlags();
    
    if (flags & Qt::FramelessWindowHint) {
        // disable
        flags &= ~Qt::FramelessWindowHint;
        setWindowState(Qt::WindowNoState);
        
        setCursor(Qt::ArrowCursor);
    } else {
        // enable
        flags |= Qt::FramelessWindowHint;
        if (maximizeInstead) {
            setWindowState(Qt::WindowMaximized);
        } else {
            setWindowState(Qt::WindowFullScreen);
        }
        
        setCursor(Qt::BlankCursor);
    }
    
    setWindowFlags(flags);
    
    // we need to show again after changing flags
    show();
}

void MainWindow::toggleRealFullscreen() {
    toggleFullscreen(false);
}

void MainWindow::toggleFakeFullscreen() {
    toggleFullscreen(true);
}
