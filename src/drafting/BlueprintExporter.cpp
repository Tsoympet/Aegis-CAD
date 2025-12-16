#include "BlueprintExporter.h"

#include <QFont>
#include <QMarginsF>
#include <QPageSize>
#include <QPainter>
#include <QPainterPath>
#include <QPdfWriter>
#include <QPen>
#include <QTextOption>

BlueprintExporter::BlueprintExporter(const DrawingDocument &document) : m_document(document) {}

bool BlueprintExporter::exportToPdf(const QString &filePath) const {
    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QSizeF(m_document.sheetWidth(), m_document.sheetHeight()), QPageSize::Millimeter));
    writer.setResolution(300);

    QPainter painter(&writer);
    if (!painter.isActive()) {
        return false;
    }

    QRectF pageRect(QPointF(0, 0), QSizeF(m_document.sheetWidth(), m_document.sheetHeight()));

    // Reserve regions: title block and BOM at bottom, scale bar in corner, views occupy rest.
    QRectF titleBlockArea(pageRect.left(), pageRect.bottom() - 40.0, pageRect.width() * 0.6, 40.0);
    QRectF bomArea(titleBlockArea.right(), pageRect.bottom() - 60.0, pageRect.width() - titleBlockArea.width(), 60.0);
    QRectF scaleArea(pageRect.left(), pageRect.top(), 60.0, 12.0);
    QRectF viewArea(pageRect.left(), pageRect.top() + scaleArea.height() + 5.0, pageRect.width(),
                    pageRect.height() - titleBlockArea.height() - scaleArea.height() - 5.0);

    drawViews(painter, viewArea.marginsRemoved(QMarginsF(10, 10, 10, 10)));
    drawDimensions(painter);
    drawLeaders(painter);
    drawAnnotations(painter);
    drawTitleBlock(painter, titleBlockArea);
    drawScaleBar(painter, scaleArea);
    drawBom(painter, bomArea);

    painter.end();
    return true;
}

void BlueprintExporter::drawTitleBlock(QPainter &painter, const QRectF &area) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 0.2));
    painter.drawRect(area);

    QTextOption center(Qt::AlignLeft | Qt::AlignVCenter);
    QFont bold = painter.font();
    bold.setBold(true);

    const TitleBlock &t = m_document.titleBlock();
    QRectF row = area.adjusted(2, 2, -2, -2);
    double rowHeight = row.height() / 3.0;

    painter.setFont(bold);
    painter.drawText(QRectF(row.left(), row.top(), row.width(), rowHeight), center,
                     QStringLiteral("Drawing: %1  Rev: %2").arg(t.drawingNumber, t.revision));

    painter.setFont(painter.font());
    painter.drawText(QRectF(row.left(), row.top() + rowHeight, row.width(), rowHeight), center,
                     QStringLiteral("Author: %1  Approver: %2").arg(t.author, t.approver));

    painter.drawText(QRectF(row.left(), row.top() + 2 * rowHeight, row.width(), rowHeight), center,
                     QStringLiteral("%1  Date: %2").arg(t.description, t.date.toString(Qt::ISODate)));
    painter.restore();
}

void BlueprintExporter::drawScaleBar(QPainter &painter, const QRectF &area) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 0.2));
    painter.drawRect(area);

    const ScaleBar &scaleBar = m_document.scaleBar();
    double division = area.width() / std::max(1, scaleBar.divisions);

    for (int i = 0; i <= scaleBar.divisions; ++i) {
        double x = area.left() + division * i;
        painter.drawLine(QPointF(x, area.top()), QPointF(x, area.top() + (i % 2 == 0 ? area.height() : area.height() / 2.0)));
    }

    painter.drawText(area.adjusted(2, 0, -2, 0), Qt::AlignCenter,
                     QStringLiteral("Scale 1:%1").arg(scaleBar.scale, 0, 'f', 1));
    painter.restore();
}

