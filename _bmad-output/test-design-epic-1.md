# Test Design: Epic 1 - Print Screen/Region Capture

**Date:** 2025-12-30
**Author:** TerminalSausage
**Status:** Draft

---

## Executive Summary

**Scope:** Full test design for Epic 1 (Print Screen/Region Capture workflow)

**Risk Summary:**

- Total risks identified: 8
- High-priority risks (≥6): 2
- Critical categories: PERF (Performance), TECH (Technical/Architecture)

**Coverage Summary:**

- P0 scenarios: 6 (12 hours)
- P1 scenarios: 9 (9 hours)
- P2/P3 scenarios: 10 (3.5 hours)
- **Total effort**: 24.5 hours (~3 days)

---

## Risk Assessment

### High-Priority Risks (Score ≥6)

| Risk ID | Category | Description | Probability | Impact | Score | Mitigation | Owner | Timeline |
| ------- | -------- | ----------- | ----------- | ------ | ----- | ---------- | ----- | -------- |
| R-001 | PERF | Overlay appears >1s after Print Screen (violates NFR1) | 2 | 3 | 6 | Performance benchmark tests, profile WGC capture latency, implement loading states | DEV | Sprint 0 |
| R-002 | TECH | Global hotkey fails to register or conflicts with other apps | 2 | 3 | 6 | Helper process always-on design, Registry Run key, Win32 hotkey API validation tests | DEV | Sprint 0 |

### Medium-Priority Risks (Score 3-4)

| Risk ID | Category | Description | Probability | Impact | Score | Mitigation | Owner |
| ------- | -------- | ----------- | ----------- | ------ | ----- | ---------- | ----- |
| R-003 | PERF | Selection drag/resize laggy or unresponsive | 2 | 2 | 4 | Frame rate monitoring tests, optimize QML rendering, test with high-DPI multi-monitor | DEV |
| R-004 | DATA | Clipboard commit fails silently | 1 | 3 | 3 | Clipboard API integration tests, verify commit success, error handling validation | QA |
| R-005 | OPS | Helper process crashes or fails to start | 1 | 3 | 3 | Process health check tests, auto-restart validation, diagnostics collection | OPS |
| R-006 | TECH | Multi-monitor DPI scaling causes visual artifacts | 2 | 2 | 4 | Multi-monitor DPI test matrix, per-monitor scaling validation | QA |

### Low-Priority Risks (Score 1-2)

| Risk ID | Category | Description | Probability | Impact | Score | Action |
| ------- | -------- | ----------- | ----------- | ------ | ----- | ------ |
| R-007 | BUS | Cancel operation doesn't cleanly abort capture | 1 | 2 | 2 | Monitor |
| R-008 | PERF | Autosave introduces noticeable delay | 1 | 2 | 2 | Monitor |

### Risk Category Legend

- **TECH**: Technical/Architecture (flaws, integration, scalability)
- **SEC**: Security (access controls, auth, data exposure)
- **PERF**: Performance (SLA violations, degradation, resource limits)
- **DATA**: Data Integrity (loss, corruption, inconsistency)
- **BUS**: Business Impact (UX harm, logic errors, revenue)
- **OPS**: Operations (deployment, config, monitoring)

---

## Test Coverage Plan

### P0 (Critical) - Run on every commit

**Criteria**: Blocks core journey + High risk (≥6) + No workaround

| Requirement | Test Level | Risk Link | Test Count | Owner | Notes |
| ----------- | ---------- | --------- | ---------- | ----- | ----- |
| Print Screen → overlay appears in <1s | E2E | R-001 | 1 | QA | Validates hotkey + WGC + UI render |
| Drag to create region, verify selection box | E2E | R-003 | 1 | QA | Validates mouse interaction |
| Resize selection via grab handles | E2E | R-003 | 1 | QA | Validates resize behavior |
| Enter confirms → clipboard contains image | E2E | R-004 | 1 | QA | Validates commit workflow |
| Escape aborts → no clipboard change | E2E | R-007 | 1 | QA | Validates cancel behavior |
| Autosave enabled → file written to disk | E2E | R-008 | 1 | QA | Validates save workflow |

**Total P0**: 6 tests, 12 hours

### P1 (High) - Run on PR to main

**Criteria**: Important features + Medium risk (3-4) + Common workflows

