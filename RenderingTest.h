#ifndef RENDERINGTEST_H
#define	RENDERINGTEST_H

#include "DeckLinkAPI.h"

#include <QWidget>
#include <QMutex>
#include <QImage>
#include <QBuffer>
#include <QAudioOutput>
#include <QThread>

class DataCallback : public QObject, public IDeckLinkInputCallback
{
    Q_OBJECT
    
    public:
        DataCallback(QImage**, QMutex*);
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE  Release(void);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
        
    private:
        QRgb ***lookupTableYCrCbToQRgb;
        
        int audioChannels;
        int audioSampleDepth;
        
        int refCount;
        QMutex refCountMutex;
        QMutex frameAcceptanceMutex;
        QMutex audioAcceptanceMutex;
        QMutex *frameDrawMutex;
        QImage **image;
        //QBuffer audioBuffer;
        //QAudioOutput *audioOutput;
    
    signals:
        void imageUpdated();
};

class RenderingTest : public QWidget
{
    Q_OBJECT
    
    public:
        RenderingTest();
    
    protected:
        void paintEvent(QPaintEvent *event);
    
    private:
        QImage *image;
        QMutex frameDrawMutex;
        QThread dataCallbackThread;
        
    private slots:
        void acceptNewImage();
};

#endif	/* RENDERINGTEST_H */

