#include "AnalysisManager.h"
#include "BackendFEA_CalculiX.h"

AnalysisManager::AnalysisManager()
    : m_backend(std::make_unique<BackendFEA_CalculiX>()) {}

void AnalysisManager::setModel(const TopoDS_Shape &shape) {
    m_shape = shape;
    m_backend->setModel(shape);
}

AnalysisManager::Result AnalysisManager::runQuickCheck() {
    AnalysisManager::Result result;
    result.summary = m_backend->runStaticCheck();
    return result;
}

