#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>

class ProjectIO {
public:
    ProjectIO();

    bool saveProject(const QString &filePath, const TopoDS_Shape &shape);
    TopoDS_Shape loadProject(const QString &filePath) const;
};

