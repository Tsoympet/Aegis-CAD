#include "StepIgesIO.h"

#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IGESControl_Reader.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>

StepIgesIO::StepIgesIO() = default;

TopoDS_Shape StepIgesIO::importFile(const QString &path) const {
    const QString lower = path.toLower();
    if (lower.endsWith(".stp") || lower.endsWith(".step")) {
        STEPControl_Reader reader;
        if (reader.ReadFile(path.toStdString().c_str()) != IFSelect_RetDone) {
            return TopoDS_Shape();
        }
        reader.TransferRoots();
        return reader.OneShape();
    }

    if (lower.endsWith(".igs") || lower.endsWith(".iges")) {
        IGESControl_Reader reader;
        if (reader.ReadFile(path.toStdString().c_str()) != IFSelect_RetDone) {
            return TopoDS_Shape();
        }
        reader.TransferRoots();
        return reader.OneShape();
    }

    return TopoDS_Shape();
}

bool StepIgesIO::exportStep(const QString &path, const TopoDS_Shape &shape) const {
    STEPControl_Writer writer;
    writer.Transfer(shape, STEPControl_AsIs);
    return writer.Write(path.toStdString().c_str()) == IFSelect_RetDone;
}

