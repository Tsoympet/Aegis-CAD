#include "RibbonBar.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

RibbonBar::RibbonBar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("RibbonBar");
    setStyleSheet("QToolButton { icon-size: 32px; padding: 6px; }");

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setMovable(false);

    // Core tabs
    m_tabWidget->addTab(createSketchTab(), QIcon(":/icons/toolbar_extrude.svg"), "Sketch");
    m_tabWidget->addTab(createModelTab(),  QIcon(":/icons/toolbar_fillet.svg"),  "Model");
    m_tabWidget->addTab(createInspectTab(), QIcon(":/icons/toolbar_analysis.svg"), "Inspect");
    m_tabWidget->addTab(createAITab(), QIcon(":/icons/toolbar_ai.svg"), "AI");

    // Layout
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(m_tabWidget);
    setLayout(layout);
}

// ---------------------------------------------
// Factory Methods for Tabs
// ---------------------------------------------

QWidget* RibbonBar::createSketchTab()
{
    auto* tab = new QWidget(this);
    auto* layout = new QHBoxLayout(tab);

    QToolButton* newSketch = makeButton(":/icons/toolbar_extrude.svg", "New Sketch", "Create a new 2D sketch");
    QToolButton* line = makeButton(":/icons/toolbar_line.svg", "Line", "Draw a line");
    QToolButton* circle = makeButton(":/icons/toolbar_circle.svg", "Circle", "Draw a circle");
    QToolButton* constraint = makeButton(":/icons/toolbar_constraint.svg", "Constraint", "Add geometric constraints");

    layout->addWidget(newSketch);
    layout->addWidget(line);
    layout->addWidget(circle);
    layout->addWidget(constraint);
    layout->addStretch();
    tab->setLayout(layout);

    connect(newSketch, &QToolButton::clicked, [this]{ emit actionTriggered("newSketch"); });
    connect(line, &QToolButton::clicked, [this]{ emit actionTriggered("drawLine"); });
    connect(circle, &QToolButton::clicked, [this]{ emit actionTriggered("drawCircle"); });
    connect(constraint, &QToolButton::clicked, [this]{ emit actionTriggered("addConstraint"); });

    return tab;
}

QWidget* RibbonBar::createModelTab()
{
    auto* tab = new QWidget(this);
    auto* layout = new QHBoxLayout(tab);

    QToolButton* extrude = makeButton(":/icons/toolbar_extrude.svg", "Extrude", "Extrude sketch to solid");
    QToolButton* revolve = makeButton(":/icons/toolbar_revolve.svg", "Revolve", "Create a revolve feature");
    QToolButton* fillet = makeButton(":/icons/toolbar_fillet.svg", "Fillet", "Add a fillet to edges");
    QToolButton* chamfer = makeButton(":/icons/toolbar_chamfer.svg", "Chamfer", "Add a chamfer to edges");

    layout->addWidget(extrude);
    layout->addWidget(revolve);
    layout->addWidget(fillet);
    layout->addWidget(chamfer);
    layout->addStretch();

    connect(extrude, &QToolButton::clicked, [this]{ emit actionTriggered("extrudeFeature"); });
    connect(revolve, &QToolButton::clicked, [this]{ emit actionTriggered("revolveFeature"); });
    connect(fillet, &QToolButton::clicked, [this]{ emit actionTriggered("filletFeature"); });
    connect(chamfer, &QToolButton::clicked, [this]{ emit actionTriggered("chamferFeature"); });

    return tab;
}

QWidget* RibbonBar::createInspectTab()
{
    auto* tab = new QWidget(this);
    auto* layout = new QHBoxLayout(tab);

    QToolButton* measure = makeButton(":/icons/toolbar_measure.svg", "Measure", "Measure distances and angles");
    QToolButton* section = makeButton(":/icons/toolbar_section.svg", "Section View", "Create a section cut view");
    QToolButton* mass = makeButton(":/icons/toolbar_mass.svg", "Mass Props", "View mass properties");

    layout->addWidget(measure);
    layout->addWidget(section);
    layout->addWidget(mass);
    layout->addStretch();

    connect(measure, &QToolButton::clicked, [this]{ emit actionTriggered("measureTool"); });
    connect(section, &QToolButton::clicked, [this]{ emit actionTriggered("sectionView"); });
    connect(mass, &QToolButton::clicked, [this]{ emit actionTriggered("massProperties"); });

    return tab;
}

QWidget* RibbonBar::createAITab()
{
    auto* tab = new QWidget(this);
    auto* layout = new QHBoxLayout(tab);

    QToolButton* suggest = makeButton(":/icons/toolbar_ai.svg", "Suggest", "AI-assisted design suggestion");
    QToolButton* analyze = makeButton(":/icons/toolbar_analysis.svg", "Analyze", "Run AI-driven analysis");
    QToolButton* reverse = makeButton(":/icons/toolbar_reverse.svg", "Reverse Engineer", "AI reverse engineering tools");

    layout->addWidget(suggest);
    layout->addWidget(analyze);
    layout->addWidget(reverse);
    layout->addStretch();

    connect(suggest, &QToolButton::clicked, [this]{ emit actionTriggered("aiSuggest"); });
    connect(analyze, &QToolButton::clicked, [this]{ emit actionTriggered("aiAnalyze"); });
    connect(reverse, &QToolButton::clicked, [this]{ emit actionTriggered("aiReverseEngineer"); });

    return tab;
}

QWidget* RibbonBar::createAnalysisTab()
{
    auto* tab = new QWidget(this);
    auto* layout = new QHBoxLayout(tab);

    QToolButton* run = makeButton(":/icons/toolbar_analysis.svg", "Run Analysis", "Run structural or thermal analysis");
    QToolButton* result = makeButton(":/icons/toolbar_results.svg", "View Results", "View simulation results");

    layout->addWidget(run);
    layout->addWidget(result);
    layout->addStretch();

    connect(run, &QToolButton::clicked, [this]{ emit actionTriggered("runAnalysis"); });
    connect(result, &QToolButton::clicked, [this]{ emit actionTriggered("viewResults"); });

    return tab;
}

// ---------------------------------------------
// Contextual Tab Handling
// ---------------------------------------------

void RibbonBar::showContextTab(const QString& name)
{
    if (m_contextTabs.contains(name)) return;

    QWidget* tab = nullptr;
    if (name == "Analysis") tab = createAnalysisTab();

    if (tab)
    {
        int index = m_tabWidget->addTab(tab, QIcon(":/icons/toolbar_analysis.svg"), name);
        m_contextTabs.insert(name, tab);
        m_tabWidget->setCurrentIndex(index);
    }
}

void RibbonBar::hideContextTab(const QString& name)
{
    if (!m_contextTabs.contains(name)) return;
    QWidget* tab = m_contextTabs.value(name);
    int idx = m_tabWidget->indexOf(tab);
    if (idx >= 0)
        m_tabWidget->removeTab(idx);
    m_contextTabs.remove(name);
}

// ---------------------------------------------
// Button Factory
// ---------------------------------------------

QToolButton* RibbonBar::makeButton(const QString& iconPath, const QString& text, const QString& tooltip)
{
    auto* btn = new QToolButton(this);
    btn->setIcon(QIcon(iconPath));
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setText(text);
    btn->setToolTip(tooltip);
    btn->setAutoRaise(true);
    btn->setFocusPolicy(Qt::NoFocus);
    return btn;
}
