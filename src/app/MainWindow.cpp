#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include "ui/OccView.h"
#include "ui/AegisAssistantDock.h"
#include "ui/PythonConsoleDock.h"
#include "ui/ReverseEngineerDock.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_view = new OccView(this);
    setCentralWidget(m_view);

    m_aiDock  = new AegisAssistantDock(this);
    m_pyDock  = new PythonConsoleDock(this);
    m_revDock = new ReverseEngineerDock(this);

    createMenus();
    createDocks();
    statusBar()->showMessage("Ready");
    setWindowTitle("AegisCAD");
}

MainWindow::~MainWindow() = default;

void MainWindow::createMenus()
{
    auto* tools = menuBar()->addMenu(tr("Tools"));
    tools->addAction(tr("Aegis AI"), [this]{
        m_aiDock->show();
        m_aiDock->raise();
    });
    tools->addAction(tr("Python Console"), [this]{
        m_pyDock->show();
        m_pyDock->raise();
    });
    tools->addAction(tr("AI Reverse Engineer"), [this]{
        m_revDock->show();
        m_revDock->raise();
    });
}

void MainWindow::createDocks()
{
    addDockWidget(Qt::RightDockWidgetArea, m_aiDock);
    m_aiDock->hide();
    addDockWidget(Qt::BottomDockWidgetArea, m_pyDock);
    m_pyDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, m_revDock);
    m_revDock->hide();
}
