#pragma once

#include <pybind11/embed.h>
#include <QString>
#include <memory>

class ScriptRunner {
public:
    ScriptRunner();
    ~ScriptRunner();

    QString runSnippet(const QString &code);

private:
    std::unique_ptr<pybind11::scoped_interpreter> m_interpreter;
};

