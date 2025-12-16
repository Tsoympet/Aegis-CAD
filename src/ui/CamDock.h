#pragma once

#include "../cam/ToolpathGenerator.h"

#include <QComboBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <functional>
#include <vector>

class OccView;

class CamDock : public QDockWidget {
    Q_OBJECT
public:
    explicit CamDock(const QString &title, QWidget *parent = nullptr);

    void setView(OccView *view);
    void setShapeProvider(const std::function<TopoDS_Shape()> &provider);

private slots:
    void beginFaceSelection();
    void beginEdgeSelection();
    void onFacesPicked(const std::vector<TopoDS_Face> &faces);
    void onEdgesPicked(const std::vector<TopoDS_Edge> &edges);
    void generateToolpath();
    void exportGcode();

private:
    ToolpathGenerator::PostFlavor currentFlavor() const;
    ToolpathGenerator::Operation currentOperation() const;

    OccView *m_view{nullptr};
    ToolpathGenerator m_generator;
    ToolpathGenerator::Toolpath m_lastPath;
    std::function<TopoDS_Shape()> m_shapeProvider;
    std::vector<TopoDS_Face> m_faces;
    std::vector<TopoDS_Edge> m_edges;

    QComboBox *m_operation{nullptr};
    QComboBox *m_flavor{nullptr};
    QDoubleSpinBox *m_depth{nullptr};
    QDoubleSpinBox *m_stepover{nullptr};
    QDoubleSpinBox *m_stepdown{nullptr};
    QDoubleSpinBox *m_feed{nullptr};
    QDoubleSpinBox *m_spindle{nullptr};
    QPushButton *m_selectFaces{nullptr};
    QPushButton *m_selectEdges{nullptr};
    QPushButton *m_generate{nullptr};
    QPushButton *m_export{nullptr};
};

