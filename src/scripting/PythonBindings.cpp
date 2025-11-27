#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "analysis/AnalysisManager.h"
#include "analysis/DomainTemplates.h"

namespace py = pybind11;

static AnalysisManager* g_manager = nullptr;
static AnalysisResult g_lastResult;

void bind_analysis(py::module_& m)
{
    py::enum_<AnalysisTemplateKind>(m, "AnalysisType")
        .value("StaticStructural", AnalysisTemplateKind::StaticStructural)
        .value("ThermalSteady",    AnalysisTemplateKind::ThermalSteady)
        .value("Modal",            AnalysisTemplateKind::Modal)
        .value("Buckling",         AnalysisTemplateKind::Buckling)
        .value("ArmorImpact",      AnalysisTemplateKind::ArmorImpact);

    py::class_<NodeResult>(m, "NodeResult")
        .def_readonly("x", &NodeResult::position.X)
        .def_readonly("y", &NodeResult::position.Y)
        .def_readonly("z", &NodeResult::position.Z)
        .def_readonly("value", &NodeResult::value);

    py::class_<AnalysisResult>(m, "Result")
        .def_readonly("caseName", &AnalysisResult::caseName)
        .def_readonly("maxStress", &AnalysisResult::maxStress)
        .def_readonly("maxDisplacement", &AnalysisResult::maxDisplacement)
        .def_readonly("succeeded", &AnalysisResult::succeeded)
        .def_property_readonly("nodes",
            [](const AnalysisResult& r){ return r.nodes; });

    m.def("run", [](const std::string& path, AnalysisTemplateKind kind) {
        if (!g_manager) g_manager = new AnalysisManager();
        g_lastResult = g_manager->m_backend()->runAnalysis(
            QString::fromStdString(path), kind, [](int){});
        return g_lastResult;
    }, "Run an FEA case");

    m.def("templates", [] {
        QVector<DomainTemplate> list = DomainTemplates::defaults();
        std::vector<std::string> names;
        for (auto& t : list) names.push_back(t.name.toStdString());
        return names;
    }, "List built-in analysis templates");

    m.def("last", [] { return g_lastResult; }, "Return last analysis result");
}

PYBIND11_MODULE(aegis_analysis, m)
{
    m.doc() = "AegisCAD Python analysis bridge";
    bind_analysis(m);
}
