#include "AegisApp.h"
#include "MainWindow.h"
#include <QStyleFactory>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QDir>
#include <QDebug>
#include <QStringList>

// Optional OpenCascade includes
#include <Standard_Version.hxx>
#include <OSD_Environment.hxx>

static bool g_safeMode = false;

AegisApp::AegisApp(int& argc, char** argv)
    : m_app(argc, argv)
{
    m_app.setApplicationName("AegisCAD");
    m_app.setApplicationVersion("1.0");
    m_app.setOrganizationName("Aegis Dynamics");

    // Detect Safe Mode argument
    QStringList args = m_app.arguments();
    g_safeMode = args.contains("--safe-mode");
    if (g_safeMode)
        qInfo() << "[SAFE MODE] AegisCAD started with minimal modules.";

    initStyle();
    if (!g_safeMode)
        initOpenCascade();
}

AegisApp::~AegisApp() = default;

void AegisApp::initStyle()
{
    m_app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::Text, Qt::white);
    m_app.setPalette(darkPalette);
}

void AegisApp::initOpenCascade()
{
    qInfo() << "Initializing OpenCascade version:" << OCC_VERSION_COMPLETE;
    OSD_Environment env;
    env.Set("CSF_LANGUAGE", "ENGLISH");
}

int AegisApp::run()
{
    if (!g_safeMode) {
        QPixmap splashImg(":/icons/app_icon.png");
        QSplashScreen splash(splashImg);
        splash.showMessage("Initializing AegisCAD...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
        splash.show();
        m_app.processEvents();

        m_mainWindow = std::make_unique<MainWindow>();
        m_mainWindow->show();

        QTimer::singleShot(1500, &splash, &QSplashScreen::close);
    } else {
        // Direct launch, no splash or heavy modules
        m_mainWindow = std::make_unique<MainWindow>();
        m_mainWindow->setWindowTitle("AegisCAD (Safe Mode)");
        m_mainWindow->show();
    }

    return m_app.exec();
}
