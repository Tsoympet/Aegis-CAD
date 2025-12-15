#pragma once

#include <pybind11/pybind11.h>
#include <TopoDS_Shape.hxx>

namespace PyBindings {
void registerBindings(pybind11::module_ &m);
}

