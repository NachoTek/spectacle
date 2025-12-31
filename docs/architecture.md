# Architecture (Spectacle)

## Executive Summary
Spectacle is a KDE/Qt desktop screenshot and screen recording utility. The architecture centers on a Qt application core with platform-specific capture backends (KWin/Wayland/X11) and a QML/Qt Widgets UI layer. Integration points are primarily DBus services for capture/recording requests and KDE configuration via KConfig.

## Technology Stack

- Language: C++
- Frameworks: Qt 6, KDE Frameworks 6
- Build: CMake
- Optional: Tesseract OCR, KF6DocTools

## Architecture Pattern
Desktop app using Qt/KDE UI with platform-specific backends and DBus integration for system capture services.

## Data Architecture
No database or persistent data models detected. Configuration is managed via KDE KConfig and application settings dialogs.

## API / Integration Design
- DBus interface: `org.kde.Spectacle` (see `src/SpectacleDBusAdapter.*` and `src/SpectacleCore.*`).
- Platform capture services: KWin/Wayland/X11 integrations via DBus and platform backends in `src/Platforms/`.
- No HTTP/REST APIs detected.

## Component Overview

Core subsystems:
- **Core logic:** `src/SpectacleCore.*` orchestrates capture/recording flows and DBus start modes.
- **Export/IO:** `src/ExportManager.*` handles save/export workflows.
- **OCR:** `src/OcrManager.*` integrates OCR features.
- **Platform backends:** `src/Platforms/` for KWin, Wayland, XCB implementations.
- **UI layer:** `src/Gui/` (QML + widgets, settings dialogs).

UI components are cataloged in `docs/ui-component-inventory-root.md`.

## Source Tree
See `docs/source-tree-analysis.md` for annotated directory layout.

## Development Workflow
See `docs/development-instructions.md` for prerequisites, build, run, tests, and formatting.

## Deployment Architecture
CI is configured via GitLab CI templates (see `docs/deployment-configuration.md`). Release and deployment are handled through KDE infrastructure.

## Testing Strategy
Tests are defined via CMake/CTest under `tests/` (e.g., `tests/FilenameTest.cpp`). Use CTest after building (see `docs/development-instructions.md`).
