#pragma once
#include <QDockWidget>
#include <functional>
#include "ai/AegisAIEngine.h"

class QTextEdit;
class QLineEdit;
class QPushButton;

class AegisAssistantDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit AegisAssistantDock(QWidget* parent = nullptr);

    void setEngine(AegisAIEngine* engine);
    void setContextProvider(std::function<AegisContext()> provider);

private slots:
    void onSend();

private:
    AegisAIEngine* m_engine = nullptr;
    std::function<AegisContext()> m_ctxProvider;

    QTextEdit*  m_chat = nullptr;
    QLineEdit*  m_input = nullptr;
    QPushButton* m_send = nullptr;
};
