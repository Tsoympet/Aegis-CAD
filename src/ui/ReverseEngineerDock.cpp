#include "ReverseEngineerDock.h"

#include "../cad/FeatureOps.h"

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <STEPControl_Writer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <QCheckBox>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QRegularExpression>
#include <QRectF>
#include <QSizeF>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

#ifdef HAVE_OPENCV
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#endif

#ifdef HAVE_TESSERACT
#include <tesseract/baseapi.h>
#endif
#include <QVBoxLayout>

ReverseEngineerDock::ReverseEngineerDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(6, 6, 6, 6);

    auto *hint = new QLabel(tr("Load a blueprint photo and describe the object. AegisCAD will emit a parametric stub."), container);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #cccccc;");
    layout->addWidget(hint);

    auto *fileRow = new QFrame(container);
    auto *rowLayout = new QHBoxLayout(fileRow);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    m_blueprintPath = new QLineEdit(fileRow);
    m_blueprintPath->setPlaceholderText(tr("Blueprint image path"));
    rowLayout->addWidget(m_blueprintPath, 1);
    m_selectBlueprint = new QPushButton(tr("Browse"), fileRow);
    rowLayout->addWidget(m_selectBlueprint);
    fileRow->setLayout(rowLayout);
    layout->addWidget(fileRow);

    m_prompt = new QPlainTextEdit(container);
    m_prompt->setPlaceholderText(tr("e.g., 'cylindrical pressure vessel with flanges'"));
    layout->addWidget(m_prompt, 1);

    m_generate = new QPushButton(tr("Generate placeholder"), container);
    layout->addWidget(m_generate);

    auto *exportRow = new QFrame(container);
    auto *exportLayout = new QHBoxLayout(exportRow);
    exportLayout->setContentsMargins(0, 0, 0, 0);
    m_exportPart = new QPushButton(tr("Export .aegispart"), exportRow);
    m_exportStep = new QPushButton(tr("Export STEP"), exportRow);
    exportLayout->addWidget(m_exportPart);
    exportLayout->addWidget(m_exportStep);
    exportRow->setLayout(exportLayout);
    layout->addWidget(exportRow);

    auto *optionsRow = new QFrame(container);
    auto *optionsLayout = new QHBoxLayout(optionsRow);
    optionsLayout->setContentsMargins(0, 0, 0, 0);
    m_previewToggle = new QCheckBox(tr("Preview contours"), optionsRow);
    m_previewToggle->setChecked(true);
    optionsLayout->addWidget(m_previewToggle);
    optionsRow->setLayout(optionsLayout);
    layout->addWidget(optionsRow);

    m_progress = new QProgressBar(container);
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    layout->addWidget(m_progress);

    connect(m_generate, &QPushButton::clicked, this, &ReverseEngineerDock::triggerGenerate);
    connect(m_selectBlueprint, &QPushButton::clicked, this, &ReverseEngineerDock::chooseBlueprint);
    connect(m_exportPart, &QPushButton::clicked, this, &ReverseEngineerDock::exportAsPart);
    connect(m_exportStep, &QPushButton::clicked, this, &ReverseEngineerDock::exportAsStep);

    container->setLayout(layout);
    setWidget(container);
}

void ReverseEngineerDock::triggerGenerate() {
    updateProgress(0);
    m_lastShape = generatePlaceholder();
    emit modelGenerated(m_lastShape);
}

void ReverseEngineerDock::chooseBlueprint() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select blueprint"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (!path.isEmpty()) {
        m_blueprintPath->setText(path);
    }
}

}

void ReverseEngineerDock::chooseBlueprint() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select blueprint"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (!path.isEmpty()) {
        m_blueprintPath->setText(path);
    }
}

TopoDS_Shape ReverseEngineerDock::generatePlaceholder() const {
    if (!m_blueprintPath->text().isEmpty()) {
        TopoDS_Shape fromBlueprint = generateFromBlueprint();
        if (!fromBlueprint.IsNull()) {
            return fromBlueprint;
        }
    }

    emit modelGenerated(generatePlaceholder());
}

void ReverseEngineerDock::chooseBlueprint() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select blueprint"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (!path.isEmpty()) {
        m_blueprintPath->setText(path);
    }
}

TopoDS_Shape ReverseEngineerDock::generatePlaceholder() const {
    const QString desc = m_prompt->toPlainText().toLower();
    const QString blueprint = m_blueprintPath->text().toLower();
    const bool cylinderHint = desc.contains(QStringLiteral("cylinder")) || blueprint.contains(QStringLiteral("cyl"));
    const bool thin = desc.contains(QStringLiteral("tube")) || desc.contains(QStringLiteral("pipe"));

    if (cylinderHint) {
        const double radius = thin ? 15.0 : 25.0;
        const double height = thin ? 80.0 : 50.0;
        return FeatureOps::makeCylinder(radius, height);
    }

    const double size = desc.contains(QStringLiteral("large")) ? 120.0 : 60.0;
    return FeatureOps::makeBox(size);
}

TopoDS_Shape ReverseEngineerDock::generateFromBlueprint() const {
#ifdef HAVE_OPENCV
    updateProgress(5);
    m_lastContours = detectContours(m_blueprintPath->text());
    updateProgress(45);

    QStringList labels = extractLabels(m_blueprintPath->text());
    updateProgress(60);

    const double scale = inferScale(labels, m_lastContours);
    TopoDS_Shape shape = buildShapeFromContours(m_lastContours, scale);
    if (m_previewToggle && !m_previewToggle->isChecked()) {
        m_lastContours.clear();
    }
    updateProgress(95);
    return shape;
#else
    return TopoDS_Shape();
#endif
}

