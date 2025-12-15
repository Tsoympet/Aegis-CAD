#pragma once

#include <QWidget>
#include <QString>

class AnalysisLegendOverlay : public QWidget {
    Q_OBJECT
public:
    explicit AnalysisLegendOverlay(QWidget *parent = nullptr);

    void setResultText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

