#pragma once
#include <QDockWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>

#include "ai/AegisReverseEngine.h"

/// Dock for Reverse Engineering interface:
/// allows users to load reference images/specs and trigger reconstruction.
class ReverseEngineerDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ReverseEngineerDock(QWidget* parent = nullptr);
    ~ReverseEngineerDock() override = default;

signals:
    void modelGenerated(const QString& outputPath);

private slots:
    void onLoadImage();
    void onAddSpecs();
    void onRunReconstruction();

private:
    void appendLog(const QString& msg);

    QTextEdit* m_log = nullptr;
    QPushButton* m_loadImageBtn = nullptr;
    QPushButton* m_addTextBtn = nullptr;
    QPushButton* m_generateBtn = nullptr;
    QLabel* m_preview = nullptr;
    QProgressBar* m_progress = nullptr;

    AegisReverseEngine m_engine;
};
