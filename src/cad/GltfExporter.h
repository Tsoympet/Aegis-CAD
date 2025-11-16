#pragma once
#include <string>

class TopoDS_Shape;

class GltfExporter
{
public:
    bool exportGltf(const std::string& path, const TopoDS_Shape& shape);
};
