#pragma once
#include <QWidget>
#include <AIS_InteractiveContext.hxx>
#include "AssemblyDocument.h"
#include "MotionManagerPanel.h"

/// Assembly visualization and motion control viewer.
class AssemblyViewer : public QWidget
{
    Q_OBJECT
public:
    explicit AssemblyViewer(QWidget* parent = nullptr);
    ~AssemblyViewer() override = default;

    void setDocument(AssemblyDocument* doc);
    void refreshDisplay();

private:
    Handle(AIS_InteractiveContext) m_context;
    AssemblyDocument* m_doc = nullptr;
    MotionManagerPanel* m_motionPanel = nullptr;
};
