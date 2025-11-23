#include "AegisAIEngine.h"
#include <pybind11/embed.h>
#include <TopoDS_Shape.hxx>
#include <QDebug>

namespace py = pybind11;
using namespace pybind11::literals;

AegisAIEngine::AegisAIEngine(QObject* parent)
    : QObject(parent)
{
    m_materialDB = {
        {"Steel", 7850.0}, {"Aluminum", 2700.0}, {"Titanium", 4500.0},
        {"Composite", 1600.0}, {"Brass", 8500.0}
    };
    initializePython();
}

AegisAIEngine::~AegisAIEngine()
{
    if (m_pythonInitialized)
        py::finalize_interpreter();
}

bool AegisAIEngine::initializePython()
{
    try {
        py::initialize_interpreter();
        m_pythonInitialized = true;
        emit pythonReady();
        emit logMessage("Python embedded interpreter initialized.");
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Python init failed:" << e.what();
        return false;
    }
}

QString AegisAIEngine::processCommand(const QString& input)
{
    if (input.isEmpty()) return "No command given.";
    const QString ruleResp = ruleBasedResponse(input);
    if (!ruleResp.isEmpty()) return ruleResp;
    if (m_pythonInitialized)
        return runPythonReasoning(input);
    return "AI Engine not ready.";
}

QString AegisAIEngine::ruleBasedResponse(const QString& input)
{
    const QString lower = input.toLower();
    if (lower.contains("optimize") || lower.contains("lighten"))
        return "Suggest reducing wall thickness or using Composite material.";
    if (lower.contains("analyze") || lower.contains("stress"))
        return "Run FEA with boundary constraints applied to fixed edges.";
    if (lower.contains("extrude"))
        emit actionTriggered("extrude");
    if (lower.contains("import step"))
        emit actionTriggered("import_step");
    if (lower.contains("export gltf"))
        emit actionTriggered("export_gltf");
    if (lower.contains("help"))
        return "Commands: optimize, analyze, extrude, import step, export gltf, material <name>";
    return {};
}

QString AegisAIEngine::runPythonReasoning(const QString& prompt)
{
    try {
        py::object globals = py::globals();
        globals["prompt"] = prompt.toStdString();
        const char* script = R"PYCODE(
import math
def respond(text):
    t = text.lower()
    if "material" in t:
        return "Material selection should balance density and yield strength."
    if "stress" in t:
        return "Stress appears within safe range for most alloys."
    if "weight" in t:
        return "Suggest topology optimization or hollowing sections."
    return "General design is acceptable."
resp = respond(prompt)
)PYCODE";
        py::exec(script, globals);
        std::string result = py::str(globals["resp"]);
        return QString::fromStdString(result);
    } catch (const std::exception& e) {
        return QString("Python error: %1").arg(e.what());
    }
}

QString AegisAIEngine::suggestOptimization(const std::shared_ptr<TopoDS_Shape>&)
{
    return "Analyze part topology for redundant edges and apply thickness reduction.";
}

QString AegisAIEngine::analyzeMaterial(const QString& matName, double stress, double strain)
{
    if (!m_materialDB.contains(matName)) return "Unknown material.";
    double density = m_materialDB[matName];
    double strengthIndex = stress / (strain + 1e-6);
    return QString("Material: %1 | Density: %2 kg/m³ | Strength Index: %3")
        .arg(matName).arg(density, 0, 'f', 2).arg(strengthIndex, 0, 'f', 2);
}

QString AegisAIEngine::generateSummary() const
{
    return "Aegis AI Assistant ready. Supports hybrid reasoning and rule-based CAD guidance.";
}
