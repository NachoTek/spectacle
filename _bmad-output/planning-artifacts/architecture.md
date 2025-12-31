---
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8]
inputDocuments:
  - C:\projects\spectacle\_bmad-output\planning-artifacts\prd.md
  - C:\projects\spectacle\_bmad-output\planning-artifacts\ux-design-specification.md
  - C:\projects\spectacle\docs\index.md
  - C:\projects\spectacle\docs\project-overview.md
  - C:\projects\spectacle\docs\architecture.md
  - C:\projects\spectacle\docs\source-tree-analysis.md
  - C:\projects\spectacle\docs\integration-analysis.md
  - C:\projects\spectacle\docs\development-guide.md
  - C:\projects\spectacle\docs\development-instructions.md
  - C:\projects\spectacle\docs\deployment-guide.md
  - C:\projects\spectacle\docs\deployment-configuration.md
  - C:\projects\spectacle\docs\contribution-guide.md
  - C:\projects\spectacle\docs\contribution-guidelines.md
  - C:\projects\spectacle\docs\critical-folders-summary.md
  - C:\projects\spectacle\docs\asset-inventory-root.md
  - C:\projects\spectacle\docs\component-inventory-root.md
  - C:\projects\spectacle\docs\ui-component-inventory-root.md
  - C:\projects\spectacle\docs\state-management-root.md
  - C:\projects\spectacle\docs\comprehensive-analysis-root.md
workflowType: 'architecture'
lastStep: 8
status: 'complete'
completedAt: '2025-12-30T00:55:40.198684'
project_name: 'spectacle'
user_name: 'TerminalSausage'
date: '2025-12-30'
---

# Architecture Decision Document

_This document builds collaboratively through step-by-step discovery. Sections are appended as we work through each architectural decision together._

## Project Context Analysis

### Requirements Overview

**Functional Requirements:**
31 FRs across these categories:
- Capture and selection (full screen, region, active window, delayed, cancel)
- Pre-capture overlay and annotation (free draw, arrows, boxes, circles, resize, adjust underlying UI)
- Post-capture annotation (same tools)
- Screen recording (start, stop, save)
- OCR (extract and access text)
- Output and saving (save location, format, clipboard)
- Hotkeys and launch (global Print Screen override, remap)
- Updates and distribution (EXE/MSI, winget, optional updates)
- Diagnostics and support (bug report with logs)
- Offline operation (all core workflows offline)

**Non-Functional Requirements:**
- Performance: capture result <1s; recording start <1s
- Reliability: 100 percent crash-free in acceptance; 100 percent hotkey reliability
- Privacy: no telemetry
- Platform: Windows 11 only
- UX-driven constraints: parity with Linux Spectacle, pre-capture annotation, live clipboard updates, multi-monitor and DPI correctness, WCAG 2.1 AA

**Scale and Complexity:**
- Primary domain: desktop_app (Windows 11)
- Complexity level: low to medium (brownfield port with new platform backends)
- Estimated architectural components: 9 core components

### Technical Constraints and Dependencies

- Existing codebase is C++20 with Qt6 and KDE Frameworks; Windows port must preserve this architecture.
- Current Linux backends (KWin/XCB/Wayland) must be replaced or abstracted for Windows-native capture and recording.
- Global hotkey override is mandatory and must be reliable under Windows.
- Offline-first requirement limits reliance on cloud services (including OCR).
- Packaging and distribution must support EXE/MSI and winget.

### Cross-Cutting Concerns Identified

- Global hotkey handling and conflict recovery
- Overlay performance and responsiveness (<1s)
- Multi-monitor and per-monitor DPI scaling correctness
- Live clipboard updates on annotation changes
- Offline OCR integration and parity with Linux results
- Diagnostics collection without telemetry
- Consistent UX between overlay and app window modes

## Starter Template Evaluation

### Primary Technology Domain

Desktop (C++/Qt/KDE, CMake) based on project requirements analysis.

### Starter Options Considered

- Generic Qt 6 CMake starter templates (example: GitHub cmake_qt6_template, Qt CMake getting-started docs) - suitable for greenfield apps but not for a brownfield port.
- KDE/Qt Creator app templates - web search returned no usable results (HTTP 202), so not evaluated further.

### Selected Starter: Existing Spectacle Codebase (No New Starter)

