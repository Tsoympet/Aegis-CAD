#pragma once

#include <pybind11/embed.h>
#include <QString>
#include <memory>

class OccView;
class AnalysisManager;
class AegisAIEngine;
class ProjectIO;

class ScriptRunner {
public:
    ScriptRunner(OccView *view = nullptr, AnalysisManager *analysis = nullptr, AegisAIEngine *ai = nullptr, ProjectIO *projectIO = nullptr);
    ~ScriptRunner();

    QString runSnippet(const QString &code);
    QString runFile(const QString &scriptPath, const QString &projectPath = QString());

private:
    void registerModule();

    OccView *m_view{nullptr};
    AnalysisManager *m_analysis{nullptr};
    AegisAIEngine *m_ai{nullptr};
    ProjectIO *m_projectIO{nullptr};
    std::unique_ptr<pybind11::scoped_interpreter> m_interpreter;
};

