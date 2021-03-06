#include "DataCallback.h"

#include <time.h>
#include <math.h>

#include "TimedPacket.h"

// TODO: figure out if there's a better way to include that file on compilation - it's not a lib
// needs to stay prefixed with link/ or building via CMake will not find
// the file due to different include search path within build dir
#include "link/include_DeckLinkAPIDispatch.cpp"

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
    audioPacketCounter = 0;
    videoFrameCounter = 0;
    
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
    if (!outputToQImage) {
        std::cout << "skipping setup of color space conversion table because output to QImage is disabled\n";
    } else {
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
    }
    
    // initialize DeckLink API
    BMDDisplayMode displayMode = bmdModeHD720p5994;
    BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
    BMDVideoInputFlags inputFlags = 0;
    //BMDAudioSampleRate audioSampleRate = bmdAudioSampleRate48kHz;
    audioChannels = 2; // stereo
    audioSampleDepth = 16; // 16 bit
    audioSampleRate = 48000; // 48kHz
    
    // get iterator
    deckLinkIterator = CreateDeckLinkIteratorInstance();
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
    
    /*
    // try to disable any previous capture
    if (deckLinkInput->DisableVideoInput() != S_OK) {
        std::cout << "pre: could not disable video input\n";
    }
    if (deckLinkInput->DisableAudioInput() != S_OK) {
        std::cout << "pre: could not disable audio input\n";
    }
    if (deckLinkInput->FlushStreams() != S_OK) {
        std::cout << "pre: could not flush streams before stop\n";
    }
    if (deckLinkInput->StopStreams() != S_OK) {
        std::cout << "pre: could not stop streams\n";
    }
    if (deckLinkInput->FlushStreams() != S_OK) {
        std::cout << "pre: could not flush streams after stop\n";
    }
    */
    
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
    if (deckLinkInput->EnableAudioInput(audioSampleRate, audioSampleDepth, audioChannels) != S_OK)
    {
		std::cout << "could not configure audio stream\n";
        return;
    }
    
    // start streaming
    if(deckLinkInput->StartStreams() != S_OK)
    {
		std::cout << "could not start capture streams\n";
        return;
    }
    
    // register shutdown callback to release DeckLink resources
    // NOTE: that doesn't work and isn't adivsable as this is an object that
    //       may cease to exist before the method can be called; we need some
    //       shutdown hook setup by main() to call destructors instead
    //atexit(&DataCallback::shutdown);
    
    std::cout << "initialized\n";
}

