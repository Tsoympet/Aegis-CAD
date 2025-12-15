#include "OccView.h"

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TopoDS_Shape.hxx>
#include <WNT_Window.hxx>
#ifndef _WIN32
#include <Xw_Window.hxx>
#endif

#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

OccView::OccView(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_PaintOnScreen);
    setFocusPolicy(Qt::StrongFocus);
    initializeViewer();
}

OccView::~OccView() = default;

void OccView::initializeViewer() {
#ifdef _WIN32
    Handle(Aspect_DisplayConnection) display = new Aspect_DisplayConnection();
#else
    Handle(Aspect_DisplayConnection) display = new Aspect_DisplayConnection();
#endif
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(display);
    m_viewer = new V3d_Viewer(driver);
    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();

#ifdef _WIN32
    Handle(WNT_Window) window = new WNT_Window((Aspect_Handle)winId());
#else
    Handle(Xw_Window) window = new Xw_Window(display, (Window)winId());
#endif
    m_view = m_viewer->CreateView();
    m_view->SetWindow(window);
    m_view->SetBackgroundColor(Quantity_NOC_BLACK);
    if (!window->IsMapped()) {
        window->Map();
    }

    m_context = new AIS_InteractiveContext(m_viewer);
    m_initialized = true;
}

void OccView::displayShape(const TopoDS_Shape &shape) {
    if (!m_initialized) return;
    m_context->RemoveAll(false);
    Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
    m_context->Display(aisShape, Standard_True);
    m_view->FitAll();
    update();
}

void OccView::paintEvent(QPaintEvent *) {
    if (m_initialized) {
        m_view->Redraw();
    }
}

void OccView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_initialized) {
        m_view->MustBeResized();
    }
}

void OccView::mousePressEvent(QMouseEvent *event) {
    m_lastButton = event->button();
    m_lastPos = event->pos();
}

void OccView::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    m_lastButton = Qt::NoButton;
}

void OccView::mouseMoveEvent(QMouseEvent *event) {
    if (!m_initialized) return;
    const QPoint delta = event->pos() - m_lastPos;
    m_lastPos = event->pos();

    if (m_lastButton == Qt::LeftButton) {
        m_view->Rotate(delta.x(), delta.y());
    } else if (m_lastButton == Qt::MiddleButton) {
        m_view->Pan(delta.x(), -delta.y());
    } else if (m_lastButton == Qt::RightButton) {
        m_view->Zoom(0.0, 0.0, delta.y());
    }
    update();
}

void OccView::wheelEvent(QWheelEvent *event) {
    if (!m_initialized) return;
    const double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    m_view->SetScale(m_view->Scale() * factor);
    update();
}

