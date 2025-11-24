#include "MainWindow.h"
#include "ui/OccView.h"
#include "ui/AegisAssistantDock.h"
#include "ui/PythonConsoleDock.h"
#include "ui/ReverseEngineerDock.h"
#include "utils/Logging.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QLabel>

extern bool g_safeMode;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(g_safeMode ? "AegisCAD (Safe Mode)" : "AegisCAD");
    resize(1280, 800);

    // --- Core Viewer ---
    m_view = new OccView(this);
    setCentralWidget(m_view);

    // --- Optional Docks ---
    if (!g_safeMode)
    {
        m_aiDock  = new AegisAssistantDock(this);
        m_pyDock  = new PythonConsoleDock(this);
        m_revDock = new ReverseEngineerDock(this);
    }

    createMenus();
    createToolBar();
    createDocks();
    createStatusBar();

    if (g_safeMode)
    {
        Logging::info("MainWindow initialized in Safe Mode (AI + Reverse modules disabled)");
        statusBar()->showMessage("Running in Safe Mode — limited modules active", 5000);
    }
    else
    {
        statusBar()->showMessage("Ready", 2000);
    }
}

MainWindow::~MainWindow() = default;

// --- UI Construction ---

void MainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu(tr("&File"));
    m_actionNew = file->addAction(tr("&New Sketch"), this, &MainWindow::newSketch);
    m_actionImport = file->addAction(tr("&Import STEP..."), this, &MainWindow::importSTEP);
    m_actionExport = file->addAction(tr("&Export GLTF..."), this, &MainWindow::exportGLTF);
    file->addSeparator();
    file->addAction(tr("E&xit"), this, &MainWindow::close);

    QMenu* tools = menuBar()->addMenu(tr("&Tools"));
    m_actionAnalyze = tools->addAction(tr("&Run Analysis"), this, &MainWindow::runAnalysis);

    if (!g_safeMode)
        m_actionAI = tools->addAction(tr("Open &Aegis AI"), this, &MainWindow::showAIDock);

    QMenu* help = menuBar()->addMenu(tr("&Help"));
    help->addAction(tr("About"), [this] {
        QMessageBox::about(this, tr("About AegisCAD"),
                           tr("AegisCAD 1.0 – advanced CAD/FEA suite with integrated AI engine."));
    });
}

void MainWindow::createToolBar()
{
    auto* tb = addToolBar(tr("Main"));
    tb->addAction(m_actionNew);
    tb->addAction(m_actionImport);
    tb->addAction(m_actionExport);
    tb->addSeparator();
    tb->addAction(m_actionAnalyze);
    if (!g_safeMode)
    {
        tb->addSeparator();
        tb->addAction(m_actionAI);
    }
}

void MainWindow::createDocks()
{
    if (g_safeMode)
        return;

    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_pyDock);
    addDockWidget(Qt::RightDockWidgetArea, m_revDock);

    m_aiDock->hide();
    m_pyDock->hide();
    m_revDock->hide();
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel(this);
    m_statusLabel->setText(g_safeMode ? "Safe Mode Active" : "Ready");
    statusBar()->addPermanentWidget(m_statusLabel);
}

// --- Actions ---

void MainWindow::newSketch()
{
    Logging::info("Creating new sketch...");
    m_statusLabel->setText("New sketch created");
}

void MainWindow::importSTEP()
{
    const QString path = QFileDialog::getOpenFileName(this, "Import STEP", QString(), "*.stp *.step");
    if (path.isEmpty()) return;
    Logging::info("Import STEP: " + path);
    m_statusLabel->setText("Imported " + QFileInfo(path).fileName());
}

void MainWindow::exportGLTF()
{
    const QString path = QFileDialog::getSaveFileName(this, "Export GLTF", QString(), "*.gltf");
    if (path.isEmpty()) return;
    Logging::info("Export GLTF: " + path);
    m_statusLabel->setText("Exported GLTF");
}

void MainWindow::runAnalysis()
{
    Logging::info("Running analysis...");
    QMessageBox::information(this, "Analysis", "Structural analysis completed (stub).");
}

void MainWindow::showAIDock()
{
    if (!m_aiDock) return;
    m_aiDock->show();
    m_aiDock->raise();
}
