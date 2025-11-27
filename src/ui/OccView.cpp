#include "OccView.h"
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
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
    m_view->SetBackgroundColor(Quantity_NOC_BLACK);
    setFocusPolicy(Qt::StrongFocus);

    // 🔹 Create the analysis legend overlay (hidden by default)
    m_legendOverlay = std::make_unique<AnalysisLegendOverlay>(this);
    m_legendOverlay->move(width() - 180, 20);
    m_legendOverlay->hide();
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
    if (m_legendOverlay)
        m_legendOverlay->move(width() - 180, 20);
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
    m_context->RemoveAll(true);
    Handle(AIS_Shape) ais = new AIS_Shape(shape);
    Quantity_Color qcol(color.redF(), color.greenF(), color.blueF(), Quantity_TOC_RGB);
    m_context->Display(ais, true);
    m_context->SetColor(ais, qcol, true);
    m_context->UpdateCurrentViewer();
    m_view->FitAll();
}

void OccView::loadBrepModel(const QString& path)
{
    BRep_Builder b;
    TopoDS_Shape s;
    if (!BRepTools::Read(s, path.toStdString().c_str(), b)) {
        qDebug() << "BREP load fail";
        return;
    }
    displayShape(s, QColor("#a0c0ff"));
}

// =============================================================
//        ANALYSIS RESULT VISUALIZATION + LEGEND OVERLAY
// =============================================================
void OccView::showAnalysisResults(const QString& dataPath)
{
    QFile f(dataPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open analysis data";
        return;
    }

    QTextStream ts(&f);
    std::vector<std::pair<gp_Pnt, double>> nodes;
    while (!ts.atEnd()) {
        const QString line = ts.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;

        const QStringList p = line.split(',');
        if (p.size() < 4)
            continue;

        nodes.push_back({gp_Pnt(p[0].toDouble(), p[1].toDouble(), p[2].toDouble()), p[3].toDouble()});
    }
    f.close();

    if (nodes.empty()) {
        qDebug() << "No analysis values found";
        return;
    }

    // Get current displayed shape
    TopoDS_Shape shape;
    if (!m_context->DisplayedObjects()->IsEmpty()) {
        Handle(AIS_InteractiveObject) obj = m_context->DisplayedObjects()->First();
        Handle(AIS_Shape) ais = Handle(AIS_Shape)::DownCast(obj);
        if (!ais.IsNull())
            shape = ais->Shape();
    }

    if (shape.IsNull()) {
        qDebug() << "No shape in viewer";
        return;
    }

    applyColorMap(shape, nodes);

    double minVal = 1e9, maxVal = -1e9;
    for (auto& n : nodes) {
        minVal = std::min(minVal, n.second);
        maxVal = std::max(maxVal, n.second);
    }

    if (m_legendOverlay) {
        m_legendOverlay->setRange(minVal, maxVal);
        m_legendOverlay->setTitle("Von Mises Stress [MPa]");
        m_legendOverlay->show();
    }

    qDebug() << "Analysis visualization applied with legend.";
}

void OccView::applyColorMap(const TopoDS_Shape& shape,
                            const std::vector<std::pair<gp_Pnt, double>>& nodalValues)
{
    double minVal = 1e9, maxVal = -1e9;
    for (auto& n : nodalValues) {
        minVal = std::min(minVal, n.second);
        maxVal = std::max(maxVal, n.second);
    }

    Handle(AIS_ColoredShape) colored = new AIS_ColoredShape(shape);
    for (const auto& nv : nodalValues) {
        Quantity_Color col = colorForValue(nv.second, minVal, maxVal);
        colored->SetColor(col);
    }

    m_context->RemoveAll(true);
    m_context->Display(colored, true);
    m_context->UpdateCurrentViewer();
    m_view->FitAll();
}

Quantity_Color OccView::colorForValue(double val, double min, double max) const
{
    double t = (val - min) / ((max - min) + 1e-9);
    t = std::clamp(t, 0.0, 1.0);
    double r = std::min(1.0, std::max(0.0, 2.0 * (t - 0.5)));
    double g = (t < 0.5) ? 2.0 * t : 2.0 * (1.0 - t);
    double b = std::min(1.0, std::max(0.0, 2.0 * (0.5 - t)));
    return Quantity_Color(r, g, b, Quantity_TOC_RGB);
}
