#pragma once

#include "AnalysisTypes.h"

#include <TopoDS_Shape.hxx>

class DomainTemplates {
public:
    DomainTemplates() = default;

    AnalysisCase defaultCase(DomainTemplateKind kind, const TopoDS_Shape &shape) const;
    int generateCoarseMesh(const TopoDS_Shape &shape);

    AnalysisCase cubeCompressionCase(TopoDS_Shape &shape) const;
};

