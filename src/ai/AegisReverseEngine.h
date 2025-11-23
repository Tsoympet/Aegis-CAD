#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <vector>
#include <opencv2/core.hpp>

class TopoDS_Shape;

class AegisReverseEngine : public QObject
{
    Q_OBJECT
public:
    explicit AegisReverseEngine(QObject* parent = nullptr);
    ~AegisReverseEngine() override;

    bool initializePython();
    QString analyzeImage(const QString& imagePath);
    QString analyzeText(const QString& text);
    std::shared_ptr<TopoDS_Shape> reconstructModel();
    QString summary() const;

signals:
    void logMessage(const QString& msg);
    void modelReady(const std::shared_ptr<TopoDS_Shape>& shape);

private:
    bool m_pythonInitialized{false};
    cv::Mat m_lastImage;
    QString m_lastText;
    QString runPythonFusion(const QString& prompt, const cv::Mat& image);
};
