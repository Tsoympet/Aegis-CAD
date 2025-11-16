#pragma once
#include <QDockWidget>

class QTextEdit;
class QPlainTextEdit;
class QPushButton;

class PythonConsoleDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit PythonConsoleDock(QWidget* parent = nullptr);

private slots:
    void onRunClicked();

private:
    QTextEdit*      m_output = nullptr;
    QPlainTextEdit* m_input  = nullptr;
    QPushButton*    m_runBtn = nullptr;
};
