#pragma once

#include "AssemblyNode.h"
#include "TransformGraph.h"

#include <BRepExtrema_DistShapeShape.hxx>
#include <QString>
#include <unordered_map>
#include <vector>

/**
 * @brief Describes the allowable motion between two nodes.
 */
enum class JointType { Revolute, Prismatic, Fixed, Planar, Ball, Slider };

/**
 * @brief High-level mate/constraint between two assembly nodes.
 */
struct MateConstraint {
    QString id;
    QString a;
    QString b;
    JointType type{JointType::Fixed};
    gp_Trsf frameA;             //!< Frame of node A feature
    gp_Trsf frameB;             //!< Frame of node B feature
    bool suppressed{false};
    double limitMin{0.0};
    double limitMax{0.0};
};

/**
 * @brief Root document managing a hierarchy of AssemblyNode instances.
 */
class AssemblyDocument {
public:
    AssemblyDocument();

    bool addNode(const AssemblyNode &node);
    bool removeNode(const QString &id);
    bool attachShape(const QString &id, const TopoDS_Shape &shape);

    AssemblyNode *getNode(const QString &id);
    const AssemblyNode *getNode(const QString &id) const;

    bool addMate(const MateConstraint &mate);
    bool removeMate(const QString &id);
    bool suppressMate(const QString &id, bool suppressed);

    const std::vector<MateConstraint> &mates() const { return m_mates; }
    const std::unordered_map<QString, AssemblyNode> &nodes() const { return m_nodes; }

    /**
     * @brief Resolve world transforms using the transform graph.
     */
    std::unordered_map<QString, gp_Trsf> computeWorldFrames() const;

    /**
     * @brief Detects circular parent relationships.
     */
    bool hasCircularDependency(const QString &candidateId, const QString &parentId) const;

    /**
     * @brief Preview mate distance using precise shape/feature positions.
     */
    double previewDistance(const QString &a, const QString &b) const;

    /**
     * @brief Replace document contents during load.
     */
    void reset(const std::unordered_map<QString, AssemblyNode> &nodes, const std::vector<MateConstraint> &mates);

private:
    std::unordered_map<QString, AssemblyNode> m_nodes;
    std::vector<MateConstraint> m_mates;
    QString m_rootId{"root"};
};

QString jointTypeToString(JointType type);
JointType jointTypeFromString(const QString &text);

