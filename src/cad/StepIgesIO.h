#pragma once
#include <string>
#include <memory>

class TopoDS_Shape;

class StepIgesIO
{
public:
    StepIgesIO() = default;

    std::shared_ptr<TopoDS_Shape> importSTEP(const std::string& path);
    bool exportSTEP(const std::string& path, const TopoDS_Shape& shape);

    std::shared_ptr<TopoDS_Shape> importIGES(const std::string& path);
    bool exportIGES(const std::string& path, const TopoDS_Shape& shape);
};
