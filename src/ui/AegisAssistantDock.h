#pragma once
#include <QDockWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QStringList>

#include "ai/AegisAIEngine.h"

/// Dockable AI assistant for AegisCAD.
/// Provides local AI advice, design checks, and context-aware responses.
class AegisAssistantDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit AegisAssistantDock(QWidget* parent = nullptr);
    ~AegisAssistantDock() override = default;

    void loadConversation(const QString& path);
    void saveConversation(const QString& path);

signals:
    /// Emitted when AI produces a response
    void messageGenerated(const QString& message);

private slots:
    void onSendMessage();
    void onClearChat();

private:
    void appendMessage(const QString& sender, const QString& text, const QColor& color);

    QTextEdit*  m_output = nullptr;
    QLineEdit*  m_input  = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_clearBtn = nullptr;

    QStringList m_conversation;  // stored for project persistence
    AegisAIEngine m_engine;      // local AI engine instance
};
