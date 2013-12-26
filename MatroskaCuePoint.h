#ifndef MATROSKACUEPOINT_H
#define	MATROSKACUEPOINT_H

class MatroskaCuePoint {
    public:
        MatroskaCuePoint(unsigned long long, unsigned long long, unsigned long long);
        unsigned long long timeMillis;
        unsigned long long clusterOffsetVideoTrack;
        unsigned long long clusterOffsetAudioTrack;
};

#endif	/* MATROSKACUEPOINT_H */

