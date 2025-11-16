#include "ReverseEngineerDock.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

ReverseEngineerDock::ReverseEngineerDock(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("AI Reverse Engineer"));

    auto* w = new QWidget(this);
    auto* layout = new QVBoxLayout(w);

    auto* top = new QHBoxLayout();
    m_objectNameEdit = new QLineEdit(w);
    m_objectNameEdit->setPlaceholderText(tr("Object name (e.g. Tiger II turret)"));
    m_domainEdit = new QLineEdit(w);
    m_domainEdit->setPlaceholderText(tr("Domain (armor, aircraft, ship, car...)"));
    top->addWidget(m_objectNameEdit);
    top->addWidget(m_domainEdit);

    m_imageList = new QListWidget(w);
    m_textSources = new QTextEdit(w);
    m_textSources->setPlaceholderText(tr("Paste specs / wiki info / measurements here..."));
    m_log = new QTextEdit(w);
    m_log->setReadOnly(true);

    auto* buttons = new QHBoxLayout();
    auto* addImg  = new QPushButton(tr("Add Image"), w);
    auto* runBtn  = new QPushButton(tr("Run Reverse"), w);
    buttons->addWidget(addImg);
    buttons->addWidget(runBtn);

    layout->addLayout(top);
    layout->addWidget(m_imageList);
    layout->addWidget(m_textSources, 1);
    layout->addLayout(buttons);
    layout->addWidget(m_log, 1);

    connect(addImg, &QPushButton::clicked,
            this, &ReverseEngineerDock::onAddImage);
    connect(runBtn, &QPushButton::clicked,
            this, &ReverseEngineerDock::onRunReverse);

    w->setLayout(layout);
    setWidget(w);
}

void ReverseEngineerDock::setReverseEngine(AegisReverseEngine* engine)
{
    m_engine = engine;
}

void ReverseEngineerDock::onAddImage()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select image"), QString(), tr("Images (*.png *.jpg *.jpeg)"));
    if (path.isEmpty())
        return;
    QImage img(path);
    if (img.isNull())
        return;
    m_images.push_back(img);
    m_imageList->addItem(path);
}

void ReverseEngineerDock::onRunReverse()
{
    if (!m_engine)
    {
        m_log->append("No reverse engine configured.");
        return;
    }
    ReverseSpec spec;
    spec.objectName = m_objectNameEdit->text();
    spec.domain = m_domainEdit->text();
    spec.textInfo = m_textSources->toPlainText();
    spec.imageCount = m_images.size();
    ReverseResult res = m_engine->runReverse(spec);
    m_log->append("Reverse summary: " + res.summary);
}
