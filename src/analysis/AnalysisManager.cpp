#include "AnalysisManager.h"

#include "BackendFEA_CalculiX.h"
#include "DomainTemplates.h"
#include "../ui/OccView.h"
#include "../ui/AnalysisLegendOverlay.h"

#include <BRepPrimAPI_MakeBox.hxx>

AnalysisManager::AnalysisManager()
    : m_backend(std::make_unique<BackendFEA_CalculiX>()) {}

void AnalysisManager::setModel(const TopoDS_Shape &shape, const QString &partId) {
    m_shape = shape;
    m_partId = partId;
    m_backend->setModel(shape);
}

void AnalysisManager::setAnalysisCase(const AnalysisCase &analysisCase) {
    m_case = analysisCase;
    m_backend->setCase(analysisCase);
}

void AnalysisManager::attachView(OccView *view, AnalysisLegendOverlay *legend) {
    m_view = view;
    m_legend = legend;
    if (m_view && m_legend) {
        m_view->attachLegend(m_legend);
    }
}

AnalysisManager::Result AnalysisManager::runCase() {
    BackendFEA_CalculiX::Result backendResult = m_backend->runAnalysis();
    visualizeResult(backendResult);

    Result r;
    r.success = backendResult.success;
    r.summary = backendResult.summary;
    r.minStress = backendResult.minStress;
    r.maxStress = backendResult.maxStress;
    r.minTemperature = backendResult.minTemperature;
    r.maxTemperature = backendResult.maxTemperature;
    return r;
}

AnalysisManager::Result AnalysisManager::runCubeCompressionExample() {
    TopoDS_Shape cube = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    setModel(cube, QStringLiteral("cube"));
    DomainTemplates templates;
    AnalysisCase c = templates.cubeCompressionCase(cube);
    setAnalysisCase(c);
    return runCase();
}

void AnalysisManager::visualizeResult(const BackendFEA_CalculiX::Result &backendResult) {
    if (!backendResult.success) return;
    if (!m_view) return;

    std::vector<std::pair<gp_Pnt, double>> samples;
    samples.reserve(backendResult.field.size());
    for (const auto &fp : backendResult.field) {
        samples.emplace_back(fp.position, fp.stress);
    }
    m_view->applyFieldSamples(m_partId, samples, backendResult.minStress, backendResult.maxStress);
    if (m_legend) {
        m_legend->setResultText(backendResult.summary);
        m_legend->setRange(backendResult.minStress, backendResult.maxStress, QStringLiteral("Pa"));
    }
}

