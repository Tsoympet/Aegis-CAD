#pragma once

#include <QDockWidget>
#include <QTextEdit>
#include <QLineEdit>

class AegisAssistantDock : public QDockWidget {
    Q_OBJECT
public:
    explicit AegisAssistantDock(const QString &title, QWidget *parent = nullptr);

    void appendResponse(const QString &text);

signals:
    void querySubmitted(const QString &query);

private:
    QTextEdit *m_history{nullptr};
    QLineEdit *m_input{nullptr};
};

