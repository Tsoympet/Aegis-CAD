#include "StepIgesIO.h"

std::shared_ptr<TopoDS_Shape> StepIgesIO::importSTEP(const std::string& path)
{
    (void)path;
    return nullptr;
}

bool StepIgesIO::exportSTEP(const std::string& path, const TopoDS_Shape& shape)
{
    (void)path;
    (void)shape;
    return false;
}

std::shared_ptr<TopoDS_Shape> StepIgesIO::importIGES(const std::string& path)
{
    (void)path;
    return nullptr;
}

bool StepIgesIO::exportIGES(const std::string& path, const TopoDS_Shape& shape)
{
    (void)path;
    (void)shape;
    return false;
}
