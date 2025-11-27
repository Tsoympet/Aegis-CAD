#pragma once
#include <QString>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <mutex>
#include <string>
#include <optional>

/// AegisAIEngine — Local AI rule-based reasoning engine.
/// Provides contextual design advice, safety checks, and optimization guidance.
/// Integrates optional Python scripting layer via Pybind11 (if available).
class AegisAIEngine
{
public:
    AegisAIEngine();
    ~AegisAIEngine();

    /// Main conversational entry point (used by AegisAssistantDock)
    QString processMessage(const QString& input);

    /// Register external CAD/FEA context values (for rule-based reasoning)
    void setContext(const QString& key, double value);
    std::optional<double> context(const QString& key) const;

    /// Run a Python-side AI rule evaluation (optional)
    QString runPythonRule(const QString& scriptCode);

    /// Returns all available rule categories (for UI display)
    QStringList availableDomains() const;

private:
    QString evaluateDesignRules(const QString& input);
    QString suggestOptimization();
    QString handleAnalysisQuery(const QString& query);
    QString handleGeneralChat(const QString& query);

    QMap<QString, double> m_context;
    QStringList m_domains;
    mutable std::mutex m_mutex;
    bool m_pythonEnabled { false };
};
