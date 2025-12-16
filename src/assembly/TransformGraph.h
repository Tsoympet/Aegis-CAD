#pragma once

#include <QString>
#include <gp_Trsf.hxx>
#include <unordered_map>
#include <vector>

/**
 * @brief Lightweight DAG for propagating transforms through the assembly tree.
 */
class TransformGraph {
public:
    void addTransform(const QString &id, const gp_Trsf &trsf);
    void addEdge(const QString &parent, const QString &child, const gp_Trsf &local);

    /**
     * @brief Flatten all transforms to absolute/world space.
     */
    std::unordered_map<QString, gp_Trsf> flatten(const QString &root) const;

private:
    struct Edge {
        QString parent;
        QString child;
        gp_Trsf local;
    };

    std::unordered_map<QString, gp_Trsf> m_roots;
    std::vector<Edge> m_edges;
};

