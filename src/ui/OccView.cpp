#include "OccView.h"

#include "AnalysisLegendOverlay.h"

#include <AIS_ColoredShape.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <AIS_PolyLine.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Precision.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Precision.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Precision.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Precision.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GProp_GProps.hxx>
#include <WNT_Window.hxx>
#ifndef _WIN32
#include <Xw_Window.hxx>
#endif

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QWheelEvent>
#include <algorithm>

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
    configureCulling();
    m_stats = {};
    m_stats.gpuMemoryMB = driver->InquireTextureMemory() / 1024.0 / 1024.0;
    m_initialized = true;
}

void OccView::displayShape(const TopoDS_Shape &shape) {
    if (!m_initialized) return;
    m_parts.clear();
    m_cachedParts.clear();
    m_context->RemoveAll(false);
    Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
    m_parts.emplace("active", aisShape);
    m_cachedParts.emplace("active", aisShape);
    m_context->Display(aisShape, Standard_True);
    m_view->FitAll();
    update();
}

void OccView::displayPart(const QString &id, const TopoDS_Shape &shape, const Quantity_Color &color) {
    if (!m_initialized) return;
    Handle(AIS_Shape) baseShape;
    const QString cacheKey = QString::number(reinterpret_cast<std::intptr_t>(shape.TShape().get()));
    auto cached = m_cachedParts.find(cacheKey);
    if (cached != m_cachedParts.end()) {
        baseShape = cached->second;
    } else {
        baseShape = new AIS_Shape(shape);
        baseShape->SetColor(color);
        if (m_cachedParts.size() >= m_cacheLimit) {
            m_cachedParts.clear();
        }
        m_cachedParts.emplace(cacheKey, baseShape);
    }

    Handle(AIS_InteractiveObject) displayed;
    if (cached != m_cachedParts.end()) {
        Handle(AIS_ConnectedInteractive) inst = new AIS_ConnectedInteractive(baseShape);
        displayed = inst;
    } else {
        displayed = baseShape;
    }

    m_parts[id] = displayed;

    // Level of detail: relax deflection for far components
    Bnd_Box bbox;
    BRepBndLib::Add(shape, bbox);
    const gp_Pnt center = bbox.Center();
    const double dist = viewDistanceTo(center);
    const double lodFactor = dist > m_lodDistance ? m_lodCoarse : 0.1;
    Handle(AIS_Shape) lodShape = Handle(AIS_Shape)::DownCast(displayed);
    if (!lodShape.IsNull()) {
        lodShape->Attributes()->SetDeviationCoefficient(lodFactor);
    }

    m_context->Display(displayed, Standard_True);
    m_view->FitAll();
    updateFrameStats();
    update();
}

void OccView::clearView() {
    if (!m_initialized) return;
    m_parts.clear();
    m_cachedParts.clear();
    clearToolpathPreview();
    m_context->RemoveAll(false);
    m_view->FitAll();
    update();
}

void OccView::zoomToFit() {
    if (!m_initialized) return;
    m_view->FitAll();
    update();
}

void OccView::setPartVisible(const QString &id, bool visible) {
    if (!m_initialized) return;
    auto it = m_parts.find(id);
    if (it == m_parts.end()) return;
    if (visible) {
        m_context->Display(it->second, Standard_True);
    } else {
        m_context->Erase(it->second, Standard_True);
    }
    update();
}

void OccView::setFeatureColor(const QString &id, const Quantity_Color &color) {
    auto it = m_parts.find(id);
    if (it == m_parts.end()) return;
    Handle(AIS_InteractiveObject) ais = it->second;
    Handle(AIS_Shape) asShape = Handle(AIS_Shape)::DownCast(ais);
    if (!asShape.IsNull()) {
        asShape->SetColor(color);
        m_context->Redisplay(asShape, Standard_True);
        update();
    }
    it->second->SetColor(color);
    m_context->Redisplay(it->second, Standard_True);
    update();
}

void OccView::enableSectionPlane(const gp_Pln &plane) {
    if (!m_initialized) return;
    m_clipPlane = new Graphic3d_ClipPlane(plane);
    updateClipPlanes();
}

void OccView::disableSectionPlane() {
    if (!m_initialized) return;
    if (!m_clipPlane.IsNull()) {
        m_view->RemoveClipPlane(m_clipPlane);
    }
    m_clipPlane.Nullify();
    m_view->Redraw();
}

void OccView::enableCamSelection(bool faces, bool edges) {
    m_camSelectFaces = faces;
    m_camSelectEdges = edges;
    if (!m_initialized) return;
    m_context->Deactivate();
    if (faces) {
        m_context->ActivateStandardMode(TopAbs_FACE);
    }
    if (edges) {
        m_context->ActivateStandardMode(TopAbs_EDGE);
    }
}

void OccView::previewToolpath(const std::vector<gp_Pnt> &points, const Quantity_Color &color) {
    if (!m_initialized || points.size() < 2) return;
    clearToolpathPreview();
    TColgp_Array1OfPnt arr(1, static_cast<int>(points.size()));
    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        arr.SetValue(i + 1, points[static_cast<size_t>(i)]);
    }
    Handle(AIS_PolyLine) poly = new AIS_PolyLine(arr);
    poly->SetColor(color);
    m_toolpaths.push_back(poly);
    m_context->Display(poly, Standard_True);
    m_view->FitAll();
    update();
}

void OccView::clearToolpathPreview() {
    if (!m_initialized) return;
    for (auto &pl : m_toolpaths) {
        m_context->Remove(pl, false);
    }
    m_toolpaths.clear();
}

