#include "AnalysisLegendOverlay.h"
#include <QFontMetrics>
#include <QPainterPath>
#include <QLinearGradient>

AnalysisLegendOverlay::AnalysisLegendOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(160, 300);
}

void AnalysisLegendOverlay::setRange(double minVal, double maxVal)
{
    m_minVal = minVal;
    m_maxVal = maxVal;
    update();
}

void AnalysisLegendOverlay::setTitle(const QString& title)
{
    m_title = title;
    update();
}

void AnalysisLegendOverlay::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent background
    QRectF bg(0, 0, width(), height());
    p.setBrush(QColor(30, 30, 30, 200));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(bg, 8, 8);

    // Color gradient bar
    const int barMargin = 20;
    QRectF barRect(barMargin, 40, width() - 2*barMargin, height() - 80);
    QLinearGradient grad(barRect.bottomLeft(), barRect.topLeft());
    grad.setColorAt(0.0, QColor(0, 0, 255));
    grad.setColorAt(0.25, QColor(0, 255, 255));
    grad.setColorAt(0.5, QColor(0, 255, 0));
    grad.setColorAt(0.75, QColor(255, 255, 0));
    grad.setColorAt(1.0, QColor(255, 0, 0));
    p.setBrush(grad);
    p.setPen(Qt::NoPen);
    p.drawRect(barRect);

    // Title
    p.setPen(Qt::white);
    QFont f = p.font(); f.setBold(true);
    p.setFont(f);
    p.drawText(QRectF(0, 10, width(), 20), Qt::AlignCenter, m_title);

    // Min / Max values
    p.setFont(QFont("Segoe UI", 8));
    QString minStr = QString::number(m_minVal, 'f', 1);
    QString maxStr = QString::number(m_maxVal, 'f', 1);
    p.drawText(QRectF(0, height() - 30, width() - barMargin, 20),
               Qt::AlignRight | Qt::AlignVCenter, minStr);
    p.drawText(QRectF(0, 30, width() - barMargin, 20),
               Qt::AlignRight | Qt::AlignVCenter, maxStr);

    // Border
    p.setPen(QColor(80, 80, 80, 180));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(bg, 8, 8);
}
