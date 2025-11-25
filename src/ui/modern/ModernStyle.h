#pragma once
#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QProxyStyle>
#include <QPainter>
#include <QLinearGradient>

// Custom modern Fusion-style theme for AegisCAD
class ModernStyle : public QProxyStyle
{
public:
    ModernStyle(QStyle* base = nullptr);
    ~ModernStyle() override = default;

    // Applies modern palette & style globally
    static void apply(QApplication& app);

    // Custom painting for toolbars and ribbons
    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption* option,
                       QPainter* painter,
                       const QWidget* widget = nullptr) const override;

private:
    void setModernPalette(QApplication& app);
};
