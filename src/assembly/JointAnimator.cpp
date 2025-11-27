#include "JointAnimator.h"
#include <BRepBuilderAPI_Transform.hxx>
#include <QDebug>

JointAnimator::JointAnimator(QObject* parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &JointAnimator::onTick);
    m_timer.setInterval(33); // ~30 FPS
}

void JointAnimator::setTarget(JointType type, std::shared_ptr<AssemblyNode> node, const gp_Ax1& axis)
{
    m_type = type;
    m_target = node;
    m_axis = axis;
}

void JointAnimator::start(double speed)
{
    if (!m_target) return;
    m_speed = speed;
    m_angle = 0.0;
    m_timer.start();
}

void JointAnimator::stop()
{
    m_timer.stop();
}

void JointAnimator::onTick()
{
    if (!m_target) return;

    gp_Trsf trsf;
    if (m_type == JointType::Revolute) {
        m_angle += m_speed;
        trsf.SetRotation(m_axis, m_angle * (M_PI / 180.0));
    } else if (m_type == JointType::Prismatic) {
        gp_Vec vec(m_axis.Direction());
        vec.Multiply(m_speed * 0.1);
        trsf.SetTranslationPart(vec);
    }

    m_target->transform().Multiply(trsf);
    emit updated();
}
