#pragma once
#include <vector>
#include <string>

struct SketchPoint {
    double x {0.0};
    double y {0.0};
};

enum class SketchEntityType {
    Line,
    Arc,
    Circle,
    Spline
};

struct SketchEntity {
    SketchEntityType type {SketchEntityType::Line};
    int id {0};
    int p0 {-1};
    int p1 {-1};
    int p2 {-1};
};

enum class SketchConstraintType {
    Horizontal,
    Vertical,
    Coincident,
    Parallel,
    Perpendicular,
    Tangent,
    EqualLength,
    Distance,
    Radius
};

struct SketchConstraint {
    SketchConstraintType type;
    int id {0};
    std::vector<int> entities;
    double value {0.0};
};

class SketchDocument {
public:
    SketchDocument() = default;

    int addPoint(double x, double y);
    int addEntity(const SketchEntity& e);
    int addConstraint(const SketchConstraint& c);

    const std::vector<SketchPoint>& points() const { return m_points; }
    const std::vector<SketchEntity>& entities() const { return m_entities; }
    const std::vector<SketchConstraint>& constraints() const { return m_constraints; }

private:
    std::vector<SketchPoint>      m_points;
    std::vector<SketchEntity>     m_entities;
    std::vector<SketchConstraint> m_constraints;
    int m_nextEntityId {1};
    int m_nextConstraintId {1};
};
