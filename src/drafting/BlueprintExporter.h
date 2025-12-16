#pragma once

#include "DrawingDocument.h"

#include <QRectF>
#include <QString>

class QPainter;

/**
 * @brief Exports drawing documents to vector PDF output with title block and BOM.
 */
class BlueprintExporter {
public:
    BlueprintExporter(const DrawingDocument &document);

    bool exportToPdf(const QString &filePath) const;

private:
    void drawTitleBlock(QPainter &painter, const QRectF &area) const;
    void drawScaleBar(QPainter &painter, const QRectF &area) const;
    void drawViews(QPainter &painter, const QRectF &area) const;
    void drawDimensions(QPainter &painter) const;
    void drawLeaders(QPainter &painter) const;
    void drawAnnotations(QPainter &painter) const;
    void drawBom(QPainter &painter, const QRectF &area) const;

    const DrawingDocument &m_document;
};

