# AegisCAD Build Audit (CMake configure stage)

## Configure results
- `cmake -B build -S .` fails immediately because Qt6 package config files are missing. CMake reports that `Qt6Config.cmake/qt6-config.cmake` cannot be found, so Qt must be installed or `Qt6_DIR` added to `CMAKE_PREFIX_PATH` (e.g., `qt6-base-dev` on Ubuntu or a vcpkg Qt6 toolchain on Windows/MSVC).【96aab2†L1-L17】

## CMake structure observations
- `add_executable(AegisCAD src/app/main.cpp)` is declared before package discovery, then the same target name is re-declared later with only the QRC resource list. The duplicate executable definitions will either error or mask sources, and the resource-only definition drops the earlier linkage to `aegiscad_core`. Consolidating the executable definition and linking once will avoid conflicts.【F:CMakeLists.txt†L9-L95】
- Utility sources (`src/utils/*.cpp`) are built into both `aegiscad_core` and `AegisCADLib`, leading to duplicate object definitions when the library and core are both linked. Either share the target or remove the duplicate listings.【F:CMakeLists.txt†L9-L74】
- All `src` subdirectories are globbed into `AegisCADLib`, so the .cpp files under `src/app`, `ui`, `cad`, `cam`, `assembly`, `analysis`, `ai`, `drafting`, `utils`, and `scripting` are scheduled for compilation once configuration succeeds.【F:CMakeLists.txt†L31-L50】

## Header/Qt signal-slot review
- `ReverseEngineerDock.h` repeats the `chooseBlueprint` slot twice and duplicates its member declarations three times, which will cause redefinition errors and hides the intended QObject interface; the header needs de-duplication before it will compile with Qt's MOC.【F:src/ui/ReverseEngineerDock.h†L22-L67】
- Other QWidget-derived classes in `src/ui` already declare `Q_OBJECT`; no missing macros were found in the UI headers checked.【3f1d79†L1-L10】

## Resources
- `resources/aegiscad.qrc` lists all shipped icons plus `ai_rules.json` and `examples/sample_part.stp`, and each of these files exists in the `resources` tree (icons under `resources/icons/`, example under `resources/examples/`). No missing resources detected.【F:resources/aegiscad.qrc†L1-L13】【7bbbf2†L1-L3】【7ff624†L1-L2】【5d1400†L1-L2】

## Next steps for CI build parity
- Install Qt6 and OpenCascade before re-running CMake (e.g., `sudo apt-get install -y qt6-base-dev libopencascade-dev` on Ubuntu or `vcpkg install qtbase opencascade` with the MSVC toolchain file).
- Regenerate the CMake trace (`cmake -B build -S . --trace-expand > cmake_trace.log`) after dependencies are present to confirm all sources are compiled and imported targets resolve.
