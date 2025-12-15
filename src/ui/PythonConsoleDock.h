#pragma once

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <memory>

class ScriptRunner;

class PythonConsoleDock : public QDockWidget {
    Q_OBJECT
public:
    explicit PythonConsoleDock(const QString &title, QWidget *parent = nullptr);

public slots:
    void runBuffer();

private:
    void appendOutput(const QString &text);

    QPlainTextEdit *m_console{nullptr};
    QLineEdit *m_input{nullptr};
    std::unique_ptr<ScriptRunner> m_runner;
};

