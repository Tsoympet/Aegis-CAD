#pragma once
#include <QString>
#include <QVector>

/// Simple physical preset (placeholder for actual physics templates)
struct DomainTemplate {
    QString name;
    QString description;
    double density = 7800.0;
    double elasticModulus = 2.1e11;
    double poissonRatio = 0.3;
};

class DomainTemplates
{
public:
    static QVector<DomainTemplate> defaults();
};
