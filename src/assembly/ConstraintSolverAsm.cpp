#include "ConstraintSolverAsm.h"
#include <QDebug>

void ConstraintSolverAsm::solve(const QVector<Joint>& joints, QVector<std::shared_ptr<AssemblyNode>>& parts)
{
    Q_UNUSED(parts);
    qDebug() << "Solving" << joints.size() << "joints...";
    for (const auto& j : joints)
        qDebug() << " - Joint:" << j.name << "type" << static_cast<int>(j.type);
}
