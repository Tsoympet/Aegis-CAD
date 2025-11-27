#include "AssemblyDocument.h"
#include <QDebug>

AssemblyDocument::AssemblyDocument() {}

AssemblyNode* AssemblyDocument::addPart(const QString& name, const QString& path)
{
    auto node = std::make_shared<AssemblyNode>(name, path);
    m_parts.append(node);
    qDebug() << "Added part:" << name << "from" << path;
    return node.get();
}

bool AssemblyDocument::removePart(const QString& name)
{
    for (int i = 0; i < m_parts.size(); ++i) {
        if (m_parts[i]->name() == name) {
            m_parts.remove(i);
            return true;
        }
    }
    return false;
}

void AssemblyDocument::addJoint(const Joint& j)
{
    m_joints.append(j);
    qDebug() << "Joint added:" << j.name;
}

void AssemblyDocument::removeJoint(const QString& name)
{
    for (int i = 0; i < m_joints.size(); ++i) {
        if (m_joints[i].name == name) {
            m_joints.remove(i);
            break;
        }
    }
}

void AssemblyDocument::solveConstraints()
{
    m_solver.solve(m_joints, m_parts);
    m_graph.update(m_parts);
}

void AssemblyDocument::clear()
{
    m_parts.clear();
    m_joints.clear();
}
