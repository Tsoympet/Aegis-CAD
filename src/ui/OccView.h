#pragma once

#include <QWidget>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <memory>

class OccView : public QWidget {
    Q_OBJECT
public:
    explicit OccView(QWidget *parent = nullptr);
    ~OccView() override;

    void displayShape(const TopoDS_Shape &shape);

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

    Handle(V3d_Viewer) m_viewer;
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_View) m_view;
    bool m_initialized{false};
    Qt::MouseButton m_lastButton{Qt::NoButton};
    QPoint m_lastPos;
};

