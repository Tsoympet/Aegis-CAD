#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>

class GltfExporter {
public:
    bool exportShape(const QString &path, const TopoDS_Shape &shape) const;
};

