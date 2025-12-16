#include "PerformanceProfilerDock.h"

#include <QFormLayout>
#include <QVBoxLayout>

PerformanceProfilerDock::PerformanceProfilerDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    buildUi();
}

void PerformanceProfilerDock::buildUi() {
    auto *container = new QWidget(this);
    auto *layout = new QFormLayout(container);
    m_fpsLabel = new QLabel(tr("0 fps"), container);
    m_frameTimeLabel = new QLabel(tr("0 ms"), container);
    m_memoryLabel = new QLabel(tr("GPU 0 MB"), container);
    m_cacheLabel = new QLabel(tr("Cache 0"), container);
    m_partsLabel = new QLabel(tr("Parts 0"), container);

    layout->addRow(tr("FPS"), m_fpsLabel);
    layout->addRow(tr("Frame time"), m_frameTimeLabel);
    layout->addRow(tr("GPU"), m_memoryLabel);
    layout->addRow(tr("Tessellation cache"), m_cacheLabel);
    layout->addRow(tr("Active parts"), m_partsLabel);

    container->setLayout(layout);
    setWidget(container);

    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    connect(m_timer, &QTimer::timeout, this, &PerformanceProfilerDock::refreshStats);
}

void PerformanceProfilerDock::attachView(OccView *view) {
    m_view = view;
    if (m_timer) {
        m_timer->start();
    }
}

void PerformanceProfilerDock::refreshStats() {
    if (!m_view) return;
    const auto s = m_view->stats();
    m_fpsLabel->setText(QString::number(s.fps, 'f', 1));
    m_frameTimeLabel->setText(QString::number(s.frameTimeMs, 'f', 2) + tr(" ms"));
    m_memoryLabel->setText(QStringLiteral("GPU %1 MB").arg(QString::number(s.gpuMemoryMB, 'f', 1)));
    m_cacheLabel->setText(QStringLiteral("%1 cached").arg(s.cachedTessellations));
    m_partsLabel->setText(QStringLiteral("%1 parts").arg(s.activeParts));
}

