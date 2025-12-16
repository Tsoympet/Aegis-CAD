#include "SketchEngine.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Standard_Version.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <QRandomGenerator>

namespace {
QString newId() {
    return QString::number(QRandomGenerator::global()->generate64(), 16);
}
}

QString Sketch2D::addLine(const gp_Pnt2d &a, const gp_Pnt2d &b) {
    SketchPrimitive prim;
    prim.kind = SketchPrimitive::Kind::Line;
    prim.id = newId();
    prim.start = a;
    prim.end = b;
    m_primitives.push_back(prim);
    return prim.id;
}

QString Sketch2D::addCircle(const gp_Pnt2d &center, double radius) {
    SketchPrimitive prim;
    prim.kind = SketchPrimitive::Kind::Circle;
    prim.id = newId();
    prim.start = center;
    prim.radius = radius;
    m_primitives.push_back(prim);
    return prim.id;
}

void Sketch2D::addConstraint(const SketchConstraint &c) {
    m_constraints.push_back(c);
}

void Sketch2D::addDimension(const SketchDimension &d) {
    m_dimensions.push_back(d);
}

void Sketch2D::updateDimension(const QString &id, double newValue) {
    for (auto &dim : m_dimensions) {
        if (dim.id == id) {
            dim.value = newValue;
        }
    }
    for (auto &prim : m_primitives) {
        if (prim.id == id && prim.kind == SketchPrimitive::Kind::Circle) {
            prim.radius = newValue;
        }
    }
}

TopoDS_Shape Sketch2D::toFace() const {
    BRepBuilderAPI_MakeWire wireBuilder;
    for (const auto &prim : m_primitives) {
        if (prim.kind == SketchPrimitive::Kind::Line) {
            wireBuilder.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(prim.start.X(), prim.start.Y(), 0), gp_Pnt(prim.end.X(), prim.end.Y(), 0)));
        } else if (prim.kind == SketchPrimitive::Kind::Circle) {
            Handle(Geom_Circle) circle = new Geom_Circle(gp_Ax2(gp_Pnt(prim.start.X(), prim.start.Y(), 0), gp::DZ()), prim.radius);
            wireBuilder.Add(BRepBuilderAPI_MakeEdge(circle));
        }
    }
    TopoDS_Wire wire = wireBuilder.Wire();
    return BRepBuilderAPI_MakeFace(wire);
}

void FeatureTree::setProfile(const TopoDS_Shape &face) {
    m_profile = face;
}

void FeatureTree::push(const Node &node) {
    m_history.push_back(node);
}

TopoDS_Shape FeatureTree::replay() const {
    TopoDS_Shape current = m_profile;
    for (const auto &node : m_history) {
        switch (node.type) {
        case NodeType::Extrude:
            current = FeatureOps::extrude(current, node.value);
            break;
        case NodeType::Revolve:
            current = FeatureOps::revolve(current, node.axis, node.value);
            break;
        case NodeType::Cut:
            current = FeatureOps::cut(current, node.tool);
            break;
        case NodeType::Fillet:
            current = FeatureOps::fillet(current, node.value);
            break;
        case NodeType::Chamfer:
            current = FeatureOps::chamfer(current, node.value);
            break;
        case NodeType::Shell:
            current = FeatureOps::shell(current, node.value);
            break;
        case NodeType::Draft:
            current = FeatureOps::draft(current, node.draftDir, node.value);
            break;
        }
    }
    return current;
}

SketchEngine::SketchEngine()
    : m_sketch(std::make_unique<Sketch2D>()),
      m_tree(std::make_unique<FeatureTree>()) {}

TopoDS_Shape SketchEngine::rebuild3D() {
    TopoDS_Shape profile = m_sketch->toFace();
    m_tree->setProfile(profile);
    return m_tree->replay();
}

