#pragma once
#include <QVector>
#include "JointTypes.h"
#include "AssemblyNode.h"

/// Geometric constraint solver for assemblies.
class ConstraintSolverAsm
{
public:
    void solve(const QVector<Joint>& joints, QVector<std::shared_ptr<AssemblyNode>>& parts);
};