void OccView::attachLegend(AnalysisLegendOverlay *legend) {
    m_legend = legend;
}

void OccView::setLoDScaling(double lodDistance, double coarse) {
    m_lodDistance = lodDistance;
    m_lodCoarse = coarse;
}

void OccView::setTessellationCacheLimit(std::size_t maxEntries) {
    m_cacheLimit = std::max<std::size_t>(1, maxEntries);
}

Quantity_Color OccView::interpolateColor(double t) const {
    t = std::clamp(t, 0.0, 1.0);
    // blue (low) -> yellow -> red (high)
    if (t < 0.5) {
        const double alpha = t * 2.0;
        return Quantity_Color(0.0 + alpha * 1.0, 0.47 + alpha * 0.3, 1.0 - alpha * 0.3, Quantity_TOC_RGB);
    }
    const double alpha = (t - 0.5) * 2.0;
    return Quantity_Color(1.0, 0.77 + alpha * 0.2, 0.7 - alpha * 0.7, Quantity_TOC_RGB);
}

void OccView::applyFieldSamples(const QString &id, const std::vector<std::pair<gp_Pnt, double>> &samples, double minVal, double maxVal) {
    auto it = m_parts.find(id);
    if (it == m_parts.end()) return;
    if (samples.empty()) return;

    Handle(AIS_Shape) base = Handle(AIS_Shape)::DownCast(it->second);
    if (base.IsNull()) return;

    const TopoDS_Shape shape = base->Shape();
    Handle(AIS_ColoredShape) colored = new AIS_ColoredShape(shape);

    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next()) {
        const TopoDS_Face &face = TopoDS::Face(exp.Current());
        GProp_GProps props;
        BRepGProp::SurfaceProperties(face, props);
        const gp_Pnt center = props.CentreOfMass();
        double nearest = samples.front().second;
        double bestDist = center.Distance(samples.front().first);
        for (const auto &sample : samples) {
            const double d = center.Distance(sample.first);
            if (d < bestDist) {
                bestDist = d;
                nearest = sample.second;
            }
        }
        double t = (maxVal - minVal) < Precision::Confusion() ? 0.0 : (nearest - minVal) / (maxVal - minVal);
        colored->SetCustomColor(face, interpolateColor(t));
    }

    m_context->Remove(base, false);
    m_parts[id] = colored;
    m_context->Display(colored, Standard_True);
    m_view->Redraw();
    update();
}

void OccView::clearAnalysisColoring() {
    for (auto &pair : m_parts) {
        Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(pair.second);
        if (!shape.IsNull()) {
            shape->UnsetColor();
            m_context->Redisplay(shape, Standard_True);
        }
    }
    update();
}

void OccView::updateClipPlanes() {
    if (m_clipPlane.IsNull()) return;
    m_view->RemoveClipPlane(m_clipPlane);
    m_view->AddClipPlane(m_clipPlane);
    m_view->Redraw();
}

void OccView::configureCulling() {
    if (m_view.IsNull() || m_context.IsNull()) return;
    Graphic3d_RenderingParams &params = m_view->ChangeRenderingParams();
#ifdef GRAPHIC3D_RENDERINGPARAMS_V2
    params.ToFrustumCulling = Standard_True;
#else
    params.IsFrustumCullingEnabled = Standard_True;
#endif
    params.IsZLayeredCullingEnabled = Standard_True;
    params.CullingDistance = m_lodDistance * 3.0;

    // Enable fast GPU memory reuse for repeated objects.
    m_context->SetAutoActivateSelection(false);
    m_context->DefaultDrawer()->SetFaceBoundaryDraw(false);
}

void OccView::updateFrameStats() {
    static QElapsedTimer timer;
    static int frames = 0;
    if (!timer.isValid()) {
        timer.start();
    }
    ++frames;
    const qint64 elapsed = timer.elapsed();
    if (elapsed > 0) {
        m_stats.fps = frames * 1000.0 / elapsed;
        m_stats.frameTimeMs = elapsed / static_cast<double>(frames);
    }
    m_stats.cachedTessellations = m_cachedParts.size();
    m_stats.activeParts = m_parts.size();
}

double OccView::viewDistanceTo(const gp_Pnt &point) const {
    if (m_view.IsNull()) return 0.0;
    const auto camera = m_view->Camera();
    const gp_Pnt eye = camera->Eye();
    return eye.Distance(point);
}

void OccView::paintEvent(QPaintEvent *) {
    if (m_initialized) {
        m_view->Redraw();
        updateFrameStats();
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
    if (m_initialized && (m_camSelectFaces || m_camSelectEdges) && event->button() == Qt::LeftButton) {
        m_context->MoveTo(event->pos().x(), event->pos().y(), m_view, true);
        m_context->Select(true);
        std::vector<TopoDS_Face> faces;
        std::vector<TopoDS_Edge> edges;
        for (m_context->InitSelected(); m_context->MoreSelected(); m_context->NextSelected()) {
            const TopoDS_Shape sel = m_context->SelectedShape();
            if (m_camSelectFaces && sel.ShapeType() == TopAbs_FACE) {
                faces.push_back(TopoDS::Face(sel));
            }
            if (m_camSelectEdges && sel.ShapeType() == TopAbs_EDGE) {
                edges.push_back(TopoDS::Edge(sel));
            }
        }
        if (!faces.empty()) emit camFacesPicked(faces);
        if (!edges.empty()) emit camEdgesPicked(edges);
    }
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

