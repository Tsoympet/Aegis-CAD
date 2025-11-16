#pragma once
#include <unordered_map>
#include <memory>
#include <string>

class TopoDS_Shape;

class PartRegistry
{
public:
    void registerPart(const std::string& id, std::shared_ptr<TopoDS_Shape> shape);
    std::shared_ptr<TopoDS_Shape> part(const std::string& id) const;

private:
    std::unordered_map<std::string, std::shared_ptr<TopoDS_Shape>> m_parts;
};
