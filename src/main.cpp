#include <QApplication>
#include "Viewer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Viewer viewer;
    viewer.resize(800, 600);
    viewer.setWindowTitle("3D Model Viewer");
    viewer.show();

    return app.exec();
}
