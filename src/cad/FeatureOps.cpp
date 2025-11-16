#include "FeatureOps.h"
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

std::shared_ptr<TopoDS_Shape> FeatureOps::build(const std::vector<FeatureNode>& tree)
{
    (void)tree;
    // TODO: Implement full feature replay (extrude, cut, fillet etc.)
    return nullptr;
}

TopoDS_Shape FeatureOps::makeBox(double dx, double dy, double dz)
{
    return BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
}

TopoDS_Shape FeatureOps::makeCylinder(double r, double h)
{
    return BRepPrimAPI_MakeCylinder(r, h).Shape();
}

TopoDS_Shape FeatureOps::makeSphere(double r)
{
    return BRepPrimAPI_MakeSphere(r).Shape();
}
