#ifndef RENDERINGTEST_H
#define	RENDERINGTEST_H

#include "DeckLinkAPI.h"

#include <QWidget>
#include <QMutex>
#include <QImage>

class RenderingTest : public QWidget, public IDeckLinkInputCallback
{
    Q_OBJECT
    
    public:
        RenderingTest();
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE  Release(void);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
        virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
    
    protected:
        void paintEvent(QPaintEvent *event);
    
    private:
        unsigned char ***lookupTableYCrCbToR;
        unsigned char ***lookupTableYCrCbToG;
        unsigned char ***lookupTableYCrCbToB;
        
        int z;
        int refCount;
        QMutex refCountMutex;
        QMutex frameAcceptanceMutex;
        QImage image;
        
    private slots:
        void acceptNewImage();
};

#endif	/* RENDERINGTEST_H */

