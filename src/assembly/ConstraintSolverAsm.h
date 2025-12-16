#pragma once

#include "AssemblyDocument.h"
#include "TransformGraph.h"

#include <TopoDS_Shape.hxx>
#include <unordered_map>

/**
 * @brief Applies mate constraints to compute updated transforms for assembly nodes.
 */
class ConstraintSolverAsm {
public:
    ConstraintSolverAsm();

    /**
     * @brief Solve and update node transforms in place.
     */
    void solve(AssemblyDocument &doc);

    /**
     * @brief Align two frames based on joint type for preview/animation.
     */
    static gp_Trsf alignFrames(const MateConstraint &mate);
};

