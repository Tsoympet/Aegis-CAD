#include "StepIgesIO.h"
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <TopoDS_Shape.hxx>

std::shared_ptr<TopoDS_Shape> StepIgesIO::importSTEP(const std::string& path)
{
    STEPControl_Reader reader;
    if (reader.ReadFile(path.c_str()) != IFSelect_RetDone)
        return nullptr;

    reader.TransferRoots();
    TopoDS_Shape shape = reader.OneShape();
    return std::make_shared<TopoDS_Shape>(shape);
}

bool StepIgesIO::exportSTEP(const std::string& path, const TopoDS_Shape& shape)
{
    STEPControl_Writer writer;
    writer.Transfer(shape, STEPControl_AsIs);
    return writer.Write(path.c_str()) == IFSelect_RetDone;
}

std::shared_ptr<TopoDS_Shape> StepIgesIO::importIGES(const std::string& path)
{
    IGESControl_Reader reader;
    if (reader.ReadFile(path.c_str()) != IFSelect_RetDone)
        return nullptr;

    reader.TransferRoots();
    TopoDS_Shape shape = reader.OneShape();
    return std::make_shared<TopoDS_Shape>(shape);
}

bool StepIgesIO::exportIGES(const std::string& path, const TopoDS_Shape& shape)
{
    IGESControl_Writer writer;
    writer.AddShape(shape);
    return writer.Write(path.c_str()) == IFSelect_RetDone;
}
