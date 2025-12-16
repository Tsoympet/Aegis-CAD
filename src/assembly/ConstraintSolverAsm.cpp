#include "ConstraintSolverAsm.h"

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

ConstraintSolverAsm::ConstraintSolverAsm() = default;

void ConstraintSolverAsm::solve(AssemblyDocument &doc) {
    // Simple forward pass applying mates in order.
    auto frames = doc.computeWorldFrames();
    for (const auto &mate : doc.mates()) {
        if (mate.suppressed) continue;
        auto nodeA = doc.getNode(mate.a);
        auto nodeB = doc.getNode(mate.b);
        if (!nodeA || !nodeB) continue;

        const gp_Trsf alignment = alignFrames(mate);
        gp_Trsf parentFrame = frames[mate.a];
        gp_Trsf newChildFrame = alignment * parentFrame;
        nodeB->localTransform = newChildFrame;
        frames[mate.b] = newChildFrame;
    }
}

gp_Trsf ConstraintSolverAsm::alignFrames(const MateConstraint &mate) {
    gp_Trsf trsf;
    trsf.SetIdentity();
    switch (mate.type) {
    case JointType::Fixed:
        trsf = mate.frameA;
        break;
    case JointType::Revolute: {
        gp_Pnt origin = gp_Pnt(0, 0, 0).Transformed(mate.frameA);
        gp_Dir dir = gp_Dir(0, 0, 1).Transformed(mate.frameA);
        gp_Ax1 axis(origin, dir);
        trsf.SetRotation(axis, mate.limitMin);
        break;
    }
    case JointType::Prismatic: {
        gp_Vec translation(gp_Dir(0, 0, 1));
        translation.Multiply(mate.limitMax);
        trsf.SetTranslationPart(translation);
        break;
    }
    case JointType::Planar:
        trsf = mate.frameA;
        break;
    case JointType::Ball:
        trsf = mate.frameA;
        break;
    case JointType::Slider: {
        gp_Vec translation(gp_Dir(1, 0, 0));
        translation.Multiply(mate.limitMax);
        trsf.SetTranslationPart(translation);
        break;
    }
    }
    return trsf;
}

