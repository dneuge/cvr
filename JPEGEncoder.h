#ifndef JPEGENCODER_H
#define	JPEGENCODER_H

#include <iostream>
#include <stdio.h>

#include <QObject>
#include <QThread>

class JPEGEncoder : public QObject
{
    Q_OBJECT
    
    public:
        JPEGEncoder();
        unsigned long long encodeImage(char*, unsigned long long, char**);
        
    private:
        QThread thread;
};

#endif	/* JPEGENCODER_H */