**Rationale for Selection:**
- This is a Windows port of an existing C++/Qt/KDE application; re-scaffolding would add risk and drift from the Linux baseline.
- Existing repo already encodes architecture, build, and UI conventions that must be preserved for parity.
- Using the current codebase avoids duplicate work and preserves established testing and tooling.

**Initialization Command:**
No external starter. Continue with the existing repository and standard CMake build workflow.

**Architectural Decisions Provided by Existing Codebase:**

**Language and Runtime:**
C++20, Qt6, KDE Frameworks 6

**Styling Solution:**
Qt/QML components and existing QML UI patterns

**Build Tooling:**
CMake (top-level `CMakeLists.txt`)

**Testing Framework:**
CTest (via CMake)

**Code Organization:**
- `src/` core logic
- `src/Gui/` UI (QML + widgets)
- `src/Platforms/` capture and recording backends
- `tests/` unit and integration tests

**Development Experience:**
Qt Creator friendly structure, CMake build/run, existing CI templates

**Note:** First implementation story should initialize the Windows build within the existing repo, not a new scaffold.

## Core Architectural Decisions

### Decision Priority Analysis

**Critical Decisions (Block Implementation):**
- Use existing Spectacle codebase (no new starter) with C++20, Qt 6.x, KDE Frameworks 6.x
- Desktop UI stack stays Qt Quick/QML + widgets, with Windows 11 styling and DPI tuning
- Helper process for global hotkey reliability (always-on) with Win32 named pipe IPC
- No in-app authentication; rely on OS user context
- No database; use KConfig + filesystem only

**Important Decisions (Shape Architecture):**
- CI/CD: add GitHub Actions for Windows builds
- Packaging: NSIS installer for Windows, plus winget manifest
- Updates: in-app notifications plus winget updates
- Helper auto-start via Registry Run key

**Backend API Decisions (Resolved):**
- Capture: Windows Graphics Capture (WGC)
- Recording: Windows Graphics Capture (WGC) + Media Foundation pipeline
- OCR: Tesseract primary, Windows OCR fallback (offline)

### Data Architecture

- Database: none
- Persistence: KConfig for settings + filesystem for output assets
- Validation: handled in application logic and config schema
- Caching: in-memory only; no persisted cache
- Migrations: use existing KConfig update scripts where needed

### Authentication and Security

- Authentication: none (local desktop app)
- Authorization: not applicable (single-user OS context)
- Security posture: minimize stored data; rely on OS file permissions
- Diagnostics: local files only; no telemetry or network submission required

### API and Communication Patterns

- External APIs: none
- Internal communication: Qt signals/slots and QML bindings
- Helper architecture: always-on background helper for hotkeys
- IPC: Win32 named pipes with versioned message schema
- Single-instance guard and reconnection strategy for helper

### Frontend Architecture

- UI framework: Qt Quick/QML + existing widgets
- State management: Qt models + signals/slots with a shared state object exposed to QML
- View structure: QML states/stacked views (no routing framework)
- Performance: lightweight overlay, pre-load core tools, lazy-load non-critical UI
- Windows polish: Segoe UI Variable, system accent, DPI-aware scaling, D3D-backed rendering

### Infrastructure and Deployment

- CI/CD: GitHub Actions Windows builds (CMake + Qt/KF6)
- Packaging: NSIS installer; winget manifest for distribution
- Updates: in-app update notifications plus winget as update path
- Helper auto-start: Registry Run key (installer registers/removes)

### Decision Impact Analysis

**Implementation Sequence:**
1) Implement WGC capture + WGC/Media Foundation recording + OCR hybrid bindings
2) Implement helper process + Win32 named pipe IPC
3) Integrate UI overlay with backend and helper events
4) Stabilize multi-monitor/DPI behavior and performance
5) Package installer (NSIS) and publish winget manifest

**Cross-Component Dependencies:**
- Helper IPC contract impacts UI, hotkey handling, and installer registration.
- Capture/recording backend choice affects performance and overlay timing guarantees.
- DPI and multi-monitor behavior touches UI rendering, selection math, and capture backends.

## Implementation Patterns & Consistency Rules

### Pattern Categories Defined

**Critical Conflict Points Identified:**
8 areas where agents could diverge (naming, file layout, Windows-specific placement, IPC schema, logging/error formats, UI state updates, tests placement, build integration).

