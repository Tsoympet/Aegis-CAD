#include "PartRegistry.h"

void PartRegistry::registerPart(const std::string& id, std::shared_ptr<TopoDS_Shape> shape)
{
    m_parts[id] = std::move(shape);
}

std::shared_ptr<TopoDS_Shape> PartRegistry::part(const std::string& id) const
{
    auto it = m_parts.find(id);
    if (it == m_parts.end()) return {};
    return it->second;
}
