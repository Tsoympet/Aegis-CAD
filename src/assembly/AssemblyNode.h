#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <vector>

/**
 * @brief Represents a single node in an assembly tree.
 *
 * Nodes can reference external part files (.aegispart) or inline TopoDS shapes.
 * The node stores its local transform relative to the parent and a list of child identifiers
 * to enable hierarchical traversal without duplicating heavy geometry.
 */
struct AssemblyNode {
    QString id;
    QString parentId;
    QString partPath;        //!< Optional reference to a .aegispart file on disk
    TopoDS_Shape shape;      //!< Resolved shape when the part is loaded
    gp_Trsf localTransform;  //!< Local frame relative to parent
    std::vector<QString> children;
    bool isReferenceAssembly{false};
};

