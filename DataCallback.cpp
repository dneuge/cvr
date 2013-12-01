#include "DataCallback.h"

// FIXME: that is very dirty but how else can we tell NetBeans to include that file? unfortunately it's not a lib
#include "../../Downloads/Blackmagic DeckLink SDK 9.7.7/Linux/include/DeckLinkAPIDispatch.cpp"

inline unsigned char clampRGB(int x)
{
    if (x < 0)
    {
        return 0;
    }
    
    if (x > 255)
    {
        return 255;
    }
    
    return x;
}

DataCallback::DataCallback(QImage** image, unsigned char *rawImage, unsigned long rawImageLength, QMutex* frameDrawMutex){
    this->frameDrawMutex = frameDrawMutex;
    this->image = image;
    this->rawImage = rawImage;
    this->rawImageLength = rawImageLength;
    
    skipFrames = false;
    lastFrameUsed = false;
    //halfFrameRate = true; // skip every second frame?
    halfFrameRate = false; // skip every second frame?
    outputToQImage = false;
    
    // pre-calculate YCrCb conversion table
    std::cout << "pre-calculating color space conversion table, wait";
    lookupTableYCrCbToQRgb = new QRgb**[256];
    for (unsigned int y = 0; y < 256; y++) {
        std::cout << ".";
        std::cout.flush();
        lookupTableYCrCbToQRgb[y] = new QRgb*[256];
        for (unsigned int cr0 = 0; cr0 < 256; cr0++) {
            lookupTableYCrCbToQRgb[y][cr0] = new QRgb[256];
            for (unsigned int cb0 = 0; cb0 < 256; cb0++) {
                char convCr0 = cr0 - 128;
                char convCb0 = cb0 - 128;
                
                unsigned char r = clampRGB(round((double) y + 1.402 * convCr0));
                unsigned char g = clampRGB(round((double) y - 0.344 * convCb0 - 0.714 * convCr0));
                unsigned char b = clampRGB(round((double) y + 1.772 * convCb0));
                
                lookupTableYCrCbToQRgb[y][cr0][cb0] = qRgb(r, g, b);
            }
        }
    }
    std::cout << " done\n";
    
    /*
    // open audio buffer
    audioBuffer.open(QIODevice::ReadWrite);
    
    // setup Qt audio playback
    QAudioFormat format;
    format.setFrequency(48000);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannels(2);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setCodec("audio/pcm");
    
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        std::cout << "unable to setup audio playback, unsupported format\n";
        return;
    }
    audioOutput = new QAudioOutput(format, this);
    */
    
    // initialize DeckLink API
    IDeckLink *deckLink;
    IDeckLinkInput *deckLinkInput;
    
    BMDDisplayMode displayMode = bmdModeHD720p5994;
    BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
    BMDVideoInputFlags inputFlags = 0;
    //BMDAudioSampleRate audioSampleRate = bmdAudioSampleRate48kHz;
    audioChannels = 2; // stereo
    audioSampleDepth = 16; // 16 bit
    
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
    
    // configure audio stream
    /*
    if (deckLinkInput->EnableAudioInput(audioSampleRate, audioSampleDepth, audioChannels) != S_OK)
    {
		std::cout << "could not configure audio stream\n";
        return;
    }
    */
    
    // start streaming
    if(deckLinkInput->StartStreams() != S_OK)
    {
		std::cout << "could not start video stream\n";
        return;
    }
        
    std::cout << "initialized\n";
    
    /*
    // start audio playback slightly delayed
    sleep(1);
    std::cout << "starting audio playback\n";
    audioOutput->start(&audioBuffer);
    */
}

/*
HRESULT STDMETHODCALLTYPE RenderingTest::QueryInterface(REFIID iid, LPVOID *ppv)
{
    return E_NOINTERFACE;
}
*/

ULONG STDMETHODCALLTYPE DataCallback::AddRef(void)
{
    refCountMutex.lock();
    ULONG myRefCount = refCount++;
    refCountMutex.unlock();
    
    return myRefCount;
};

ULONG STDMETHODCALLTYPE DataCallback::Release(void)
{
    refCountMutex.lock();
    ULONG myRefCount = refCount--;
    refCountMutex.unlock();
    
    return myRefCount;
};

HRESULT STDMETHODCALLTYPE DataCallback::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags)
{
    std::cout << "video input format changed\n";
    return S_OK;
};

inline unsigned char* onePixelYCrCbToRGB(char y, char cr, char cb)
{
    // http://en.wikipedia.org/wiki/YUV#Y.27UV444_to_RGB888_conversion
    // integer version
    unsigned char *rgb = new unsigned char[3];
    rgb[0] = y + cr + (cr >> 2) + (cr >> 3) + (cr >> 5);
    rgb[1] = y - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5));
    rgb[2] = y + cb + (cb >> 1) + (cb >> 2) + (cb >> 6);
    return rgb;
}

