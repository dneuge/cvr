#include "JPEGEncoder.h"

#include <stdio.h>
#include <iostream>

#include <qt4/QtCore/qmutex.h>
#include <qt4/QtCore/qwaitcondition.h>

#include <jpeglib.h>

JPEGEncoder::JPEGEncoder(EncodingRingBuffer *inputQueue, EncodingRingBuffer *outputQueue) {
    this->inputQueue = inputQueue;
    this->outputQueue = outputQueue;
    
    // run in own thread
    moveToThread(&thread);
    thread.start();
    
    // tell thread to start working
    connect(this, SIGNAL(startEncoding()), this, SLOT(encodeBetweenQueues()));
    emit startEncoding();
}

void JPEGEncoder::msleep(unsigned long millis) {
    QMutex sleepMutex;
    sleepMutex.lock();
    
    QWaitCondition wc;
    wc.wait(&sleepMutex, millis);
    
    sleepMutex.unlock();
}


void JPEGEncoder::encodeBetweenQueues() {
    while (true) {
        TimedPacket* originalPacket = inputQueue->popFirst();
        
        // wait a moment if no work was available, then try again
        if (originalPacket == 0) {
            msleep(INACTIVITY_SLEEP_MILLIS);
            continue;
        }
        
        //printf("encoding frame %lld\n", originalPacket->index); // DEBUG
        
        // encode frame
        char* encodedImage;
        unsigned long long encodedImageLength = encodeImage(originalPacket->data, originalPacket->dataLength, &encodedImage);
        
        // add to output queue if successful
        if (encodedImageLength == 0) {
            std::cerr << "JPEG encoding failed, dropping video frame\n";
        } else {
            TimedPacket *encodedPacket = new TimedPacket(originalPacket->index, originalPacket->timestampMillis, encodedImage, encodedImageLength);
            if (!outputQueue->addPacket(encodedPacket)) {
                std::cerr << "adding encoded JPEG to output queue failed, dropping video frame\n";
                
                // free memory on error
                delete encodedPacket->data;
                delete encodedPacket;
            }
        }
        
        // free original packet
        delete originalPacket->data;
        delete originalPacket;
    }
}

/**
 * Encodes an image to JPEG.
 * @param originalImage pointer to original image data
 * @param originalImageLength size of original image data
 * @param newImage pointer to pointer to set for JPEG image
 * @return length of new image data
 */
unsigned long JPEGEncoder::encodeImage(char* originalImage, unsigned long long originalImageLength, char** newImage) {
    // NOTE: this code follows the commented example from http://svn.code.sf.net/p/libjpeg-turbo/code/trunk/example.c
    
    struct jpeg_compress_struct cinfo;
    
    // register default error handler
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    
    // initialize info structure
    jpeg_create_compress(&cinfo);
    
    // setup encoding destination
    unsigned long outLength = 0;
    jpeg_mem_dest(&cinfo, (unsigned char**) newImage, &outLength);
    
    // set options
    cinfo.image_width = 1280 / 2;
    cinfo.image_height = 720;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_UNKNOWN; // raw image is UYVY, we try to encode that without conversion
    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, JPEG_QUALITY, true);
    
    // feed lines to compress
    jpeg_start_compress(&cinfo, true);
    
    JSAMPROW rowPointer[1];
    long bytesPerRow = cinfo.input_components * cinfo.image_width;
    
    void *lastRow = originalImage + originalImageLength - (cinfo.image_width * cinfo.input_components);
    void *linePointer = originalImage;
    rowPointer[0] = (unsigned char*) linePointer;
    while (rowPointer[0] <= lastRow) {
        jpeg_write_scanlines(&cinfo, rowPointer, 1);
        
        linePointer += bytesPerRow;
        rowPointer[0] = (unsigned char*) linePointer;
    }
    
    jpeg_finish_compress(&cinfo);
    
    // clean up
    jpeg_destroy_compress(&cinfo);
    
    return outLength;
}
