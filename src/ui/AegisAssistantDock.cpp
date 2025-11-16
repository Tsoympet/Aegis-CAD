#include "AegisAssistantDock.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

AegisAssistantDock::AegisAssistantDock(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Aegis AI Assistant"));
    auto* w = new QWidget(this);
    auto* layout = new QVBoxLayout(w);

    m_chat = new QTextEdit(w);
    m_chat->setReadOnly(true);
    m_input = new QLineEdit(w);
    m_send  = new QPushButton(tr("Send"), w);

    auto* bottom = new QHBoxLayout();
    bottom->addWidget(m_input, 1);
    bottom->addWidget(m_send);

    layout->addWidget(m_chat, 1);
    layout->addLayout(bottom);

    connect(m_send, &QPushButton::clicked, this, &AegisAssistantDock::onSend);

    w->setLayout(layout);
    setWidget(w);
}

void AegisAssistantDock::setEngine(AegisAIEngine* engine)
{
    m_engine = engine;
}

void AegisAssistantDock::setContextProvider(std::function<AegisContext()> provider)
{
    m_ctxProvider = std::move(provider);
}

void AegisAssistantDock::onSend()
{
    if (!m_engine) return;
    const QString text = m_input->text().trimmed();
    if (text.isEmpty()) return;
    m_chat->append("You: " + text);
    m_input->clear();
    AegisContext ctx = m_ctxProvider ? m_ctxProvider() : AegisContext{};
    AegisReply rep = m_engine->respond(ctx, text);
    m_chat->append("AI: " + rep.text);
}
