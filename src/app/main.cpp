#include "AegisApp.h"
#include "MainWindow.h"
#include <QStringList>
#include <QFileInfo>
#include <QDebug>

int main(int argc, char** argv)
{
    AegisApp app(argc, argv);

    QStringList args = QCoreApplication::arguments();
    bool safeMode = args.contains("--safe-mode");
    QString projectPath;

    int openIndex = args.indexOf("--open");
    if (openIndex != -1 && openIndex + 1 < args.size())
        projectPath = args.at(openIndex + 1);

    if (safeMode)
        qInfo() << "[SAFE MODE] AegisCAD started with limited features.";

    // Run as normal
    int ret = app.run();

    // Optional: auto-open project
    if (!projectPath.isEmpty()) {
        QFileInfo fi(projectPath);
        if (fi.exists() && fi.isFile()) {
            qInfo() << "Opening project:" << projectPath;
            // Could trigger a MainWindow::loadProject(projectPath) call if desired
        } else {
            qWarning() << "Project not found:" << projectPath;
        }
    }

    return ret;
}
