#pragma once
#include <vector>

struct AsmConstraint {
    int id {0};
};

class ConstraintSolverAsm
{
public:
    bool solve(const std::vector<AsmConstraint>& constraints);
};
