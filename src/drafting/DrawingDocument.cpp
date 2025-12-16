#include "DrawingDocument.h"

#include <QHash>
#include <QVector>
#include <algorithm>
#include <cmath>

namespace {
constexpr double kIsoX = 0.86602540378; // cos(30)
constexpr double kIsoY = 0.5;           // sin(30)
}

ViewProjection::ViewProjection(const QString &name, ProjectionType type, const std::vector<QPointF> &outline)
    : m_name(name), m_type(type), m_outline(outline) {}

QRectF ViewProjection::bounds() const {
    if (m_outline.empty()) {
        return {};
    }

    double minX = m_outline.front().x();
    double maxX = minX;
    double minY = m_outline.front().y();
    double maxY = minY;

    for (const auto &pt : m_outline) {
        minX = std::min(minX, pt.x());
        maxX = std::max(maxX, pt.x());
        minY = std::min(minY, pt.y());
        maxY = std::max(maxY, pt.y());
    }

    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

ViewProjection ViewProjection::fromShape(const QString &name, const TopoDS_Shape &shape, ProjectionType type,
                                         double scale) {
    Bnd_Box bounds;
    BRepBndLib::Add(shape, bounds);

    if (bounds.IsVoid()) {
        return ViewProjection{name, type, {}};
    }

    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

    std::vector<gp_Pnt> corners = {
        gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMin, zMin), gp_Pnt(xMax, yMax, zMin), gp_Pnt(xMin, yMax, zMin),
        gp_Pnt(xMin, yMin, zMax), gp_Pnt(xMax, yMin, zMax), gp_Pnt(xMax, yMax, zMax), gp_Pnt(xMin, yMax, zMax)};

    // Use a simple bounding box outline for now.
    std::vector<QPointF> outline = {projectPoint(corners[0], type, scale), projectPoint(corners[1], type, scale),
                                    projectPoint(corners[2], type, scale), projectPoint(corners[3], type, scale),
                                    projectPoint(corners[0], type, scale)};

    return ViewProjection{name, type, outline};
}

QPointF ViewProjection::projectPoint(const gp_Pnt &point, ProjectionType type, double scale) {
    switch (type) {
    case ProjectionType::Top:
        return QPointF(point.X() * scale, point.Y() * scale);
    case ProjectionType::Right:
        return QPointF(point.Z() * scale, point.Y() * scale);
    case ProjectionType::Iso: {
        double x = point.X() * kIsoX - point.Y() * kIsoX;
        double y = point.X() * kIsoY + point.Y() * kIsoY - point.Z();
        return QPointF(x * scale, y * scale);
    }
    case ProjectionType::Custom:
    case ProjectionType::Front:
    default:
        return QPointF(point.X() * scale, point.Z() * scale);
    }
}

DrawingDocument::DrawingDocument() = default;

void DrawingDocument::setSheetSize(double widthMm, double heightMm) {
    m_sheetWidth = widthMm;
    m_sheetHeight = heightMm;
}

ViewProjection &DrawingDocument::addProjection(const ViewProjection &view) {
    m_views.push_back(view);
    return m_views.back();
}

void DrawingDocument::generateOrthographic(const TopoDS_Shape &shape, double scale) {
    m_views.push_back(ViewProjection::fromShape("Front", shape, ProjectionType::Front, scale));
    m_views.push_back(ViewProjection::fromShape("Top", shape, ProjectionType::Top, scale));
    m_views.push_back(ViewProjection::fromShape("Right", shape, ProjectionType::Right, scale));
}

void DrawingDocument::generateIso(const TopoDS_Shape &shape, double scale) {
    m_views.push_back(ViewProjection::fromShape("Iso", shape, ProjectionType::Iso, scale));
}

void DrawingDocument::addDimension(const Dimension &dimension) { m_dimensions.push_back(dimension); }

void DrawingDocument::addLeader(const Leader &leader) { m_leaders.push_back(leader); }

void DrawingDocument::addAnnotation(const Annotation &annotation) { m_annotations.push_back(annotation); }

void DrawingDocument::setTitleBlock(const TitleBlock &block) { m_titleBlock = block; }

void DrawingDocument::setScaleBar(const ScaleBar &scaleBar) { m_scaleBar = scaleBar; }

QString DrawingDocument::preferredKey(const AssemblyNode &node) {
    if (!node.partPath.isEmpty()) {
        return node.partPath;
    }
    return node.id;
}

void DrawingDocument::generateBillOfMaterials(const AssemblyDocument &assembly) {
    m_bom.clear();

    QHash<QString, BillOfMaterialRow> byKey;
    const auto &nodes = assembly.nodes();
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        const AssemblyNode &node = it->second;
        QString key = preferredKey(node);
        if (!byKey.contains(key)) {
            BillOfMaterialRow row;
            row.key = key;
            row.description = node.partPath.isEmpty() ? QStringLiteral("Assembly Node %1").arg(node.id) : node.partPath;
            row.quantity = 1;
            row.isAssembly = node.isReferenceAssembly;
            byKey.insert(key, row);
        } else {
            BillOfMaterialRow &row = byKey[key];
            row.quantity += 1;
            row.isAssembly = row.isAssembly || node.isReferenceAssembly;
        }
    }

    m_bom = byKey.values().toVector().toStdVector();
}

