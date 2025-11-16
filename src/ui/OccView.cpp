#include "OccView.h"
#include <OpenGl_GraphicDriver.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <QMouseEvent>
#include <QDebug>

OccView::OccView(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    initViewer();

    // demo box
    TopoDS_Shape box = BRepPrimAPI_MakeBox(100, 80, 60).Shape();
    displayShape(box);
    fitAll();
}

OccView::~OccView() = default;

void OccView::initViewer()
{
    m_displayConnection = new Aspect_DisplayConnection();
    m_graphicDriver = new OpenGl_GraphicDriver(m_displayConnection);
    m_viewer = new V3d_Viewer(m_graphicDriver);
    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();

    m_view = m_viewer->CreateView();
    m_context = new AIS_InteractiveContext(m_viewer);
}

void OccView::paintEvent(QPaintEvent*)
{
    if (m_view.IsNull()) return;
    m_view->Redraw();
}

void OccView::resizeEvent(QResizeEvent*)
{
    if (!m_view.IsNull())
        m_view->MustBeResized();
}

void OccView::mousePressEvent(QMouseEvent* ev)
{
    m_lastMousePos = ev->pos();
}

void OccView::mouseMoveEvent(QMouseEvent* ev)
{
    if (m_view.IsNull()) return;
    QPoint delta = ev->pos() - m_lastMousePos;
    m_lastMousePos = ev->pos();

    if (ev->buttons() & Qt::LeftButton)
    {
        m_view->Rotation(ev->x(), ev->y());
    }
    else if (ev->buttons() & Qt::RightButton)
    {
        m_view->Pan(delta.x(), -delta.y());
    }
    updateView();
}

void OccView::wheelEvent(QWheelEvent* ev)
{
    if (m_view.IsNull()) return;
    const double factor = ev->angleDelta().y() > 0 ? 1.1 : 0.9;
    m_view->Zoom(0.5, 0.5, factor);
    updateView();
}

void OccView::updateView()
{
    if (!m_view.IsNull())
        m_view->Redraw();
}

void OccView::displayShape(const TopoDS_Shape& shape)
{
    if (m_context.IsNull()) return;
    Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
    m_context->Display(aisShape, AIS_Shaded, 0, false);
}

void OccView::clearScene()
{
    if (m_context.IsNull()) return;
    m_context->RemoveAll(true);
}

void OccView::fitAll()
{
    if (!m_view.IsNull())
    {
        m_view->FitAll();
        updateView();
    }
}
