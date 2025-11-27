#include "AssemblyNode.h"
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <QDebug>

AssemblyNode::AssemblyNode(const QString& name, const QString& path)
    : m_name(name), m_path(path)
{
    BRep_Builder builder;
    if (!BRepTools::Read(m_shape, path.toStdString().c_str(), builder)) {
        qDebug() << "Failed to load shape from" << path;
    }
    m_trsf.SetTranslation(gp_Vec(0, 0, 0));
}
