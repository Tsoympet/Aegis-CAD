#pragma once
#include <QWidget>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ColoredShape.hxx>
#include <V3d_View.hxx>
#include <TopoDS_Shape.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <memory>

/// 3D viewer with analysis overlay (stress / temperature).
class OccView : public QWidget
{
    Q_OBJECT
public:
    explicit OccView(QWidget* parent = nullptr);
    ~OccView() override;

    void displayShape(const TopoDS_Shape& shape, const QColor& color = Qt::lightGray);
    void loadBrepModel(const QString& brepPath);

public slots:
    /// Visualizes analysis results (.csv or .json with node + value)
    void showAnalysisResults(const QString& dataPath);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    void applyColorMap(const TopoDS_Shape& shape,
                       const std::vector<std::pair<gp_Pnt,double>>& nodalValues);
    Quantity_Color colorForValue(double val, double min, double max) const;

    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_View) m_view;
    QPoint m_lastMousePos;
    std::unique_ptr<Graphic3d_GraduatedTrihedron> m_legend;
};
