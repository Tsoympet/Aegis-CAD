#include "FeatureOps.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <TopoDS.hxx>

namespace FeatureOps {

TopoDS_Shape makeBox(double size) {
    return BRepPrimAPI_MakeBox(size, size, size).Shape();
}

TopoDS_Shape makeCylinder(double radius, double height) {
    return BRepPrimAPI_MakeCylinder(radius, height).Shape();
}

TopoDS_Shape hollowOut(const TopoDS_Shape &shape, double offset) {
    try {
        BRepOffsetAPI_MakeThickSolid hollow(shape, TopTools_ListOfShape(), offset);
        return hollow.Shape();
    } catch (...) {
        return shape;
    }
}

} // namespace FeatureOps

