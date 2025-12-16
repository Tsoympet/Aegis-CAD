#include "AnalysisLegendOverlay.h"

#include <QPainter>
#include <QLinearGradient>
#include <QStyleOption>
#include <QStringLiteral>

AnalysisLegendOverlay::AnalysisLegendOverlay(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedWidth(260);
}

void AnalysisLegendOverlay::setResultText(const QString &text) {
    m_text = text;
    update();
}

void AnalysisLegendOverlay::setRange(double minValue, double maxValue, const QString &units) {
    m_min = minValue;
    m_max = maxValue;
    m_units = units;
    update();
}

void AnalysisLegendOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (m_text.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int barHeight = 140;
    QRect legendRect(10, 10, width() - 20, barHeight);
    QLinearGradient gradient(legendRect.topLeft(), legendRect.bottomLeft());
    gradient.setColorAt(0.0, QColor(230, 70, 70));
    gradient.setColorAt(0.5, QColor(250, 200, 80));
    gradient.setColorAt(1.0, QColor(40, 120, 210));

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 32, 38, 200));
    painter.drawRoundedRect(legendRect.adjusted(-8, -8, 8, 8), 10, 10);

    painter.setBrush(gradient);
    painter.drawRoundedRect(legendRect, 8, 8);

    painter.setPen(Qt::white);
    painter.drawText(legendRect.adjusted(12, 12, -12, -12), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_text);

    QString rangeLine = tr("Min: %1 %3\nMax: %2 %3").arg(m_min, 0, 'f', 2).arg(m_max, 0, 'f', 2).arg(m_units);
    painter.drawText(legendRect.adjusted(12, barHeight - 54, -12, -12), Qt::AlignLeft | Qt::AlignBottom, rangeLine);
}

