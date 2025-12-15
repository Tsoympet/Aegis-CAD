#include "PartRegistry.h"
#include "FeatureOps.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

PartRegistry::PartRegistry() = default;

void PartRegistry::addPart(const QString &name, const TopoDS_Shape &shape) {
    m_parts.push_back({name, shape});
}

TopoDS_Shape PartRegistry::activeShape() const {
    if (m_parts.empty()) return TopoDS_Shape();
    return m_parts.back().shape;
}

TopoDS_Shape PartRegistry::synthesizeFromPrompt(const QString &prompt) {
    QString lower = prompt.toLower();
    if (lower.contains("cylinder")) {
        auto shape = FeatureOps::makeCylinder(20.0, 40.0);
        addPart("PromptCylinder", shape);
        return shape;
    }
    auto shape = FeatureOps::makeBox(50.0);
    addPart("PromptBox", shape);
    return shape;
}

