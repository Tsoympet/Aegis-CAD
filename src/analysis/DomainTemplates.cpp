#include "DomainTemplates.h"

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <algorithm>

int DomainTemplates::generateCoarseMesh(const TopoDS_Shape &shape) {
    if (shape.IsNull()) {
        return 0;
    }
    int faces = 0;
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next()) {
        ++faces;
    }
    return std::max(12, faces * 20);
}

