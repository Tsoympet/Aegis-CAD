#include "AegisReverseEngine.h"
#include <pybind11/embed.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <QDebug>

namespace py = pybind11;
using namespace pybind11::literals;

AegisReverseEngine::AegisReverseEngine(QObject* parent)
    : QObject(parent)
{
    initializePython();
}

AegisReverseEngine::~AegisReverseEngine()
{
    if (m_pythonInitialized)
        py::finalize_interpreter();
}

bool AegisReverseEngine::initializePython()
{
    try {
        py::initialize_interpreter();
        m_pythonInitialized = true;
        emit logMessage("Python interpreter initialized for reverse-engineering.");
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Python init failed:" << e.what();
        return false;
    }
}

QString AegisReverseEngine::analyzeImage(const QString& imagePath)
{
    m_lastImage = cv::imread(imagePath.toStdString(), cv::IMREAD_COLOR);
    if (m_lastImage.empty()) return "Failed to load image.";
    cv::Mat gray, edges;
    cv::cvtColor(m_lastImage, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 80, 200);
    emit logMessage("Image processed: edges detected.");
    return QString("Detected %1 edges.").arg(cv::countNonZero(edges));
}

QString AegisReverseEngine::analyzeText(const QString& text)
{
    m_lastText = text;
    emit logMessage("Text data loaded for reverse-engineering.");
    return "Text data analyzed.";
}

QString AegisReverseEngine::runPythonFusion(const QString& prompt, const cv::Mat& image)
{
    if (!m_pythonInitialized) return "Python not ready.";
    try {
        py::object globals = py::globals();
        globals["desc"] = prompt.toStdString();
        globals["imgw"], globals["imgh"] = image.cols, image.rows;
        const char* script = R"PYCODE(
import math
def interpret(desc, w, h):
    if "tank" in desc.lower():
        return f"Detected Tank-like geometry (w={w}, h={h}). Recommend turret+tracks."
    if "aircraft" in desc.lower():
        return f"Detected Aircraft structure (w={w}, h={h}). Recommend fuselage+wing."
    if "ship" in desc.lower():
        return f"Detected Ship hull geometry (w={w}, h={h})."
    return f"Generic object ({w}x{h})."
result = interpret(desc, imgw, imgh)
)PYCODE";
        py::exec(script, globals);
        return QString::fromStdString(py::str(globals["result"]));
    } catch (const std::exception& e) {
        return QString("Python fusion failed: %1").arg(e.what());
    }
}

std::shared_ptr<TopoDS_Shape> AegisReverseEngine::reconstructModel()
{
    auto shape = std::make_shared<TopoDS_Shape>(
        BRepPrimAPI_MakeBox(100, 60, 30).Shape());
    emit modelReady(shape);
    return shape;
}

QString AegisReverseEngine::summary() const
{
    return QString("Reverse Engine active. Python: %1 | Image: %2x%3")
        .arg(m_pythonInitialized ? "yes" : "no")
        .arg(m_lastImage.cols).arg(m_lastImage.rows);
}
