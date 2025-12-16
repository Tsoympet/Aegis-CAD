#pragma once

#include <QWidget>
#include <AIS_InteractiveContext.hxx>
#include <AIS_PolyLine.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <memory>
#include <unordered_map>
#include <vector>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

class AnalysisLegendOverlay;
#include <V3d_View.hxx>
#include <memory>
#include <unordered_map>

class OccView : public QWidget {
    Q_OBJECT
public:
    explicit OccView(QWidget *parent = nullptr);
    ~OccView() override;

    void displayShape(const TopoDS_Shape &shape);
    void displayPart(const QString &id, const TopoDS_Shape &shape, const Quantity_Color &color = Quantity_Color(0.8, 0.8, 0.8, Quantity_TOC_RGB));
    void setPartVisible(const QString &id, bool visible);
    void setFeatureColor(const QString &id, const Quantity_Color &color);
    void enableSectionPlane(const gp_Pln &plane);
    void disableSectionPlane();
    void enableCamSelection(bool faces, bool edges);
    void previewToolpath(const std::vector<gp_Pnt> &points, const Quantity_Color &color = Quantity_Color(0.0, 0.8, 0.1, Quantity_TOC_RGB));
    void clearToolpathPreview();

    void attachLegend(AnalysisLegendOverlay *legend);
    void applyFieldSamples(const QString &id, const std::vector<std::pair<gp_Pnt, double>> &samples, double minVal, double maxVal);
    void clearAnalysisColoring();

signals:
    void camFacesPicked(const std::vector<TopoDS_Face> &faces);
    void camEdgesPicked(const std::vector<TopoDS_Edge> &edges);

protected:
    QPaintEngine *paintEngine() const override { return nullptr; }
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void initializeViewer();
    void updateClipPlanes();
    Quantity_Color interpolateColor(double t) const;

    Handle(V3d_Viewer) m_viewer;
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_View) m_view;
    Handle(Graphic3d_ClipPlane) m_clipPlane;
    bool m_initialized{false};
    Qt::MouseButton m_lastButton{Qt::NoButton};
    QPoint m_lastPos;
    std::unordered_map<QString, Handle(AIS_InteractiveObject)> m_parts;
    AnalysisLegendOverlay *m_legend{nullptr};
    bool m_camSelectFaces{false};
    bool m_camSelectEdges{false};
    std::vector<Handle(AIS_PolyLine)> m_toolpaths;
    std::unordered_map<QString, Handle(AIS_Shape)> m_parts;
};

