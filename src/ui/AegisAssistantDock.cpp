#include "AegisAssistantDock.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

AegisAssistantDock::AegisAssistantDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(6, 6, 6, 6);

    auto *label = new QLabel(tr("Rule-based helper"), container);
    label->setStyleSheet("color: #00aaff; font-weight: 600;");
    layout->addWidget(label);

    m_history = new QTextEdit(container);
    m_history->setReadOnly(true);
    m_history->setPlaceholderText(tr("AI insights will appear here..."));
    layout->addWidget(m_history, 1);

    m_input = new QLineEdit(container);
    m_input->setPlaceholderText(tr("Ask the assistant..."));
    layout->addWidget(m_input);

    auto *send = new QPushButton(tr("Send"), container);
    layout->addWidget(send);

    connect(send, &QPushButton::clicked, this, [this]() {
        emit querySubmitted(m_input->text());
    });
    connect(m_input, &QLineEdit::returnPressed, this, [this]() {
        emit querySubmitted(m_input->text());
    });

    container->setLayout(layout);
    setWidget(container);
}

void AegisAssistantDock::appendResponse(const QString &text) {
    m_history->append(QStringLiteral("<b>AegisAI:</b> %1").arg(text.toHtmlEscaped()));
}

