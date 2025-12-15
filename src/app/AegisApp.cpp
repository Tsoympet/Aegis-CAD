#include "AegisApp.h"
#include "../ui/ModernStyle.h"
#include <QFont>
#include <QIcon>

AegisApp::AegisApp(int &argc, char **argv) : QApplication(argc, argv) {
    setApplicationName("AegisCAD");
    setOrganizationName("AegisCAD");
    setApplicationDisplayName("AegisCAD");
    setWindowIcon(QIcon(":/icons/app_icon.svg"));
    setFont(QFont("Segoe UI Semibold", 10));

    m_style = std::make_unique<ModernStyle>();
    m_style->applyTo(*this);
    setupPalette();
}

AegisApp::~AegisApp() = default;

void AegisApp::setupPalette() {
    // Additional palette customization can be added here when theming new widgets.
    // ModernStyle already sets a Fusion dark palette.
}

