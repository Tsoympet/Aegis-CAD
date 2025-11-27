#include "ReverseEngineerDock.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QPixmap>

ReverseEngineerDock::ReverseEngineerDock(QWidget* parent)
    : QDockWidget("Reverse Engineering", parent)
{
    QWidget* main = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(main);
    layout->setContentsMargins(6, 6, 6, 6);

    m_log = new QTextEdit(main);
    m_log->setReadOnly(true);
    m_log->setStyleSheet("QTextEdit { background: #111; color: #ccc; border: 1px solid #333; }");

    m_preview = new QLabel(main);
    m_preview->setMinimumHeight(160);
    m_preview->setAlignment(Qt::AlignCenter);
    m_preview->setStyleSheet("QLabel { background: #222; border: 1px solid #333; }");

    m_loadImageBtn = new QPushButton("Load Image", main);
    m_addTextBtn = new QPushButton("Add Specs/Text", main);
    m_generateBtn = new QPushButton("Generate CAD Model", main);
    m_progress = new QProgressBar(main);
    m_progress->setRange(0, 0);
    m_progress->hide();

    QHBoxLayout* btns = new QHBoxLayout();
    btns->addWidget(m_loadImageBtn);
    btns->addWidget(m_addTextBtn);
    btns->addWidget(m_generateBtn);

    layout->addWidget(m_preview);
    layout->addLayout(btns);
    layout->addWidget(m_log);
    layout->addWidget(m_progress);
    main->setLayout(layout);
    setWidget(main);

    connect(m_loadImageBtn, &QPushButton::clicked, this, &ReverseEngineerDock::onLoadImage);
    connect(m_addTextBtn, &QPushButton::clicked, this, &ReverseEngineerDock::onAddSpecs);
    connect(m_generateBtn, &QPushButton::clicked, this, &ReverseEngineerDock::onRunReconstruction);

    appendLog("Reverse Engineering module ready.");
}

void ReverseEngineerDock::appendLog(const QString& msg)
{
    m_log->append("• " + msg);
}

void ReverseEngineerDock::onLoadImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Load Reference Image", QString(), "Images (*.png *.jpg *.jpeg *.bmp)");
    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) {
        appendLog("Failed to load image.");
        return;
    }

    m_preview->setPixmap(QPixmap::fromImage(img.scaled(240, 160, Qt::KeepAspectRatio)));
    m_engine.addReferenceImage(img);
    appendLog("Loaded image: " + QFileInfo(path).fileName());
}

void ReverseEngineerDock::onAddSpecs()
{
    QString path = QFileDialog::getOpenFileName(this, "Load Text Specs", QString(), "Text Files (*.txt *.md)");
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        appendLog("Failed to open specs file.");
        return;
    }

    QString text = QString::fromUtf8(f.readAll());
    f.close();

    m_engine.addTextSpec(text);
    appendLog("Loaded text specs.");
}

void ReverseEngineerDock::onRunReconstruction()
{
    m_progress->show();
    appendLog("Starting model reconstruction...");

    QtConcurrent::run([this] {
        QString output = "generated_model.brep";
        bool ok = m_engine.generateModel(output);

        QMetaObject::invokeMethod(this, [=]() {
            m_progress->hide();
            appendLog(m_engine.status());
            if (ok) emit modelGenerated(output);
        });
    });
}
