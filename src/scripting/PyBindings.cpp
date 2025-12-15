#include "PyBindings.h"
#include "../cad/FeatureOps.h"

namespace py = pybind11;

namespace PyBindings {

void registerBindings(py::module_ &m) {
    m.def("make_box", [](double size) { return FeatureOps::makeBox(size); }, "Generate a simple box");
    m.def("make_cylinder", [](double r, double h) { return FeatureOps::makeCylinder(r, h); }, "Generate a cylinder");
}

} // namespace PyBindings

