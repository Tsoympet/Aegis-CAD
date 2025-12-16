#include "MainWindow.h"
#include "ProjectIO.h"
#include "../ui/OccView.h"
#include "../ui/AnalysisLegendOverlay.h"
#include "../ui/AegisAssistantDock.h"
#include "../ui/PerformanceProfilerDock.h"
#include "../ui/ReverseEngineerDock.h"
#include "../ui/CamDock.h"
#include "../ui/PythonConsoleDock.h"
#include "../cad/StepIgesIO.h"
#include "../cad/GltfExporter.h"
#include "../cad/PartRegistry.h"
#include "../analysis/AnalysisManager.h"
#include "../analysis/DomainTemplates.h"
#include "../ai/AegisAIEngine.h"
#include "../utils/Logging.h"

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <QToolBar>
#include <QFileDialog>
#include <QFileInfo>
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
    setWindowTitle(tr("AegisCAD 1.0"));

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
    connect(m_reverseDock, &ReverseEngineerDock::modelGenerated, this, &MainWindow::regenerateFromReverse);

    m_camDock = new CamDock(tr("CAM"), this);
    m_camDock->setView(m_view);
    m_camDock->setShapeProvider([this]() { return m_partRegistry->activeShape(); });
    addDockWidget(Qt::RightDockWidgetArea, m_camDock);

    m_profilerDock = new PerformanceProfilerDock(tr("Performance"), this);
    m_profilerDock->attachView(m_view);
    addDockWidget(Qt::RightDockWidgetArea, m_profilerDock);

    m_pythonDock = new PythonConsoleDock(tr("Python Console"), m_view, m_analysis.get(), m_aiEngine.get(), m_projectIO.get(), this);
    addDockWidget(Qt::BottomDockWidgetArea, m_pythonDock);

    // Quick hide/show shortcuts
    new QShortcut(QKeySequence("Ctrl+`"), this, [this]() { m_pythonDock->setVisible(!m_pythonDock->isVisible()); });
}

void MainWindow::setupToolbar() {
    auto *toolbar = addToolBar(tr("Aegis"));
    toolbar->setMovable(false);

    QAction *importStep = toolbar->addAction(QIcon(":/icons/toolbar_import.svg"), tr("Import STEP"), this, &MainWindow::openStepFile);
    importStep->setShortcut(QKeySequence::Open);

    toolbar->addAction(QIcon(":/icons/toolbar_import.svg"), tr("Open Project"), this, &MainWindow::loadProject);
    toolbar->addAction(QIcon(":/icons/toolbar_export.svg"), tr("Save Project"), this, &MainWindow::saveProject);

    QAction *exportStep = toolbar->addAction(QIcon(":/icons/toolbar_export.svg"), tr("Export STEP"), this, &MainWindow::exportStepFile);
    QAction *exportGltf = toolbar->addAction(QIcon(":/icons/toolbar_gltf.svg"), tr("Export glTF"), this, &MainWindow::exportGltfFile);

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
    DomainTemplates templates;
    m_analysis->setAnalysisCase(templates.defaultCase(DomainTemplateKind::Car, shape));
    auto result = m_analysis->runCase();
    if (m_legend) {
        m_legend->setResultText(result.summary);
        m_legend->show();
    }
    statusBar()->showMessage(tr("Analysis complete"), 3000);
}

void MainWindow::regenerateFromReverse(const TopoDS_Shape &shape) {
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Reverse engineer"), tr("Could not synthesize geometry from the prompt."));
        return;
    }
    const QString id = m_partRegistry->addPart(tr("ReverseModel"), shape);
    m_partRegistry->setMaterial(id, QStringLiteral("Aluminum 6061"));
    m_view->displayShape(shape);
    statusBar()->showMessage(tr("Generated geometry from prompt"), 2000);
}

void MainWindow::evaluateAIAssistant(const QString &prompt) {
    if (m_aiDock) {
        m_aiDock->appendUserPrompt(prompt);
    }
    m_aiEngine->setSceneInsights(buildInsights());
    m_aiEngine->setStressSnapshot(m_analysis->lastResult());
    auto advice = m_aiEngine->evaluate(prompt);
    if (m_aiDock) {
        m_aiDock->appendAdvice(advice);
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

void MainWindow::saveProject() {
    const QString file = QFileDialog::getSaveFileName(this, tr("Save Project"), QString(), tr("Aegis Project (*.aegisproj)"));
    if (file.isEmpty()) return;

    ProjectSnapshot snapshot;
    snapshot.shape = m_partRegistry->activeShape();
    if (m_aiDock) {
        snapshot.chatHistory = m_aiDock->history();
    }

    if (!m_projectIO->saveProject(file, snapshot)) {
        QMessageBox::warning(this, tr("Save failed"), tr("Project could not be written."));
    } else {
        statusBar()->showMessage(tr("Saved project"), 2000);
    }
}

void MainWindow::loadProject() {
    const QString file = QFileDialog::getOpenFileName(this, tr("Open Project"), QString(), tr("Aegis Project (*.aegisproj)"));
    if (file.isEmpty()) return;

    auto snapshot = m_projectIO->loadProject(file);
    if (!snapshot.shape.IsNull()) {
        m_partRegistry->addPart(QFileInfo(file).fileName(), snapshot.shape);
        m_view->displayShape(snapshot.shape);
    }
    if (m_aiDock && !snapshot.chatHistory.isEmpty()) {
        m_aiDock->setHistory(snapshot.chatHistory);
    }
    statusBar()->showMessage(tr("Project loaded"), 2000);
}

std::vector<AegisAIEngine::PartInsight> MainWindow::buildInsights() {
    std::vector<AegisAIEngine::PartInsight> insights;
    const auto parts = m_partRegistry->parts();
    for (const auto &entry : parts) {
        if (entry.shape.IsNull()) continue;
        AegisAIEngine::PartInsight insight;
        insight.id = entry.id;
        insight.name = entry.name;
        insight.material = entry.material;
        GProp_GProps props;
        BRepGProp::Volume(entry.shape, props);
        insight.volume = props.Mass();
        insight.mass = insight.volume * entry.density;
        insight.peakStress = m_analysis->lastResult().maxStress;
        const double stress = std::max(1.0, insight.peakStress);
        insight.safetyFactor = entry.yieldStrength / stress;
        insights.push_back(insight);
    }
    return insights;
}