### Naming Patterns

**Database Naming Conventions:**
- Not applicable (no database). Do not introduce DB artifacts without an explicit architecture update.

**API Naming Conventions:**
- No external API. IPC message types and fields use lowerCamelCase.

**Code Naming Conventions:**
- C++ classes: PascalCase (match existing Spectacle classes).
- C++ files: match class name (e.g., `ImagePlatformWin.cpp/.h`).
- Member variables: `mCamelCase` (KDE style).
- Methods: lowerCamelCase.
- QML components: PascalCase file names (e.g., `CaptureOverlay.qml`), ids lowerCamelCase.
- Signals/slots: lowerCamelCase, past-tense for events (e.g., `captureCompleted`).

### Structure Patterns

**Project Organization:**
- Windows-specific platform code lives under `src/Platforms/Windows/`.
- Backends: `ImagePlatformWin.*` and `VideoPlatformWin.*` under `src/Platforms/Windows/`.
- Helper process code under `src/Platforms/Windows/Helper/` with a separate entry point and target.
- UI/QML stays in `src/Gui/` and must remain platform-agnostic where possible.

**File Structure Patterns:**
- New Windows-only files must be guarded in CMake and source with platform checks.
- Tests for Windows-only logic go under `tests/windows/` to avoid cross-platform failures.
- Shared utilities go in `src/` (not duplicated under platform folders).

### Format Patterns

**IPC Message Format:**
- UTF-8 JSON, length-prefixed framing.
- Envelope fields: `version`, `type`, `requestId`, `payload`, `error`.
- `type` values in lowerCamelCase (e.g., `hotkeyPressed`, `captureRequest`).
- Errors use `code` + `message` (code prefix `WINHELPER_`).

**Data Exchange Formats:**
- JSON fields are lowerCamelCase.
- Timestamps are ISO 8601 strings in UTC.

### Communication Patterns

**Event System Patterns:**
- UI events use Qt signals/slots; no direct cross-component calls for capture state.
- Helper IPC is request/response where applicable; events only for hotkey/foreground notifications.

**State Management Patterns:**
- Centralized state object exposes capture state to QML.
- UI state changes go through dedicated setters (no scattered property mutation).

### Process Patterns

**Error Handling Patterns:**
- Helper errors: return structured `error` in IPC envelope.
- UI errors: inline status text (no toasts), with logs via QLoggingCategory.

**Loading State Patterns:**
- Overlay startup shows inline "Initializing capture" and offers Cancel.
- No global loading spinners.

### Enforcement Guidelines

**All AI Agents MUST:**
- Follow KDE naming and file conventions (PascalCase classes, `mCamelCase` members).
- Place Windows-only code under `src/Platforms/Windows/` and helper under `src/Platforms/Windows/Helper/`.
- Use the IPC envelope and naming rules exactly as specified.

**Pattern Enforcement:**
- PRs and reviews check for naming and placement compliance.
- IPC schema changes require version bump and explicit note in architecture doc.

### Pattern Examples

**Good Examples:**
- `src/Platforms/Windows/ImagePlatformWin.cpp`
- IPC message: `{"version":1,"type":"hotkeyPressed","requestId":"123","payload":{"source":"PrintScreen"}}`

**Anti-Patterns:**
- Placing Windows capture code in `src/Gui/`
- Introducing `WinImagePlatform.cpp` (breaks naming consistency)
- Sending raw strings over IPC without the envelope

## Project Structure & Boundaries

### Complete Project Directory Structure
```
spectacle/
  .github/
    workflows/
      windows.yml
  cmake/
  dbus/
  desktop/
  doc/
  docs/
  icons/
  installer/
    nsis/
      spectacle.nsi
    winget/
      manifest.yaml
  kconf_update/
  po/
  src/
    Main.cpp
    SpectacleCore.*
    ExportManager.*
    OcrManager.*
    Platforms/
      ImagePlatform*
      VideoPlatform*
      Windows/
        ImagePlatformWin.*
        VideoPlatformWin.*
        WinCaptureBackend.*
        WinRecordingBackend.*
        WinDpiUtils.*
        Helper/
          SpectacleHelperMain.cpp
          WinHelperIpcServer.*
          WinHelperIpcClient.*
    Gui/
      CaptureWindow.*
      ViewerWindow.*
      Selection*.*
      ScreenshotView.qml
      RecordingView.qml
      CaptureOverlay.qml
      AnnotationEditor.qml
      AnnotationsToolBarContents.qml
      AnnotationOptionsToolBarContents.qml
      FloatingToolBar.qml
      SizeLabel.qml
      RecordAction.qml
      OcrAction.qml
      SaveAction.qml
      SaveAsAction.qml
      OptionsMenu*.*
      SettingsDialog/
        *.ui
        *.h
        *.cpp
  tests/
    windows/
      WinHelperIpcTests.cpp
      WinCaptureBackendTests.cpp
```

