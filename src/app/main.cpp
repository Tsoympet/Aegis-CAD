#include "AegisApp.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    AegisApp app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
