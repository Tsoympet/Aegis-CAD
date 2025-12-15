#include "ModernStyle.h"

#include <QPalette>
#include <QStyleFactory>

void ModernStyle::applyTo(QApplication &app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(30, 32, 38));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 26, 30));
    palette.setColor(QPalette::AlternateBase, QColor(45, 48, 58));
    palette.setColor(QPalette::ToolTipBase, QColor(30, 32, 38));
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(45, 48, 58));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Highlight, QColor(0, 170, 255));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(palette);
}

