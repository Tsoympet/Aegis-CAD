#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>

class StepIgesIO {
public:
    StepIgesIO();

    TopoDS_Shape importFile(const QString &path) const;
    bool exportStep(const QString &path, const TopoDS_Shape &shape) const;
};