| Requirement | Test Level | Risk Link | Test Count | Owner | Notes |
| ----------- | ---------- | --------- | ---------- | ----- | ----- |
| Helper process IPC round-trip latency | API | R-001 | 1 | QA | Named pipe performance |
| Clipboard commit success/failure scenarios | API | R-004 | 1 | QA | Error handling validation |
| WGC capture frame buffer validation | API | R-001 | 1 | QA | Backend integration |
| Named pipe message serialization | API | - | 1 | QA | IPC protocol validation |
| QML overlay mouse event routing | Component | R-003 | 1 | DEV | UI event handling |
| Selection box resize handle hit-testing | Component | R-003 | 1 | DEV | Interaction testing |
| Crosshair cursor visibility | Component | - | 1 | DEV | Visual validation |
| Quick tray positioning logic | Component | - | 1 | DEV | Layout calculation |
| Cancel button state transitions | Component | R-007 | 1 | DEV | State machine validation |

**Total P1**: 9 tests, 9 hours

### P2 (Medium) - Run nightly/weekly

**Criteria**: Secondary features + Low risk (1-2) + Edge cases

| Requirement | Test Level | Risk Link | Test Count | Owner | Notes |
| ----------- | ---------- | --------- | ---------- | ----- | ----- |
| Selection rectangle geometry calculations | Unit | - | 2 | DEV | Edge cases (negative, zero, huge) |
| Image encoding (PNG/JPEG/WEBP) | Unit | - | 2 | DEV | Format validation |
| Clipboard format validation | Unit | R-004 | 1 | DEV | CF_DIB vs CF_HDROP |
| Autosave path resolution | Unit | R-008 | 1 | DEV | Config resolution |
| DPI scaling factor calculations | Unit | R-006 | 1 | DEV | Per-monitor scaling |
| Multi-monitor coordinate mapping | Unit | R-006 | 1 | DEV | Virtual screen coords |

**Total P2**: 8 tests, 4 hours

### P3 (Low) - Run on-demand

**Criteria**: Nice-to-have + Exploratory + Performance benchmarks

| Requirement | Test Level | Test Count | Owner | Notes |
| ----------- | ---------- | ---------- | ----- | ----- |
| Resource cleanup on abort | Unit | 1 | DEV | Memory leak check |
| Error handling for WGC failures | Unit | 1 | DEV | Graceful degradation |

**Total P3**: 2 tests, 0.5 hours

---

## Execution Order

### Smoke Tests (<5 min)

**Purpose**: Fast feedback, catch build-breaking issues

- [ ] Print Screen triggers overlay (30s)
- [ ] Drag to create selection box (30s)
- [ ] Enter confirms capture to clipboard (1min)

**Total**: 3 scenarios

### P0 Tests (<10 min)

**Purpose**: Critical path validation

- [ ] Print Screen → overlay appears in <1s (E2E)
- [ ] Drag to create region → verify selection box (E2E)
- [ ] Resize selection via grab handles (E2E)
- [ ] Enter confirms → clipboard contains image (E2E)
- [ ] Escape aborts → no clipboard change (E2E)
- [ ] Autosave enabled → file written to disk (E2E)

**Total**: 6 scenarios

### P1 Tests (<30 min)

**Purpose**: Important feature coverage

- [ ] Helper process IPC round-trip latency (API)
- [ ] Clipboard commit success/failure scenarios (API)
- [ ] WGC capture frame buffer validation (API)
- [ ] Named pipe message serialization (API)
- [ ] QML overlay mouse event routing (Component)
- [ ] Selection box resize handle hit-testing (Component)
- [ ] Crosshair cursor visibility (Component)
- [ ] Quick tray positioning logic (Component)
- [ ] Cancel button state transitions (Component)

**Total**: 9 scenarios

### P2/P3 Tests (<60 min)

**Purpose**: Full regression coverage

- [ ] Selection rectangle geometry calculations (Unit)
- [ ] Image encoding (PNG/JPEG/WEBP) (Unit)
- [ ] Clipboard format validation (Unit)
- [ ] Autosave path resolution (Unit)
- [ ] DPI scaling factor calculations (Unit)
- [ ] Multi-monitor coordinate mapping (Unit)
- [ ] Resource cleanup on abort (Unit)
- [ ] Error handling for WGC failures (Unit)
- [ ] Performance benchmarks (E2E)
- [ ] Stress tests (rapid hotkey presses) (E2E)

**Total**: 10 scenarios

---

## Resource Estimates

### Test Development Effort

| Priority | Count | Hours/Test | Total Hours | Notes |
| -------- | ----- | ---------- | ----------- | ----- |
| P0       | 6     | 2.0        | 12          | Complex setup, E2E scenarios |
| P1       | 9     | 1.0        | 9           | API integration, component tests |
| P2       | 8     | 0.5        | 4           | Unit tests, edge cases |
| P3       | 2     | 0.25       | 0.5         | Exploratory, benchmarks |
| **Total** | **25** | **-** | **25.5** | **~3 days** |

### Prerequisites

