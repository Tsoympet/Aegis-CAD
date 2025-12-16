#include "ScriptRunner.h"
#include "PyBindings.h"

#include "../app/ProjectIO.h"

#include <QFile>

namespace py = pybind11;

ScriptRunner::ScriptRunner(OccView *view, AnalysisManager *analysis, AegisAIEngine *ai, ProjectIO *projectIO)
    : m_view(view), m_analysis(analysis), m_ai(ai), m_projectIO(projectIO) {
    m_interpreter = std::make_unique<py::scoped_interpreter>();
    registerModule();
}

ScriptRunner::~ScriptRunner() = default;

void ScriptRunner::registerModule() {
    py::module_ aegis = py::module_::create_extension_module("aegiscad", nullptr, new py::module_::module_def);
    PyBindings::registerBindings(aegis, m_view, m_analysis, m_ai);
    py::module_::import("sys").attr("modules")["aegiscad"] = aegis;
}

QString ScriptRunner::runSnippet(const QString &code) {
    try {
        py::exec(code.toStdString());
        return QStringLiteral("[ok] Script executed");
    } catch (const py::error_already_set &err) {
        return QString::fromStdString(err.what());
    }
}

QString ScriptRunner::runFile(const QString &scriptPath, const QString &projectPath) {
    if (!QFile::exists(scriptPath)) {
        return QStringLiteral("Script not found");
    }

    py::dict locals;
    locals["project_path"] = projectPath.toStdString();

    if (m_projectIO && !projectPath.isEmpty()) {
        const ProjectSnapshot snapshot = m_projectIO->loadProject(projectPath);
        locals["project_shape"] = py::cast(snapshot.shape);
        locals["project_chat"] = snapshot.chatHistory;
    }

    try {
        py::eval_file(scriptPath.toStdString(), py::globals(), locals);
        return QStringLiteral("[ok] Script executed");
    } catch (const py::error_already_set &err) {
        return QString::fromStdString(err.what());
    }
}

