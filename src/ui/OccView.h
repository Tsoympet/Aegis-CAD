#pragma once
#include <QWidget>

class OccView : public QWidget
{
    Q_OBJECT
public:
    explicit OccView(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* ev) override;
};
