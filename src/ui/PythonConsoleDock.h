#pragma once
#include <QDockWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringList>
#include <QProcessEnvironment>

/// Dockable Python interactive console for embedded scripting.
class PythonConsoleDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit PythonConsoleDock(QWidget* parent = nullptr);
    ~PythonConsoleDock() override;

    void printMessage(const QString& msg, const QColor& color = Qt::lightGray);
    void executeCommand(const QString& command);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void onCommandEntered();

private:
    void initPython();
    void finalizePython();

    QTextEdit*  output = nullptr;
    QLineEdit*  input  = nullptr;
    QStringList history;
    int         historyIndex = -1;
    bool        initialized = false;
};
