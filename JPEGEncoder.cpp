#include "JPEGEncoder.h"

JPEGEncoder::JPEGEncoder() {
    // run in own thread
    moveToThread(&thread);
    thread.start();
}

/**
 * Encodes an image to JPEG.
 * @param originalImage pointer to original image data
 * @param originalImageLength size of original image data
 * @param newImage pointer to pointer to set for JPEG image
 * @return length of new image data
 */
unsigned long long JPEGEncoder::encodeImage(char* originalImage, unsigned long long originalImageLength, char** newImage) {
    return 0;
}
