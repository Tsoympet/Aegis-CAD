#include "ModernStyle.h"
#include <QToolBar>
#include <QTabBar>
#include <QPainterPath>
#include <QDebug>

ModernStyle::ModernStyle(QStyle* base)
    : QProxyStyle(base ? base : QStyleFactory::create("Fusion"))
{
}

// ----------------------------------------
// Apply global Fusion 360-like theme
// ----------------------------------------
void ModernStyle::apply(QApplication& app)
{
    app.setStyle(new ModernStyle);

    // Dark gray palette
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(37, 37, 38));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(27, 27, 28));
    palette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(45, 45, 48));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(110, 110, 110));

    app.setPalette(palette);
}

// ----------------------------------------
// Custom Ribbon / Toolbar look
// ----------------------------------------
void ModernStyle::drawPrimitive(PrimitiveElement element,
                                const QStyleOption* option,
                                QPainter* painter,
                                const QWidget* widget) const
{
    switch (element)
    {
    case PE_PanelToolBar:
    case PE_PanelTabBar:
    {
        QRect r = option->rect;
        QLinearGradient grad(r.topLeft(), r.bottomLeft());
        grad.setColorAt(0.0, QColor(58, 58, 61));
        grad.setColorAt(0.5, QColor(45, 45, 48));
        grad.setColorAt(1.0, QColor(35, 35, 38));

        painter->fillRect(r, grad);
        painter->setPen(QColor(25, 25, 25));
        painter->drawRect(r.adjusted(0, 0, -1, -1));
        return;
    }
    case PE_PanelLineEdit:
    case PE_PanelButtonCommand:
        // Add subtle highlight edges
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(55, 55, 58));
        painter->drawRoundedRect(option->rect, 4, 4);
        painter->restore();
        return;
    default:
        break;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}
