#pragma once

#include "AnalysisTypes.h"
#include "BackendFEA_CalculiX.h"

#include <TopoDS_Shape.hxx>
#include <QString>
#include <memory>

class BackendFEA_CalculiX;
class OccView;
class AnalysisLegendOverlay;

class AnalysisManager {
public:
    struct Result {
        bool success{false};
        QString summary;
        double minStress{0.0};
        double maxStress{0.0};
        double minTemperature{0.0};
        double maxTemperature{0.0};
    };

    AnalysisManager();
    void setModel(const TopoDS_Shape &shape, const QString &partId = QStringLiteral("active"));
    void setAnalysisCase(const AnalysisCase &analysisCase);
    void attachView(OccView *view, AnalysisLegendOverlay *legend = nullptr);

    Result runCase();
    Result runCubeCompressionExample();

private:
    void visualizeResult(const BackendFEA_CalculiX::Result &backendResult);

    TopoDS_Shape m_shape;
    QString m_partId{QStringLiteral("active")};
    AnalysisCase m_case;
    std::unique_ptr<BackendFEA_CalculiX> m_backend;
    OccView *m_view{nullptr};
    AnalysisLegendOverlay *m_legend{nullptr};
};

