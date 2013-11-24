#include <QtGui>

#include "MainWindow.h"

MainWindow::MainWindow()
{
    renderingTest = new RenderingTest();
    
    QGridLayout *mainLayout = new QGridLayout;
    
    mainLayout->addWidget(renderingTest, 0, 0);
    
    setLayout(mainLayout);
    
    resize(1280, 720);
}
