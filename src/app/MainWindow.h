#pragma once
#include <QMainWindow>
#include "cad/StepIgesIO.h"
#include "cad/GltfExporter.h"

class OccView;
class AegisAssistantDock;
class PythonConsoleDock;
class ReverseEngineerDock;
class QAction;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    bool isSafeMode() const { return m_safeMode; }

private slots:
    // Core operations
    void newSketch();

    // Feature operations
    void extrudeBox();
    void extrudeCylinder();

    // Import / Export
    void importCAD();   // Handles STEP + IGES
    void exportCAD();   // Handles GLTF + STEP

    // Tools
    void runAnalysis();
    void showAIDock();

private:
    // UI setup
    void createMenus();
    void createToolBar();
    void createDocks();
    void createStatusBar();

    // Widgets
    OccView*             m_view = nullptr;
    AegisAssistantDock*  m_aiDock = nullptr;
    PythonConsoleDock*   m_pyDock = nullptr;
    ReverseEngineerDock* m_revDock = nullptr;

    // Actions
    QAction* m_actionNew = nullptr;
    QAction* m_actionImport = nullptr;
    QAction* m_actionExport = nullptr;
    QAction* m_actionAnalyze = nullptr;
    QAction* m_actionAI = nullptr;

    // Status bar
    QLabel* m_statusLabel = nullptr;

    // CAD backend helpers
    StepIgesIO    m_stepIO;        // STEP/IGES import/export handler
    GltfExporter  m_gltfExporter;  // GLTF exporter

    // Safe mode flag
    bool m_safeMode = false;
};

