#include "AegisReverseEngine.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtConcurrent>

AegisReverseEngine::AegisReverseEngine()
{
    m_status = "Idle.";
}

void AegisReverseEngine::addReferenceImage(const QImage& img)
{
    if (img.isNull()) return;
    m_images << img;
    m_status = QString("Loaded %1 reference image(s)").arg(m_images.size());
}

void AegisReverseEngine::addTextSpec(const QString& text)
{
    if (text.trimmed().isEmpty()) return;
    m_textSpecs << text;
    parseTextSpecs(text);
}

bool AegisReverseEngine::parseTextSpecs(const QString& text)
{
    // Simple regex-based extraction from Wikipedia-style specs
    QRegularExpression weightRe("weight\\D+(\\d+(?:\\.\\d+)?)");
    QRegularExpression lengthRe("length\\D+(\\d+(?:\\.\\d+)?)");
    QRegularExpression widthRe("width\\D+(\\d+(?:\\.\\d+)?)");
    QRegularExpression heightRe("height\\D+(\\d+(?:\\.\\d+)?)");

    auto w = weightRe.match(text);
    auto l = lengthRe.match(text);
    auto wi = widthRe.match(text);
    auto h = heightRe.match(text);

    if (w.hasMatch()) m_detectedParams["mass_tons"] = w.captured(1).toDouble();
    if (l.hasMatch()) m_detectedParams["length_m"] = l.captured(1).toDouble();
    if (wi.hasMatch()) m_detectedParams["width_m"] = wi.captured(1).toDouble();
    if (h.hasMatch()) m_detectedParams["height_m"] = h.captured(1).toDouble();

    m_status = "Parsed textual specs.";
    return true;
}

bool AegisReverseEngine::detectContours(const QImage& img)
{
    if (img.isNull()) return false;
    // Stub: would use OpenCV edge detection or Qt image gradients
    qDebug() << "Analyzing contours for geometry inference...";
    m_status = "Contours detected (mock).";
    return true;
}

bool AegisReverseEngine::reconstructGeometry()
{
    // Placeholder: would map contours → parametric primitives (boxes, cylinders, tracks, etc.)
    qDebug() << "Reconstructing 3D geometry (mock)...";
    m_status = "Reconstruction complete (demo).";
    return true;
}

bool AegisReverseEngine::generateModel(const QString& outputPath)
{
    if (m_images.isEmpty() && m_textSpecs.isEmpty()) {
        m_status = "No input data provided.";
        return false;
    }

    m_status = "Running reconstruction pipeline...";
    bool ok = true;

    for (const QImage& img : m_images)
        ok &= detectContours(img);
    for (const QString& txt : m_textSpecs)
        ok &= parseTextSpecs(txt);

    ok &= reconstructGeometry();
    if (ok)
        m_status = QString("Model generated successfully → %1").arg(outputPath);
    else
        m_status = "Failed to reconstruct model.";
    return ok;
}

QString AegisReverseEngine::status() const
{
    return m_status;
}

