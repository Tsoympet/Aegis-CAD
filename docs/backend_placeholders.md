# Backend placeholders and MVP coverage

## Analysis (CalculiX backend)
- **Placeholder**: Input deck writes only an 8-node bounding-box cube and clamps a fixed boundary on the first constraint entry, ignoring all other case data.
- **Limitation**: Loads are simplified to a single node load/temperature; solver errors fall back to synthetic fields.
- **MVP behavior**: Minimal CalculiX submission now checks for `ccx` on `PATH`, writes a transient deck, and surfaces whether the solver timed out or failed, falling back with a clear summary.

## CAM
- **Placeholder**: Toolpaths are generated from coarse bounding boxes/face samples without collision checks, stock awareness, or ordering logic.
- **Limitation**: Only a single preview is shown; post flavors are limited to GRBL/Fanuc with no machine limits or tool libraries.
- **MVP behavior**: Existing generator feeds the viewer preview directly, and a menu hook triggers refresh so users can iterate on selections and feeds quickly.

## AI dock
- **Placeholder**: Recommendations were previously baked into code and could not be adjusted without rebuilding.
- **Limitation**: Rules match only on prompt text; no learning loop or telemetry exists yet.
- **MVP behavior**: Rule sets can now be reloaded from a JSON file at runtime (trigger/recommendation pairs), with summaries echoed into the dock for transparency.
