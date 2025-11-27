#pragma once
#include <QVector>
#include <memory>
#include "AssemblyNode.h"

/// Maintains dependency graph for assembly transforms.
class TransformGraph
{
public:
    void update(const QVector<std::shared_ptr<AssemblyNode>>& parts);
};
