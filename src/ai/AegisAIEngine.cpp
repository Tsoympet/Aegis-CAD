#include "AegisAIEngine.h"

AegisAIEngine::AegisAIEngine() {
    m_rules.push_back({QStringLiteral("fillet"), QStringLiteral("Consider adding 3mm fillets to ease manufacturing.")});
    m_rules.push_back({QStringLiteral("analysis"), QStringLiteral("Run a static check before exporting for FEA." )});
    m_rules.push_back({QStringLiteral("python"), QStringLiteral("You can automate operations via the Python dock using pybind11.")});
}

QString AegisAIEngine::evaluate(const QString &prompt) const {
    const QString lower = prompt.toLower();
    for (const auto &rule : m_rules) {
        if (lower.contains(rule.trigger)) {
            return rule.response;
        }
    }
    return QStringLiteral("Model validated. Maintain 0.5mm chamfers on all exposed edges for assembly safety.");
}

