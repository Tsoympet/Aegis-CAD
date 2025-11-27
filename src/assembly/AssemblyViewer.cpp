#include "AssemblyViewer.h"
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <V3d_Viewer.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <QVBoxLayout>
#include <QDebug>

AssemblyViewer::AssemblyViewer(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    Handle(Aspect_DisplayConnection) dc = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(dc);
    Handle(V3d_Viewer) viewer = new V3d_Viewer(driver);
    viewer->SetDefaultLights();
    viewer->SetLightOn();
    m_context = new AIS_InteractiveContext(viewer);

    m_motionPanel = new MotionManagerPanel(this);
    connect(m_motionPanel, &MotionManagerPanel::updateRequested, this, &AssemblyViewer::refreshDisplay);
    layout->addWidget(m_motionPanel);
    setLayout(layout);
}

void AssemblyViewer::setDocument(AssemblyDocument* doc)
{
    m_doc = doc;
    refreshDisplay();
    if (m_motionPanel)
        m_motionPanel->setDocument(doc);
}

void AssemblyViewer::refreshDisplay()
{
    if (!m_doc) return;
    m_context->RemoveAll(true);

    for (auto& node : m_doc->parts()) {
        if (node->shape().IsNull()) continue;
        BRepBuilderAPI_Transform xf(node->shape(), node->transform());
        Handle(AIS_Shape) ais = new AIS_Shape(xf.Shape());
        m_context->Display(ais, true);
    }

    m_context->UpdateCurrentViewer();
    qDebug() << "Assembly refreshed with" << m_doc->parts().size() << "parts.";
}
