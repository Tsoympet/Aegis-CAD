#include "TransformGraph.h"
#include <QDebug>

void TransformGraph::update(const QVector<std::shared_ptr<AssemblyNode>>& parts)
{
    qDebug() << "TransformGraph updated for" << parts.size() << "nodes.";
}
