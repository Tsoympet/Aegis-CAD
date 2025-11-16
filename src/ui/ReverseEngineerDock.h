#pragma once
#include <QDockWidget>
#include <QImage>
#include "ai/AegisReverseEngine.h"

class QListWidget;
class QTextEdit;
class QLineEdit;

class ReverseEngineerDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ReverseEngineerDock(QWidget* parent = nullptr);

    void setReverseEngine(AegisReverseEngine* engine);

private slots:
    void onAddImage();
    void onRunReverse();

private:
    AegisReverseEngine* m_engine = nullptr;
    QLineEdit*   m_objectNameEdit = nullptr;
    QLineEdit*   m_domainEdit = nullptr;
    QListWidget* m_imageList = nullptr;
    QTextEdit*   m_textSources = nullptr;
    QTextEdit*   m_log = nullptr;
    QList<QImage> m_images;
};
