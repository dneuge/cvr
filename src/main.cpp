#include <QApplication>

#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);
    
    QApplication app(argc, argv);

    MainWindow mainWindow(&app);
    mainWindow.show();

    return app.exec();
}
