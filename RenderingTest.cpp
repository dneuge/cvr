#include <QtGui>
#include <iostream>

#include "DeckLinkAPI.h"

#include "RenderingTest.h"

// FIXME: that is very dirty but how else can we tell NetBeans to include that file? unfortunately it's not a lib
#include "../../Downloads/Blackmagic DeckLink SDK 9.7.7/Linux/include/DeckLinkAPIDispatch.cpp"

RenderingTest::RenderingTest(){
    z = 0;
    
    IDeckLink *deckLink;
    IDeckLinkInput *deckLinkInput;
    
    BMDDisplayMode displayMode = bmdModeHD720p5994;
    BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
    BMDVideoInputFlags inputFlags = 0;
    
    // get iterator
    IDeckLinkIterator *deckLinkIterator = CreateDeckLinkIteratorInstance();
    if (!deckLinkIterator)
    {
        std::cout << "no iterator\n";
        return;
    }
    
    // get first device
	if (deckLinkIterator->Next(&deckLink) != S_OK)
	{
        std::cout << "no device found by iterator\n";
        return;
    }
    
    // get input interface
	if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK)
    {
        std::cout << "failed to get input interface\n";
        return;
    }
    
    // register ourselves as receiver
    deckLinkInput->SetCallback(this);
    
    // verify video mode
    BMDDisplayModeSupport res;
    deckLinkInput->DoesSupportVideoMode(displayMode, pixelFormat, bmdVideoInputFlagDefault, &res, NULL);
    if (res == bmdDisplayModeNotSupported)
    {
        std::cout << "video mode is not supported\n";
        return;
    }
    
    // configure video stream
    if (deckLinkInput->EnableVideoInput(displayMode, pixelFormat, inputFlags) != S_OK)
    {
		std::cout << "could not configure video stream\n";
        return;
    }
    
    // start streaming
    if(deckLinkInput->StartStreams() != S_OK)
    {
		std::cout << "could not start video stream\n";
        return;
    }
        
    std::cout << "initialized\n";
}

/*
HRESULT STDMETHODCALLTYPE RenderingTest::QueryInterface(REFIID iid, LPVOID *ppv)
{
    return E_NOINTERFACE;
}
*/

ULONG STDMETHODCALLTYPE RenderingTest::AddRef(void)
{
    refCountMutex.lock();
    ULONG myRefCount = refCount++;
    refCountMutex.unlock();
    
    return myRefCount;
};

ULONG STDMETHODCALLTYPE RenderingTest::Release(void)
{
    refCountMutex.lock();
    ULONG myRefCount = refCount--;
    refCountMutex.unlock();
    
    return myRefCount;
};

HRESULT STDMETHODCALLTYPE RenderingTest::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags)
{
    std::cout << "video input format changed\n";
    return S_OK;
};

HRESULT STDMETHODCALLTYPE RenderingTest::VideoInputFrameArrived(IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioPacket)
{
    // drop frame if previous call is still being processed
    if (!frameAcceptanceMutex.tryLock()) {
        // DEBUG: show that we are dropping frames
        std::cout << "d";
        
        return S_OK;
    }
    
    // DEBUG: just increment dummy color variable
    z = (z + 1) % 256;
    
    // ask Qt to repaint the widget optimized
    update();
    
    // we are ready for next frame
    frameAcceptanceMutex.unlock();
};

void RenderingTest::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    
    //painter.drawRect(QRect(0, 0, 200, 200));
    
    for (int y=0; y<256; y++) {
        for (int x=0; x<256; x++) {
            painter.setPen(QColor::fromRgb(x, y, z));
            painter.drawPoint(x, y);
        }
    }
}

void RenderingTest::acceptNewImage()
{
}