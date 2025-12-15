#pragma once

#include <QString>
#include <vector>

class AegisAIEngine {
public:
    AegisAIEngine();
    QString evaluate(const QString &prompt) const;

private:
    struct Rule {
        QString trigger;
        QString response;
    };

    std::vector<Rule> m_rules;
};

