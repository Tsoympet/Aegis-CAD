#include "AegisAIEngine.h"

#include <QFile>
#include <QJsonArray>
#include <QObject>
#include <QSet>
#include <algorithm>

AegisAIEngine::AegisAIEngine() {
    loadRules(QStringLiteral(":/ai_rules.json"));
}

void AegisAIEngine::setSceneInsights(std::vector<PartInsight> insights) {
    m_insights = std::move(insights);
}

void AegisAIEngine::setStressSnapshot(const AnalysisManager::Result &result) {
    m_lastResult = result;
}

static AegisAIEngine::Rule toRule(const QJsonValue &value) {
    Rule rule;
    if (!value.isObject()) return rule;
    const auto obj = value.toObject();
    rule.trigger = obj.value(QStringLiteral("trigger")).toString();
    rule.recommendation = obj.value(QStringLiteral("recommendation")).toString();
    return rule;
}

bool AegisAIEngine::loadRules(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) {
        return false;
    }

    m_rules.clear();
    for (const auto &entry : doc.array()) {
        Rule rule = toRule(entry);
        if (!rule.trigger.isEmpty() && !rule.recommendation.isEmpty()) {
            rule.trigger = rule.trigger.toLower();
            m_rules.push_back(rule);
        }
    }
    return !m_rules.empty();
}

QStringList AegisAIEngine::ruleSummaries() const {
    QStringList summaries;
    for (const auto &rule : m_rules) {
        summaries << QObject::tr("'%1' => %2").arg(rule.trigger, rule.recommendation);
    }
    return summaries;
}

AegisAIEngine::Advice AegisAIEngine::evaluate(const QString &prompt) const {
    Advice advice;

    QStringList lines;
    lines << QObject::tr("Request: %1").arg(prompt);
    lines << QObject::tr("Parts analyzed: %1").arg(m_insights.size());
    lines << QObject::tr("Rule set: %1 active rule(s)").arg(m_rules.size());
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
    const QString lowerPrompt = prompt.toLower();
    QSet<QString> uniqueRecs = QSet<QString>(recs.begin(), recs.end());
    for (const auto &rule : m_rules) {
        if (lowerPrompt.contains(rule.trigger)) {
            uniqueRecs.insert(rule.recommendation);
        }
    }
    recs = QStringList(uniqueRecs.begin(), uniqueRecs.end());
    if (recs.isEmpty()) {
        recs << QObject::tr("No critical issues detected. Consider adding light chamfers for assembly friendliness.");
    }

    advice.summary = lines.join("\n");
    advice.recommendations = recs;
    return advice;
}

AegisAIEngine::Advice AegisAIEngine::optimize(const QString &objective) const {
    const QString scopedPrompt = QObject::tr("Optimize for %1").arg(objective);
    return evaluate(scopedPrompt);
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

