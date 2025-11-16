#pragma once
#include <vector>

struct TransformNode {
    int id {0};
};

class TransformGraph
{
public:
    void clear() { m_nodes.clear(); }
private:
    std::vector<TransformNode> m_nodes;
};
