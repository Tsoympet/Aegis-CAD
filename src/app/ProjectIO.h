#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>
#include <memory>

class AssemblyDocument;

class ProjectIO {
public:
    ProjectIO();

    bool saveProject(const QString &filePath, const TopoDS_Shape &shape);
    TopoDS_Shape loadProject(const QString &filePath) const;

    bool saveAssembly(const QString &filePath, const AssemblyDocument &assembly) const;
    std::shared_ptr<AssemblyDocument> loadAssembly(const QString &filePath) const;
};

