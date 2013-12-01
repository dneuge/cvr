#include "PulseAudioSink.h"

PulseAudioSink::PulseAudioSink() {
    std::cout << "starting PA sink\n";
    
    pa_sample_spec sampleSpec;
    sampleSpec.channels = 2;
    sampleSpec.format = PA_SAMPLE_S16LE;
    sampleSpec.rate = 48000;
    
    paSimple = pa_simple_new(NULL, APP_NAME, PA_STREAM_PLAYBACK, NULL, STREAM_DESCRIPTION, &sampleSpec, NULL, NULL, NULL);
    if (!paSimple) {
        std::cout << "failed to setup PulseAudio simple\n";
        return;
    }
}

void PulseAudioSink::playAudio(char* rawAudio, unsigned long byteLength) {
    //printf("Audio: %lu bytes\n", byteLength); // DEBUG
    
    int error = 0;
    bool success = !pa_simple_write(paSimple, rawAudio, (size_t) byteLength, &error);
    if (!success) {
        printf("PulseAudio error code %i\n", error);
    }
    
    delete rawAudio;
}
