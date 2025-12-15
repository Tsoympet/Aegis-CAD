#pragma once

#include <QApplication>
#include <memory>

class ModernStyle;

class AegisApp : public QApplication {
    Q_OBJECT
public:
    AegisApp(int &argc, char **argv);
    ~AegisApp() override;

private:
    std::unique_ptr<ModernStyle> m_style;
    void setupPalette();
};

