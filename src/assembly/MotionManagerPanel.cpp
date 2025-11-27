#include "MotionManagerPanel.h"
#include <QHBoxLayout>
#include <QDebug>

MotionManagerPanel::MotionManagerPanel(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    setLayout(m_layout);
}

void MotionManagerPanel::setDocument(AssemblyDocument* doc)
{
    m_doc = doc;
    qDeleteAll(findChildren<QWidget*>());
    m_controls.clear();

    if (!m_doc || m_doc->joints().isEmpty()) {
        m_layout->addWidget(new QLabel("No joints defined."));
        return;
    }

    for (const Joint& j : m_doc->joints()) {
        QWidget* row = new QWidget(this);
        QHBoxLayout* hl = new QHBoxLayout(row);

        QLabel* nameLbl = new QLabel(j.name, row);
        QSlider* slider = new QSlider(Qt::Horizontal, row);
        slider->setRange((j.type == JointType::Revolute) ? 0 : -100,
                         (j.type == JointType::Revolute) ? 360 : 100);
        slider->setValue(0);
        QPushButton* playBtn = new QPushButton("▶", row);

        hl->addWidget(nameLbl);
        hl->addWidget(slider, 1);
        hl->addWidget(playBtn);
        row->setLayout(hl);
        m_layout->addWidget(row);

        JointControl ctrl;
        ctrl.label = nameLbl;
        ctrl.slider = slider;
        ctrl.playButton = playBtn;
        ctrl.animator = std::make_unique<JointAnimator>(this);
        ctrl.playing = false;

        m_controls.insert(j.name, std::move(ctrl));

        connect(slider, &QSlider::valueChanged, this, [this, jointName = j.name](int val) {
            onSliderChanged(jointName, val);
        });
        connect(playBtn, &QPushButton::clicked, this, [this, jointName = j.name]() {
            onPlayClicked(jointName);
        });
    }
}

void MotionManagerPanel::onSliderChanged(const QString& jointName, int value)
{
    if (!m_doc || !m_controls.contains(jointName)) return;
    const Joint& j = *std::find_if(m_doc->joints().begin(), m_doc->joints().end(),
                                   [&](const Joint& jj){ return jj.name == jointName; });
    if (m_doc->parts().isEmpty()) return;
    auto part = m_doc->parts().first();
    gp_Trsf trsf;

    if (j.type == JointType::Revolute)
        trsf.SetRotation(j.axis, value * M_PI / 180.0);
    else if (j.type == JointType::Prismatic)
        trsf.SetTranslationPart(j.axis.Direction().Multiplied(value * 0.5));

    part->transform() = trsf;
    emit updateRequested();
}

void MotionManagerPanel::onPlayClicked(const QString& jointName)
{
    if (!m_doc || !m_controls.contains(jointName)) return;
    auto& ctrl = m_controls[jointName];
    const Joint& j = *std::find_if(m_doc->joints().begin(), m_doc->joints().end(),
                                   [&](const Joint& jj){ return jj.name == jointName; });

    auto part = m_doc->parts().first();
    if (!ctrl.playing) {
        ctrl.animator->setTarget(j.type, part, j.axis);
        ctrl.animator->start(2.0);
        connect(ctrl.animator.get(), &JointAnimator::updated, this, &MotionManagerPanel::updateRequested);
        ctrl.playButton->setText("⏸");
        ctrl.playing = true;
    } else {
        ctrl.animator->stop();
        ctrl.playButton->setText("▶");
        ctrl.playing = false;
    }
}

void MotionManagerPanel::stopAll()
{
    for (auto& c : m_controls) {
        c.animator->stop();
        c.playing = false;
        c.playButton->setText("▶");
    }
}