void BlueprintExporter::drawViews(QPainter &painter, const QRectF &area) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 0.25));

    // Lay out views horizontally.
    int viewCount = static_cast<int>(m_document.views().size());
    if (viewCount == 0) {
        painter.restore();
        return;
    }

    double segmentWidth = area.width() / viewCount;
    for (int i = 0; i < viewCount; ++i) {
        QRectF slot(area.left() + i * segmentWidth, area.top(), segmentWidth, area.height());

        const ViewProjection &view = m_document.views()[i];
        if (view.outline().empty()) {
            continue;
        }

        QRectF bounds = view.bounds();
        QPointF center = bounds.center();
        double scaleX = slot.width() / std::max(1.0, bounds.width());
        double scaleY = slot.height() / std::max(1.0, bounds.height());
        double fitScale = 0.8 * std::min(scaleX, scaleY);

        painter.save();
        painter.translate(slot.center());
        painter.scale(fitScale, -fitScale); // flip Y for drawing conventions
        painter.translate(-center);

        QPainterPath path;
        const auto &outline = view.outline();
        path.moveTo(outline.front());
        for (size_t idx = 1; idx < outline.size(); ++idx) {
            path.lineTo(outline[idx]);
        }
        painter.drawPath(path);

        painter.restore();
        painter.drawText(QRectF(slot.left(), slot.bottom() - 10.0, slot.width(), 10.0), Qt::AlignCenter, view.name());
    }

    painter.restore();
}

void BlueprintExporter::drawDimensions(QPainter &painter) const {
    painter.save();
    painter.setPen(QPen(Qt::blue, 0.2));

    for (const auto &dim : m_document.dimensions()) {
        painter.drawLine(dim.start, dim.end);
        QPointF mid = (dim.start + dim.end) / 2.0;
        painter.drawText(QRectF(mid.x() - 20, mid.y() - 5, 40, 10), Qt::AlignCenter, dim.label);
    }

    painter.restore();
}

void BlueprintExporter::drawLeaders(QPainter &painter) const {
    painter.save();
    painter.setPen(QPen(Qt::darkGreen, 0.2));

    for (const auto &leader : m_document.leaders()) {
        if (leader.segments.empty()) {
            continue;
        }

        QPainterPath path(leader.anchor);
        for (const auto &pt : leader.segments) {
            path.lineTo(pt);
        }
        painter.drawPath(path);
        painter.drawText(leader.segments.back() + QPointF(2, 2), leader.text);
    }

    painter.restore();
}

void BlueprintExporter::drawAnnotations(QPainter &painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 0.2));

    for (const auto &note : m_document.annotations()) {
        painter.drawText(QRectF(note.position.x(), note.position.y(), 120, 12), Qt::AlignLeft, note.text);
    }

    painter.restore();
}

void BlueprintExporter::drawBom(QPainter &painter, const QRectF &area) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 0.2));
    painter.drawRect(area);

    if (m_document.bom().empty()) {
        painter.restore();
        return;
    }

    int rows = static_cast<int>(m_document.bom().size());
    double rowHeight = area.height() / (rows + 1);

    painter.drawLine(QPointF(area.left(), area.top() + rowHeight), QPointF(area.right(), area.top() + rowHeight));
    painter.drawText(QRectF(area.left(), area.top(), area.width(), rowHeight), Qt::AlignCenter, "BOM");

    double colWidth = area.width() / 3.0;
    for (int i = 0; i < rows; ++i) {
        const BillOfMaterialRow &row = m_document.bom()[i];
        double y = area.top() + rowHeight * (i + 1);
        painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));

        painter.drawText(QRectF(area.left(), y, colWidth, rowHeight), Qt::AlignLeft | Qt::AlignVCenter, row.key);
        painter.drawText(QRectF(area.left() + colWidth, y, colWidth, rowHeight), Qt::AlignLeft | Qt::AlignVCenter,
                         row.description + (row.isAssembly ? QStringLiteral(" (ASM)") : QString()));
        painter.drawText(QRectF(area.left() + 2 * colWidth, y, colWidth, rowHeight), Qt::AlignCenter,
                         QString::number(row.quantity));
    }

    painter.restore();
}