std::vector<std::vector<QPointF>> ReverseEngineerDock::detectContours(const QString &path) const {
    std::vector<std::vector<QPointF>> outlines;
#ifdef HAVE_OPENCV
    cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        return outlines;
    }

    cv::Mat blurred, edges;
    cv::GaussianBlur(img, blurred, cv::Size(5, 5), 0);
    cv::Canny(blurred, edges, 50, 150);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto &c : contours) {
        std::vector<cv::Point2f> approx;
        cv::approxPolyDP(c, approx, 2.0, true);

        std::vector<QPointF> path;
        for (size_t i = 0; i < approx.size(); ++i) {
            cv::Point2f p = approx[i];
            if (!path.empty()) {
                QPointF delta = QPointF(p.x, p.y) - path.back();
                if (std::abs(delta.x()) < 1e-3 && std::abs(delta.y()) < 1e-3) {
                    continue;
                }
            }
            // horizontal/vertical snapping
            if (!path.empty()) {
                QPointF prev = path.back();
                if (std::abs(p.x - prev.x()) < 3.0) {
                    p.x = prev.x;
                }
                if (std::abs(p.y - prev.y()) < 3.0) {
                    p.y = prev.y;
                }
            }
            path.emplace_back(p.x, p.y);
        }

        if (path.size() > 2) {
            outlines.push_back(path);
        }
    }
#else
    Q_UNUSED(path);
#endif
    return outlines;
}

QStringList ReverseEngineerDock::extractLabels(const QString &path) const {
    QStringList labels;
#if defined(HAVE_TESSERACT) && defined(HAVE_OPENCV)
    cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        return labels;
    }

    tesseract::TessBaseAPI api;
    if (api.Init(nullptr, "eng")) {
        return labels;
    }
    api.SetPageSegMode(tesseract::PSM_AUTO);
    api.SetImage(img.data, img.cols, img.rows, img.channels(), img.step);
    char *text = api.GetUTF8Text();
    if (text) {
        labels = QString::fromUtf8(text).split('\n', Qt::SkipEmptyParts);
        delete[] text;
    }
#else
    Q_UNUSED(path);
#endif
    return labels;
}

double ReverseEngineerDock::inferScale(const QStringList &labels, const std::vector<std::vector<QPointF>> &paths) const {
    double targetLen = 0.0;
    const QRegularExpression re(QStringLiteral("(\\d+\n?\\.?\\d*)\s*(mm|cm|m)?"), QRegularExpression::CaseInsensitiveOption);
    for (const QString &label : labels) {
        auto match = re.match(label);
        if (match.hasMatch()) {
            double value = match.captured(1).toDouble();
            QString unit = match.captured(2).toLower();
            if (unit == "cm") value *= 10.0;
            else if (unit == "m") value *= 1000.0;
            targetLen = value;
            break;
        }
    }

    if (targetLen <= 0.0 || paths.empty()) {
        return 1.0;
    }

    QRectF bounds;
    for (const auto &path : paths) {
        for (const QPointF &p : path) {
            if (bounds.isNull()) bounds = QRectF(p, QSizeF(1, 1));
            bounds |= QRectF(p, QSizeF(1, 1));
        }
    }
    double pixelSpan = std::max(bounds.width(), bounds.height());
    if (pixelSpan <= 0.0) {
        return 1.0;
    }
    return targetLen / pixelSpan;
}

TopoDS_Shape ReverseEngineerDock::buildShapeFromContours(const std::vector<std::vector<QPointF>> &paths, double scale) const {
    BRep_Builder builder;
    TopoDS_Compound compound;
    builder.MakeCompound(compound);

    for (const auto &path : paths) {
        BRepBuilderAPI_MakePolygon poly;
        for (const QPointF &p : path) {
            gp_Pnt pt(p.x() * scale, p.y() * scale, 0.0);
            poly.Add(pt);
        }
        poly.Close();
        TopoDS_Wire wire = poly.Wire();
        builder.Add(compound, wire);
        if (path.size() >= 3) {
            TopoDS_Face face = BRepBuilderAPI_MakeFace(wire, true);
            builder.Add(compound, face);
        }
    }

    updateProgress(85);
    return compound;
}

void ReverseEngineerDock::exportAsPart() {
    if (m_lastShape.IsNull()) {
        return;
    }
    const QString path = QFileDialog::getSaveFileName(this, tr("Save as .aegispart"), QString(), tr("Aegis Part (*.aegispart)"));
    if (path.isEmpty()) {
        return;
    }
    BRepTools::Write(m_lastShape, path.toStdString().c_str());
}

void ReverseEngineerDock::exportAsStep() {
    if (m_lastShape.IsNull()) {
        return;
    }
    const QString path = QFileDialog::getSaveFileName(this, tr("Save as STEP"), QString(), tr("STEP Files (*.step *.stp)"));
    if (path.isEmpty()) {
        return;
    }
    STEPControl_Writer writer;
    writer.Transfer(m_lastShape, STEPControl_AsIs);
    writer.Write(path.toStdString().c_str());
}

void ReverseEngineerDock::updateProgress(int value) const {
    if (m_progress) {
        m_progress->setValue(value);
    }
}

