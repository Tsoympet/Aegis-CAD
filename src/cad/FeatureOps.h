#pragma once
#include <memory>
#include <vector>
#include <string>
#include <TopoDS_Shape.hxx>

// --------------------
// Parametric feature definitions
// --------------------
enum class FeatureKind {
    SketchBase,
    Extrude,
    Cut,
    Revolve,
    Fillet,
    Chamfer,
    Shell,
    Draft
};

struct FeatureNode {
    int         id {0};
    FeatureKind kind {FeatureKind::SketchBase};
    std::string name;
    int         parentId {0};
};

// --------------------
// Feature operations API
// --------------------
class FeatureOps
{
public:
    FeatureOps() = default;

    // Parametric build (future replay of the feature tree)
    std::shared_ptr<TopoDS_Shape> build(const std::vector<FeatureNode>& tree);

    // Direct modeling helpers (usable from MainWindow / OccView)
    static TopoDS_Shape makeBox(double dx, double dy, double dz);
    static TopoDS_Shape makeCylinder(double radius, double height);
    static TopoDS_Shape makeSphere(double radius);
};
