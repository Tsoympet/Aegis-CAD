#include "NodeSpatialIndex.h"

void NodeSpatialIndex::build(const std::vector<NodeSpatialEntry>& nodes)
{
    m_nodes = nodes;
}

std::vector<int> NodeSpatialIndex::queryBox(double xmin, double ymin, double zmin,
                                            double xmax, double ymax, double zmax) const
{
    std::vector<int> out;
    for (const auto& n : m_nodes)
    {
        if (n.x >= xmin && n.x <= xmax &&
            n.y >= ymin && n.y <= ymax &&
            n.z >= zmin && n.z <= zmax)
        {
            out.push_back(n.nodeId);
        }
    }
    return out;
}
