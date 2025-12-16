#include "StepIgesIO.h"

#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include "../utils/Logging.h"

StepIgesIO::StepIgesIO() = default;

namespace {
TopoDS_Shape collectFreeShapes(const Handle(XCAFDoc_ShapeTool) &tool) {
    TDF_LabelSequence labels;
    tool->GetFreeShapes(labels);
    BRep_Builder builder;
    TopoDS_Compound compound;
    builder.MakeCompound(compound);
    for (Standard_Integer i = 1; i <= labels.Length(); ++i) {
        TopoDS_Shape shape = tool->GetShape(labels.Value(i));
        builder.Add(compound, shape);
    }
    return compound;
}
}

TopoDS_Shape StepIgesIO::importFile(const QString &path) const {
    const QString lower = path.toLower();
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("MDTV-CAF", doc);
    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());

    if (lower.endsWith(".stp") || lower.endsWith(".step")) {
        STEPCAFControl_Reader reader;
        reader.SetColorMode(true);
        reader.SetNameMode(true);
        reader.SetMaterialMode(true);
        if (reader.ReadFile(path.toStdString().c_str()) != IFSelect_RetDone) {
            Logging::warn(QStringLiteral("Failed to read STEP file: %1").arg(path));
            return TopoDS_Shape();
        }
        reader.Transfer(doc);
        return collectFreeShapes(shapeTool);
    }

    if (lower.endsWith(".igs") || lower.endsWith(".iges")) {
        IGESControl_Reader reader;
        if (reader.ReadFile(path.toStdString().c_str()) != IFSelect_RetDone) {
            Logging::warn(QStringLiteral("Failed to read IGES file: %1").arg(path));
            return TopoDS_Shape();
        }
        reader.TransferRoots();
        return reader.OneShape();
    }

    Logging::warn(QStringLiteral("Unsupported CAD format: %1").arg(path));
    return TopoDS_Shape();
}

bool StepIgesIO::exportStep(const QString &path, const TopoDS_Shape &shape) const {
    if (shape.IsNull()) {
        Logging::warn(QStringLiteral("Cannot export STEP: no geometry for %1").arg(path));
        return false;
    }
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("MDTV-CAF", doc);

    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    shapeTool->AddShape(shape);

    STEPCAFControl_Writer writer;
    writer.SetColorMode(true);
    writer.SetNameMode(true);
    writer.SetMaterialMode(true);
    IFSelect_ReturnStatus status = writer.Transfer(doc, STEPControl_AsIs);
    if (status != IFSelect_RetDone) {
        Logging::warn(QStringLiteral("Failed to transfer STEP document for %1").arg(path));
        return false;
    }
    const bool ok = writer.Write(path.toStdString().c_str()) == IFSelect_RetDone;
    if (!ok) {
        Logging::warn(QStringLiteral("Failed to write STEP file: %1").arg(path));
    }
    return ok;
}

bool StepIgesIO::exportIges(const QString &path, const TopoDS_Shape &shape) const {
    if (shape.IsNull()) {
        Logging::warn(QStringLiteral("Cannot export IGES: no geometry for %1").arg(path));
        return false;
    }
    IGESControl_Writer writer;
    writer.AddShape(shape);
    const bool ok = writer.Write(path.toStdString().c_str()) == IFSelect_RetDone;
    if (!ok) {
        Logging::warn(QStringLiteral("Failed to write IGES file: %1").arg(path));
    }
    return ok;
}

