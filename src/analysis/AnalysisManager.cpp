#include "AnalysisManager.h"
#include "BackendFEA_CalculiX.h"
#include <QThread>
#include <QtConcurrent>

AnalysisManager::AnalysisManager(QObject* parent)
    : QObject(parent)
{
    m_backend = new BackendFEA_CalculiX(this);
}

AnalysisManager::~AnalysisManager()
{
    delete m_backend;
}

void AnalysisManager::runCase(const QString& projectPath, AnalysisTemplateKind kind)
{
    QtConcurrent::run([this, projectPath, kind]() {
        AnalysisResult result;

        emit progressUpdated(5);
        result = m_backend->runAnalysis(projectPath, kind, [this](int p) {
            emit progressUpdated(p);
        });

        emit progressUpdated(100);
        emit finished(result);
    });
}
