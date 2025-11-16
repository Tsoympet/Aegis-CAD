#include "PythonConsoleDock.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

PythonConsoleDock::PythonConsoleDock(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Python Console"));
    auto* w = new QWidget(this);
    auto* layout = new QVBoxLayout(w);

    m_output = new QTextEdit(w);
    m_output->setReadOnly(true);
    m_input = new QPlainTextEdit(w);
    m_runBtn = new QPushButton(tr("Run Script"), w);

    layout->addWidget(m_output, 2);
    layout->addWidget(m_input, 1);
    layout->addWidget(m_runBtn);

    connect(m_runBtn, &QPushButton::clicked,
            this, &PythonConsoleDock::onRunClicked);

    w->setLayout(layout);
    setWidget(w);
}

void PythonConsoleDock::onRunClicked()
{
    m_output->append("Python bridge stub – not wired yet.");
}
