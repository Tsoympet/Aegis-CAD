#include "ToolpathGenerator.h"

#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <QStringList>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <cmath>

ToolpathGenerator::ToolpathGenerator() = default;

void ToolpathGenerator::setModel(const TopoDS_Shape &shape) {
    m_model = shape;
}

void ToolpathGenerator::setSelections(const std::vector<TopoDS_Face> &faces, const std::vector<TopoDS_Edge> &edges) {
    m_faces = faces;
    m_edges = edges;
}

ToolpathGenerator::Toolpath ToolpathGenerator::generatePocket(double stepover, double stepdown, double depth, double safeHeight, double feed, double spindle) const {
    Toolpath path;
    path.op = Operation::Pocket;
    path.name = QStringLiteral("Pocket");
    path.feed = feed;
    path.spindle = spindle;
    path.safeHeight = safeHeight;
    path.targetDepth = depth;
    path.points = pocketOutline(depth, stepover, stepdown);
    return path;
}

ToolpathGenerator::Toolpath ToolpathGenerator::generateContour(double depth, double safeHeight, double feed, double spindle) const {
    Toolpath path;
    path.op = Operation::Contour;
    path.name = QStringLiteral("Contour");
    path.feed = feed;
    path.spindle = spindle;
    path.safeHeight = safeHeight;
    path.targetDepth = depth;
    path.points = contourLoopFromEdges(depth);
    return path;
}

ToolpathGenerator::Toolpath ToolpathGenerator::generateDrilling(double depth, double peck, double safeHeight, double feed, double spindle) const {
    Toolpath path;
    path.op = Operation::Drilling;
    path.name = QStringLiteral("Drill");
    path.feed = feed;
    path.plunge = feed;
    path.spindle = spindle;
    path.safeHeight = safeHeight;
    path.targetDepth = depth;
    path.points = drillPoints(depth, peck);
    return path;
}

std::vector<gp_Pnt> ToolpathGenerator::contourLoopFromEdges(double depth) const {
    std::vector<gp_Pnt> pts;
    const double z = depth;
    if (!m_edges.empty()) {
        for (const auto &edge : m_edges) {
            BRepAdaptor_Curve c(edge);
            GCPnts_UniformAbscissa sampler(c, 5.0);
            const int count = sampler.IsDone() ? sampler.NbPoints() : 0;
            for (int i = 1; i <= count; ++i) {
                const gp_Pnt p = sampler.Value(i);
                pts.emplace_back(p.X(), p.Y(), z);
            }
        }
    }

    if (pts.empty() && !m_model.IsNull()) {
        Bnd_Box box;
        BRepBndLib::Add(m_model, box);
        double xmin, ymin, zmin, xmax, ymax, zmax;
        box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        pts.emplace_back(xmin, ymin, z);
        pts.emplace_back(xmax, ymin, z);
        pts.emplace_back(xmax, ymax, z);
        pts.emplace_back(xmin, ymax, z);
        pts.emplace_back(xmin, ymin, z);
    }
    return pts;
}

std::vector<gp_Pnt> ToolpathGenerator::pocketOutline(double depth, double stepover, double stepdown) const {
    std::vector<gp_Pnt> pts;
    if (stepover <= 0.0) return pts;

    Bnd_Box box;
    if (!m_faces.empty()) {
        for (const auto &f : m_faces) {
            BRepBndLib::Add(f, box);
        }
    } else if (!m_model.IsNull()) {
        BRepBndLib::Add(m_model, box);
    }

    double xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    const double width = xmax - xmin;
    const double height = ymax - ymin;
    const int passes = static_cast<int>(std::max(width, height) / stepover) + 1;
    const double effectiveStep = std::abs(stepdown) < 1e-3 ? depth : stepdown;
    for (double z = 0.0; z >= depth; z -= std::abs(effectiveStep)) {
        for (int i = 0; i < passes; ++i) {
            const double offset = i * stepover * 0.5;
            const double x0 = xmin + offset;
            const double x1 = xmax - offset;
            const double y0 = ymin + offset;
            const double y1 = ymax - offset;
            if (x0 >= x1 || y0 >= y1) break;
            if (i % 2 == 0) {
                pts.emplace_back(x0, y0, z);
                pts.emplace_back(x1, y0, z);
                pts.emplace_back(x1, y1, z);
            } else {
                pts.emplace_back(x1, y1, z);
                pts.emplace_back(x0, y1, z);
                pts.emplace_back(x0, y0, z);
            }
        }
    }

    if (pts.empty()) {
        pts = contourLoopFromEdges(depth);
    }
    return pts;
}

std::vector<gp_Pnt> ToolpathGenerator::drillPoints(double depth, double peck) const {
    std::vector<gp_Pnt> pts;
    const double step = std::abs(peck) < 1e-3 ? std::abs(depth) : std::abs(peck);
    if (!m_faces.empty()) {
        for (const auto &face : m_faces) {
            GProp_GProps props;
            BRepGProp::SurfaceProperties(face, props);
            const gp_Pnt c = props.CentreOfMass();
            for (double z = 0.0; z >= depth; z -= step) {
                pts.emplace_back(c.X(), c.Y(), z);
            }
            pts.emplace_back(c.X(), c.Y(), depth);
        }
    }

    if (pts.empty() && !m_model.IsNull()) {
        Bnd_Box box;
        BRepBndLib::Add(m_model, box);
        double xmin, ymin, zmin, xmax, ymax, zmax;
        box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        const gp_Pnt c((xmin + xmax) * 0.5, (ymin + ymax) * 0.5, depth);
        pts.push_back(c);
    }
    return pts;
}

QString ToolpathGenerator::Toolpath::toGCode(PostFlavor flavor) const {
    QStringList lines;
    lines << QStringLiteral("(AegisCAD %1)").arg(name);
    lines << QStringLiteral("G21") << QStringLiteral("G90");
    lines << QStringLiteral("S%1 M3").arg(spindle, 0, 'f', 0);
    lines << QStringLiteral("G0 Z%1").arg(safeHeight, 0, 'f', 3);

    double lastFeed = feed;
    for (size_t i = 0; i < points.size(); ++i) {
        const gp_Pnt &p = points[i];
        const bool plungeMove = (op == Operation::Drilling) && (i > 0) && (points[i - 1].X() == p.X()) && (points[i - 1].Y() == p.Y());
        const double appliedFeed = plungeMove ? plunge : feed;
        if (appliedFeed != lastFeed) {
            lines << QStringLiteral("F%1").arg(appliedFeed, 0, 'f', 2);
            lastFeed = appliedFeed;
        }
        const QString mode = i == 0 ? QStringLiteral("G0") : QStringLiteral("G1");
        lines << QStringLiteral("%1 X%2 Y%3 Z%4")
                    .arg(mode)
                    .arg(p.X(), 0, 'f', 3)
                    .arg(p.Y(), 0, 'f', 3)
                    .arg(p.Z(), 0, 'f', 3);
    }

    if (flavor == PostFlavor::Fanuc) {
        lines.push_front(QStringLiteral("O0001"));
        lines << QStringLiteral("M30");
    } else {
        lines << QStringLiteral("M5") << QStringLiteral("G0 Z%1").arg(safeHeight, 0, 'f', 3) << QStringLiteral("M2");
    }
    return lines.join('\n');
}

bool ToolpathGenerator::Toolpath::exportToFile(const QString &path, PostFlavor flavor) const {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream stream(&f);
    stream << toGCode(flavor);
    stream.flush();
    f.close();
    return true;
}

