#include "PyBindings.h"

#include "../ai/AegisAIEngine.h"
#include "../analysis/AnalysisManager.h"
#include "../analysis/AnalysisTypes.h"
#include "../cad/FeatureOps.h"
#include "../ui/OccView.h"

#include <array>
#include <sstream>
#include <QJsonDocument>
#include <QString>
#include <gp.hxx>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace {

double requirePositive(const char *name, double value) {
    if (value <= 0.0) {
        std::ostringstream oss;
        oss << name << " must be positive";
        throw py::value_error(oss.str());
    }
    return value;
}

gp_Vec requireDirection(const std::array<double, 3> &arr, const char *name) {
    const gp_Vec vec = vecFromArray(arr);
    if (vec.SquareMagnitude() <= gp::Resolution()) {
        std::ostringstream oss;
        oss << name << " must be a non-zero vector";
        throw py::value_error(oss.str());
    }
    return vec;
}

void ensureShapeValid(const TopoDS_Shape &shape, const char *name) {
    if (shape.IsNull()) {
        std::ostringstream oss;
        oss << name << " cannot be an empty shape";
        throw py::value_error(oss.str());
    }
}

gp_Vec vecFromArray(const std::array<double, 3> &arr) {
    return gp_Vec(arr[0], arr[1], arr[2]);
}

std::array<double, 3> arrayFromVec(const gp_Vec &vec) {
    return {vec.X(), vec.Y(), vec.Z()};
}

std::array<double, 3> arrayFromPoint(const gp_Pnt &pnt) {
    return {pnt.X(), pnt.Y(), pnt.Z()};
}

gp_Pnt pointFromArray(const std::array<double, 3> &arr) {
    return gp_Pnt(arr[0], arr[1], arr[2]);
}

void bindAnalysis(py::module_ &m) {
    py::enum_<LoadType>(m, "LoadType")
        .value("Force", LoadType::Force)
        .value("Pressure", LoadType::Pressure)
        .value("Temperature", LoadType::Temperature);

    py::enum_<ConstraintType>(m, "ConstraintType")
        .value("Fixed", ConstraintType::Fixed)
        .value("Slider", ConstraintType::Slider)
        .value("Symmetry", ConstraintType::Symmetry);

    py::enum_<DomainTemplateKind>(m, "DomainTemplateKind")
        .value("Car", DomainTemplateKind::Car)
        .value("Ship", DomainTemplateKind::Ship)
        .value("Aircraft", DomainTemplateKind::Aircraft)
        .value("Armor", DomainTemplateKind::Armor);

    py::class_<MaterialProperty>(m, "MaterialProperty")
        .def(py::init<>())
        .def_readwrite("name", &MaterialProperty::name)
        .def_readwrite("density", &MaterialProperty::density)
        .def_readwrite("elastic_modulus", &MaterialProperty::elasticModulus)
        .def_readwrite("yield_strength", &MaterialProperty::yieldStrength)
        .def_readwrite("thermal_conductivity", &MaterialProperty::thermalConductivity);

    py::class_<LoadDefinition>(m, "LoadDefinition")
        .def(py::init<>())
        .def_readwrite("type", &LoadDefinition::type)
        .def_readwrite("target_part_id", &LoadDefinition::targetPartId)
        .def_readwrite("magnitude", &LoadDefinition::magnitude)
        .def_readwrite("region_hint", &LoadDefinition::regionHint)
        .def_property(
            "direction",
            [](const LoadDefinition &l) { return arrayFromVec(l.direction); },
            [](LoadDefinition &l, const std::array<double, 3> &v) { l.direction = vecFromArray(v); });

    py::class_<ConstraintDefinition>(m, "ConstraintDefinition")
        .def(py::init<>())
        .def_readwrite("type", &ConstraintDefinition::type)
        .def_readwrite("region_hint", &ConstraintDefinition::regionHint)
        .def_property(
            "anchor",
            [](const ConstraintDefinition &c) { return arrayFromPoint(c.anchor); },
            [](ConstraintDefinition &c, const std::array<double, 3> &p) { c.anchor = pointFromArray(p); })
        .def_property(
            "normal",
            [](const ConstraintDefinition &c) { return arrayFromVec(gp_Vec(c.normal)); },
            [](ConstraintDefinition &c, const std::array<double, 3> &v) { c.normal = gp_Dir(vecFromArray(v)); });

    py::class_<AnalysisCase>(m, "AnalysisCase")
        .def(py::init<>())
        .def_readwrite("name", &AnalysisCase::name)
        .def_readwrite("material", &AnalysisCase::material)
        .def_readwrite("loads", &AnalysisCase::loads)
        .def_readwrite("constraints", &AnalysisCase::constraints)
        .def_readwrite("domain", &AnalysisCase::domain);

    py::class_<AnalysisManager::Result>(m, "AnalysisResult")
        .def(py::init<>())
        .def_readwrite("success", &AnalysisManager::Result::success)
        .def_readwrite("summary", &AnalysisManager::Result::summary)
        .def_readwrite("min_stress", &AnalysisManager::Result::minStress)
        .def_readwrite("max_stress", &AnalysisManager::Result::maxStress)
        .def_readwrite("min_temperature", &AnalysisManager::Result::minTemperature)
        .def_readwrite("max_temperature", &AnalysisManager::Result::maxTemperature);

    py::class_<AnalysisManager>(m, "AnalysisManager")
        .def(py::init<>())
        .def("set_model",
             [](AnalysisManager &mgr, const TopoDS_Shape &shape, const std::string &partId) {
                 mgr.setModel(shape, QString::fromStdString(partId));
             },
             py::arg("shape"), py::arg("part_id") = std::string("active"))
        .def("set_analysis_case", &AnalysisManager::setAnalysisCase)
        .def("attach_view",
             [](AnalysisManager &mgr, OccView *view) { mgr.attachView(view); },
             py::arg("view"))
        .def("run_case", &AnalysisManager::runCase)
        .def("last_result", &AnalysisManager::lastResult, py::return_value_policy::copy);
}

