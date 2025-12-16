#pragma once

#include "FeatureOps.h"

#include <QString>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <memory>
#include <unordered_map>
#include <vector>

struct SketchConstraint {
    enum class Type { Equal, Parallel, Perpendicular, Tangent, Coincident, Radius, Distance };
    Type type;
    QString a;
    QString b;
    double value{0.0};
};

struct SketchDimension {
    QString id;
    QString target;
    double value{0.0};
};

struct SketchPrimitive {
    enum class Kind { Line, Circle };
    Kind kind;
    QString id;
    gp_Pnt2d start;
    gp_Pnt2d end;
    double radius{0.0};
};

class Sketch2D {
public:
    QString addLine(const gp_Pnt2d &a, const gp_Pnt2d &b);
    QString addCircle(const gp_Pnt2d &center, double radius);
    void addConstraint(const SketchConstraint &c);
    void addDimension(const SketchDimension &d);
    void updateDimension(const QString &id, double newValue);

    TopoDS_Shape toFace() const;

private:
    std::vector<SketchPrimitive> m_primitives;
    std::vector<SketchConstraint> m_constraints;
    std::vector<SketchDimension> m_dimensions;
};

class FeatureTree {
public:
    enum class NodeType { Extrude, Revolve, Cut, Fillet, Chamfer, Shell, Draft };
    struct Node {
        NodeType type;
        double value;
        gp_Ax1 axis;
        gp_Dir draftDir;
        TopoDS_Shape tool;
    };

    void setProfile(const TopoDS_Shape &face);
    void push(const Node &node);
    TopoDS_Shape replay() const;

private:
    TopoDS_Shape m_profile;
    std::vector<Node> m_history;
};

class SketchEngine {
public:
    SketchEngine();

    Sketch2D &sketch() { return *m_sketch; }
    FeatureTree &history() { return *m_tree; }

    TopoDS_Shape rebuild3D();

private:
    std::unique_ptr<Sketch2D> m_sketch;
    std::unique_ptr<FeatureTree> m_tree;
};

