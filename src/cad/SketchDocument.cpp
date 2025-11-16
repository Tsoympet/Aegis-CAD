#include "SketchDocument.h"

int SketchDocument::addPoint(double x, double y)
{
    SketchPoint p;
    p.x = x;
    p.y = y;
    m_points.push_back(p);
    return static_cast<int>(m_points.size() - 1);
}

int SketchDocument::addEntity(const SketchEntity& eIn)
{
    SketchEntity e = eIn;
    e.id = m_nextEntityId++;
    m_entities.push_back(e);
    return e.id;
}

int SketchDocument::addConstraint(const SketchConstraint& cIn)
{
    SketchConstraint c = cIn;
    c.id = m_nextConstraintId++;
    m_constraints.push_back(c);
    return c.id;
}
