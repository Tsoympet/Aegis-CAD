#pragma once
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <QString>

namespace FeatureOps {
TopoDS_Shape makeBox(double size);
TopoDS_Shape makeCylinder(double radius, double height);
TopoDS_Shape hollowOut(const TopoDS_Shape &shape, double offset);

TopoDS_Shape extrude(const TopoDS_Shape &profile, double height, const gp_Vec &direction = gp_Vec(0, 0, 1));
TopoDS_Shape cut(const TopoDS_Shape &base, const TopoDS_Shape &tool);
TopoDS_Shape revolve(const TopoDS_Shape &profile, const gp_Ax1 &axis, double angle);
TopoDS_Shape fillet(const TopoDS_Shape &shape, double radius);
TopoDS_Shape chamfer(const TopoDS_Shape &shape, double distance);
TopoDS_Shape shell(const TopoDS_Shape &shape, double thickness);
TopoDS_Shape draft(const TopoDS_Shape &shape, const gp_Dir &dir, double angleDegrees);
}

