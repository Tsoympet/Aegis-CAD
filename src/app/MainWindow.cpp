#include "MainWindow.h"
#include "ui/RibbonBar.h"
#include "ui/OccView.h"
#include "ui/AegisAssistantDock.h"
#include "ui/PythonConsoleDock.h"
#include "ui/ReverseEngineerDock.h"
#include "utils/Logging.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QShortcut>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

extern bool g_safeMode;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_safeMode = g_safeMode;
    setWindowTitle(m_safeMode ? "AegisCAD (Safe Mode)" : "AegisCAD");
    resize(1440, 900);

    // === Layout Root ===
    QWidget* central = new QWidget(this);
    QVBoxLayout* vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);

    // === Ribbon ===
    auto* ribbon = new RibbonBar(this);
    vbox->addWidget(ribbon);

    // === OCC 3D Viewport ===
    m_view = new OccView(this);
    vbox->addWidget(m_view, 1);

    central->setLayout(vbox);
    setCentralWidget(central);

    // === Optional Docks ===
    if (!m_safeMode)
    {
        m_aiDock  = new AegisAssistantDock(this);
        m_pyDock  = new PythonConsoleDock(this);
        m_revDock = new ReverseEngineerDock(this);
    }

    createMenus();
    createStatusBar();
    createDocks();

    // === Connect Ribbon Signals ===
    connect(ribbon, &RibbonBar::actionTriggered, this, &MainWindow::handleAction);

    // === Keyboard Shortcuts ===
    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(newSketch()));
    new QShortcut(QKeySequence("Ctrl+E"), this, SLOT(extrudeBox()));
    new QShortcut(QKeySequence("Ctrl+R"), this, SLOT(extrudeCylinder()));
    new QShortcut(QKeySequence("Ctrl+Shift+A"), this, SLOT(runAnalysis()));

    if (m_safeMode)
    {
        Logging::info("MainWindow initialized in Safe Mode (AI + Reverse modules disabled)");
        statusBar()->showMessage("Running in Safe Mode — limited modules active", 5000);
    }
    else
        statusBar()->showMessage("Ready", 2000);
}

MainWindow::~MainWindow() = default;

// =========================
// Menus & Status
// =========================

void MainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu(tr("&File"));
    m_actionNew = file->addAction(tr("&New Sketch"), this, &MainWindow::newSketch);
    m_actionImport = file->addAction(tr("&Import CAD..."), this, &MainWindow::importCAD);
    m_actionExport = file->addAction(tr("&Export CAD..."), this, &MainWindow::exportCAD);
    file->addSeparator();
    file->addAction(tr("E&xit"), this, &MainWindow::close);

    QMenu* help = menuBar()->addMenu(tr("&Help"));
    help->addAction(tr("About"), [this] {
        QMessageBox::about(this, tr("About AegisCAD"),
            tr("AegisCAD 1.0 — Advanced CAD/FEA suite with integrated AI engine."));
    });
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel(this);
    m_statusLabel->setText(m_safeMode ? "Safe Mode Active" : "Ready");
    statusBar()->addPermanentWidget(m_statusLabel);
}

void MainWindow::createDocks()
{
    if (m_safeMode) return;

    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_pyDock);
    addDockWidget(Qt::RightDockWidgetArea, m_revDock);

    m_aiDock->hide();
    m_pyDock->hide();
    m_revDock->hide();
}

// =========================
// Core CAD Actions
// =========================

void MainWindow::newSketch()
{
    Logging::info("New Sketch created");
    m_statusLabel->setText("New sketch created");
}

void MainWindow::extrudeBox()
{
    Logging::info("Extrude Box executed");
    m_statusLabel->setText("Box extruded");
}

void MainWindow::extrudeCylinder()
{
    Logging::info("Extrude Cylinder executed");
    m_statusLabel->setText("Cylinder extruded");
}

// =========================
// Import / Export
// =========================

void MainWindow::importCAD()
{
    const QString path = QFileDialog::getOpenFileName(this, "Import CAD", QString(),
        "STEP Files (*.stp *.step);;IGES Files (*.igs *.iges)");
    if (path.isEmpty()) return;

    bool success = m_stepIO.importModel(path);
    if (success)
        m_statusLabel->setText("Imported " + QFileInfo(path).fileName());
    else
        QMessageBox::warning(this, "Import Failed", "Could not import CAD file.");
}

void MainWindow::exportCAD()
{
    const QString path = QFileDialog::getSaveFileName(this, "Export CAD", QString(),
        "GLTF Files (*.gltf);;STEP Files (*.stp)");
    if (path.isEmpty()) return;

    QString ext = QFileInfo(path).suffix().toLower();
    bool success = false;

    if (ext == "stp" || ext == "step")
        success = m_stepIO.exportModel(path);
    else if (ext == "gltf")
        success = m_gltfExporter.exportModel(path);

    if (success)
        m_statusLabel->setText("Exported " + QFileInfo(path).fileName());
    else
        QMessageBox::warning(this, "Export Failed", "Could not export file.");
}

// =========================
// Analysis / AI
// =========================

void MainWindow::runAnalysis()
{
    Logging::info("Analysis started");

    // Show contextual "Analysis" tab
    auto* ribbon = findChild<RibbonBar*>("RibbonBar");
    if (ribbon)
        ribbon->showContextTab("Analysis");

    QMessageBox::information(this, "Analysis", "Structural analysis completed (stub).");

    // Hide contextual tab after operation
    if (ribbon)
        ribbon->hideContextTab("Analysis");
}

void MainWindow::showAIDock()
{
    if (m_safeMode || !m_aiDock)
    {
        QMessageBox::information(this, "Safe Mode",
            "AI dock is disabled in Safe Mode for performance and stability.");
        return;
    }
    m_aiDock->show();
    m_aiDock->raise();
}

// =========================
// Central Action Dispatcher
// =========================

void MainWindow::handleAction(const QString& actionName)
{
    if (actionName == "newSketch") newSketch();
    else if (actionName == "drawLine") Logging::info("Draw line (stub)");
    else if (actionName == "drawCircle") Logging::info("Draw circle (stub)");
    else if (actionName == "extrudeFeature") extrudeBox();
    else if (actionName == "revolveFeature") extrudeCylinder();
    else if (actionName == "runAnalysis") runAnalysis();
    else if (actionName == "aiSuggest") showAIDock();
    else if (actionName == "aiAnalyze") runAnalysis();
    else if (actionName == "aiReverseEngineer") Logging::info("Reverse engineer (stub)");
    else Logging::warn("Unhandled ribbon action: " + actionName);
}

