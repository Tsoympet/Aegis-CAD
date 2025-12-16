#include "AssemblyViewer.h"

#include <AIS_Shape.hxx>
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
    m_initialized = true;
}

void AssemblyViewer::setDocument(const std::shared_ptr<AssemblyDocument> &doc) {
    m_document = doc;
    displayAssembly();
}

void AssemblyViewer::displayAssembly() {
    if (!m_initialized || !m_document) return;
    m_context->RemoveAll(false);
    auto frames = m_document->computeWorldFrames();
    for (const auto &pair : m_document->nodes()) {
        const AssemblyNode &node = pair.second;
        if (node.shape.IsNull()) continue;
        Handle(AIS_Shape) ais = new AIS_Shape(node.shape);
        if (frames.count(node.id)) {
            ais->SetLocalTransformation(frames[node.id]);
        }
        m_context->Display(ais, Standard_True);
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

