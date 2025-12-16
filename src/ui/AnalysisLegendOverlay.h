#pragma once

#include <QWidget>
#include <QString>

class AnalysisLegendOverlay : public QWidget {
    Q_OBJECT
public:
    explicit AnalysisLegendOverlay(QWidget *parent = nullptr);

    void setResultText(const QString &text);
    void setRange(double minValue, double maxValue, const QString &units = QString());

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
    double m_min{0.0};
    double m_max{0.0};
    QString m_units;
};

