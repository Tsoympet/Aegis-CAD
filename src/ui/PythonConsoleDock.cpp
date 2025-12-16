#include "PythonConsoleDock.h"
#include "../scripting/ScriptRunner.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

PythonConsoleDock::PythonConsoleDock(const QString &title,
                                     OccView *view,
                                     AnalysisManager *analysis,
                                     AegisAIEngine *ai,
                                     ProjectIO *projectIO,
                                     QWidget *parent)
    : QDockWidget(title, parent), m_runner(std::make_unique<ScriptRunner>(view, analysis, ai, projectIO)) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(6, 6, 6, 6);

    auto *header = new QLabel(tr("Embedded CPython via pybind11"), container);
    header->setStyleSheet("color: #00aaff; font-weight: 600;");
    layout->addWidget(header);

    m_console = new QPlainTextEdit(container);
    m_console->setReadOnly(true);
    m_console->setPlaceholderText(tr("Python output"));
    layout->addWidget(m_console, 1);

    m_input = new QLineEdit(container);
    m_input->setPlaceholderText(tr("print('hello from AegisCAD')"));
    layout->addWidget(m_input);

    auto *runBtn = new QPushButton(tr("Run"), container);
    layout->addWidget(runBtn);

    connect(runBtn, &QPushButton::clicked, this, &PythonConsoleDock::runBuffer);
    connect(m_input, &QLineEdit::returnPressed, this, &PythonConsoleDock::runBuffer);

    container->setLayout(layout);
    setWidget(container);
}

void PythonConsoleDock::appendOutput(const QString &text) {
    m_console->appendPlainText(text);
}

void PythonConsoleDock::runBuffer() {
    const QString code = m_input->text();
    const auto result = m_runner->runSnippet(code);
    appendOutput(result);
}

