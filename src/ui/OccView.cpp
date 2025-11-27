#include "OccView.h"
#include <V3d_Viewer.hxx>
#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRep_Builder.hxx>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

OccView::OccView(QWidget* parent)
    : QWidget(parent)
{
    Handle(Aspect_DisplayConnection) dc = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(dc);

    Handle(V3d_Viewer) viewer = new V3d_Viewer(driver);
    viewer->SetDefaultLights();
    viewer->SetLightOn();
    m_view = viewer->CreateView();

    m_context = new AIS_InteractiveContext(viewer);
    setFocusPolicy(Qt::StrongFocus);
}

OccView::~OccView() = default;

void OccView::paintEvent(QPaintEvent*)
{
    if (!m_view.IsNull())
        m_view->Redraw();
}

void OccView::resizeEvent(QResizeEvent*)
{
    if (!m_view.IsNull())
        m_view->MustBeResized();
}

void OccView::wheelEvent(QWheelEvent* e)
{
    if (e->angleDelta().y() > 0)
        m_view->Zoom(0.8);
    else
        m_view->Zoom(1.2);
}

void OccView::mousePressEvent(QMouseEvent* e)
{
    m_lastMousePos = e->pos();
}

void OccView::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_view->Rotation(e->x(), e->y());
    } else if (e->buttons() & Qt::RightButton) {
        int dx = e->x() - m_lastMousePos.x();
        int dy = e->y() - m_lastMousePos.y();
        m_view->Pan(dx, -dy);
    }
    m_lastMousePos = e->pos();
}

void OccView::displayShape(const TopoDS_Shape& shape, const QColor& color)
{
    if (m_context.IsNull()) return;

    m_context->RemoveAll(true);
    Handle(AIS_Shape) ais = new AIS_Shape(shape);
    Quantity_Color qcol(color.redF(), color.greenF(), color.blueF(), Quantity_TOC_RGB);
    m_context->Display(ais, Standard_True);
    m_context->SetColor(ais, qcol, Standard_True);
    m_context->UpdateCurrentViewer();
    m_view->FitAll();
}

void OccView::showAnalysisResults(const QString& dataPath)
{
    qDebug() << "Displaying analysis overlay for:" << dataPath;
    // Placeholder: in full version, load stress/temperature field mapping.
}

void OccView::loadBrepModel(const QString& brepPath)
{
    if (!QFile::exists(brepPath)) {
        qDebug() << "BREP file not found:" << brepPath;
        return;
    }

    BRep_Builder builder;
    TopoDS_Shape shape;
    if (!BRepTools::Read(shape, brepPath.toStdString().c_str(), builder)) {
        qDebug() << "Failed to load BREP model.";
        return;
    }

    displayShape(shape, QColor("#a0c0ff"));
    qDebug() << "Loaded reverse-engineered model:" << brepPath;
}
