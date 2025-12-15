#pragma once

#include <TopoDS_Shape.hxx>

class DomainTemplates {
public:
    int generateCoarseMesh(const TopoDS_Shape &shape);
};

