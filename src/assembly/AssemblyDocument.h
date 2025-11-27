#pragma once
#include <QString>
#include <QVector>
#include <memory>
#include "AssemblyNode.h"
#include "ConstraintSolverAsm.h"
#include "JointTypes.h"
#include "TransformGraph.h"

/// Represents a complete multi-part assembly document.
class AssemblyDocument
{
public:
    AssemblyDocument();
    ~AssemblyDocument() = default;

    AssemblyNode* addPart(const QString& name, const QString& path);
    bool removePart(const QString& name);

    void addJoint(const Joint& j);
    void removeJoint(const QString& name);

    void solveConstraints();
    void clear();

    const QVector<std::shared_ptr<AssemblyNode>>& parts() const { return m_parts; }
    const QVector<Joint>& joints() const { return m_joints; }

private:
    QVector<std::shared_ptr<AssemblyNode>> m_parts;
    QVector<Joint> m_joints;
    ConstraintSolverAsm m_solver;
    TransformGraph m_graph;
};
