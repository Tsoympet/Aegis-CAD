#pragma once
#include <QDockWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <memory>

class AegisAIEngine;
class AegisReverseEngine;

class AegisAssistantDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit AegisAssistantDock(QWidget* parent = nullptr);
    ~AegisAssistantDock() override;

signals:
    void sendCommand(const QString& cmd);

private slots:
    void onSendClicked();
    void handleAIResponse(const QString& msg);
    void handleLog(const QString& msg);

private:
    std::unique_ptr<AegisAIEngine> m_ai;
    std::unique_ptr<AegisReverseEngine> m_rev;

    QTextEdit* m_console{nullptr};
    QLineEdit* m_input{nullptr};
    QPushButton* m_sendBtn{nullptr};

    void connectSignals();
};
