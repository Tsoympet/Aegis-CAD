#pragma once
#include <vector>
#include <string>

struct AssemblyNodeRef {
    int id {0};
    int parentId {0};
    std::string partId;
};

class AssemblyDocument
{
public:
    int addNode(const std::string& partId, int parentId = 0);
    const std::vector<AssemblyNodeRef>& nodes() const { return m_nodes; }
private:
    std::vector<AssemblyNodeRef> m_nodes;
    int m_nextId {1};
};
