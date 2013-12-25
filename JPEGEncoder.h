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
        void encodeSingleFrame(char*, char**);
};

#endif	/* JPEGENCODER_H */

