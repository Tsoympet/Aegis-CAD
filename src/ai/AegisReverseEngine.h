#pragma once
#include <QString>
#include <QImage>
#include <QVector3D>
#include <QMap>
#include <QThreadPool>

/// Reverse-engineering backend:
/// Takes reference data (images, specs, dimensions, wiki text)
/// and generates base 3D CAD shapes or assemblies.
class AegisReverseEngine
{
public:
    AegisReverseEngine();
    ~AegisReverseEngine() = default;

    /// Feed raw reference image
    void addReferenceImage(const QImage& img);

    /// Feed textual data (e.g., specifications)
    void addTextSpec(const QString& text);

    /// Perform full reconstruction (runs async via QThreadPool)
    bool generateModel(const QString& outputPath);

    /// Optional: preview generation status
    QString status() const;

private:
    bool parseTextSpecs(const QString& text);
    bool detectContours(const QImage& img);
    bool reconstructGeometry();

    QList<QImage> m_images;
    QStringList   m_textSpecs;
    QString       m_status;
    QMap<QString, double> m_detectedParams;
};
