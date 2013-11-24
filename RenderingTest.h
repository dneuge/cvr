#ifndef RENDERINGTEST_H
#define	RENDERINGTEST_H

#include <QWidget>

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    protected:
        void paintEvent(QPaintEvent *event);
};

#endif	/* RENDERINGTEST_H */

