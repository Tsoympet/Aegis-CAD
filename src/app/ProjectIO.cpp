#include "ProjectIO.h"
#include "../cad/StepIgesIO.h"

ProjectIO::ProjectIO() = default;

bool ProjectIO::saveProject(const QString &filePath, const TopoDS_Shape &shape) {
    StepIgesIO io;
    return io.exportStep(filePath, shape);
}

TopoDS_Shape ProjectIO::loadProject(const QString &filePath) const {
    StepIgesIO io;
    return io.importFile(filePath);
}

