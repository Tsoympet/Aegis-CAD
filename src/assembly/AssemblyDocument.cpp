#include "AssemblyDocument.h"

int AssemblyDocument::addNode(const std::string& partId, int parentId)
{
    AssemblyNodeRef r;
    r.id = m_nextId++;
    r.parentId = parentId;
    r.partId = partId;
    m_nodes.push_back(r);
    return r.id;
}
