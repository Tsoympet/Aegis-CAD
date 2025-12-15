#pragma once

#include <QMainWindow>
#include <memory>

class OccView;
class AnalysisLegendOverlay;
class AegisAssistantDock;
class ReverseEngineerDock;
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
    void runAnalysis();
    void regenerateFromReverse(const QString &prompt);
    void evaluateAIAssistant(const QString &prompt);

private:
    void setupUi();
    void setupDocks();
    void setupToolbar();
    void loadSamplePart();

    OccView *m_view{nullptr};
    AnalysisLegendOverlay *m_legend{nullptr};
    AegisAssistantDock *m_aiDock{nullptr};
    ReverseEngineerDock *m_reverseDock{nullptr};
    PythonConsoleDock *m_pythonDock{nullptr};

    std::unique_ptr<AnalysisManager> m_analysis;
    std::unique_ptr<AegisAIEngine> m_aiEngine;
    std::unique_ptr<PartRegistry> m_partRegistry;
    std::unique_ptr<StepIgesIO> m_io;
    std::unique_ptr<GltfExporter> m_gltf;
    std::unique_ptr<ProjectIO> m_projectIO;
};

