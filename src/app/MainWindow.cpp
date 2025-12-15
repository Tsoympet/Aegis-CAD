#include "MainWindow.h"
#include "ProjectIO.h"
#include "../ui/OccView.h"
#include "../ui/AnalysisLegendOverlay.h"
#include "../ui/AegisAssistantDock.h"
#include "../ui/ReverseEngineerDock.h"
#include "../ui/PythonConsoleDock.h"
#include "../cad/StepIgesIO.h"
#include "../cad/GltfExporter.h"
#include "../cad/PartRegistry.h"
#include "../analysis/AnalysisManager.h"
#include "../ai/AegisAIEngine.h"
#include "../utils/Logging.h"

#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QDockWidget>
#include <QShortcut>
#include <QAction>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_analysis(std::make_unique<AnalysisManager>()),
      m_aiEngine(std::make_unique<AegisAIEngine>()),
      m_partRegistry(std::make_unique<PartRegistry>()),
      m_io(std::make_unique<StepIgesIO>()),
      m_gltf(std::make_unique<GltfExporter>()),
      m_projectIO(std::make_unique<ProjectIO>()) {
    setupUi();
    setupToolbar();
    setupDocks();
    loadSamplePart();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    resize(1280, 800);
    setWindowTitle(tr("AegisCAD"));

    m_view = new OccView(this);
    setCentralWidget(m_view);

    m_legend = new AnalysisLegendOverlay(m_view);
    m_legend->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_legend->hide();

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupDocks() {
    m_aiDock = new AegisAssistantDock(tr("AI Assistant"), this);
    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
    connect(m_aiDock, &AegisAssistantDock::querySubmitted, this, &MainWindow::evaluateAIAssistant);

    m_reverseDock = new ReverseEngineerDock(tr("Reverse Engineer"), this);
    addDockWidget(Qt::LeftDockWidgetArea, m_reverseDock);
    connect(m_reverseDock, &ReverseEngineerDock::generateShapeRequested, this, &MainWindow::regenerateFromReverse);

    m_pythonDock = new PythonConsoleDock(tr("Python Console"), this);
    addDockWidget(Qt::BottomDockWidgetArea, m_pythonDock);

    // Quick hide/show shortcuts
    new QShortcut(QKeySequence("Ctrl+`"), this, [this]() { m_pythonDock->setVisible(!m_pythonDock->isVisible()); });
}

void MainWindow::setupToolbar() {
    auto *toolbar = addToolBar(tr("Aegis"));
    toolbar->setMovable(false);

    QAction *importStep = toolbar->addAction(QIcon(":/icons/toolbar_import.svg"), tr("Import STEP"), this, &MainWindow::openStepFile);
    importStep->setShortcut(QKeySequence::Open);

    QAction *exportStep = toolbar->addAction(QIcon(":/icons/toolbar_export.svg"), tr("Export STEP"), this, &MainWindow::exportStepFile);
    QAction *exportGltf = toolbar->addAction(QIcon(":/icons/toolbar_analysis.svg"), tr("Export glTF"), this, &MainWindow::exportGltfFile);

    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/toolbar_analysis.svg"), tr("Run Analysis"), this, &MainWindow::runAnalysis);
    toolbar->addAction(QIcon(":/icons/toolbar_ai.svg"), tr("AI"), this, [this]() {
        evaluateAIAssistant(tr("Summarize model"));
    });
    toolbar->addAction(QIcon(":/icons/toolbar_python.svg"), tr("Run Script"), m_pythonDock, &PythonConsoleDock::runBuffer);
    toolbar->addAction(QIcon(":/icons/toolbar_reverse.svg"), tr("Reverse"), m_reverseDock, &ReverseEngineerDock::triggerGenerate);
}

void MainWindow::openStepFile() {
    const QString file = QFileDialog::getOpenFileName(this, tr("Open STEP/IGES"), QString(), tr("STEP/IGES (*.stp *.step *.igs *.iges)"));
    if (file.isEmpty()) return;

    auto shape = m_io->importFile(file);
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Import failed"), tr("No geometry could be loaded."));
        return;
    }

    m_partRegistry->addPart(file, shape);
    m_view->displayShape(shape);
    statusBar()->showMessage(tr("Loaded %1").arg(QFileInfo(file).fileName()), 3000);
}

void MainWindow::exportStepFile() {
    const QString file = QFileDialog::getSaveFileName(this, tr("Export STEP"), QString(), tr("STEP (*.stp *.step)"));
    if (file.isEmpty()) return;

    auto shape = m_partRegistry->activeShape();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Nothing to export"), tr("No geometry in the workspace."));
        return;
    }

    if (!m_io->exportStep(file, shape)) {
        QMessageBox::warning(this, tr("Export failed"), tr("The STEP file could not be written."));
    } else {
        statusBar()->showMessage(tr("Exported STEP to %1").arg(QFileInfo(file).fileName()), 3000);
    }
}

void MainWindow::exportGltfFile() {
    const QString file = QFileDialog::getSaveFileName(this, tr("Export glTF"), QString(), tr("glTF (*.gltf)"));
    if (file.isEmpty()) return;

    auto shape = m_partRegistry->activeShape();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Nothing to export"), tr("No geometry in the workspace."));
        return;
    }

    if (!m_gltf->exportShape(file, shape)) {
        QMessageBox::warning(this, tr("Export failed"), tr("The glTF file could not be written."));
    } else {
        statusBar()->showMessage(tr("Exported glTF to %1").arg(QFileInfo(file).fileName()), 3000);
    }
}

void MainWindow::runAnalysis() {
    auto shape = m_partRegistry->activeShape();
    if (shape.IsNull()) {
        QMessageBox::information(this, tr("Analysis"), tr("Load or generate geometry before running analysis."));
        return;
    }

    m_analysis->setModel(shape);
    auto result = m_analysis->runQuickCheck();
    m_legend->setResultText(result.summary);
    m_legend->show();
    statusBar()->showMessage(tr("Analysis complete"), 3000);
}

void MainWindow::regenerateFromReverse(const QString &prompt) {
    auto shape = m_partRegistry->synthesizeFromPrompt(prompt);
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Reverse engineer"), tr("Could not synthesize geometry from the prompt."));
        return;
    }
    m_view->displayShape(shape);
    statusBar()->showMessage(tr("Generated geometry from prompt"), 2000);
}

void MainWindow::evaluateAIAssistant(const QString &prompt) {
    const QString guidance = m_aiEngine->evaluate(prompt);
    if (m_aiDock) {
        m_aiDock->appendResponse(guidance);
    }
    statusBar()->showMessage(tr("AI assistant responded"), 2000);
}

void MainWindow::loadSamplePart() {
    const QString samplePath = ":/examples/sample_part.stp";
    auto shape = m_io->importFile(samplePath);
    if (!shape.IsNull()) {
        m_partRegistry->addPart("Sample Cube", shape);
        m_view->displayShape(shape);
        statusBar()->showMessage(tr("Loaded sample model"), 2000);
    }
}

