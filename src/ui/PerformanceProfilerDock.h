#pragma once

#include "OccView.h"

#include <QDockWidget>
#include <QLabel>
#include <QTimer>

class PerformanceProfilerDock : public QDockWidget {
    Q_OBJECT
public:
    explicit PerformanceProfilerDock(const QString &title, QWidget *parent = nullptr);

    void attachView(OccView *view);

private slots:
    void refreshStats();

private:
    void buildUi();

    QLabel *m_fpsLabel{nullptr};
    QLabel *m_frameTimeLabel{nullptr};
    QLabel *m_memoryLabel{nullptr};
    QLabel *m_cacheLabel{nullptr};
    QLabel *m_partsLabel{nullptr};
    QTimer *m_timer{nullptr};
    OccView *m_view{nullptr};
};

