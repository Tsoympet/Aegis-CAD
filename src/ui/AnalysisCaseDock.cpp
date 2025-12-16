#include "AnalysisCaseDock.h"

#include <QVBoxLayout>
#include <QLabel>

AnalysisCaseDock::AnalysisCaseDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(8, 8, 8, 8);

    auto *hint = new QLabel(tr("Set up materials, loads, and constraints before running CalculiX."), container);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    auto *form = new QFormLayout();
    m_template = new QComboBox(container);
    m_template->addItems({tr("Car"), tr("Ship"), tr("Aircraft"), tr("Armor")});
    form->addRow(tr("Template"), m_template);

    m_density = new QDoubleSpinBox(container);
    m_density->setRange(1.0, 50000.0);
    m_density->setValue(7850.0);
    m_density->setSuffix(tr(" kg/m^3"));
    form->addRow(tr("Density"), m_density);

    m_modulus = new QDoubleSpinBox(container);
    m_modulus->setRange(1e8, 1e12);
    m_modulus->setDecimals(0);
    m_modulus->setValue(2.1e11);
    form->addRow(tr("Elastic E"), m_modulus);

    m_yield = new QDoubleSpinBox(container);
    m_yield->setRange(1e5, 2e9);
    m_yield->setDecimals(0);
    m_yield->setValue(350e6);
    form->addRow(tr("Yield"), m_yield);

    m_k = new QDoubleSpinBox(container);
    m_k->setRange(0.1, 500.0);
    m_k->setValue(45.0);
    form->addRow(tr("Thermal k"), m_k);

    m_force = new QDoubleSpinBox(container);
    m_force->setRange(10.0, 1e7);
    m_force->setValue(5000.0);
    m_force->setDecimals(0);
    m_force->setSuffix(tr(" N"));
    form->addRow(tr("Load"), m_force);

    layout->addLayout(form);

    m_run = new QPushButton(tr("Run Analysis"), container);
    m_cubeTest = new QPushButton(tr("Cube Compression Demo"), container);
    layout->addWidget(m_run);
    layout->addWidget(m_cubeTest);

    connect(m_run, &QPushButton::clicked, this, &AnalysisCaseDock::triggerRun);
    connect(m_cubeTest, &QPushButton::clicked, this, &AnalysisCaseDock::cubeCompressionRequested);

    container->setLayout(layout);
    setWidget(container);
}

AnalysisCase AnalysisCaseDock::collect() const {
    AnalysisCase c;
    c.name = tr("UI Case");
    c.material.name = tr("Custom");
    c.material.density = m_density->value();
    c.material.elasticModulus = m_modulus->value();
    c.material.yieldStrength = m_yield->value();
    c.material.thermalConductivity = m_k->value();

    const int idx = m_template->currentIndex();
    c.domain = static_cast<DomainTemplateKind>(idx);

    LoadDefinition load;
    load.type = LoadType::Force;
    load.magnitude = m_force->value();
    load.direction = gp_Vec(0, 0, -1);
    load.targetPartId = QStringLiteral("active");
    c.loads.push_back(load);

    ConstraintDefinition bc;
    bc.type = ConstraintType::Fixed;
    bc.anchor = gp_Pnt(0, 0, 0);
    bc.normal = gp_Dir(0, 0, 1);
    c.constraints.push_back(bc);
    return c;
}

void AnalysisCaseDock::triggerRun() {
    emit runCaseRequested(collect());
}

