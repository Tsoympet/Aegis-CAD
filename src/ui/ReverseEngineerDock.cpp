#include "ReverseEngineerDock.h"

#include "../cad/FeatureOps.h"

#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

ReverseEngineerDock::ReverseEngineerDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(6, 6, 6, 6);

    auto *hint = new QLabel(tr("Load a blueprint photo and describe the object. AegisCAD will emit a parametric stub."), container);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #cccccc;");
    layout->addWidget(hint);

    auto *fileRow = new QFrame(container);
    auto *rowLayout = new QHBoxLayout(fileRow);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    m_blueprintPath = new QLineEdit(fileRow);
    m_blueprintPath->setPlaceholderText(tr("Blueprint image path"));
    rowLayout->addWidget(m_blueprintPath, 1);
    m_selectBlueprint = new QPushButton(tr("Browse"), fileRow);
    rowLayout->addWidget(m_selectBlueprint);
    fileRow->setLayout(rowLayout);
    layout->addWidget(fileRow);

    m_prompt = new QPlainTextEdit(container);
    m_prompt->setPlaceholderText(tr("e.g., 'cylindrical pressure vessel with flanges'"));
    layout->addWidget(m_prompt, 1);

    m_generate = new QPushButton(tr("Generate placeholder"), container);
    layout->addWidget(m_generate);

    connect(m_generate, &QPushButton::clicked, this, &ReverseEngineerDock::triggerGenerate);
    connect(m_selectBlueprint, &QPushButton::clicked, this, &ReverseEngineerDock::chooseBlueprint);

    container->setLayout(layout);
    setWidget(container);
}

void ReverseEngineerDock::triggerGenerate() {
    emit modelGenerated(generatePlaceholder());
}

void ReverseEngineerDock::chooseBlueprint() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select blueprint"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (!path.isEmpty()) {
        m_blueprintPath->setText(path);
    }
}

TopoDS_Shape ReverseEngineerDock::generatePlaceholder() const {
    const QString desc = m_prompt->toPlainText().toLower();
    const QString blueprint = m_blueprintPath->text().toLower();
    const bool cylinderHint = desc.contains(QStringLiteral("cylinder")) || blueprint.contains(QStringLiteral("cyl"));
    const bool thin = desc.contains(QStringLiteral("tube")) || desc.contains(QStringLiteral("pipe"));

    if (cylinderHint) {
        const double radius = thin ? 15.0 : 25.0;
        const double height = thin ? 80.0 : 50.0;
        return FeatureOps::makeCylinder(radius, height);
    }

    const double size = desc.contains(QStringLiteral("large")) ? 120.0 : 60.0;
    return FeatureOps::makeBox(size);
}

