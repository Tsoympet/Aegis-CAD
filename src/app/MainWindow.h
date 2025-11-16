#pragma once
#include <QMainWindow>

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

private slots:
    void newSketch();
    void importSTEP();
    void exportGLTF();
    void runAnalysis();
    void showAIDock();

private:
    void createMenus();
    void createToolBar();
    void createDocks();
    void createStatusBar();

    OccView*             m_view = nullptr;
    AegisAssistantDock*  m_aiDock = nullptr;
    PythonConsoleDock*   m_pyDock = nullptr;
    ReverseEngineerDock* m_revDock = nullptr;

    QAction* m_actionNew = nullptr;
    QAction* m_actionImport = nullptr;
    QAction* m_actionExport = nullptr;
    QAction* m_actionAnalyze = nullptr;
    QAction* m_actionAI = nullptr;

    QLabel*  m_statusLabel = nullptr;
};
