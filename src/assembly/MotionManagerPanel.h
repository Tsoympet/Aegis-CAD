#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <memory>
#include "JointAnimator.h"
#include "AssemblyDocument.h"

class MotionManagerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MotionManagerPanel(QWidget* parent = nullptr);
    void setDocument(AssemblyDocument* doc);
    void stopAll();

signals:
    void updateRequested();

private slots:
    void onPlayClicked(const QString& jointName);
    void onSliderChanged(const QString& jointName, int value);

private:
    struct JointControl {
        QLabel* label;
        QSlider* slider;
        QPushButton* playButton;
        std::unique_ptr<JointAnimator> animator;
        bool playing = false;
    };

    QVBoxLayout* m_layout = nullptr;
    AssemblyDocument* m_doc = nullptr;
    QMap<QString, JointControl> m_controls;
};
