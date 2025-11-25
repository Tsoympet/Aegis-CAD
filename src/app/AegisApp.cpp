#include "AegisApp.h"
#include "MainWindow.h"
#include "ui/modern/ModernStyle.h"

#include <QStyleFactory>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QDir>
#include <QDebug>
#include <QLinearGradient>
#include <QPainter>
#include <QFont>
#include <QApplication>

// OpenCascade headers
#include <Standard_Version.hxx>
#include <OSD_Environment.hxx>

static bool g_safeMode = false;

AegisApp::AegisApp(int& argc, char** argv)
    : m_app(argc, argv)
{
    m_app.setApplicationName("AegisCAD");
    m_app.setApplicationVersion("1.1");
    m_app.setOrganizationName("Aegis Dynamics");

    QStringList args = m_app.arguments();
    g_safeMode = args.contains("--safe-mode");

    // Apply the modern Fusion/ SolidWorks-inspired style
    ModernStyle::apply(m_app);

    if (g_safeMode)
        qInfo() << "[SAFE MODE] AegisCAD started with minimal modules.";
    else
        initOpenCascade();
}

AegisApp::~AegisApp() = default;

// -------------------------------------
// OCC initialization
// -------------------------------------
void AegisApp::initOpenCascade()
{
    qInfo() << "Initializing OpenCascade version:" << OCC_VERSION_COMPLETE;
    OSD_Environment env;
    env.Set("CSF_LANGUAGE", "ENGLISH");
}

// -------------------------------------
// Run main window + splash screen
// -------------------------------------
int AegisApp::run()
{
    QPixmap splashImg(800, 400);
    splashImg.fill(Qt::transparent);

    // Custom gradient splash
    QLinearGradient grad(0, 0, 0, splashImg.height());
    grad.setColorAt(0.0, QColor(30, 30, 35));
    grad.setColorAt(1.0, QColor(65, 65, 70));

    QPainter p(&splashImg);
    p.fillRect(splashImg.rect(), grad);
    QPixmap logo(":/icons/app_icon.png");
    QSize logoSize(128, 128);
    QPoint logoPos((splashImg.width() - logoSize.width()) / 2, 80);
    p.drawPixmap(logoPos, logo.scaled(logoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFont font("Segoe UI", 20, QFont::Bold);
    p.setFont(font);
    p.setPen(Qt::white);
    p.drawText(QRect(0, 250, splashImg.width(), 50), Qt::AlignCenter, "AegisCAD");
    p.setFont(QFont("Segoe UI", 10));
    p.drawText(QRect(0, 300, splashImg.width(), 40), Qt::AlignCenter, "Modern CAD + AI Suite");
    p.end();

    QSplashScreen splash(splashImg);
    splash.show();
    splash.showMessage("Initializing AegisCAD...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    m_app.processEvents();

    // Create main window
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->show();

    QTimer::singleShot(2000, &splash, &QSplashScreen::close);

    if (g_safeMode)
        m_mainWindow->setWindowTitle("AegisCAD (Safe Mode)");

    return m_app.exec();
}
