#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <vector>

class ToolpathGenerator {
public:
    enum class Operation { Pocket, Contour, Drilling };
    enum class PostFlavor { GRBL, Fanuc };

    struct Toolpath {
        Operation op{Operation::Pocket};
        QString name;
        std::vector<gp_Pnt> points;
        double feed{800.0};
        double plunge{300.0};
        double spindle{10000.0};
        double safeHeight{5.0};
        double targetDepth{-5.0};

        QString toGCode(PostFlavor flavor) const;
        bool exportToFile(const QString &path, PostFlavor flavor) const;
    };

    ToolpathGenerator();

    void setModel(const TopoDS_Shape &shape);
    void setSelections(const std::vector<TopoDS_Face> &faces, const std::vector<TopoDS_Edge> &edges);

    Toolpath generatePocket(double stepover, double stepdown, double depth, double safeHeight, double feed, double spindle) const;
    Toolpath generateContour(double depth, double safeHeight, double feed, double spindle) const;
    Toolpath generateDrilling(double depth, double peck, double safeHeight, double feed, double spindle) const;

private:
    std::vector<gp_Pnt> contourLoopFromEdges(double depth) const;
    std::vector<gp_Pnt> pocketOutline(double depth, double stepover, double stepdown) const;
    std::vector<gp_Pnt> drillPoints(double depth, double peck) const;

    TopoDS_Shape m_model;
    std::vector<TopoDS_Face> m_faces;
    std::vector<TopoDS_Edge> m_edges;
};