void bindAI(py::module_ &m) {
    py::class_<AegisAIEngine::Advice>(m, "Advice")
        .def(py::init<>())
        .def_readwrite("summary", &AegisAIEngine::Advice::summary)
        .def_readwrite("recommendations", &AegisAIEngine::Advice::recommendations)
        .def("to_json", [](const AegisAIEngine::Advice &advice) {
            return QString::fromUtf8(advice.toJson().toJson(QJsonDocument::Indented)).toStdString();
        });

    py::class_<AegisAIEngine>(m, "AegisAIEngine")
        .def(py::init<>())
        .def("set_scene_insights", &AegisAIEngine::setSceneInsights)
        .def("set_stress_snapshot", &AegisAIEngine::setStressSnapshot)
        .def("query", &AegisAIEngine::query, py::arg("prompt"))
        .def("optimize", &AegisAIEngine::optimize, py::arg("objective"));
}

} // namespace

namespace PyBindings {

void registerBindings(py::module_ &m, OccView *view, AnalysisManager *analysis, AegisAIEngine *aiEngine) {
    py::class_<TopoDS_Shape>(m, "Shape").def(py::init<>());

    m.def("make_box",
          [](double size) {
              requirePositive("size", size);
              return FeatureOps::makeBox(size);
          },
          py::arg("size"),
          R"doc(Create a box solid.

Args:
    size (float): Uniform edge length (> 0).

Returns:
    Shape: A TopoDS_Shape representing the box.
          )doc");
    m.def("make_cylinder",
          [](double r, double h) {
              requirePositive("radius", r);
              requirePositive("height", h);
              return FeatureOps::makeCylinder(r, h);
          },
          py::arg("radius"), py::arg("height"),
          R"doc(Create a right circular cylinder.

Args:
    radius (float): Cylinder radius (> 0).
    height (float): Cylinder height (> 0).

Returns:
    Shape: A TopoDS_Shape representing the cylinder.
          )doc");
    m.def("extrude",
          [](const TopoDS_Shape &profile, double height, const std::array<double, 3> &direction) {
              ensureShapeValid(profile, "profile");
              requirePositive("height", height);
              const gp_Vec dir = requireDirection(direction, "direction");
              return FeatureOps::extrude(profile, height, dir);
          },
          py::arg("profile"), py::arg("height"), py::arg("direction") = std::array<double, 3>{0, 0, 1},
          R"doc(Extrude a profile along a direction vector.

Args:
    profile (Shape): The profile to extrude; cannot be empty.
    height (float): Length of the extrusion (> 0).
    direction (tuple[float, float, float], optional): Direction vector; must be non-zero. Defaults to (0, 0, 1).

Returns:
    Shape: The resulting prismatic solid.
          )doc");
    m.def("revolve",
          [](const TopoDS_Shape &profile, const std::array<double, 3> &axisPoint, const std::array<double, 3> &axisDir, double angle) {
              ensureShapeValid(profile, "profile");
              const gp_Vec axisVec = requireDirection(axisDir, "axis_dir");
              gp_Ax1 axis(pointFromArray(axisPoint), gp_Dir(axisVec));
              return FeatureOps::revolve(profile, axis, angle);
          },
          py::arg("profile"), py::arg("axis_point"), py::arg("axis_dir"), py::arg("angle"),
          R"doc(Revolve a profile about an axis.

Args:
    profile (Shape): The profile to revolve; cannot be empty.
    axis_point (tuple[float, float, float]): A point on the rotation axis.
    axis_dir (tuple[float, float, float]): Direction vector of the axis; must be non-zero.
    angle (float): Rotation angle in radians.

Returns:
    Shape: The resulting revolved solid.
          )doc");
    m.def("fillet",
          [](const TopoDS_Shape &shape, double radius) {
              ensureShapeValid(shape, "shape");
              requirePositive("radius", radius);
              return FeatureOps::fillet(shape, radius);
          },
          py::arg("shape"), py::arg("radius"),
          R"doc(Apply a constant-radius fillet to all eligible edges.

Args:
    shape (Shape): Target shape; cannot be empty.
    radius (float): Fillet radius (> 0).

Returns:
    Shape: A shape with filleted edges.
          )doc");

    if (view) {
        m.def("display",
              [view](const TopoDS_Shape &shape) {
                  ensureShapeValid(shape, "shape");
                  view->displayShape(shape);
              },
              py::arg("shape"), R"doc(Display a shape in the active view.

Args:
    shape (Shape): The shape to render; cannot be empty.
          )doc");
        m.def("clear", [view]() { view->clearView(); }, R"doc(Clear all shapes from the view.)doc");
        m.def("zoom_fit", [view]() { view->zoomToFit(); }, R"doc(Zoom the viewer to fit displayed content.)doc");
    }

    bindAnalysis(m);
    if (analysis) {
        m.attr("analysis") = py::cast(analysis, py::return_value_policy::reference);
    }

    bindAI(m);
    if (aiEngine) {
        m.attr("ai") = py::cast(aiEngine, py::return_value_policy::reference);
    }
}

} // namespace PyBindings

