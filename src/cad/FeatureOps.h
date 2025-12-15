#pragma once

#include <TopoDS_Shape.hxx>
#include <QString>

namespace FeatureOps {
TopoDS_Shape makeBox(double size);
TopoDS_Shape makeCylinder(double radius, double height);
TopoDS_Shape hollowOut(const TopoDS_Shape &shape, double offset);
}

