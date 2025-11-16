#include "AegisApp.h"

AegisApp::AegisApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("AegisCAD");
    setOrganizationName("AegisCAD Project");
}
