#pragma once

#include "../assembly/AssemblyDocument.h"
#include "../assembly/ConstraintSolverAsm.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Line.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <V3d_View.hxx>
#include <QToolBar>
#include <QWidget>
#include <memory>
#include <unordered_map>
#include <vector>

/**
 * @brief Viewer specialized for hierarchical assemblies with constraint visualization.
 */
class AssemblyViewer : public QWidget {
    Q_OBJECT
public:
    explicit AssemblyViewer(QWidget *parent = nullptr);
    ~AssemblyViewer() override;

    void setDocument(const std::shared_ptr<AssemblyDocument> &doc);
    void displayAssembly();
    void highlightConstraints(bool enabled);
    void previewMateMotion(const QString &mateId, double parameter);
    void exportMotion(const QString &filePath) const;

    QToolBar *constraintToolbar() const { return m_toolbar; }

protected:
    QPaintEngine *paintEngine() const override { return nullptr; }
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAddMate();
    void onDeleteMate();
    void onToggleSuppress();

private:
    void initializeViewer();
    void drawMateLink(const MateConstraint &mate);
    void recordFrame();

    Handle(V3d_Viewer) m_viewer;
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_View) m_view;
    bool m_initialized{false};

    std::shared_ptr<AssemblyDocument> m_document;
    ConstraintSolverAsm m_solver;
    std::unordered_map<QString, Handle(AIS_Shape)> m_cachedShapes;

    std::vector<std::unordered_map<QString, gp_Trsf>> m_motionFrames;

    QToolBar *m_toolbar{nullptr};
    QWidget *m_canvas{nullptr};
};

