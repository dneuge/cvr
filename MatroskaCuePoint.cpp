#include "MatroskaCuePoint.h"

MatroskaCuePoint::MatroskaCuePoint(unsigned long long timeMillis, unsigned long long clusterOffsetVideoTrack, unsigned long long clusterOffsetAudioTrack) {
    this->timeMillis = timeMillis;
    this->clusterOffsetAudioTrack = clusterOffsetAudioTrack;
    this->clusterOffsetVideoTrack = clusterOffsetVideoTrack;
}
