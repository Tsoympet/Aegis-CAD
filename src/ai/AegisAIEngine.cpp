#include "AegisAIEngine.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTextStream>
#include <QDateTime>

#ifdef USE_PYBIND11
    #include <pybind11/embed.h>
    namespace py = pybind11;
#endif

AegisAIEngine::AegisAIEngine()
{
    m_domains = { "Materials", "Stress", "Safety", "Thermal", "Aerodynamics", "Armor", "Structure" };

#ifdef USE_PYBIND11
    try {
        py::initialize_interpreter();
        m_pythonEnabled = true;
    } catch (...) {
        m_pythonEnabled = false;
    }
#endif
}

AegisAIEngine::~AegisAIEngine()
{
#ifdef USE_PYBIND11
    if (m_pythonEnabled)
        py::finalize_interpreter();
#endif
}

// ============================================================================
//  Context
// ============================================================================
void AegisAIEngine::setContext(const QString& key, double value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_context[key] = value;
}

std::optional<double> AegisAIEngine::context(const QString& key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_context.contains(key))
        return std::nullopt;
    return m_context[key];
}

// ============================================================================
//  Message Processing
// ============================================================================
QString AegisAIEngine::processMessage(const QString& input)
{
    QString lower = input.toLower();

    if (lower.contains("stress") || lower.contains("factor of safety"))
        return handleAnalysisQuery(input);
    if (lower.contains("optimiz") || lower.contains("reduce"))
        return suggestOptimization();
    if (lower.contains("material") || lower.contains("yield"))
        return evaluateDesignRules(input);

    // fallback to general chat
    return handleGeneralChat(input);
}

// ============================================================================
//  Rule Evaluation
// ============================================================================
QString AegisAIEngine::evaluateDesignRules(const QString& input)
{
    double yield = m_context.value("yield_strength", 250.0);
    double stress = m_context.value("max_stress", 120.0);
    double fos = yield / (stress + 1e-6);

    QString msg = QString("Material check:\n"
                          "• Yield strength = %1 MPa\n"
                          "• Max stress = %2 MPa\n"
                          "• Factor of Safety = %3\n")
                      .arg(yield, 0, 'f', 2)
                      .arg(stress, 0, 'f', 2)
                      .arg(fos, 0, 'f', 2);

    if (fos < 1.5)
        msg += "\n⚠️ Safety margin too low — consider thicker section or higher-grade alloy.";
    else if (fos > 4.0)
        msg += "\n✅ Overdesigned — you can reduce mass or wall thickness safely.";
    else
        msg += "\n✅ Safety margin acceptable.";

    return msg;
}

// ============================================================================
QString AegisAIEngine::suggestOptimization()
{
    double mass = m_context.value("mass", 0.0);
    double stress = m_context.value("max_stress", 0.0);
    double fos = m_context.value("fos", 0.0);

    QString out = "Optimization suggestion:\n";

    if (mass > 0 && stress > 0) {
        if (fos > 3.5)
            out += "• Component appears overdesigned. Try reducing thickness by 10–20%.\n";
        else if (fos < 1.2)
            out += "• Component underdesigned. Increase support ribs or cross-section area.\n";
    } else {
        out += "• Provide analysis results (mass, stress, FoS) for optimization hints.\n";
    }

    return out + "\n(Use 'Run Analysis' before optimization for best accuracy.)";
}

// ============================================================================
QString AegisAIEngine::handleAnalysisQuery(const QString& query)
{
    double stress = m_context.value("max_stress", 0.0);
    double yield = m_context.value("yield_strength", 0.0);
    double fos = yield / (stress + 1e-6);

    QString result;
    QTextStream s(&result);
    s << "Analysis summary:\n"
      << "  Max stress: " << stress << " MPa\n"
      << "  Yield: " << yield << " MPa\n"
      << "  Calculated FoS: " << fos << "\n";

    if (fos < 1.5)
        s << "  ⚠️ High stress detected, design may fail under load.\n";
    else
        s << "  ✅ Within safe range.\n";

    return result;
}

// ============================================================================
QString AegisAIEngine::handleGeneralChat(const QString& query)
{
    QStringList canned = {
        "I'm your onboard design assistant — ready to analyze or optimize any part.",
        "Try commands like 'check stress', 'suggest material', or 'optimize weight'.",
        "Your design context is automatically updated after each FEA run."
    };

    int idx = QDateTime::currentSecsSinceEpoch() % canned.size();
    return canned[idx];
}

// ============================================================================
QString AegisAIEngine::runPythonRule(const QString& scriptCode)
{
#ifndef USE_PYBIND11
    Q_UNUSED(scriptCode);
    return "⚙️ Python integration not available (rebuild with USE_PYBIND11).";
#else
    if (!m_pythonEnabled)
        return "Python runtime disabled.";

    try {
        py::object result = py::eval(scriptCode.toStdString());
        return QString::fromStdString(py::str(result));
    } catch (std::exception& e) {
        return QString("Python error: %1").arg(e.what());
    }
#endif
}

// ============================================================================
QStringList AegisAIEngine::availableDomains() const
{
    return m_domains;
}
