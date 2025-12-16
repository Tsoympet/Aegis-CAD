#include "AssemblyViewer.h"

#include <AIS_Shape.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TopoDS_Shape.hxx>
#include <WNT_Window.hxx>
#ifndef _WIN32
#include <Xw_Window.hxx>
#endif

#include <QAction>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <gp_Pnt.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>

AssemblyViewer::AssemblyViewer(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_PaintOnScreen);
    setFocusPolicy(Qt::StrongFocus);

    m_toolbar = new QToolBar(this);
    m_toolbar->addAction("Add", this, &AssemblyViewer::onAddMate);
    m_toolbar->addAction("Delete", this, &AssemblyViewer::onDeleteMate);
    m_toolbar->addAction("Suppress", this, &AssemblyViewer::onToggleSuppress);

    m_canvas = new QWidget(this);
    m_canvas->setAttribute(Qt::WA_NoSystemBackground);
    m_canvas->setAttribute(Qt::WA_PaintOnScreen);
    m_canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_toolbar);
    layout->addWidget(m_canvas, 1);
    setLayout(layout);

    initializeViewer();
}

AssemblyViewer::~AssemblyViewer() = default;

void AssemblyViewer::initializeViewer() {
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
    Handle(WNT_Window) window = new WNT_Window((Aspect_Handle)m_canvas->winId());
#else
    Handle(Xw_Window) window = new Xw_Window(display, (Window)m_canvas->winId());
#endif
    m_view = m_viewer->CreateView();
    m_view->SetWindow(window);
    if (!window->IsMapped()) {
        window->Map();
    }

    m_context = new AIS_InteractiveContext(m_viewer);
    Graphic3d_RenderingParams &params = m_view->ChangeRenderingParams();
#ifdef GRAPHIC3D_RENDERINGPARAMS_V2
    params.ToFrustumCulling = Standard_True;
#else
    params.IsFrustumCullingEnabled = Standard_True;
#endif
    params.CullingDistance = 5000.0;
    m_context->SetAutoActivateSelection(false);
    m_initialized = true;
}

void AssemblyViewer::setDocument(const std::shared_ptr<AssemblyDocument> &doc) {
    m_document = doc;
    displayAssembly();
}

void AssemblyViewer::displayAssembly() {
    if (!m_initialized || !m_document) return;
    m_context->RemoveAll(false);
    m_cachedShapes.clear();
    auto frames = m_document->computeWorldFrames();
    for (const auto &pair : m_document->nodes()) {
        const AssemblyNode &node = pair.second;
        if (node.shape.IsNull()) continue;
        const QString cacheKey = QString::number(reinterpret_cast<std::intptr_t>(node.shape.TShape().get()));
        Handle(AIS_Shape) base;
        auto found = m_cachedShapes.find(cacheKey);
        if (found != m_cachedShapes.end()) {
            base = found->second;
        } else {
            base = new AIS_Shape(node.shape);
            m_cachedShapes.emplace(cacheKey, base);
        }

        Handle(AIS_InteractiveObject) toDisplay;
        if (found != m_cachedShapes.end()) {
            Handle(AIS_ConnectedInteractive) inst = new AIS_ConnectedInteractive(base);
            toDisplay = inst;
        } else {
            toDisplay = base;
        }

        if (frames.count(node.id)) {
            toDisplay->SetLocalTransformation(frames[node.id]);
        }

        // Distance-based deflection for coarse LOD on far items
        Bnd_Box bbox;
        BRepBndLib::Add(node.shape, bbox);
        const gp_Pnt center = bbox.Center();
        const gp_Pnt eye = m_view->Camera()->Eye();
        const double dist = eye.Distance(center);
        Handle(AIS_Shape) lodShape = Handle(AIS_Shape)::DownCast(toDisplay);
        if (!lodShape.IsNull()) {
            const double deflection = dist > 2500.0 ? 0.5 : 0.1;
            lodShape->Attributes()->SetDeviationCoefficient(deflection);
        }

        m_context->Display(toDisplay, Standard_False);
    }
    m_view->FitAll();
    update();
}

void AssemblyViewer::highlightConstraints(bool enabled) {
    if (!m_initialized || !m_document) return;
    if (!enabled) {
        m_context->RemoveAll(false);
        displayAssembly();
        return;
    }
    displayAssembly();
    for (const auto &mate : m_document->mates()) {
        if (mate.suppressed) continue;
        drawMateLink(mate);
    }
    m_view->Redraw();
}

void AssemblyViewer::previewMateMotion(const QString &mateId, double parameter) {
    if (!m_document) return;
    auto frames = m_document->computeWorldFrames();
    for (const auto &mate : m_document->mates()) {
        if (mate.id != mateId) continue;
        MateConstraint preview = mate;
        preview.limitMax = parameter;
        gp_Trsf trsf = ConstraintSolverAsm::alignFrames(preview);
        if (auto *node = m_document->getNode(mate.b)) {
            node->localTransform = trsf * frames[mate.a];
        }
        break;
    }
    recordFrame();
    displayAssembly();
}

void AssemblyViewer::exportMotion(const QString &filePath) const {
    QJsonArray frames;
    for (const auto &frame : m_motionFrames) {
        QJsonObject obj;
        for (const auto &kv : frame) {
            const gp_Trsf &t = kv.second;
            QJsonArray values;
            for (int r = 1; r <= 3; ++r) {
                for (int c = 1; c <= 4; ++c) {
                    values.append(t.Value(r, c));
                }
            }
            obj[kv.first] = values;
        }
        frames.append(obj);
    }
    QJsonObject root;
    root["frames"] = frames;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

void AssemblyViewer::paintEvent(QPaintEvent *) {
    if (m_initialized) {
        m_view->Redraw();
    }
}

void AssemblyViewer::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_initialized) {
        m_view->MustBeResized();
    }
}

void AssemblyViewer::onAddMate() {
    if (!m_document) return;
    MateConstraint m;
    m.id = QStringLiteral("mate_%1").arg(m_document->mates().size());
    m_document->addMate(m);
}

void AssemblyViewer::onDeleteMate() {
    if (!m_document || m_document->mates().empty()) return;
    m_document->removeMate(m_document->mates().back().id);
    displayAssembly();
}

void AssemblyViewer::onToggleSuppress() {
    if (!m_document || m_document->mates().empty()) return;
    const auto &last = m_document->mates().back();
    m_document->suppressMate(last.id, !last.suppressed);
    highlightConstraints(true);
}

void AssemblyViewer::drawMateLink(const MateConstraint &mate) {
    const AssemblyNode *a = m_document->getNode(mate.a);
    const AssemblyNode *b = m_document->getNode(mate.b);
    if (!a || !b) return;
    auto frames = m_document->computeWorldFrames();
    if (!frames.count(mate.a) || !frames.count(mate.b)) return;
    gp_Pnt pa = gp_Pnt(0, 0, 0).Transformed(frames[mate.a]);
    gp_Pnt pb = gp_Pnt(0, 0, 0).Transformed(frames[mate.b]);
    Handle(AIS_Line) line = new AIS_Line(pa, pb);
    m_context->Display(line, Standard_False);
}

void AssemblyViewer::recordFrame() {
    if (!m_document) return;
    m_motionFrames.push_back(m_document->computeWorldFrames());
}

