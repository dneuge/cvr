#ifndef DELAYEDRECEPTIONCALLBACK_H
#define	DELAYEDRECEPTIONCALLBACK_H

#include "TimedPacket.h"

class DelayedReceptionCallback {
    public:
        virtual void dataReceived(TimedPacket* audioPacket, TimedPacket* videoFrame) = 0;
};

#endif	/* DELAYEDRECEPTIONCALLBACK_H */

