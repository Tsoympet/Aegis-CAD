#include "GltfExporter.h"

#include <RWGltf_CafWriter.hxx>
#include <RWMesh_CafReader.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <Message_ProgressRange.hxx>

bool GltfExporter::exportShape(const QString &path, const TopoDS_Shape &shape) const {
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("MDTV-CAF", doc);

    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    if (shape.IsNull()) {
        return false;
    }
    shapeTool->AddShape(shape);

    RWGltf_CafWriter writer(path.toStdString().c_str(), false);
    writer.SetCoordinateSystem(RWGltf_CafWriter::CoordinateSystem_ZUp);
    Message_ProgressRange range;
    return writer.Perform(doc, range);
}

