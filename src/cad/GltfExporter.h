#pragma once
#include <string>
#include <TopoDS_Shape.hxx>

class GltfExporter
{
public:
    bool exportGltf(const std::string& path, const TopoDS_Shape& shape);
};
