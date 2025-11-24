#pragma once
#include <QApplication>
#include <QString>
#include <memory>

class MainWindow;

class AegisApp
{
public:
    AegisApp(int& argc, char** argv);
    ~AegisApp();

    int run();

private:
    QApplication m_app;
    std::unique_ptr<MainWindow> m_mainWindow;

    void initStyle();
    void initOpenCascade();
};
