#include "PythonConsoleDock.h"
#include <QTextCursor>
#include <QScrollBar>
#include <QDir>
#include <QMessageBox>
#include <QStyle>
#include <QFontDatabase>
#include <Python.h>

PythonConsoleDock::PythonConsoleDock(QWidget* parent)
    : QDockWidget("Python Console", parent)
{
    QWidget* main = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(main);
    layout->setContentsMargins(4, 4, 4, 4);

    output = new QTextEdit(main);
    output->setReadOnly(true);
    output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    output->setStyleSheet(
        "QTextEdit { background: #111; color: #ddd; border: 1px solid #333; }");

    input = new QLineEdit(main);
    input->setPlaceholderText(">>> Enter Python command...");
    input->setStyleSheet(
        "QLineEdit { background: #1a1a1a; color: #0af; border: 1px solid #333; }");

    layout->addWidget(output);
    layout->addWidget(input);
    main->setLayout(layout);
    setWidget(main);

    connect(input, &QLineEdit::returnPressed, this, &PythonConsoleDock::onCommandEntered);

    initPython();
    printMessage("AegisCAD Python Console Ready", QColor("#00aaff"));
}

PythonConsoleDock::~PythonConsoleDock()
{
    finalizePython();
}

void PythonConsoleDock::initPython()
{
    if (initialized) return;
    Py_Initialize();

    // Add the current working directory to sys.path
    PyRun_SimpleString("import sys, os");
    PyRun_SimpleString("sys.path.insert(0, os.getcwd())");
    PyRun_SimpleString("import aegis_analysis as analysis");
    PyRun_SimpleString("print('Imported aegis_analysis')");
    initialized = true;
}

void PythonConsoleDock::finalizePython()
{
    if (Py_IsInitialized())
        Py_Finalize();
}

void PythonConsoleDock::onCommandEntered()
{
    const QString command = input->text().trimmed();
    if (command.isEmpty()) return;

    printMessage(">>> " + command, QColor("#00ffff"));
    history << command;
    historyIndex = history.size();
    input->clear();
    executeCommand(command);
}

void PythonConsoleDock::executeCommand(const QString& command)
{
    if (!Py_IsInitialized()) {
        printMessage("Python interpreter not initialized.", QColor("#ff5555"));
        return;
    }

    QByteArray utf8Cmd = command.toUtf8();
    PyObject* pyMain = PyImport_AddModule("__main__");
    PyObject* pyDict = PyModule_GetDict(pyMain);
    PyObject* result = PyRun_String(utf8Cmd.constData(), Py_single_input, pyDict, pyDict);

    if (!result) {
        PyErr_Print();
        printMessage("[Error] Python execution failed", QColor("#ff5555"));
    } else {
        Py_XDECREF(result);
    }
}

void PythonConsoleDock::printMessage(const QString& msg, const QColor& color)
{
    output->setTextColor(color);
    output->append(msg);
    QScrollBar* sb = output->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void PythonConsoleDock::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Up && historyIndex > 0) {
        historyIndex--;
        input->setText(history[historyIndex]);
    } else if (e->key() == Qt::Key_Down && historyIndex < history.size() - 1) {
        historyIndex++;
        input->setText(history[historyIndex]);
    } else {
        QDockWidget::keyPressEvent(e);
    }
}
