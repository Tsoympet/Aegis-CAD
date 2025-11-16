#pragma once
#include "SketchDocument.h"

class SketchSolver
{
public:
    SketchSolver() = default;
    bool solve(SketchDocument& doc, int iterations = 64, double alpha = 0.25);
};
