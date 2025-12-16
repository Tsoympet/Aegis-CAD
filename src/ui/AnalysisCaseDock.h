#pragma once

#include <QDockWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>

#include "../analysis/AnalysisTypes.h"

class AnalysisCaseDock : public QDockWidget {
    Q_OBJECT
public:
    explicit AnalysisCaseDock(const QString &title, QWidget *parent = nullptr);

signals:
    void runCaseRequested(const AnalysisCase &analysisCase);
    void cubeCompressionRequested();

private slots:
    void triggerRun();

private:
    AnalysisCase collect() const;

    QComboBox *m_template{nullptr};
    QDoubleSpinBox *m_density{nullptr};
    QDoubleSpinBox *m_modulus{nullptr};
    QDoubleSpinBox *m_yield{nullptr};
    QDoubleSpinBox *m_k{nullptr};
    QDoubleSpinBox *m_force{nullptr};
    QPushButton *m_run{nullptr};
    QPushButton *m_cubeTest{nullptr};
};

