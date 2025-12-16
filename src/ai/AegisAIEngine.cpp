#include "AegisAIEngine.h"

#include <QJsonArray>
#include <QObject>
#include <algorithm>

AegisAIEngine::AegisAIEngine() = default;

void AegisAIEngine::setSceneInsights(std::vector<PartInsight> insights) {
    m_insights = std::move(insights);
}

void AegisAIEngine::setStressSnapshot(const AnalysisManager::Result &result) {
    m_lastResult = result;
}

AegisAIEngine::Advice AegisAIEngine::evaluate(const QString &prompt) const {
    Advice advice;

    QStringList lines;
    lines << QObject::tr("Request: %1").arg(prompt);
    lines << QObject::tr("Parts analyzed: %1").arg(m_insights.size());
    if (m_lastResult.success) {
        lines << QObject::tr("Latest FEA: %1 (%.2f to %.2f Pa)")
                     .arg(m_lastResult.summary)
                     .arg(m_lastResult.minStress)
                     .arg(m_lastResult.maxStress);
    }

    QStringList recs;
    for (const auto &part : m_insights) {
        const QString materialHint = materialGuidance(part);
        const QString stressHint = stressGuidance(part);
        if (!materialHint.isEmpty()) recs << materialHint;
        if (!stressHint.isEmpty()) recs << stressHint;
        if (part.mass > 1.0 && prompt.contains(QStringLiteral("weight"), Qt::CaseInsensitive)) {
            recs << QObject::tr("%1: pocket interior faces or thin ribs to shed ~%.1fg while keeping stiffness.")
                           .arg(part.name)
                           .arg(part.mass * 0.15);
        }
    }

    const QString global = globalGuidance();
    if (!global.isEmpty()) {
        recs << global;
    }
    if (recs.isEmpty()) {
        recs << QObject::tr("No critical issues detected. Consider adding light chamfers for assembly friendliness.");
    }

    advice.summary = lines.join("\n");
    advice.recommendations = recs;
    return advice;
}

QString AegisAIEngine::materialGuidance(const PartInsight &part) const {
    if (part.material.contains(QStringLiteral("steel"), Qt::CaseInsensitive)) {
        if (part.mass > 500.0) {
            return QObject::tr("%1: swap to 7075 aluminum to cut mass; re-check stiffness.").arg(part.name);
        }
        return QObject::tr("%1: steel ok; evaluate corrosion coating if exposed.").arg(part.name);
    }
    if (part.material.contains(QStringLiteral("aluminum"), Qt::CaseInsensitive)) {
        if (part.peakStress > 0.7 * 250e6) {
            return QObject::tr("%1: Aluminum near yield. Upsize ribs or switch to 17-4PH stainless.").arg(part.name);
        }
        return QObject::tr("%1: aluminum allows aggressive pocketing; keep >2mm wall.").arg(part.name);
    }
    return {};
}

QString AegisAIEngine::stressGuidance(const PartInsight &part) const {
    if (part.peakStress <= 0.0) return {};
    if (part.safetyFactor < 1.2) {
        return QObject::tr("%1: reinforce fillets/adjacent ribs; safety factor %.2f is low.").arg(part.name).arg(part.safetyFactor);
    }
    if (part.peakStress > 0.6 * 250e6) {
        return QObject::tr("%1: consider local thickness increase near %.1f MPa hotspot.")
            .arg(part.name)
            .arg(part.peakStress / 1e6);
    }
    return {};
}

QString AegisAIEngine::globalGuidance() const {
    if (m_insights.empty()) return {};
    const bool heavy = std::any_of(m_insights.begin(), m_insights.end(), [](const PartInsight &p) { return p.mass > 1000.0; });
    const bool stressed = m_lastResult.success && m_lastResult.maxStress > 0.0;
    if (heavy && !stressed) {
        return QObject::tr("Global: evaluate topology optimization or latticing to reduce overall mass.");
    }
    if (stressed) {
        return QObject::tr("Global: redistribute loads with gussets; validate with another FEA pass after edits.");
    }
    return QObject::tr("Global: keep datum planes locked to avoid mate drift in assemblies.");
}

QJsonDocument AegisAIEngine::Advice::toJson() const {
    QJsonObject obj;
    obj["summary"] = summary;
    QJsonArray arr;
    for (const auto &rec : recommendations) {
        arr.append(rec);
    }
    obj["recommendations"] = arr;
    return QJsonDocument(obj);
}

