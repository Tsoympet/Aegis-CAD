# Scripting with `aegiscad`

The embedded Python runtime is exposed through the `aegiscad` module that
`ScriptRunner` registers inside the application. The bindings are available to
snippets executed from the Python console as well as to external files executed
through `ScriptRunner::runFile`.

## Exported helpers
- `make_box(size: float) -> Shape`: Create a cube. `size` must be positive.
- `make_cylinder(radius: float, height: float) -> Shape`: Create a right
  circular cylinder. Both parameters must be positive.
- `extrude(profile: Shape, height: float, direction: tuple[float, float, float]) -> Shape`:
  Extrude a non-empty profile along a non-zero direction vector. Height must be
  positive. The default direction is `(0, 0, 1)`.
- `revolve(profile: Shape, axis_point: tuple[float, float, float], axis_dir: tuple[float, float, float], angle: float) -> Shape`:
  Revolve a non-empty profile around a non-zero axis direction by a given angle
  (radians).
- `fillet(shape: Shape, radius: float) -> Shape`: Apply a constant-radius
  fillet to all eligible edges of a non-empty shape. Radius must be positive.
- `display(shape: Shape)`: Render a non-empty shape in the active view.
- `clear()`: Remove all shapes from the view.
- `zoom_fit()`: Fit the current view to the rendered content.

Analysis bindings expose `AnalysisManager`, `AnalysisCase`, `LoadDefinition`,
`ConstraintDefinition`, and `AnalysisResult`, and are available via the global
`aegiscad.analysis` instance when the application provides one. AI bindings are
exposed similarly via `aegiscad.ai`.

## Sample scripts
- `scripts/sample_geometry.py` builds a few solids and renders them when a view
  is available.
- `scripts/sample_analysis.py` constructs a minimal `AnalysisCase` and executes
  it, falling back to a default model when no project is injected by
  `ScriptRunner`.

## Running scripts programmatically
Use `ScriptRunner` when embedding scripts in tests or tooling:

```cpp
ScriptRunner runner(view, analysisManager, aiEngine, projectIO);
const QString status = runner.runFile("/path/to/script.py");
```

`runSnippet` is available for quick one-off statements:

```cpp
const QString status = runner.runSnippet("import aegiscad\nshape = aegiscad.make_box(5.0)");
```

Both methods return `"[ok] Script executed"` on success or propagate the Python
exception text.
