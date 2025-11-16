#include "FeatureTree.h"

int FeatureTree::addFeature(const std::string& name, int parentId)
{
    FeatureEntry e;
    e.id = m_nextId++;
    e.name = name;
    e.parentId = parentId;
    m_entries.push_back(e);
    return e.id;
}
