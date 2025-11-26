#pragma once
#include <QObject>
#include <QString>
#include <functional>
#include "AnalysisTypes.h"

/// Interface to CalculiX backend (input generation, run, result parsing).
class BackendFEA_CalculiX : public QObject
{
    Q_OBJECT
public:
    explicit BackendFEA_CalculiX(QObject* parent = nullptr);
    ~BackendFEA_CalculiX() override = default;

    /// Run FEA case and return parsed results.
    AnalysisResult runAnalysis(
        const QString& projectPath,
        AnalysisTemplateKind kind,
        std::function<void(int)> progressCallback);

private:
    QString generateInputFile(const QString& projectPath, AnalysisTemplateKind kind);
    AnalysisResult parseResults(const QString& projectPath, const QString& datFile);
};
