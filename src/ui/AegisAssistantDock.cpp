#include "AegisAssistantDock.h"

#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QVBoxLayout>

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

    auto *buttonRow = new QFrame(container);
    auto *rowLayout = new QHBoxLayout(buttonRow);
    rowLayout->setContentsMargins(0, 0, 0, 0);

    auto *send = new QPushButton(tr("Send"), buttonRow);
    rowLayout->addWidget(send);

    m_export = new QPushButton(tr("Save Advice JSON"), buttonRow);
    rowLayout->addWidget(m_export);

    buttonRow->setLayout(rowLayout);
    layout->addWidget(buttonRow);

    connect(send, &QPushButton::clicked, this, [this]() {
        emit querySubmitted(m_input->text());
    });
    connect(m_input, &QLineEdit::returnPressed, this, [this]() {
        emit querySubmitted(m_input->text());
    });
    connect(m_export, &QPushButton::clicked, this, &AegisAssistantDock::handleSaveAdvice);

    container->setLayout(layout);
    setWidget(container);
}

void AegisAssistantDock::appendUserPrompt(const QString &text) {
    m_history->append(QStringLiteral("<b>You:</b> %1").arg(text.toHtmlEscaped()));
    m_historyLog << QStringLiteral("user:%1").arg(text);
}

void AegisAssistantDock::appendAdvice(const AegisAIEngine::Advice &advice) {
    m_lastAdvice = advice;
    m_history->append(QStringLiteral("<b>AegisAI:</b><pre>%1</pre>").arg(advice.summary.toHtmlEscaped()));
    for (const auto &rec : advice.recommendations) {
        m_history->append(QStringLiteral("• %1").arg(rec.toHtmlEscaped()));
    }
    m_historyLog << QStringLiteral("assistant:%1").arg(advice.summary);
    for (const auto &rec : advice.recommendations) {
        m_historyLog << QStringLiteral("assistant:%1").arg(rec);
    }
}

void AegisAssistantDock::setHistory(const QStringList &history) {
    m_historyLog = history;
    m_history->clear();
    for (const auto &line : history) {
        if (line.startsWith(QStringLiteral("user:"))) {
            m_history->append(QStringLiteral("<b>You:</b> %1").arg(line.mid(5).toHtmlEscaped()));
        } else if (line.startsWith(QStringLiteral("assistant:"))) {
            m_history->append(QStringLiteral("<b>AegisAI:</b> %1").arg(line.mid(10).toHtmlEscaped()));
        } else {
            m_history->append(line.toHtmlEscaped());
        }
    }
}

void AegisAssistantDock::handleSaveAdvice() {
    writeAdviceJson(m_lastAdvice);
}

void AegisAssistantDock::writeAdviceJson(const AegisAIEngine::Advice &advice) {
    const QString file = QFileDialog::getSaveFileName(this, tr("Export Advice"), QString(), tr("JSON (*.json)"));
    if (file.isEmpty()) return;
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(advice.toJson().toJson(QJsonDocument::Indented));
}

