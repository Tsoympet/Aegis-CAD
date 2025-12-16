"""Minimal analysis pipeline using the embedded bindings.

The script assumes it is executed through :class:`ScriptRunner` so that the
``aegiscad`` module is pre-registered and ``project_path`` is optionally
provided. When no project is supplied, it builds a default box for testing.
"""

from __future__ import annotations

import aegiscad


DEFAULT_MATERIAL = aegiscad.MaterialProperty()
DEFAULT_MATERIAL.name = "Aluminum"
DEFAULT_MATERIAL.density = 2700
DEFAULT_MATERIAL.elastic_modulus = 69e9
DEFAULT_MATERIAL.yield_strength = 276e6
DEFAULT_MATERIAL.thermal_conductivity = 237


def build_case() -> aegiscad.AnalysisCase:
    case = aegiscad.AnalysisCase()
    case.name = "smoke"
    case.material = DEFAULT_MATERIAL

    load = aegiscad.LoadDefinition()
    load.type = aegiscad.LoadType.Force
    load.target_part_id = "active"
    load.magnitude = 1_000
    load.direction = (0.0, 0.0, -1.0)
    case.loads.append(load)

    constraint = aegiscad.ConstraintDefinition()
    constraint.type = aegiscad.ConstraintType.Fixed
    constraint.region_hint = "base"
    case.constraints.append(constraint)
    return case


def get_shape() -> aegiscad.Shape:
    try:
        # Provided when ScriptRunner loads a project.
        return project_shape  # type: ignore[name-defined]
    except NameError:
        return aegiscad.make_box(10.0)


def run() -> aegiscad.AnalysisResult:
    shape = get_shape()
    manager = getattr(aegiscad, "analysis", aegiscad.AnalysisManager())
    manager.set_model(shape, "active")
    manager.set_analysis_case(build_case())
    return manager.run_case()


if __name__ == "__main__":
    result = run()
    print("Success:", result.success)
    print("Summary:\n", result.summary)
