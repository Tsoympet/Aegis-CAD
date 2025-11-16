#include "OccView.h"
#include <QPainter>

OccView::OccView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);
}

void OccView::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    p.setPen(Qt::black);
    p.drawText(20, 40, "AegisCAD – OCC view placeholder");
}
