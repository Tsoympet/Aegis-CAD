#pragma once
#include <QWidget>
#include <QLinearGradient>
#include <QPainter>

/// Semi-transparent color legend for analysis visualization.
/// Displays color bar, min/max values, and title.
class AnalysisLegendOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit AnalysisLegendOverlay(QWidget* parent = nullptr);

    void setRange(double minVal, double maxVal);
    void setTitle(const QString& title);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    double m_minVal {0.0};
    double m_maxVal {1.0};
    QString m_title {"Stress [MPa]"};
};
