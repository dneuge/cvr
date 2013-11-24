#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QWidget>

#include "RenderingTest.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    public:
        MainWindow();
    
    private:
        RenderingTest *renderingTest;
};

#endif	/* MAINWINDOW_H */

