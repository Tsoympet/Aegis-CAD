#pragma once
#include <QString>
#include <gp_Ax1.hxx>

enum class JointType { Fixed, Revolute, Prismatic };

struct Joint
{
    QString name;
    QString partA;
    QString partB;
    JointType type { JointType::Fixed };
    gp_Ax1 axis;
};
