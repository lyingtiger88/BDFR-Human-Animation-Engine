#include "StudioMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("BDFR Studio");
    QApplication::setOrganizationName("BDFR");
    StudioMainWindow window;
    window.show();
    return app.exec();
}
