#include "MainWindow.h"
#include "ui/OccView.h"
#include "ui/AegisAssistantDock.h"
#include "ui/PythonConsoleDock.h"
#include "ui/ReverseEngineerDock.h"
#include "ui/AnalysisDock.h"
#include "utils/Logging.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QIcon>
#include <QActionGroup>
#include <QSpacerItem>
#include <QStyle>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("AegisCAD");
    resize(1400, 900);

    // ===== Core 3D Viewport =====
    m_view = new OccView(this);
    setCentralWidget(m_view);

    // ===== Docks =====
    m_aiDock   = new AegisAssistantDock(this);
    m_pyDock   = new PythonConsoleDock(this);
    m_revDock  = new ReverseEngineerDock(this);
    m_analysisDock = new AnalysisDock(this);

    createMenus();
    createToolBar();
    createDocks();
    createStatusBar();

    // ===== Signal Connections =====
    connect(m_analysisDock, &AnalysisDock::analysisReady,
            m_view, &OccView::showAnalysisResults);
}

MainWindow::~MainWindow() = default;

// =============================================================
//                       UI Construction
// =============================================================

void MainWindow::createMenus()
{
    // --- File menu ---
    QMenu* file = menuBar()->addMenu("&File");
    m_actionNew    = file->addAction(QIcon(":/icons/toolbar_new.svg"), "&New Sketch", this, &MainWindow::newSketch);
    m_actionImport = file->addAction(QIcon(":/icons/toolbar_import.svg"), "&Import CAD...", this, &MainWindow::importCAD);
    m_actionExport = file->addAction(QIcon(":/icons/toolbar_export.svg"), "&Export CAD...", this, &MainWindow::exportCAD);
    file->addSeparator();
    file->addAction("&Exit", this, &MainWindow::close);

    // --- Tools menu ---
    QMenu* tools = menuBar()->addMenu("&Tools");
    m_actionAnalyze = tools->addAction(QIcon(":/icons/toolbar_analysis.svg"), "&Run Analysis", this, &MainWindow::runAnalysis);
    m_actionAI      = tools->addAction(QIcon(":/icons/toolbar_ai.svg"), "Open &Aegis AI", this, &MainWindow::showAIDock);
    QAction* actionPython = tools->addAction(QIcon(":/icons/toolbar_python.svg"), "Open &Python Console", [this] {
        m_pyDock->show(); m_pyDock->raise();
    });

    // --- Help menu ---
    QMenu* help = menuBar()->addMenu("&Help");
    help->addAction("About", [this] {
        QMessageBox::about(this, "About AegisCAD",
            "AegisCAD 1.0 — Integrated CAD, FEA, and AI platform.\n"
            "© 2025 Aegis Engineering Systems.");
    });
}

void MainWindow::createToolBar()
{
    QToolBar* tb = addToolBar("Main Toolbar");
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));
    tb->setStyleSheet("QToolBar { background: #2a2a2a; border: none; }");

    auto addActionStyled = [&](QAction* act, const QString& tip) {
        act->setToolTip(tip);
        tb->addAction(act);
        tb->widgetForAction(act)->setStyleSheet(
            "QToolButton { border: none; padding: 4px; } "
            "QToolButton:hover { background-color: #3a3a3a; }");
    };

    addActionStyled(m_actionNew, "Start a new sketch");
    addActionStyled(m_actionImport, "Import STEP/IGES file");
    addActionStyled(m_actionExport, "Export GLTF/STEP");
    tb->addSeparator();
    addActionStyled(m_actionAnalyze, "Run analysis");
    tb->addSeparator();
    addActionStyled(m_actionAI, "Open Aegis AI Assistant");

    tb->addSeparator();
    QAction* actConsole = tb->addAction(QIcon(":/icons/toolbar_python.svg"), "");
    actConsole->setToolTip("Toggle Python Console");
    connect(actConsole, &QAction::triggered, [this] {
        if (m_pyDock->isHidden()) { m_pyDock->show(); m_pyDock->raise(); }
        else m_pyDock->hide();
    });
}

void MainWindow::createDocks()
{
    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_pyDock);
    addDockWidget(Qt::RightDockWidgetArea, m_revDock);
    addDockWidget(Qt::RightDockWidgetArea, m_analysisDock);

    m_aiDock->hide();
    m_pyDock->hide();
    m_revDock->hide();
    m_analysisDock->hide();
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->setStyleSheet("QStatusBar { background: #202020; color: #aaa; }");
}

// =============================================================
//                        Core Operations
// =============================================================

void MainWindow::newSketch()
{
    Logging::info("Creating new sketch...");
    m_statusLabel->setText("New sketch created");
}

void MainWindow::importCAD()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Import CAD", QString(), "STEP/IGES Files (*.stp *.step *.igs *.iges)");
    if (path.isEmpty()) return;

    if (m_stepIO.importShape(path)) {
        Logging::info("Imported CAD: " + path);
        m_statusLabel->setText("Imported " + QFileInfo(path).fileName());
    } else {
        QMessageBox::warning(this, "Import", "Failed to import the selected CAD file.");
    }
}

void MainWindow::exportCAD()
{
    const QString path = QFileDialog::getSaveFileName(
        this, "Export CAD", QString(), "GLTF (*.gltf);;STEP (*.stp *.step)");
    if (path.isEmpty()) return;

    bool ok = false;
    if (path.endsWith(".gltf"))
        ok = m_gltfExporter.exportScene(path);
    else
        ok = m_stepIO.exportShape(path);

    m_statusLabel->setText(ok ? "Exported " + QFileInfo(path).fileName()
                              : "Export failed");
}

void MainWindow::runAnalysis()
{
    if (m_analysisDock->isHidden()) {
        m_analysisDock->show();
        m_analysisDock->raise();
    }
}

void MainWindow::showAIDock()
{
    m_aiDock->show();
    m_aiDock->raise();
}
