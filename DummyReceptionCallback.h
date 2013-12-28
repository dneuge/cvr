#ifndef DUMMYRECEPTIONCALLBACK_H
#define	DUMMYRECEPTIONCALLBACK_H

#include "DelayedReceptionCallback.h"

class DummyReceptionCallback : public DelayedReceptionCallback {
    public:
        DummyReceptionCallback();
        virtual void dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame);
        
    private:
        unsigned long long millisLastAudioPacket;
        unsigned long long millisLastVideoFrame;
};


#endif	/* DUMMYRECEPTIONCALLBACK_H */

