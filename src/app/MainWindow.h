#pragma once
#include <QMainWindow>
#include <memory>

#include "cad/StepIgesIO.h"
#include "cad/GltfExporter.h"

class OccView;
class AegisAssistantDock;
class PythonConsoleDock;
class ReverseEngineerDock;
class AnalysisDock;
class QAction;
class QLabel;

/// Main application window for AegisCAD.
/// Hosts the 3D viewport, toolbars, docks, and menus.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    // Core CAD operations
    void newSketch();

    // Import / Export operations
    void importCAD();
    void exportCAD();

    // Tools and analysis
    void runAnalysis();
    void showAIDock();

private:
    // Internal setup helpers
    void createMenus();
    void createToolBar();
    void createDocks();
    void createStatusBar();

    // Widgets
    OccView*             m_view = nullptr;
    AegisAssistantDock*  m_aiDock = nullptr;
    PythonConsoleDock*   m_pyDock = nullptr;
    ReverseEngineerDock* m_revDock = nullptr;
    AnalysisDock*        m_analysisDock = nullptr;

    // Actions
    QAction* m_actionNew     = nullptr;
    QAction* m_actionImport  = nullptr;
    QAction* m_actionExport  = nullptr;
    QAction* m_actionAnalyze = nullptr;
    QAction* m_actionAI      = nullptr;

    // Status bar
    QLabel* m_statusLabel = nullptr;

    // CAD helpers
    StepIgesIO   m_stepIO;
    GltfExporter m_gltfExporter;
};
