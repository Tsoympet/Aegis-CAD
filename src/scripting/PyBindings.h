#pragma once

#include <pybind11/pybind11.h>
#include <TopoDS_Shape.hxx>

class OccView;
class AnalysisManager;
class AegisAIEngine;

namespace PyBindings {
void registerBindings(pybind11::module_ &m,
                      OccView *view = nullptr,
                      AnalysisManager *analysis = nullptr,
                      AegisAIEngine *aiEngine = nullptr);
}

