#ifndef RENDERINGTEST_H
#define	RENDERINGTEST_H

#include <QWidget>

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    public:
        RenderingTest();
    
    protected:
        void paintEvent(QPaintEvent *event);
    
    private:
        int z;
        
    private slots:
        void acceptNewImage();
};

#endif	/* RENDERINGTEST_H */

