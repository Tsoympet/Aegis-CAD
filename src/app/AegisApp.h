#pragma once
#include <QApplication>
#include <QTranslator>

class AegisApp : public QApplication
{
public:
    AegisApp(int& argc, char** argv);

private:
    void loadTranslations();
    QTranslator m_translator;
};
