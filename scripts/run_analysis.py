"""Example: run a simple analysis on a loaded .aegisproj file.

ScriptRunner injects `project_shape` and `project_chat` when a project
path is provided. This script displays the shape, configures a minimal
load case, and runs the analysis manager bound to the host application.
"""
from aegiscad import (
    analysis,
    AnalysisCase,
    MaterialProperty,
    LoadDefinition,
    ConstraintDefinition,
    LoadType,
    ConstraintType,
    display,
    zoom_fit,
)

shape = globals().get("project_shape")
if shape is None:
    raise RuntimeError("No project shape provided; run via ScriptRunner.runFile with a .aegisproj file")

display(shape)
zoom_fit()

case = AnalysisCase()
case.name = "Python-driven load"
case.material = MaterialProperty()
case.material.name = "7075 Aluminum"
case.material.density = 2810
case.loads.append(LoadDefinition())
case.loads[0].type = LoadType.Force
case.loads[0].magnitude = 1500
case.constraints.append(ConstraintDefinition())
case.constraints[0].type = ConstraintType.Fixed

analysis.set_model(shape, "python_shape")
analysis.set_analysis_case(case)
result = analysis.run_case()
print("Analysis summary:", result.summary)
