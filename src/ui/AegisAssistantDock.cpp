#include "AegisAssistantDock.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QFontDatabase>

AegisAssistantDock::AegisAssistantDock(QWidget* parent)
    : QDockWidget("Aegis AI Assistant", parent)
{
    QWidget* main = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(main);
    layout->setContentsMargins(6, 6, 6, 6);

    m_output = new QTextEdit(main);
    m_output->setReadOnly(true);
    m_output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_output->setStyleSheet(
        "QTextEdit { background: #111; color: #ddd; border: 1px solid #333; }");

    m_input = new QLineEdit(main);
    m_input->setPlaceholderText("Ask Aegis AI about this model...");
    m_input->setStyleSheet(
        "QLineEdit { background: #1a1a1a; color: #0af; border: 1px solid #333; }");

    m_sendBtn = new QPushButton("Send", main);
    m_clearBtn = new QPushButton("Clear", main);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_input);
    bottomLayout->addWidget(m_sendBtn);
    bottomLayout->addWidget(m_clearBtn);

    layout->addWidget(m_output);
    layout->addLayout(bottomLayout);
    main->setLayout(layout);
    setWidget(main);

    connect(m_sendBtn, &QPushButton::clicked, this, &AegisAssistantDock::onSendMessage);
    connect(m_clearBtn, &QPushButton::clicked, this, &AegisAssistantDock::onClearChat);
    connect(m_input, &QLineEdit::returnPressed, this, &AegisAssistantDock::onSendMessage);

    appendMessage("System", "Aegis AI Assistant ready.", QColor("#00aaff"));
}

void AegisAssistantDock::appendMessage(const QString& sender, const QString& text, const QColor& color)
{
    m_output->setTextColor(color);
    m_output->append(QString("[%1] %2").arg(sender, text));
    QScrollBar* sb = m_output->verticalScrollBar();
    sb->setValue(sb->maximum());
    m_conversation << QString("%1: %2").arg(sender, text);
}

void AegisAssistantDock::onSendMessage()
{
    const QString userInput = m_input->text().trimmed();
    if (userInput.isEmpty()) return;

    appendMessage("You", userInput, QColor("#00ffff"));
    m_input->clear();

    QString response = m_engine.processMessage(userInput);
    appendMessage("AegisAI", response, QColor("#aaff00"));

    emit messageGenerated(response);
}

void AegisAssistantDock::onClearChat()
{
    m_output->clear();
    m_conversation.clear();
    appendMessage("System", "Chat cleared.", QColor("#888"));
}

// -----------------------------------------------------------------------------
// Persistence: Save / Load chat history as part of .aegisproj
// -----------------------------------------------------------------------------

void AegisAssistantDock::saveConversation(const QString& path)
{
    QJsonArray arr;
    for (const QString& msg : m_conversation) arr.append(msg);

    QJsonObject root;
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["conversation"] = arr;

    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
    }
}

void AegisAssistantDock::loadConversation(const QString& path)
{
    QFile f(path);
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    m_output->clear();
    m_conversation.clear();

    const QJsonArray arr = doc.object().value("conversation").toArray();
    for (const auto& v : arr)
        appendMessage("Log", v.toString(), QColor("#888"));
}
