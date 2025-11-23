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
        emit logMessage("Python interpreter initialized for Aegis AI Engine.");
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Python init failed:" << e.what();
        return false;
    }
}

void AegisAIEngine::setCurrentPart(const PartContext& ctx)
{
    m_currentPart = ctx;
    emit logMessage(QString("Context updated: %1 (%2, %3)")
                    .arg(ctx.name, ctx.material, ctx.type));
}

QString AegisAIEngine::processCommand(const QString& input)
{
    if (input.isEmpty()) return "No command given.";
    const QString lower = input.toLower();

    // Explicit context usage: only when user mentions “selected” / “this part”
    bool useContext = lower.contains("selected") || lower.contains("this part") || lower.contains("current part");

    QString ruleResp = ruleBasedResponse(input);
    if (!ruleResp.isEmpty()) {
        if (useContext)
            return contextualResponse(ruleResp, true);
        return ruleResp;
    }

    if (m_pythonInitialized) {
        QString resp = runPythonReasoning(input);
        if (useContext)
            return contextualResponse(resp, true);
        return resp;
    }

    return "AI Engine not ready.";
}

QString AegisAIEngine::ruleBasedResponse(const QString& input)
{
    const QString lower = input.toLower();
    if (lower.contains("optimize"))
        return "Suggested optimization: reduce wall thickness or switch to composite.";
    if (lower.contains("analyze"))
        return "Run FEA on selected geometry for local stress hotspots.";
    if (lower.contains("material"))
        return "Material analysis: check density-to-strength ratio.";
    if (lower.contains("extrude"))
        emit actionTriggered("extrude");
    if (lower.contains("import step"))
        emit actionTriggered("import_step");
    if (lower.contains("export gltf"))
        emit actionTriggered("export_gltf");
    if (lower.contains("help"))
        return "Available commands: analyze, optimize, import step, export gltf, material <name>";
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
        return "Material choice should consider yield strength and mass efficiency."
    if "stress" in t:
        return "Stress distribution within acceptable FoS range."
    if "weight" in t or "optimize" in t:
        return "Consider reducing part density or applying topology optimization."
    return "General structure within operational limits."
resp = respond(prompt)
)PYCODE";
        py::exec(script, globals);
        std::string result = py::str(globals["resp"]);
        return QString::fromStdString(result);
    } catch (const std::exception& e) {
        return QString("Python reasoning failed: %1").arg(e.what());
    }
}

QString AegisAIEngine::suggestOptimization(const std::shared_ptr<TopoDS_Shape>&)
{
    return "Optimization suggestion: merge redundant edges and simplify topology.";
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
    return QString("Aegis AI Assistant — Python: %1 | Context: %2")
        .arg(m_pythonInitialized ? "ready" : "off")
        .arg(m_currentPart.isValid() ? m_currentPart.name : "none");
}

QString AegisAIEngine::contextualResponse(const QString& base, bool conversational) const
{
    if (!m_currentPart.isValid())
        return base + "\n(No active part context.)";

    QString header = QString("🧩 %1 (%2)\n").arg(m_currentPart.name, m_currentPart.material);
    QString technical = QString("FoS: %1  |  Stress: %2 MPa  |  Mass: %3 kg  |  Volume: %4 cm³")
        .arg(m_currentPart.fos, 0, 'f', 2)
        .arg(m_currentPart.stress, 0, 'f', 1)
        .arg(m_currentPart.mass, 0, 'f', 1)
        .arg(m_currentPart.volume, 0, 'f', 1);

    QString narrative = QString("\nThe selected %1 made of %2 has a safety factor of %3 under current load.")
        .arg(m_currentPart.type, m_currentPart.material)
        .arg(m_currentPart.fos, 0, 'f', 2);

    return header + technical + (conversational ? narrative : QString()) + "\n\n" + base;
}
