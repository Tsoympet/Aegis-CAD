#include "ScriptRunner.h"
#include "PyBindings.h"

namespace py = pybind11;

ScriptRunner::ScriptRunner() {
    m_interpreter = std::make_unique<py::scoped_interpreter>();
    py::module_ aegis = py::module_::create_extension_module("aegiscad", nullptr, new py::module_::module_def);
    PyBindings::registerBindings(aegis);
    py::module_::import("sys").attr("modules")["aegiscad"] = aegis;
}

ScriptRunner::~ScriptRunner() = default;

QString ScriptRunner::runSnippet(const QString &code) {
    try {
        py::exec(code.toStdString());
        return QStringLiteral("[ok] Script executed");
    } catch (const py::error_already_set &err) {
        return QString::fromStdString(err.what());
    }
}

