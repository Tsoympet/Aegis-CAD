#include "TransformGraph.h"

#include <algorithm>

void TransformGraph::addTransform(const QString &id, const gp_Trsf &trsf) {
    m_roots[id] = trsf;
}

void TransformGraph::addEdge(const QString &parent, const QString &child, const gp_Trsf &local) {
    m_edges.push_back({parent, child, local});
}

std::unordered_map<QString, gp_Trsf> TransformGraph::flatten(const QString &root) const {
    std::unordered_map<QString, gp_Trsf> resolved = m_roots;
    bool progress = true;
    while (progress) {
        progress = false;
        for (const auto &edge : m_edges) {
            if (resolved.count(edge.child)) continue;
            auto parentIt = resolved.find(edge.parent);
            if (parentIt != resolved.end()) {
                gp_Trsf combined = parentIt->second * edge.local;
                resolved[edge.child] = combined;
                progress = true;
            }
        }
    }
    if (resolved.count(root) == 0) {
        resolved[root].SetIdentity();
    }
    return resolved;
}

