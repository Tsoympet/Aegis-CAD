#pragma once
#include <vector>

struct NodeSpatialEntry {
    int    nodeId {0};
    double x {0.0};
    double y {0.0};
    double z {0.0};
};

class NodeSpatialIndex
{
public:
    void build(const std::vector<NodeSpatialEntry>& nodes);
    std::vector<int> queryBox(double xmin, double ymin, double zmin,
                              double xmax, double ymax, double zmax) const;
private:
    std::vector<NodeSpatialEntry> m_nodes;
};
