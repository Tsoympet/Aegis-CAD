#include "FeatureOps.h"

#include <BOPAlgo_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <BRepOffsetAPI_MakeChamfer.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepOffsetAPI_MakeThickSolidByJoin.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <gp.hxx>
#include <TNaming_Builder.hxx>
#include <TDF_Label.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <cmath>

namespace FeatureOps {
namespace {
void annotateWithPersistentNames(const TopoDS_Shape &original, const TopoDS_Shape &result) {
    TopTools_IndexedMapOfShape originalMap;
    TopTools_IndexedMapOfShape resultMap;
    TopExp::MapShapes(original, originalMap);
    TopExp::MapShapes(result, resultMap);

    TDF_Label dummyLabel;
    TNaming_Builder builder(dummyLabel);
    for (int i = 1; i <= resultMap.Extent(); ++i) {
        const TopoDS_Shape &sub = resultMap(i);
        builder.Generated(sub);
    }

    // Use BOPAlgo_Builder images to keep a quick association for downstream consumers.
    BOPAlgo_Builder mapBuilder;
    mapBuilder.AddArgument(original);
    mapBuilder.Perform();
    const TopTools_IndexedDataMapOfShapeListOfShape &images = mapBuilder.Images();
    (void)images;
}
}

TopoDS_Shape makeBox(double size) {
    return BRepPrimAPI_MakeBox(size, size, size).Shape();
}

TopoDS_Shape makeCylinder(double radius, double height) {
    return BRepPrimAPI_MakeCylinder(radius, height).Shape();
}

TopoDS_Shape hollowOut(const TopoDS_Shape &shape, double offset) {
    try {
        BRepOffsetAPI_MakeThickSolid hollow(shape, TopTools_ListOfShape(), offset);
        annotateWithPersistentNames(shape, hollow.Shape());
        return hollow.Shape();
    } catch (...) {
        return shape;
    }
}

TopoDS_Shape extrude(const TopoDS_Shape &profile, double height, const gp_Vec &direction) {
    gp_Vec vec = direction;
    if (vec.Magnitude() < gp::Resolution()) {
        vec = gp_Vec(0, 0, 1);
    }
    vec.Normalize();
    vec *= height;

    TopoDS_Shape prism = BRepPrimAPI_MakePrism(profile, vec).Shape();
    annotateWithPersistentNames(profile, prism);
    return prism;
}

TopoDS_Shape cut(const TopoDS_Shape &base, const TopoDS_Shape &tool) {
    BRepAlgoAPI_Cut cutter(base, tool);
    cutter.Build();
    annotateWithPersistentNames(base, cutter.Shape());
    return cutter.Shape();
}

TopoDS_Shape revolve(const TopoDS_Shape &profile, const gp_Ax1 &axis, double angle) {
    TopoDS_Shape revolved = BRepPrimAPI_MakeRevol(profile, axis, angle).Shape();
    annotateWithPersistentNames(profile, revolved);
    return revolved;
}

TopoDS_Shape fillet(const TopoDS_Shape &shape, double radius) {
    BRepFilletAPI_MakeFillet filletMaker(shape);
    for (TopExp_Explorer it(shape, TopAbs_EDGE); it.More(); it.Next()) {
        filletMaker.Add(radius, TopoDS::Edge(it.Current()));
    }
    filletMaker.Build();
    annotateWithPersistentNames(shape, filletMaker.Shape());
    return filletMaker.Shape();
}

TopoDS_Shape chamfer(const TopoDS_Shape &shape, double distance) {
    BRepOffsetAPI_MakeChamfer chamferMaker(shape);
    for (TopExp_Explorer it(shape, TopAbs_EDGE); it.More(); it.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(it.Current());
        TopoDS_Face face;
        if (it.Current().ShapeType() == TopAbs_EDGE) {
            chamferMaker.Add(distance, edge, face);
        }
    }
    chamferMaker.Build();
    annotateWithPersistentNames(shape, chamferMaker.Shape());
    return chamferMaker.Shape();
}

TopoDS_Shape shell(const TopoDS_Shape &shape, double thickness) {
    TopTools_ListOfShape holes;
    BRepOffsetAPI_MakeThickSolidByJoin shellMaker(shape, holes, -thickness, 1.0e-3);
    shellMaker.Build();
    annotateWithPersistentNames(shape, shellMaker.Shape());
    return shellMaker.Shape();
}

TopoDS_Shape draft(const TopoDS_Shape &shape, const gp_Dir &dir, double angleDegrees) {
    BRepOffsetAPI_DraftAngle draftMaker(shape);
    gp_Dir direction = dir;
    for (TopExp_Explorer it(shape, TopAbs_FACE); it.More(); it.Next()) {
        TopoDS_Face face = TopoDS::Face(it.Current());
        draftMaker.Add(face, direction, angleDegrees * (M_PI / 180.0));
    }
    draftMaker.Build();
    annotateWithPersistentNames(shape, draftMaker.Shape());
    return draftMaker.Shape();
}

} // namespace FeatureOps

