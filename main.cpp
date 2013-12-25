#include <QApplication>

#include "MainWindow.h"

#include "MatroskaEncoder.h"

int main(int argc, char *argv[]) {
    /*
    // DEBUG: test encoder
    MatroskaEncoder encoder = MatroskaEncoder("test.out");
    encoder.writeFileHeader();
    encoder.closeFile();
    */
    
    
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);
    
    QApplication app(argc, argv);

    MainWindow mainWindow(&app);
    mainWindow.show();

    return app.exec();
}
