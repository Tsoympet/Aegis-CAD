#include "GltfExporter.h"
#include <RWGltf_CafWriter.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <BinXCAFDrivers.hxx>

bool GltfExporter::exportGltf(const std::string& path, const TopoDS_Shape& shape)
{
    Handle(TDocStd_Document) doc;
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    app->NewDocument("MDTV-XCAF", doc);

    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    shapeTool->AddShape(shape);

    RWGltf_CafWriter writer(path.c_str(), false);
    writer.Perform(doc, path.c_str());
    return true;
}
