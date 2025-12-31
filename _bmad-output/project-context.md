---
project_name: 'spectacle'
user_name: 'TerminalSausage'
date: '2025-12-30'
sections_completed: ['technology_stack', 'language_rules', 'framework_rules', 'testing_rules', 'quality_rules', 'workflow_rules', 'critical_rules']
existing_patterns_found: 6
status: 'complete'
rule_count: 44
optimized_for_llm: true
---

# Project Context for AI Agents

_This file contains critical rules and patterns that AI agents must follow when implementing code in this project. Focus on unobvious details that agents might otherwise miss._

---

## Technology Stack & Versions

- Language: C++20
- Build: CMake >= 3.16
- UI: Qt 6 >= 6.9.0 (Core, Qml, Quick, Widgets, Multimedia, etc.)
- KDE Frameworks: 6 >= 6.18.0
- OpenCV: 4.7 (core, imgproc)
- Optional OCR: Tesseract (offline)
- Optional: KQuickImageEditor (CMake requirement)
- Tests: CTest (via CMake)
- Windows helper IPC: Win32 named pipes
- Packaging: NSIS (installer), winget manifest
- CI: GitHub Actions (Windows)

## Critical Implementation Rules

### Language-Specific Rules (C++/Qt)

- Use C++20; follow KDELibs style with Spectacle exceptions.
- Member variables use `mCamelCase` (not `m_camelCase`).
- Access modifier order: public, signals, public slots, protected slots, protected, private slots, private.
- Source/header filenames match class names (mixed case).
- Use Qt types for UI-facing data (QString, QVector/QList) and Qt signals/slots for UI state.
- Windows-specific code must be behind `#ifdef Q_OS_WIN` guards.

### Framework-Specific Rules (Qt/QML)

- Keep UI in QML under `src/Gui/`; platform-specific logic stays in C++.
- Use Qt signals/slots to bridge core state into QML; avoid direct cross-component calls.
- Reuse the annotation toolset across pre- and post-capture (single component).
- Keep overlay lightweight: pre-load core tools, lazy-load non-critical UI.
- Windows capture uses Windows Graphics Capture (WGC).
- Recording uses WGC + Media Foundation pipeline.
- OCR uses Tesseract primary; Windows OCR fallback (offline).
- IPC uses Win32 named pipes with JSON envelope (`version`, `type`, `requestId`, `payload`, `error`).

### Testing Rules

- Use CTest (via CMake); keep tests under `tests/`.
- Windows-only tests live in `tests/windows/` to avoid cross-platform failures.
- Unit tests for helper IPC and Windows backends are required before marking work complete.
- Prefer deterministic tests; avoid timing-sensitive flakiness in overlay/recording tests.

### Code Quality & Style Rules

- Follow KDELibs style; do not mix formatting-only changes with functional changes.
- Keep commits small and atomic.
- Source file names match class names; member vars use `mCamelCase`.
- Preserve existing QML component naming (PascalCase files, lowerCamelCase ids).
- Use QLoggingCategory for logging (no ad-hoc `printf`/`std::cout`).

### Development Workflow Rules

- Windows CI builds run in GitHub Actions; keep CMake builds compatible.
- NSIS installer and winget manifest are the distribution sources.
- Helper auto-start uses Registry Run key; installer must register/unregister cleanly.
- No telemetry; diagnostics are local-only.

### Critical Don't-Miss Rules

- Do NOT add a database or network service without updating architecture decisions.
- Do NOT put Windows-specific capture code in `src/Gui/`; keep it in `src/Platforms/Windows/`.
- Do NOT bypass the IPC envelope; all helper comms must be JSON with `version/type/requestId/payload/error`.
- Always handle multi-monitor and mixed DPI; never assume a single screen.
- Overlay must remain responsive (<1s); avoid long blocking calls on UI thread.
- Clipboard must update on annotation changes (pre and post capture).

---

## Usage Guidelines

**For AI Agents:**

- Read this file before implementing any code
- Follow ALL rules exactly as documented
- When in doubt, prefer the more restrictive option
- Update this file if new patterns emerge

**For Humans:**

- Keep this file lean and focused on agent needs
- Update when technology stack changes
- Review quarterly for outdated rules
- Remove rules that become obvious over time

Last Updated: 2025-12-30
