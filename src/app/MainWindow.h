#pragma once
#include <QMainWindow>

class OccView;
class AegisAssistantDock;
class PythonConsoleDock;
class ReverseEngineerDock;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
private:
    void createMenus();
    void createDocks();

    OccView*             m_view = nullptr;
    AegisAssistantDock*  m_aiDock = nullptr;
    PythonConsoleDock*   m_pyDock = nullptr;
    ReverseEngineerDock* m_revDock = nullptr;
};
