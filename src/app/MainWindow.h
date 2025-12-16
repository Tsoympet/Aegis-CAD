#pragma once

#include "../ai/AegisAIEngine.h"

#include <QMainWindow>
#include <TopoDS_Shape.hxx>
#include <memory>

class OccView;
class AnalysisLegendOverlay;
class AegisAssistantDock;
class PerformanceProfilerDock;
class ReverseEngineerDock;
class CamDock;
class PythonConsoleDock;
class AnalysisManager;
class AegisAIEngine;
class PartRegistry;
class StepIgesIO;
class GltfExporter;
class ProjectIO;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void openStepFile();
    void exportStepFile();
    void exportGltfFile();
    void saveProject();
    void loadProject();
    void submitCalculixJob();
    void previewCamPath();
    void reloadAiRules();
    void runAnalysis();
    void regenerateFromReverse(const TopoDS_Shape &shape);
    void evaluateAIAssistant(const QString &prompt);

private:
    void setupUi();
    void setupDocks();
    void setupToolbar();
    void setupMenus();
    void loadSamplePart();
    std::vector<AegisAIEngine::PartInsight> buildInsights();

    OccView *m_view{nullptr};
    AnalysisLegendOverlay *m_legend{nullptr};
    AegisAssistantDock *m_aiDock{nullptr};
    PerformanceProfilerDock *m_profilerDock{nullptr};
    ReverseEngineerDock *m_reverseDock{nullptr};
    CamDock *m_camDock{nullptr};
    PythonConsoleDock *m_pythonDock{nullptr};

    std::unique_ptr<AnalysisManager> m_analysis;
    std::unique_ptr<AegisAIEngine> m_aiEngine;
    std::unique_ptr<PartRegistry> m_partRegistry;
    std::unique_ptr<StepIgesIO> m_io;
    std::unique_ptr<GltfExporter> m_gltf;
    std::unique_ptr<ProjectIO> m_projectIO;
};

