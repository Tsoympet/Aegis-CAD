#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>
#include <QStringList>
#include <memory>

class AssemblyDocument;

struct ProjectSnapshot {
    TopoDS_Shape shape;
    QStringList chatHistory;
};

class ProjectIO {
public:
    ProjectIO();

    bool saveProject(const QString &filePath, const ProjectSnapshot &snapshot);
    ProjectSnapshot loadProject(const QString &filePath) const;

    bool saveAssembly(const QString &filePath, const AssemblyDocument &assembly) const;
    std::shared_ptr<AssemblyDocument> loadAssembly(const QString &filePath) const;
};

