#include "AegisApp.h"
#include "MainWindow.h"
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    AegisApp app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

