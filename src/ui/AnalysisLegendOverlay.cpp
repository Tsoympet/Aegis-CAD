#include "AnalysisLegendOverlay.h"

#include <QPainter>
#include <QLinearGradient>
#include <QStyleOption>

AnalysisLegendOverlay::AnalysisLegendOverlay(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedWidth(220);
}

void AnalysisLegendOverlay::setResultText(const QString &text) {
    m_text = text;
    update();
}

void AnalysisLegendOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (m_text.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect legendRect(10, 10, width() - 20, 120);
    QLinearGradient gradient(legendRect.topLeft(), legendRect.bottomLeft());
    gradient.setColorAt(0.0, QColor(0, 170, 255));
    gradient.setColorAt(1.0, QColor(50, 50, 50));

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 32, 38, 180));
    painter.drawRoundedRect(legendRect.adjusted(-6, -6, 6, 6), 10, 10);

    painter.setBrush(gradient);
    painter.drawRoundedRect(legendRect, 8, 8);

    painter.setPen(Qt::white);
    painter.drawText(legendRect.adjusted(8, 8, -8, -8), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_text);
}

