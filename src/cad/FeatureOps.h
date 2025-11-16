#pragma once
#include <memory>
#include <vector>
#include <string>

class TopoDS_Shape;

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

class FeatureOps
{
public:
    FeatureOps() = default;
    std::shared_ptr<TopoDS_Shape> build(const std::vector<FeatureNode>& tree);
};