**Test Data:**

- Screenshot image factory (faker-based, variable sizes/formats)
- Test selection rectangle fixture (setup/teardown)
- Clipboard state fixture (clean before/after)

**Tooling:**

- Qt Test Framework for C++ unit tests
- CTest integration for CI execution
- Win32 API mocking for helper process tests
- WGC simulator for capture backend tests

**Environment:**

- Windows 11 test machines (multi-monitor configs)
- DPI scaling test matrix (100%, 125%, 150%, 200%)
- Virtual display setup for multi-monitor testing

---

## Quality Gate Criteria

### Pass/Fail Thresholds

- **P0 pass rate**: 100% (no exceptions)
- **P1 pass rate**: ≥95% (waivers required for failures)
- **P2/P3 pass rate**: ≥90% (informational)
- **High-risk mitigations**: 100% complete or approved waivers

### Coverage Targets

- **Critical paths**: ≥80%
- **Security scenarios**: 100% (N/A for this epic)
- **Business logic**: ≥70%
- **Edge cases**: ≥50%

### Non-Negotiable Requirements

- [ ] All P0 tests pass
- [ ] No high-risk (≥6) items unmitigated
- [ ] Performance targets met (<1s overlay latency)
- [ ] Multi-monitor DPI scaling validated

---

## Mitigation Plans

### R-001: Overlay appears >1s after Print Screen (Score: 6)

**Mitigation Strategy:**
1. Implement performance benchmark tests measuring hotkey → overlay latency
2. Profile WGC capture initialization to identify bottlenecks
3. Optimize QML overlay rendering pipeline
4. Add loading state indicator if >500ms latency expected
5. Cache WGC capture session after first use

**Owner:** DEV
**Timeline:** Sprint 0 (Framework setup)
**Status:** Planned
**Verification:** Benchmark suite shows P95 latency <1s under load

### R-002: Global hotkey fails to register or conflicts with other apps (Score: 6)

**Mitigation Strategy:**
1. Implement helper process with Registry Run key for auto-start
2. Use Win32 RegisterHotKey API with fallback handling
3. Detect and report hotkey conflicts to user
4. Validate hotkey registration on app startup
5. Add health check to verify helper process responsiveness

**Owner:** DEV
**Timeline:** Sprint 0 (Framework setup)
**Status:** Planned
**Verification:** 100% hotkey reliability in acceptance testing (NFR4)

---

## Assumptions and Dependencies

### Assumptions

1. Windows 11 test environment available with multi-monitor support
2. WGC API available on all target Windows 11 versions
3. Qt 6.x QML rendering performance adequate for <1s requirement
4. Existing Spectacle codebase can be built on Windows

### Dependencies

1. Qt 6.x Windows build framework - Required by Sprint 0
2. Windows Graphics Capture API documentation - Required by Sprint 0
3. Multi-monitor test hardware - Required by Sprint 1

### Risks to Plan

- **Risk**: WGC API may have unexpected limitations on certain Windows 11 builds
  - **Impact**: May require alternative capture backend or workarounds
  - **Contingency**: Evaluate GDI+ fallback if WGC unavailable

- **Risk**: Multi-monitor DPI testing may reveal Qt rendering issues
  - **Impact**: Additional development time for DPI fixes
  - **Contingency**: Allocate buffer in Sprint 1 for DPI bug fixes

---

## Follow-on Workflows (Manual)

- Run `*atdd` to generate failing P0 tests (separate workflow; not auto-run).
- Run `*automate` for broader coverage once implementation exists.

---

## Approval

**Test Design Approved By:**

- [ ] Product Manager: _____________ Date: ______
- [ ] Tech Lead: _____________ Date: ______
- [ ] QA Lead: _____________ Date: ______

**Comments:**

---

---

## Appendix

### Knowledge Base References

- `risk-governance.md` - Risk classification framework
- `probability-impact.md` - Risk scoring methodology
- `test-levels-framework.md` - Test level selection
- `test-priorities-matrix.md` - P0-P3 prioritization

### Related Documents

- PRD: `C:\projects\spectacle\_bmad-output\planning-artifacts\prd.md`
- Epic: `C:\projects\spectacle\_bmad-output\planning-artifacts\epics.md`
- Architecture: `C:\projects\spectacle\_bmad-output\planning-artifacts\architecture.md`
- Story 1.1: `C:\projects\spectacle\_bmad-output\implementation-artifacts\1-1-print-screen-region-capture-overlay-confirm-cancel.md`

---

**Generated by**: BMad TEA Agent - Test Architect Module
**Workflow**: `_bmad/bmm/testarch/test-design`
**Version**: 4.0 (BMad v6)
