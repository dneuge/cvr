#include <QtGui>
#include <QAction>
#include <QKeySequence>

#include "MainWindow.h"
#include "DummyReceptionCallback.h"

MainWindow::MainWindow(QApplication *application)
{
    renderingTest = new RenderingTest();
    
    // DEBUG: add dummy callback to print reception info
    renderingTest->dataCallback->registerDelayedReceptionCallback(new DummyReceptionCallback());
    
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    
    mainLayout->addWidget(renderingTest, 0, 0);
    
    setLayout(mainLayout);
    
    resize(1280, 720);
    
    QAction *actionToggleCapture = new QAction(this);
    actionToggleCapture->setShortcut(QKeySequence(Qt::Key_Space));
    connect(actionToggleCapture, SIGNAL(triggered()), renderingTest->dataCallback, SLOT(toggleCapture()));
    addAction(actionToggleCapture);
    
    QAction *actionQuit = new QAction(this);
    actionQuit->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(actionQuit, SIGNAL(triggered()), application, SLOT(quit()));
    addAction(actionQuit);
    
    connect(renderingTest, SIGNAL(doubleClicked()), this, SLOT(toggleRealFullscreen()));
    connect(renderingTest, SIGNAL(doubleClickedRight()), this, SLOT(toggleFakeFullscreen()));
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
