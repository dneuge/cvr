#ifndef DATACALLBACK_H
#define	DATACALLBACK_H

#include "DeckLinkAPI.h"
#include <iostream>

#include <QtGui>
#include <QWidget>
#include <QMutex>
#include <QImage>
//#include <QBuffer>
//#include <QAudioOutput>
#include <QThread>

class DataCallback : public QObject, public IDeckLinkInputCallback
{
    Q_OBJECT
    
    public:
        DataCallback(QImage**, unsigned char*, unsigned long, QMutex*);
        void shutdown();
        
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { 
            Q_UNUSED(iid);
            Q_UNUSED(ppv);
            return E_NOINTERFACE;
        }
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE  Release(void);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
        
    private:
        IDeckLink *deckLink;
        IDeckLinkInput *deckLinkInput;
        IDeckLinkIterator *deckLinkIterator;
        
        QRgb ***lookupTableYCrCbToQRgb;
        
        int audioChannels;
        int audioSampleDepth;
        int audioSampleRate;
        
        bool skipFrames;
        bool outputToQImage;
        
        int refCount;
        QMutex refCountMutex;
        QMutex frameAcceptanceMutex;
        QMutex audioAcceptanceMutex;
        QMutex *frameDrawMutex;
        QImage **image;
        
        unsigned char* rawImage;
        unsigned long rawImageLength;
        
        bool halfFrameRate;
        bool lastFrameUsed;
        
    public slots:
        void toggleCapture();
    
    signals:
        void imageUpdated();
        void audioArrived(char* rawAudio, unsigned long byteLength);
};

#endif	/* DATACALLBACK_H */

