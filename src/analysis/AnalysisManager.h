#pragma once

#include <TopoDS_Shape.hxx>
#include <QString>
#include <memory>

class BackendFEA_CalculiX;

class AnalysisManager {
public:
    struct Result {
        QString summary;
    };

    AnalysisManager();
    void setModel(const TopoDS_Shape &shape);
    Result runQuickCheck();

private:
    TopoDS_Shape m_shape;
    std::unique_ptr<BackendFEA_CalculiX> m_backend;
};

