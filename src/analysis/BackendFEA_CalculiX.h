#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>

class BackendFEA_CalculiX {
public:
    BackendFEA_CalculiX();
    void setModel(const TopoDS_Shape &shape);
    QString runStaticCheck();

private:
    TopoDS_Shape m_shape;
};

