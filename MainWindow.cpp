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
    
    
    
    QAction *actionToggleCapture = new QAction(this);
    actionToggleCapture->setShortcut(QKeySequence(Qt::Key_Space));
    connect(actionToggleCapture, SIGNAL(triggered()), renderingTest->dataCallback, SLOT(toggleCapture()));
    addAction(actionToggleCapture);
}