### Architectural Boundaries

**API Boundaries:**
- No external APIs. All communication stays local.

**Component Boundaries:**
- UI/QML in `src/Gui/` (platform-agnostic).
- Core orchestration in `src/SpectacleCore.*`, `ExportManager.*`, `OcrManager.*`.
- Platform backends in `src/Platforms/` with Windows under `src/Platforms/Windows/`.
- Helper process isolated under `src/Platforms/Windows/Helper/`.

**Service Boundaries:**
- Helper process is a separate executable; main app is the UI host.
- IPC boundary via Win32 named pipes (no shared state across process boundary).

**Data Boundaries:**
- Settings in KConfig (existing mechanism).
- Output assets saved to filesystem only.
- No database.

### Requirements to Structure Mapping

**Capture and Selection:**
- `src/Platforms/Windows/ImagePlatformWin.*`
- `src/Gui/CaptureOverlay.qml`
- `src/Gui/Selection*.*`

**Pre-Capture Overlay and Annotation:**
- `src/Gui/CaptureOverlay.qml`
- `src/Gui/AnnotationEditor.qml`
- `src/Gui/AnnotationsToolBarContents.qml`

**Post-Capture Annotation:**
- `src/Gui/ViewerWindow.*`
- `src/Gui/ViewerPage.qml`
- `src/Gui/AnnotationEditor.qml`

**Screen Recording:**
- `src/Platforms/Windows/VideoPlatformWin.*`
- `src/Gui/RecordingView.qml`
- `src/RecordingModeModel.*`

**OCR:**
- `src/OcrManager.*`
- `src/Gui/OcrAction.qml`

**Output and Saving:**
- `src/ExportManager.*`
- `src/Gui/SaveAction.qml`
- `src/Gui/SaveAsAction.qml`

**Hotkeys and Launch:**
- `src/ShortcutActions.*`
- `src/Platforms/Windows/Helper/*`
- IPC client in `src/Platforms/Windows/Helper/WinHelperIpcClient.*`

**Updates and Distribution:**
- `installer/nsis/`
- `installer/winget/`
- `.github/workflows/windows.yml`

**Diagnostics and Support:**
- `src/DebugUtils.h`
- `src/Gui/SettingsDialog/*`

### Integration Points

**Internal Communication:**
- UI to Core: Qt signals/slots and QML bindings.
- Core to Platform: abstract interfaces under `src/Platforms/`.
- Helper to UI/Core: Win32 named pipe IPC with JSON envelope.

**External Integrations:**
- None (offline-first).

**Data Flow:**
- Hotkey event (Helper) -> IPC -> Core -> Overlay UI -> Capture -> Save/Clipboard.

### File Organization Patterns

**Configuration Files:**
- Build: `CMakeLists.txt`, `cmake/`
- CI: `.github/workflows/windows.yml`, `.gitlab-ci.yml`
- Installer: `installer/nsis/`, `installer/winget/`

**Source Organization:**
- `src/` core and platform code
- `src/Gui/` UI and QML
- `src/Platforms/Windows/` Windows backends
- `src/Platforms/Windows/Helper/` helper process

**Test Organization:**
- `tests/` for shared tests
- `tests/windows/` for Windows-specific components

**Asset Organization:**
- `icons/`, `doc/`, `docs/`, `po/`

### Development Workflow Integration

**Development Build:**
- CMake builds core and helper process targets.
- Windows helper built as a separate executable.

**Deployment Structure:**
- NSIS installer packages main app and helper.
- Winget manifest references the NSIS package.

## Architecture Validation Results

### Coherence Validation

