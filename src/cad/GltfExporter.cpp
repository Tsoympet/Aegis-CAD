#include "GltfExporter.h"

#include <RWGltf_CafWriter.hxx>
#include <RWMesh_CafReader.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <Message_ProgressRange.hxx>
#include "../utils/Logging.h"

bool GltfExporter::exportShape(const QString &path, const TopoDS_Shape &shape) const {
    if (shape.IsNull()) {
        Logging::warn(QStringLiteral("Cannot export glTF: no geometry for %1").arg(path));
        return false;
    }
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
    const bool ok = writer.Perform(doc, range);
    if (!ok) {
        Logging::warn(QStringLiteral("Failed to write glTF file: %1").arg(path));
    }
    return ok;
}

