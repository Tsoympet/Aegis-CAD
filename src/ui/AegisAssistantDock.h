#pragma once

#include "../ai/AegisAIEngine.h"

#include <QDockWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

class AegisAssistantDock : public QDockWidget {
    Q_OBJECT
public:
    explicit AegisAssistantDock(const QString &title, QWidget *parent = nullptr);

    void appendUserPrompt(const QString &text);
    void appendAdvice(const AegisAIEngine::Advice &advice);
    void setHistory(const QStringList &history);
    QStringList history() const { return m_historyLog; }

signals:
    void querySubmitted(const QString &query);

private slots:
    void handleSaveAdvice();

private:
    void writeAdviceJson(const AegisAIEngine::Advice &advice);

    QTextEdit *m_history{nullptr};
    QLineEdit *m_input{nullptr};
    QPushButton *m_export{nullptr};
    QStringList m_historyLog;
    AegisAIEngine::Advice m_lastAdvice;
};

