#pragma once

#include "assembly/AssemblyDocument.h"

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <QDate>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <vector>

/**
 * @brief Supported 2D/3D projection types for drafting views.
 */
enum class ProjectionType { Top, Front, Right, Iso, Custom };

/**
 * @brief Stores the resulting 2D geometry for a single view.
 */
class ViewProjection {
public:
    ViewProjection(const QString &name, ProjectionType type, const std::vector<QPointF> &outline);

    const QString &name() const { return m_name; }
    ProjectionType type() const { return m_type; }
    const std::vector<QPointF> &outline() const { return m_outline; }

    QRectF bounds() const;

    static ViewProjection fromShape(const QString &name, const TopoDS_Shape &shape, ProjectionType type,
                                    double scale = 1.0);

private:
    static QPointF projectPoint(const gp_Pnt &point, ProjectionType type, double scale);

    QString m_name;
    ProjectionType m_type{ProjectionType::Front};
    std::vector<QPointF> m_outline;
};

struct Dimension {
    QString label;
    QPointF start;
    QPointF end;
    double value{0.0};
};

struct Leader {
    QString text;
    QPointF anchor;
    std::vector<QPointF> segments;
};

struct Annotation {
    QString text;
    QPointF position;
};

struct TitleBlock {
    QString drawingNumber;
    QString revision;
    QString author;
    QString approver;
    QString description;
    QDate date{QDate::currentDate()};
};

struct ScaleBar {
    double scale{1.0};
    int divisions{5};
    double divisionLength{10.0};
};

struct BillOfMaterialRow {
    QString key;
    QString description;
    int quantity{1};
    bool isAssembly{false};
};

/**
 * @brief Document used to collect views, annotations, and title block data.
 */
class DrawingDocument {
public:
    DrawingDocument();

    void setSheetSize(double widthMm, double heightMm);
    double sheetWidth() const { return m_sheetWidth; }
    double sheetHeight() const { return m_sheetHeight; }

    ViewProjection &addProjection(const ViewProjection &view);
    void generateOrthographic(const TopoDS_Shape &shape, double scale = 1.0);
    void generateIso(const TopoDS_Shape &shape, double scale = 1.0);

    void addDimension(const Dimension &dimension);
    void addLeader(const Leader &leader);
    void addAnnotation(const Annotation &annotation);

    void setTitleBlock(const TitleBlock &block);
    void setScaleBar(const ScaleBar &scaleBar);

    void generateBillOfMaterials(const AssemblyDocument &assembly);

    const std::vector<ViewProjection> &views() const { return m_views; }
    const std::vector<Dimension> &dimensions() const { return m_dimensions; }
    const std::vector<Leader> &leaders() const { return m_leaders; }
    const std::vector<Annotation> &annotations() const { return m_annotations; }
    const TitleBlock &titleBlock() const { return m_titleBlock; }
    const ScaleBar &scaleBar() const { return m_scaleBar; }
    const std::vector<BillOfMaterialRow> &bom() const { return m_bom; }

private:
    static QString preferredKey(const AssemblyNode &node);

    double m_sheetWidth{420.0};
    double m_sheetHeight{297.0};
    std::vector<ViewProjection> m_views;
    std::vector<Dimension> m_dimensions;
    std::vector<Leader> m_leaders;
    std::vector<Annotation> m_annotations;
    TitleBlock m_titleBlock;
    ScaleBar m_scaleBar;
    std::vector<BillOfMaterialRow> m_bom;
};

