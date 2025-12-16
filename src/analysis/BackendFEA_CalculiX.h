#pragma once

#include "AnalysisTypes.h"

#include <QString>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <vector>

class BackendFEA_CalculiX {
public:
    struct FieldPoint {
        int id{0};
        gp_Pnt position;
        double stress{0.0};
        double temperature{0.0};
    };

    struct Result {
        bool success{false};
        QString summary;
        double minStress{0.0};
        double maxStress{0.0};
        double minTemperature{0.0};
        double maxTemperature{0.0};
        std::vector<FieldPoint> field;
        QString rawOutput;
    };

    BackendFEA_CalculiX();

    void setModel(const TopoDS_Shape &shape);
    void setCase(const AnalysisCase &analysisCase);
    Result runAnalysis();

private:
    QString writeInputDeck(const QString &workDir) const;
    Result parseResultFile(const QString &path) const;
    Result synthesizeFallback() const;

    TopoDS_Shape m_shape;
    AnalysisCase m_case;
};

