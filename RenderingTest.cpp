#include <QtGui>

#include "RenderingTest.h"

void RenderingTest::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    
    //painter.drawRect(QRect(0, 0, 200, 200));
    
    for (int y=0; y<256; y++) {
        for (int x=0; x<256; x++) {
            painter.setPen(QColor::fromRgb(x, y, 0));
            painter.drawPoint(x, y);
        }
    }
}