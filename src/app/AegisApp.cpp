#include "AegisApp.h"
#include <QLocale>
#include <QDebug>

AegisApp::AegisApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("AegisCAD");
    setOrganizationName("AegisCAD Project");
    setApplicationVersion("1.0.0");
    loadTranslations();
}

void AegisApp::loadTranslations()
{
    const QString locale = QLocale::system().name();
    if (m_translator.load("aegiscad_" + locale, ":/translations/")) {
        installTranslator(&m_translator);
        qInfo() << "Loaded translation for" << locale;
    }
}
