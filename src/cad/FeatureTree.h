#pragma once
#include <vector>
#include <string>

struct FeatureEntry {
    int         id {0};
    std::string name;
    int         parentId {0};
};

class FeatureTree
{
public:
    int addFeature(const std::string& name, int parentId = 0);
    const std::vector<FeatureEntry>& entries() const { return m_entries; }
private:
    std::vector<FeatureEntry> m_entries;
    int m_nextId {1};
};