void DataCallback::shutdown() {
    // I've seen crashes in the kernel module most likely related to unclean
    // termination of the application. We try to fix that by releasing all
    // interface resources.
    // FIXME: currently unused
    deckLinkIterator->Release();
    deckLinkInput->Release();
    deckLink->Release();
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
    // remember what time the data arrived
    // CLOCK_MONOTONIC should give a clock related to system runtime, so it
    // should be safe in case of clock adjustments during a recording
    timespec timeOfReception;
    clock_gettime(CLOCK_MONOTONIC, &timeOfReception);
    
    // skip? (capturing disabled)
    if (skipFrames) {
        return S_OK;
    }
    
    // remember AV packet indices and increment
    packetCounterMutex.lock();
    
    unsigned long long currentAudioPacketIndex = audioPacketCounter;
    unsigned long long currentVideoFrameIndex = videoFrameCounter;
    if (audioPacket != 0) {
        audioPacketCounter++;
    }
    if (videoFrame != 0) {
        videoFrameCounter++;
    }
    
    packetCounterMutex.unlock();
    
    // process audio packet
    char *audioBuffer;
    unsigned long audioLength = 0;
    if (audioPacket != 0) {
        if (audioPacket->GetBytes((void**) &audioBuffer) != S_OK)
        {
            std::cout << "error getting audio packet data\n";
        }
        else
        {
            audioLength = audioPacket->GetSampleFrameCount() * audioChannels * (audioSampleDepth / 8);
            if (audioLength > 0) {
                // copy buffer to variable and feed connected Qt slots
                char* rawAudio = new char[audioLength];
                memcpy(rawAudio, audioBuffer, audioLength);

                emit audioArrived(rawAudio, audioLength);
            }
        }
    }
    
    // ignore every second frame if half frame rate is requested
    if (halfFrameRate && lastFrameUsed) {
        lastFrameUsed = false;
        return S_OK;
    }
    lastFrameUsed = true;
    
    // it may happen that we only got audio, only process video frame if we got data
    unsigned char *imageBuffer;
    unsigned long imageBufferLength = 0;
    if (videoFrame != 0) {
        bool videoFrameContinue = true;
        
        long width = videoFrame->GetWidth();
        long height = videoFrame->GetHeight();
        long rowBytes = videoFrame->GetRowBytes();
        BMDPixelFormat pixelFormat = videoFrame->GetPixelFormat();
        //printf("%li x %li, row bytes %li, pixel format %i\n", width, height, rowBytes, pixelFormat); // DEBUG

        QImage *newImage;
        
        // drop frame if previous call is still being processed
        if (!frameAcceptanceMutex.tryLock()) {
            // DEBUG: show that we are dropping frames
            std::cout << "d";

            videoFrameContinue = false;
        }
        
        if (videoFrameContinue && (pixelFormat != bmdFormat8BitYUV)) {
            std::cout << "unexpected pixel format, cannot decode\n";
            videoFrameContinue = false;
        }
        
        if (videoFrameContinue) {
            if (outputToQImage) {
                newImage = new QImage(width, height, QImage::Format_RGB32);
            }
            
            imageBufferLength = rowBytes * videoFrame->GetHeight();

            if (videoFrame->GetBytes((void**) &imageBuffer) != S_OK)
            {
                std::cout << "failed to fill buffer\n";
                if (outputToQImage) {
                    delete newImage;
                }
                videoFrameContinue = false;
            }
        }
            
        if (videoFrameContinue) 
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

            // switch image
            frameDrawMutex->lock();

            if (outputToQImage) {
                delete *image;
                *image = newImage;
            }

            // copy buffer content to rawImage
            if (rawImageLength < imageBufferLength) {
                printf("buffer length %lu exceeds raw image length %lu, cannot copy raw image!\n", imageBufferLength, rawImageLength);
            } else {
                memcpy(rawImage, imageBuffer, imageBufferLength);
            }

            frameDrawMutex->unlock();

            // ask Qt to repaint the widget optimized
            emit imageUpdated();
        }
        
        // we are ready for next frame
        frameAcceptanceMutex.unlock();
    }
    
    // forward data to all delayed listeners
    delayedReceptionCallbacksMutex.lock();
    if (!delayedReceptionCallbacks.empty()) {
        unsigned long long timeOfReceptionMillis = timeOfReception.tv_sec * 1000 + lround((double) (timeOfReception.tv_nsec / 100000) / 10.0);
        
        std::vector<DelayedReceptionCallback*>::iterator it = delayedReceptionCallbacks.begin();
        while (it != delayedReceptionCallbacks.end()) {
            DelayedReceptionCallback *callback = (*it);
            
            // copy data and wrap in timestamped and indexed packets
            TimedPacket *timedAudioPacket = 0;
            if (audioLength != 0) {
                char *tmp = new char[audioLength];
                memcpy(tmp, audioBuffer, audioLength);
                timedAudioPacket = new TimedPacket(currentAudioPacketIndex, timeOfReceptionMillis, tmp, audioLength);
            }
            
            TimedPacket *timedVideoFrame = 0;
            if (imageBufferLength != 0) {
                char *tmp = new char[imageBufferLength];
                memcpy(tmp, imageBuffer, imageBufferLength);
                timedVideoFrame = new TimedPacket(currentVideoFrameIndex, timeOfReceptionMillis, tmp, imageBufferLength);
            }
            
            // forward packets
            callback->dataReceived(timedAudioPacket, timedVideoFrame);
            
            it++;
        }
    }
    delayedReceptionCallbacksMutex.unlock();
    
    return S_OK;
};

void DataCallback::toggleCapture()
{
    frameAcceptanceMutex.lock();
    skipFrames = !skipFrames;
    frameAcceptanceMutex.unlock();
}

void DataCallback::registerDelayedReceptionCallback(DelayedReceptionCallback *callback) {
    if (callback != 0) {
        delayedReceptionCallbacksMutex.lock();
        delayedReceptionCallbacks.push_back(callback);
        delayedReceptionCallbacksMutex.unlock();
    }
}
