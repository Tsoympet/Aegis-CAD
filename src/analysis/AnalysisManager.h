#pragma once
#include <QObject>
#include <QThread>
#include "AnalysisTypes.h"

class BackendFEA_CalculiX;

/// Central coordinator for FEA analysis execution.
class AnalysisManager : public QObject
{
    Q_OBJECT
public:
    explicit AnalysisManager(QObject* parent = nullptr);
    ~AnalysisManager() override;

    /// Launches a simulation case asynchronously.
    void runCase(const QString& projectPath, AnalysisTemplateKind kind);

signals:
    void progressUpdated(int percent);
    void finished(const AnalysisResult& result);

private:
    BackendFEA_CalculiX* m_backend = nullptr;
};