HRESULT STDMETHODCALLTYPE DataCallback::VideoInputFrameArrived(IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioPacket)
{
    Q_UNUSED(audioPacket);
    
    /*
    // audio frames should always be appended
    audioAcceptanceMutex.lock();
    char *audioBytes;
    if (audioPacket->GetBytes((void**) &audioBytes) != S_OK)
    {
        std::cout << "error getting audio packet data\n";
    }
    else
    {
        long audioLength = audioPacket->GetSampleFrameCount() * audioChannels * (audioSampleDepth / 8);
        audioBuffer.write(audioBytes, audioLength);
    }
    audioAcceptanceMutex.unlock();
    */
    
    // ignore every second frame if half frame rate is requested
    if (halfFrameRate && lastFrameUsed) {
        lastFrameUsed = false;
        return S_OK;
    }
    lastFrameUsed = true;
    
    // discard null (it may happen that we only got audio) or ignored video frames
    if ((videoFrame == 0) || skipFrames)
    {
        return S_OK;
    }
    
    // drop frame if previous call is still being processed
    if (!frameAcceptanceMutex.tryLock()) {
        // DEBUG: show that we are dropping frames
        std::cout << "d";
        
        return S_OK;
    }
    
    long width = videoFrame->GetWidth();
    long height = videoFrame->GetHeight();
    long rowBytes = videoFrame->GetRowBytes();
    BMDPixelFormat pixelFormat = videoFrame->GetPixelFormat();
    printf("%li x %li, row bytes %li, pixel format %i\n", width, height, rowBytes, pixelFormat);
    
    if (pixelFormat != bmdFormat8BitYUV)
    {
        std::cout << "unexpected pixel format, cannot decode\n";
    }
    else
    {
        QImage *newImage = new QImage(width, height, QImage::Format_RGB32);
        
        unsigned long bufferLength = rowBytes * videoFrame->GetHeight();
        unsigned char *imageBuffer = new unsigned char[bufferLength];
        
        if (videoFrame->GetBytes((void**) &imageBuffer) != S_OK)
        {
            std::cout << "failed to fill buffer\n";
            delete newImage;
            return S_OK;
        }
        else
        {
            if (outputToQImage) {
                // convert color space and set pixels on QImage
                long bufferPos = 0;
                long widthHalf = width/2;
                for (long y=0; y<height; y++)
                {
                    for (long xHalf=0; xHalf<widthHalf; xHalf++)
                    {
                        unsigned char cb0 = imageBuffer[bufferPos++];
                        unsigned char y0  = imageBuffer[bufferPos++];
                        unsigned char cr0 = imageBuffer[bufferPos++];
                        unsigned char y1  = imageBuffer[bufferPos++];

                        /*
                        char convCr0 = cr0 - 128;
                        char convCb0 = cb0 - 128;
                        */

                        // http://en.wikipedia.org/wiki/YUV#Y.27UV444_to_RGB888_conversion
                        /*
                        // integer version
                        unsigned char *rgb0 = onePixelYCrCbToRGB(y0, convCr0, convCb0);
                        unsigned char *rgb1 = onePixelYCrCbToRGB(y1, convCr0, convCb0);

                        image.setPixel(xHalf * 2,     y, qRgb(rgb0[0], rgb0[1], rgb0[2]));
                        image.setPixel(xHalf * 2 + 1, y, qRgb(rgb1[0], rgb1[1], rgb1[2]));

                        delete rgb0;
                        delete rgb1;
                        */

                        /*
                        int r0 = clampRGB(round((double) y0 + 1.402 * convCr0));
                        int g0 = clampRGB(round((double) y0 - 0.344 * convCb0 - 0.714 * convCr0));
                        int b0 = clampRGB(round((double) y0 + 1.772 * convCb0));

                        int r1 = clampRGB(round((double) y1 + 1.402 * convCr0));
                        int g1 = clampRGB(round((double) y1 - 0.344 * convCb0 - 0.714 * convCr0));
                        int b1 = clampRGB(round((double) y1 + 1.772 * convCb0));

                        image.setPixel(xHalf * 2,     y, qRgb(r0, g0, b0));
                        image.setPixel(xHalf * 2 + 1, y, qRgb(r1, g1, b1));
                        */

                        //newImage.setPixel(xHalf * 2,     y, qRgb(lookupTableYCrCbToR[y0][cr0][cb0], lookupTableYCrCbToG[y0][cr0][cb0], lookupTableYCrCbToB[y0][cr0][cb0]));
                        //newImage.setPixel(xHalf * 2 + 1, y, qRgb(lookupTableYCrCbToR[y1][cr0][cb0], lookupTableYCrCbToG[y1][cr0][cb0], lookupTableYCrCbToB[y1][cr0][cb0]));
                        newImage->setPixel(xHalf * 2,     y, lookupTableYCrCbToQRgb[y0][cr0][cb0]);
                        newImage->setPixel(xHalf * 2 + 1, y, lookupTableYCrCbToQRgb[y1][cr0][cb0]);
                    }
                }
            }
        }
        
        // switch image
        frameDrawMutex->lock();
        delete *image;
        *image = newImage;
        
        // copy buffer content to rawImage
        if (rawImageLength < bufferLength) {
            printf("buffer length %lu exceeds raw image length %lu, cannot copy raw image!\n", bufferLength, rawImageLength);
        } else {
            memcpy(rawImage, imageBuffer, bufferLength);
        }
        
        frameDrawMutex->unlock();
        
        // ask Qt to repaint the widget optimized
        emit imageUpdated();
    }
    
    // we are ready for next frame
    frameAcceptanceMutex.unlock();
    
    return S_OK;
};

void DataCallback::toggleCapture()
{
    frameAcceptanceMutex.lock();
    skipFrames = !skipFrames;
    frameAcceptanceMutex.unlock();
}
