#pragma once

#include "../analysis/AnalysisManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <vector>

class AegisAIEngine {
public:
    struct PartInsight {
        QString id;
        QString name;
        QString material;
        double volume{0.0};
        double mass{0.0};
        double peakStress{0.0};
        double safetyFactor{1.0};
    };

    struct Advice {
        QString summary;
        QStringList recommendations;

        QJsonDocument toJson() const;
    };

    AegisAIEngine();

    void setSceneInsights(std::vector<PartInsight> insights);
    void setStressSnapshot(const AnalysisManager::Result &result);

    Advice evaluate(const QString &prompt) const;

private:
    QString materialGuidance(const PartInsight &part) const;
    QString stressGuidance(const PartInsight &part) const;
    QString globalGuidance() const;

    std::vector<PartInsight> m_insights;
    AnalysisManager::Result m_lastResult;
};

