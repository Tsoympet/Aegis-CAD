#pragma once

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <memory>

class OccView;
class AnalysisManager;
class AegisAIEngine;
class ProjectIO;

class ScriptRunner;

class PythonConsoleDock : public QDockWidget {
    Q_OBJECT
public:
    explicit PythonConsoleDock(const QString &title,
                               OccView *view,
                               AnalysisManager *analysis,
                               AegisAIEngine *ai,
                               ProjectIO *projectIO,
                               QWidget *parent = nullptr);

public slots:
    void runBuffer();

private:
    void appendOutput(const QString &text);

    QPlainTextEdit *m_console{nullptr};
    QLineEdit *m_input{nullptr};
    std::unique_ptr<ScriptRunner> m_runner;
};

