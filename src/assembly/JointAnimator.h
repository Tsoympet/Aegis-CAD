#pragma once
#include <QObject>
#include <QTimer>
#include <memory>
#include "JointTypes.h"
#include "AssemblyNode.h"

class JointAnimator : public QObject
{
    Q_OBJECT
public:
    explicit JointAnimator(QObject* parent = nullptr);
    void setTarget(JointType type, std::shared_ptr<AssemblyNode> node, const gp_Ax1& axis);
    void start(double speed = 1.0);
    void stop();

signals:
    void updated();

private slots:
    void onTick();

private:
    QTimer m_timer;
    std::shared_ptr<AssemblyNode> m_target;
    gp_Ax1 m_axis;
    JointType m_type { JointType::Fixed };
    double m_angle { 0.0 };
    double m_speed { 1.0 };
};
