# AegisCAD

AegisCAD is a Qt 6 + OpenCascade based CAD/FEA playground that ships with a dark, production-ready UI, OCC visualization, STEP/IGES I/O, glTF export, a CalculiX-oriented analysis stub, and a pybind11-driven scripting console.

## Features
- Fusion-style dark UI with Segoe UI Semibold typography and tech-blue accents (#00aaff)
- Qt Widgets main window with OCC 3D viewport (zoom/pan/orbit)
- STEP/IGES import/export and glTF export
- AI assistant dock (rule-based) and reverse-engineering prompt dock
- Embedded Python console powered by pybind11 (run snippets, use `aegiscad.make_box` and `make_cylinder`)
- Analysis stub using a CalculiX-oriented backend and on-screen legend overlay
- Sample STEP cube available from the resource bundle for quick smoke testing

## Building
### Prerequisites
- CMake 3.20+
- A C++17 compiler (MSVC 2019+, GCC 11+, or Clang 12+)
- Qt 6 Widgets
- OpenCascade
- pybind11

### vcpkg (recommended on Windows)
```bash
vcpkg install qtbase opencascade pybind11
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### Linux (system packages example)
```bash
sudo apt-get install qtbase5-dev libocct-data-exchange-dev libocct-ocaf-dev libocct-visualization-dev pybind11-dev
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Running
```bash
./build/AegisCAD
```

The application launches with the sample cube loaded. Use the toolbar to import/export STEP, export glTF, run the analysis stub, interact with the AI dock, or issue Python commands.

## Repository Layout
- `src/app` – application bootstrap, main window, project IO
- `src/ui` – OCC view widget, docks, and styling
- `src/cad` – geometry helpers, STEP/IGES IO, glTF export, part registry
- `src/analysis` – analysis manager and CalculiX-oriented stub backend
- `src/ai` – rule-based assistant
- `src/scripting` – pybind11 bindings and script runner
- `src/utils` – logging, settings, JSON helpers
- `resources` – icons, sample STEP part, Qt resource collection

## License
MIT License. See [LICENSE](LICENSE).
