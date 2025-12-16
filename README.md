# AegisCAD 1.0

AegisCAD is a Qt 6 + OpenCascade based CAD/FEA playground that ships with a production-ready dark UI, OCC visualization, STEP/IGES I/O, glTF export, and a pybind11-driven scripting console. The 1.0 release also ships a Windows installer and reproducible CI builds for Windows (MSVC) and Ubuntu (GCC).

## Features
- Fusion-style dark UI with Segoe UI Semibold typography and tech-blue accents (`#00aaff`).
- Qt Widgets main window with OCC 3D viewport supporting zoom/pan/orbit and a sample STEP cube loaded on startup.
- STEP/IGES import/export and glTF export via OpenCascade helpers.
- AI assistant dock (rule-based) and reverse-engineering prompt dock.
- Embedded Python console powered by pybind11 (run snippets using `aegiscad.make_box`, `make_cylinder`, etc.).
- Analysis stub using a CalculiX-oriented backend and on-screen legend overlay.
- CAM dock with toolpath preview stubs and performance profiler dock for quick diagnostics.
- Configurable settings backed by `QSettings` and lightweight logging helpers.

## Build Prerequisites
- CMake 3.20+
- C++17 compiler (MSVC 2019+, GCC 11+, or Clang 12+)
- Qt 6 Widgets
- OpenCascade
- pybind11
- (Optional) OpenCV and Tesseract for enhanced AI/computer-vision features.

## Quick Build
Use the standard CMake configure/build pipeline from the repository root:

```bash
cmake -B build -S . && cmake --build build
```

## Platform Notes
### Windows (MSVC + vcpkg)
```powershell
# Install dependencies
vcpkg install qtbase opencascade pybind11

# Configure with the vcpkg toolchain
$toolchain = "$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake -B build -S . -A x64 -DCMAKE_TOOLCHAIN_FILE=$toolchain -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure --build-config Release
```

### Ubuntu (GCC)
```bash
sudo apt-get update
sudo apt-get install -y qtbase5-dev libocct-data-exchange-dev libocct-ocaf-dev libocct-visualization-dev pybind11-dev
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### Artifact Packaging
- `cmake --install build --prefix out` installs the runnable bundle into `out/` (used by CI artifacts).
- Windows CI also runs an Inno Setup script to generate an installer alongside the zipped install tree.

## Running
The built executable lives in `build/AegisCAD` (or `build/Release/AegisCAD.exe` on Windows). Launch it to load the sample cube, then use the toolbar for STEP/glTF operations, run the analysis stub, or drive the AI/Python docks.

## Repository Layout
- `src/app` – application bootstrap, main window, project IO
- `src/ui` – OCC view widget, docks, and styling
- `src/cad` – geometry helpers, STEP/IGES IO, glTF export, part registry
- `src/analysis` – analysis manager and CalculiX-oriented stub backend
- `src/ai` – rule-based assistant
- `src/cam` – CAM preview stubs
- `src/scripting` – pybind11 bindings and script runner
- `src/utils` – logging, settings, JSON helpers
- `resources` – icons, sample STEP part, Qt resource collection
- `installer` – Windows Inno Setup definition used by CI
- `.github/workflows` – CI/CD pipelines for build, test, and packaging

## License
MIT License. See [LICENSE](LICENSE).
