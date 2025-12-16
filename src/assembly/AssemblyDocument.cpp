#include "AssemblyDocument.h"

#include "ConstraintSolverAsm.h"

#include <algorithm>

AssemblyDocument::AssemblyDocument() {
    AssemblyNode root;
    root.id = m_rootId;
    root.parentId.clear();
    root.localTransform.SetIdentity();
    m_nodes.emplace(root.id, root);
}

bool AssemblyDocument::addNode(const AssemblyNode &node) {
    if (node.id.isEmpty() || m_nodes.count(node.id) > 0) {
        return false;
    }
    if (hasCircularDependency(node.id, node.parentId)) {
        return false;
    }
    AssemblyNode copy = node;
    copy.localTransform = node.localTransform;
    copy.children.clear();
    m_nodes.emplace(copy.id, copy);
    if (!copy.parentId.isEmpty() && m_nodes.count(copy.parentId)) {
        m_nodes[copy.parentId].children.push_back(copy.id);
    }
    return true;
}

bool AssemblyDocument::removeNode(const QString &id) {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end() || it->first == m_rootId) {
        return false;
    }
    if (!it->second.parentId.isEmpty()) {
        auto &siblings = m_nodes[it->second.parentId].children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), id), siblings.end());
    }
    m_nodes.erase(it);
    m_mates.erase(std::remove_if(m_mates.begin(), m_mates.end(), [&](const MateConstraint &m) {
                        return m.a == id || m.b == id;
                    }),
                  m_mates.end());
    return true;
}

bool AssemblyDocument::attachShape(const QString &id, const TopoDS_Shape &shape) {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return false;
    }
    it->second.shape = shape;
    return true;
}

AssemblyNode *AssemblyDocument::getNode(const QString &id) {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) return nullptr;
    return &it->second;
}

const AssemblyNode *AssemblyDocument::getNode(const QString &id) const {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) return nullptr;
    return &it->second;
}

bool AssemblyDocument::addMate(const MateConstraint &mate) {
    if (mate.a.isEmpty() || mate.b.isEmpty()) {
        return false;
    }
    if (!getNode(mate.a) || !getNode(mate.b)) {
        return false;
    }
    auto dup = std::find_if(m_mates.begin(), m_mates.end(), [&](const MateConstraint &m) { return m.id == mate.id; });
    if (dup != m_mates.end()) {
        return false;
    }
    m_mates.push_back(mate);
    return true;
}

bool AssemblyDocument::removeMate(const QString &id) {
    auto before = m_mates.size();
    m_mates.erase(std::remove_if(m_mates.begin(), m_mates.end(), [&](const MateConstraint &m) { return m.id == id; }), m_mates.end());
    return before != m_mates.size();
}

bool AssemblyDocument::suppressMate(const QString &id, bool suppressed) {
    for (auto &mate : m_mates) {
        if (mate.id == id) {
            mate.suppressed = suppressed;
            return true;
        }
    }
    return false;
}

std::unordered_map<QString, gp_Trsf> AssemblyDocument::computeWorldFrames() const {
    TransformGraph graph;
    for (const auto &pair : m_nodes) {
        const AssemblyNode &node = pair.second;
        if (node.parentId.isEmpty()) {
            graph.addTransform(node.id, gp_Trsf());
        } else {
            graph.addEdge(node.parentId, node.id, node.localTransform);
        }
    }
    return graph.flatten(m_rootId);
}

bool AssemblyDocument::hasCircularDependency(const QString &candidateId, const QString &parentId) const {
    if (candidateId == parentId) {
        return true;
    }
    QString cursor = parentId;
    while (!cursor.isEmpty()) {
        auto it = m_nodes.find(cursor);
        if (it == m_nodes.end()) break;
        if (it->second.parentId == candidateId) {
            return true;
        }
        cursor = it->second.parentId;
    }
    return false;
}

double AssemblyDocument::previewDistance(const QString &a, const QString &b) const {
    const AssemblyNode *nodeA = getNode(a);
    const AssemblyNode *nodeB = getNode(b);
    if (!nodeA || !nodeB || nodeA->shape.IsNull() || nodeB->shape.IsNull()) {
        return -1.0;
    }
    BRepExtrema_DistShapeShape extrema(nodeA->shape, nodeB->shape);
    extrema.Perform();
    if (!extrema.IsDone()) {
        return -1.0;
    }
    return extrema.Value();
}

void AssemblyDocument::reset(const std::unordered_map<QString, AssemblyNode> &nodes, const std::vector<MateConstraint> &mates) {
    m_nodes = nodes;
    m_mates = mates;
}

QString jointTypeToString(JointType type) {
    switch (type) {
    case JointType::Revolute:
        return "revolute";
    case JointType::Prismatic:
        return "prismatic";
    case JointType::Fixed:
        return "fixed";
    case JointType::Planar:
        return "planar";
    case JointType::Ball:
        return "ball";
    case JointType::Slider:
        return "slider";
    }
    return "fixed";
}

JointType jointTypeFromString(const QString &text) {
    const QString lower = text.toLower();
    if (lower == "revolute") return JointType::Revolute;
    if (lower == "prismatic") return JointType::Prismatic;
    if (lower == "planar") return JointType::Planar;
    if (lower == "ball") return JointType::Ball;
    if (lower == "slider") return JointType::Slider;
    return JointType::Fixed;
}

