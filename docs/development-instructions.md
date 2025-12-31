# Development Instructions

## Prerequisites

- CMake >= 3.16
- C++20 compiler
- Qt6 >= 6.9.0 (Core, Concurrent, Qml, Quick, QuickControls2, QuickTemplates2, Widgets, DBus, PrintSupport, Test, WaylandClient, Multimedia)
- KDE Frameworks 6 >= 6.18.0 (CoreAddons, WidgetsAddons, DBusAddons, Notifications, Config, I18n, KIO, WindowSystem, GlobalAccel, XmlGui, GuiAddons, KirigamiPlatform, StatusNotifierItem, Prison, Crash)
- Wayland (Client), PlasmaWaylandProtocols, LayerShellQt, KPipeWire
- OpenCV 4.7 (core, imgproc)
- XCB components: XFIXES, IMAGE, UTIL, CURSOR
- Optional: Tesseract OCR (enables OCR support if language packs are available)
- Optional: KF6DocTools (builds documentation)
- Optional: KQuickImageEditor (required by CMake; see package in KDE libs)

## Build

Standard CMake build (out-of-source):

```sh
cmake -S . -B build
cmake --build build
```

## Run

The main entry point is `src/Main.cpp`. Binaries are produced via the CMake build in `build/`. Use your platform's standard run step for CMake builds.

## Tests

Tests are configured via CMake (see `tests/` and `ECMAddTests`). Typical CMake/CTest usage:

```sh
ctest --test-dir build
```

## Formatting

CMake enables KDE clang-format integration and configures a Git pre-commit hook.