**Decision Compatibility:**
All decisions align with the existing C++/Qt/KDE base. The helper process and Win32 IPC complement the QML/UI stack without introducing conflicting tech. Packaging and CI choices (NSIS + GitHub Actions) are compatible with the Windows port.

**Pattern Consistency:**
Implementation patterns (naming, structure, IPC schema) reinforce the architecture. Windows-specific code placement and IPC envelope definitions align with helper-based reliability.

**Structure Alignment:**
The project tree supports platform separation, helper isolation, and UI portability. Integration points are clearly defined between helper, core, and UI.

### Requirements Coverage Validation

**Epic and Feature Coverage:**
No epics were provided; FR categories are fully mapped.

**Functional Requirements Coverage:**
All FR categories map to architectural components: capture/selection, pre/post annotation, recording, OCR, saving, hotkeys, diagnostics, updates, offline mode.

**Non-Functional Requirements Coverage:**
Performance (<1s), reliability, offline-only, privacy/no telemetry, and Windows-only constraints are addressed in architecture decisions and patterns.

### Implementation Readiness Validation

**Decision Completeness:**
All critical decisions are captured; deferred items are explicitly listed (Windows capture/recording backends, OCR engine bindings).

**Structure Completeness:**
Project structure is explicit, includes Windows folders, helper process, CI, packaging, and tests.

**Pattern Completeness:**
Naming, IPC schema, placement, and error/loading patterns are explicit and enforceable.

### Gap Analysis Results

**Critical Gaps:**
- None. Backend APIs selected (WGC capture, WGC + Media Foundation recording, Tesseract + Windows OCR hybrid).

**Important Gaps:**
- Define IPC reconnection/backoff policy in more detail (implementation note).

**Nice-to-Have Gaps:**
- Define logging categories and levels for helper vs UI.

### Validation Issues Addressed

- All remaining gaps are explicitly listed as deferred decisions to resolve before implementation.

### Architecture Completeness Checklist

**Requirements Analysis**
- [x] Project context thoroughly analyzed
- [x] Scale and complexity assessed
- [x] Technical constraints identified
- [x] Cross-cutting concerns mapped

**Architectural Decisions**
- [x] Critical decisions documented with versions
- [x] Technology stack fully specified
- [x] Integration patterns defined
- [x] Performance considerations addressed

**Implementation Patterns**
- [x] Naming conventions established
- [x] Structure patterns defined
- [x] Communication patterns specified
- [x] Process patterns documented

**Project Structure**
- [x] Complete directory structure defined
- [x] Component boundaries established
- [x] Integration points mapped
- [x] Requirements to structure mapping complete

### Architecture Readiness Assessment

**Overall Status:** READY FOR IMPLEMENTATION

**Confidence Level:** High

**Key Strengths:**
- Strong parity with existing Spectacle architecture
- Clear separation of Windows-specific code and helper process
- Explicit IPC patterns and UI integration rules

**Areas for Future Enhancement:**
- IPC backoff and retry policy details
- Logging categories and levels for helper vs UI

### Implementation Handoff

**AI Agent Guidelines:**
- Follow all architectural decisions exactly as documented
- Use implementation patterns consistently across all components
- Respect project structure and boundaries
- Refer to this document for all architectural questions

**First Implementation Priority:**
Implement WGC capture/recording + OCR hybrid bindings; then implement helper process + IPC.


## Architecture Completion Summary

### Workflow Completion

Architecture Decision Workflow: COMPLETED
Total Steps Completed: 8
Date Completed: 2025-12-30
Document Location: C:\projects\spectacle\_bmad-output\planning-artifactsrchitecture.md

### Final Architecture Deliverables

- Complete architecture decision document
- Implementation patterns for agent consistency
- Project structure with Windows-specific boundaries
- Validation confirming coherence and coverage

### Implementation Handoff

For AI agents:
- Follow all architectural decisions exactly as documented
- Use implementation patterns consistently across components
- Respect project structure and boundaries
- Refer to this document for all architectural questions

First Implementation Priority:
- Implement WGC capture/recording + OCR hybrid bindings
- Implement helper process and IPC

### Quality Assurance Checklist

- Architecture decisions are compatible and consistent
- Requirements are covered by the architecture
- Patterns prevent agent conflicts
- Structure is complete and unambiguous

Architecture Status: READY FOR IMPLEMENTATION
Document Maintenance: update this file when major technical decisions change
