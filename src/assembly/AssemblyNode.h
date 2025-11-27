#pragma once
#include <QString>
#include <gp_Trsf.hxx>
#include <TopoDS_Shape.hxx>

/// Represents an individual part node within an assembly.
class AssemblyNode
{
public:
    AssemblyNode(const QString& name, const QString& path);

    const QString& name() const { return m_name; }
    const QString& path() const { return m_path; }

    gp_Trsf& transform() { return m_trsf; }
    const gp_Trsf& transform() const { return m_trsf; }

    const TopoDS_Shape& shape() const { return m_shape; }
    void setShape(const TopoDS_Shape& s) { m_shape = s; }

private:
    QString m_name;
    QString m_path;
    gp_Trsf m_trsf;
    TopoDS_Shape m_shape;
};
