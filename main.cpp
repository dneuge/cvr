#include <QApplication>

#include "MainWindow.h"

#include "MatroskaEncoder.h"
#include "TimedPacket.h"

#include <time.h>

int main(int argc, char *argv[]) {
    /*
    // DEBUG: test encoder
    MatroskaEncoder encoder = MatroskaEncoder("test.out");
    encoder.writeFileHeader();
    
    unsigned long simulatedTimestamp = 12345000;
    srand(time(NULL));
    
    FILE *fh = fopen("testdata/video.raw", "r");
    std::vector<unsigned char> bytes;
    unsigned char prev = 0;
    while (!feof(fh)) {
        unsigned char current = (unsigned char) fgetc(fh);
        bytes.push_back(current);
        
        // FF D9 is "End of Image" mark in JPEG
        if (prev == 0xFF && current == 0xD9) {
            // assemble frame byte sequence
            unsigned char *frame = new unsigned char[bytes.size()];
            
            std::vector<unsigned char>::iterator it = bytes.begin();
            long i = 0;
            while (it != bytes.end()) {
                frame[i++] = (*it);
                it++;
            }
            
            // forward to encoder
            TimedPacket *timedPacket = new TimedPacket();
            timedPacket->timestamp = simulatedTimestamp;
            timedPacket->data = (char*) frame;
            timedPacket->dataLength = bytes.size();
            encoder.addVideoFrame(timedPacket);
            
            // advance simulated timestamp
            //simulatedTimestamp += (1000/24); // 24fps
            simulatedTimestamp += rand() % 100; // testing variable frame duration
            
            // free memory
            delete frame;
            delete timedPacket;
            
            // reset collector
            bytes.clear();
        }
        
        prev = current;
    }
    
    encoder.closeFile();
    */
    
    
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);
    
    QApplication app(argc, argv);

    MainWindow mainWindow(&app);
    mainWindow.show();

    return app.exec();
}
