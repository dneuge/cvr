#include <QtGui>
#include <QAction>
#include <QKeySequence>

#include "MainWindow.h"

MainWindow::MainWindow()
{
    renderingTest = new RenderingTest();
    
    QGridLayout *mainLayout = new QGridLayout;
    
    mainLayout->addWidget(renderingTest, 0, 0);
    
    setLayout(mainLayout);
    
    resize(1280, 720);
    
    
    
    QAction *actionAcceptNewImage = new QAction(this);
    actionAcceptNewImage->setShortcut(QKeySequence(Qt::Key_Space));
    connect(actionAcceptNewImage, SIGNAL(triggered()), renderingTest, SLOT(acceptNewImage()));
    addAction(actionAcceptNewImage);
}
