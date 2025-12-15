#include "BackendFEA_CalculiX.h"
#include "DomainTemplates.h"

BackendFEA_CalculiX::BackendFEA_CalculiX() = default;

void BackendFEA_CalculiX::setModel(const TopoDS_Shape &shape) {
    m_shape = shape;
}

QString BackendFEA_CalculiX::runStaticCheck() {
    if (m_shape.IsNull()) {
        return QStringLiteral("No geometry loaded.");
    }
    DomainTemplates mesher;
    const int nodes = mesher.generateCoarseMesh(m_shape);
    return QStringLiteral("CalculiX stub: coarse mesh generated with %1 nodes. Ready for boundary conditions.").arg(nodes);
}

