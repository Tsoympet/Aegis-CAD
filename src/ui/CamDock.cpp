#include "CamDock.h"
#include "OccView.h"

#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <TopoDS_Shape.hxx>

CamDock::CamDock(const QString &title, QWidget *parent) : QDockWidget(title, parent) {
    auto *root = new QWidget(this);
    auto *layout = new QVBoxLayout(root);

    auto *opRow = new QHBoxLayout();
    opRow->addWidget(new QLabel(tr("Operation")));
    m_operation = new QComboBox(root);
    m_operation->addItems({tr("Pocket"), tr("Contour"), tr("Drilling")});
    opRow->addWidget(m_operation, 1);
    layout->addLayout(opRow);

    auto *depthRow = new QHBoxLayout();
    depthRow->addWidget(new QLabel(tr("Depth")));
    m_depth = new QDoubleSpinBox(root);
    m_depth->setRange(-1000, 0);
    m_depth->setDecimals(3);
    m_depth->setValue(-5.0);
    depthRow->addWidget(m_depth);
    layout->addLayout(depthRow);

    auto *stepRow = new QHBoxLayout();
    stepRow->addWidget(new QLabel(tr("Stepover")));
    m_stepover = new QDoubleSpinBox(root);
    m_stepover->setRange(0.1, 100);
    m_stepover->setValue(5.0);
    stepRow->addWidget(m_stepover);
    stepRow->addWidget(new QLabel(tr("Stepdown")));
    m_stepdown = new QDoubleSpinBox(root);
    m_stepdown->setRange(0.1, 100);
    m_stepdown->setValue(2.0);
    stepRow->addWidget(m_stepdown);
    layout->addLayout(stepRow);

    auto *feedRow = new QHBoxLayout();
    feedRow->addWidget(new QLabel(tr("Feed")));
    m_feed = new QDoubleSpinBox(root);
    m_feed->setRange(1, 5000);
    m_feed->setValue(800);
    feedRow->addWidget(m_feed);
    feedRow->addWidget(new QLabel(tr("Spindle")));
    m_spindle = new QDoubleSpinBox(root);
    m_spindle->setRange(0, 40000);
    m_spindle->setValue(10000);
    feedRow->addWidget(m_spindle);
    layout->addLayout(feedRow);

    auto *selectRow = new QHBoxLayout();
    m_selectFaces = new QPushButton(tr("Select Faces"), root);
    m_selectEdges = new QPushButton(tr("Select Edges"), root);
    selectRow->addWidget(m_selectFaces);
    selectRow->addWidget(m_selectEdges);
    layout->addLayout(selectRow);

    auto *postRow = new QHBoxLayout();
    postRow->addWidget(new QLabel(tr("Post")));
    m_flavor = new QComboBox(root);
    m_flavor->addItems({tr("GRBL"), tr("Fanuc")});
    postRow->addWidget(m_flavor);
    layout->addLayout(postRow);

    auto *actionRow = new QHBoxLayout();
    m_generate = new QPushButton(tr("Generate"), root);
    m_export = new QPushButton(tr("Export"), root);
    actionRow->addWidget(m_generate);
    actionRow->addWidget(m_export);
    layout->addLayout(actionRow);

    layout->addStretch(1);
    setWidget(root);

    connect(m_selectFaces, &QPushButton::clicked, this, &CamDock::beginFaceSelection);
    connect(m_selectEdges, &QPushButton::clicked, this, &CamDock::beginEdgeSelection);
    connect(m_generate, &QPushButton::clicked, this, &CamDock::generateToolpath);
    connect(m_export, &QPushButton::clicked, this, &CamDock::exportGcode);
}

void CamDock::setView(OccView *view) {
    m_view = view;
    if (!m_view) return;
    connect(m_view, &OccView::camFacesPicked, this, &CamDock::onFacesPicked);
    connect(m_view, &OccView::camEdgesPicked, this, &CamDock::onEdgesPicked);
}

void CamDock::setShapeProvider(const std::function<TopoDS_Shape()> &provider) {
    m_shapeProvider = provider;
}

void CamDock::beginFaceSelection() {
    if (m_view) {
        m_view->enableCamSelection(true, false);
    }
}

void CamDock::beginEdgeSelection() {
    if (m_view) {
        m_view->enableCamSelection(false, true);
    }
}

void CamDock::onFacesPicked(const std::vector<TopoDS_Face> &faces) {
    m_faces = faces;
    if (m_view) {
        m_view->enableCamSelection(false, false);
    }
}

void CamDock::onEdgesPicked(const std::vector<TopoDS_Edge> &edges) {
    m_edges = edges;
    if (m_view) {
        m_view->enableCamSelection(false, false);
    }
}

ToolpathGenerator::PostFlavor CamDock::currentFlavor() const {
    return m_flavor && m_flavor->currentIndex() == 1 ? ToolpathGenerator::PostFlavor::Fanuc : ToolpathGenerator::PostFlavor::GRBL;
}

ToolpathGenerator::Operation CamDock::currentOperation() const {
    switch (m_operation ? m_operation->currentIndex() : 0) {
    case 0:
        return ToolpathGenerator::Operation::Pocket;
    case 1:
        return ToolpathGenerator::Operation::Contour;
    case 2:
    default:
        return ToolpathGenerator::Operation::Drilling;
    }
}

void CamDock::generateToolpath() {
    if (!m_shapeProvider) {
        QMessageBox::warning(this, tr("CAM"), tr("No model available."));
        return;
    }
    TopoDS_Shape shape = m_shapeProvider();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("CAM"), tr("Load or create a solid first."));
        return;
    }

    m_generator.setModel(shape);
    m_generator.setSelections(m_faces, m_edges);

    const double depth = m_depth->value();
    const double stepover = m_stepover->value();
    const double stepdown = m_stepdown->value();
    const double feed = m_feed->value();
    const double spindle = m_spindle->value();

    switch (currentOperation()) {
    case ToolpathGenerator::Operation::Pocket:
        m_lastPath = m_generator.generatePocket(stepover, stepdown, depth, 5.0, feed, spindle);
        break;
    case ToolpathGenerator::Operation::Contour:
        m_lastPath = m_generator.generateContour(depth, 5.0, feed, spindle);
        break;
    case ToolpathGenerator::Operation::Drilling:
        m_lastPath = m_generator.generateDrilling(depth, stepdown, 5.0, feed, spindle);
        break;
    }

    if (m_view) {
        m_view->previewToolpath(m_lastPath.points);
    }
}

void CamDock::exportGcode() {
    if (m_lastPath.points.empty()) {
        QMessageBox::information(this, tr("CAM"), tr("Generate a toolpath first."));
        return;
    }

    const QString filter = tr("G-code (*.nc *.gcode)");
    const QString target = QFileDialog::getSaveFileName(this, tr("Export G-code"), QString(), filter);
    if (target.isEmpty()) return;

    if (!m_lastPath.exportToFile(target, currentFlavor())) {
        QMessageBox::warning(this, tr("Export"), tr("Could not write file."));
    }
}

