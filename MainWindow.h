#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QWidget>

#include "RenderingTest.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    public:
        MainWindow(QApplication*);
        void toggleFullscreen(bool);
        
    public slots:
        void toggleRealFullscreen();
        void toggleFakeFullscreen();
    
    private:
        RenderingTest *renderingTest;
};

#endif	/* MAINWINDOW_H */

