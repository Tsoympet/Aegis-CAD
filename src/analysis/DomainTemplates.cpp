#include "DomainTemplates.h"

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <gp_Vec.hxx>
#include <algorithm>

AnalysisCase DomainTemplates::defaultCase(DomainTemplateKind kind, const TopoDS_Shape &shape) const {
    AnalysisCase c;
    c.domain = kind;
    c.name = QStringLiteral("Default Case");

    gp_Vec loadDir(0, 0, -1);
    switch (kind) {
    case DomainTemplateKind::Car:
        c.material = {QStringLiteral("Steel"), 7850.0, 2.1e11, 350e6, 43.0};
        loadDir = gp_Vec(0, 0, -1);
        break;
    case DomainTemplateKind::Ship:
        c.material = {QStringLiteral("Marine Alloy"), 2700.0, 7.0e10, 320e6, 160.0};
        loadDir = gp_Vec(0, 0, -1);
        break;
    case DomainTemplateKind::Aircraft:
        c.material = {QStringLiteral("Aerospace Al"), 2800.0, 7.2e10, 400e6, 170.0};
        loadDir = gp_Vec(0, 1, -0.2);
        break;
    case DomainTemplateKind::Armor:
        c.material = {QStringLiteral("Armor Steel"), 7850.0, 2.1e11, 1200e6, 40.0};
        loadDir = gp_Vec(0, 0, -1);
        break;
    }

    LoadDefinition load;
    load.type = LoadType::Force;
    load.direction = loadDir;
    load.magnitude = 5000.0;
    load.targetPartId = QStringLiteral("active");
    c.loads.push_back(load);

    ConstraintDefinition constraint;
    constraint.type = ConstraintType::Fixed;
    constraint.anchor = gp_Pnt(0, 0, 0);
    constraint.normal = gp_Dir(0, 0, 1);
    c.constraints.push_back(constraint);

    Bnd_Box box;
    BRepBndLib::Add(shape, box);
    if (!box.IsVoid()) {
        Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
        box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        const double area = (xmax - xmin) * (ymax - ymin);
        if (area > 1e-6) {
            c.loads.front().magnitude = area * 1.2e5; // Pa to N placeholder
            c.loads.front().type = LoadType::Pressure;
        }
    }
    return c;
}

int DomainTemplates::generateCoarseMesh(const TopoDS_Shape &shape) {
    if (shape.IsNull()) {
        return 0;
    }
    int faces = 0;
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next()) {
        ++faces;
    }
    return std::max(32, faces * 32);
}

AnalysisCase DomainTemplates::cubeCompressionCase(TopoDS_Shape &shape) const {
    if (shape.IsNull()) {
        shape = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    }
    AnalysisCase c;
    c.name = QStringLiteral("Cube Compression");
    c.domain = DomainTemplateKind::Armor;
    c.material = {QStringLiteral("Test Steel"), 7800.0, 2.0e11, 600e6, 50.0};

    ConstraintDefinition constraint;
    constraint.type = ConstraintType::Fixed;
    constraint.regionHint = QStringLiteral("base");
    constraint.anchor = gp_Pnt(5.0, 5.0, 0.0);
    constraint.normal = gp_Dir(0, 0, -1);
    c.constraints.push_back(constraint);

    LoadDefinition load;
    load.type = LoadType::Force;
    load.direction = gp_Vec(0, 0, -1);
    load.magnitude = 20000.0;
    load.regionHint = QStringLiteral("top");
    c.loads.push_back(load);
    return c;
}

