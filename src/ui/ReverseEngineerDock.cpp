#include "ReverseEngineerDock.h"

#include <QVBoxLayout>
#include <QLabel>

ReverseEngineerDock::ReverseEngineerDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(6, 6, 6, 6);

    auto *hint = new QLabel(tr("Describe the target object or drop reference imagery. AegisCAD will generate a parametric stub."), container);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #cccccc;");
    layout->addWidget(hint);

    m_prompt = new QPlainTextEdit(container);
    m_prompt->setPlaceholderText(tr("e.g., \"Create a 50mm cube with a 10mm through-hole\""));
    layout->addWidget(m_prompt, 1);

    m_generate = new QPushButton(tr("Generate"), container);
    layout->addWidget(m_generate);

    connect(m_generate, &QPushButton::clicked, this, &ReverseEngineerDock::triggerGenerate);

    container->setLayout(layout);
    setWidget(container);
}

QString ReverseEngineerDock::promptText() const {
    return m_prompt->toPlainText();
}

void ReverseEngineerDock::triggerGenerate() {
    emit generateShapeRequested(promptText());
}

