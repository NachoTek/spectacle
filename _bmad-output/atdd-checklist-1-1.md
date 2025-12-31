# ATDD Checklist: Story 1.1 - Print Screen Region Capture

**Story ID:** 1-1
**Date:** 2025-12-30
**Author:** TerminalSausage
**Status:** RED Phase (Tests Failing)

---

## Executive Summary

**Story:** Print Screen Region Capture (Overlay + Confirm/Cancel)

**Primary Test Level:** Integration + Unit (Qt Test Framework)

**Failing Tests Created:**

- Integration tests: 4 tests
- Unit tests: 5 tests

**Supporting Infrastructure:**

- Test fixtures: 2 fixtures (HelperProcessIPC, ClipboardState)
- Mock requirements: 2 services (WGC Backend, Win32 Hotkey)

**Implementation Checklist:**

- Total tasks: 24
- Estimated effort: 16 hours

---

## Acceptance Criteria Breakdown

### AC1: Print Screen triggers overlay within 1 second (P0)

**Given** Spectacle is running and the Print Screen hotkey is enabled
**When** I press Print Screen
**Then** a full-screen overlay appears within 1 second and shows a crosshair cursor

**Test Level:** Integration Test
**Risk Link:** R-001 (Performance), R-002 (Hotkey reliability)

---

### AC2: Click-drag creates resizable selection region (P0)

**Given** the overlay is active
**When** I click-drag to define a rectangular region
**Then** the selection box is created and can be resized via grab handles

**Test Level:** Integration Test
**Risk Link:** R-003 (Drag/resize performance)

---

### AC3: Enter confirms capture to clipboard (P0)

**Given** a region is selected
**When** I press Enter
**Then** the capture is finalized and the image is committed to the clipboard (and autosaved if enabled)

**Test Level:** Integration Test
**Risk Link:** R-004 (Clipboard commit reliability)

---

### AC4: Escape/Click aborts capture (P0)

**Given** the overlay is active
**When** I press Escape or click Cancel
**Then** the capture is aborted and no image is committed

**Test Level:** Integration Test
**Risk Link:** R-007 (Cancel cleanup)

---

## Test Files Created

### Integration Tests

**File:** `tests/Integration/PrintScreenCaptureTest.cpp`

```cpp
/*
 * ATDD Test for Story 1.1 - Print Screen Region Capture
 * RED Phase: Tests will fail until implementation is complete
 */

#include <QTest>
#include <QClipboard>
#include <QApplication>
#include <QTimer>

#include "Platforms/Windows/HelperProcess.h"
#include "Gui/Overlay/SelectionOverlay.h"
#include "Platforms/Windows/WGCCapture.h"

using namespace Qt::StringLiterals;

class PrintScreenCaptureTest : public QObject
{
    Q_OBJECT

private:
    HelperProcess *mHelperProcess;
    SelectionOverlay *mOverlay;
    WGCCapture *mCaptureBackend;
    QClipboard *mClipboard;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // AC1: Print Screen triggers overlay within 1 second
    void testPrintScreenTriggersOverlayWithinOneSecond();
    void testOverlayShowsCrosshairCursor();

    // AC2: Click-drag creates resizable selection region
    void testClickDragCreatesSelectionBox();
    void testSelectionHasResizeHandles();

    // AC3: Enter confirms capture to clipboard
    void testEnterConfirmsCaptureToClipboard();
    void testAutosaveWhenEnabled();

    // AC4: Escape aborts capture
    void testEscapeAbortsCapture();
    void testCancelButtonAbortsCapture();
    void testAbortDoesNotModifyClipboard();
};

void PrintScreenCaptureTest::initTestCase()
{
    // GIVEN: Spectacle is running
    mHelperProcess = HelperProcess::instance();
    mOverlay = nullptr; // Will be created when hotkey pressed
    mCaptureBackend = WGCCapture::instance();
    mClipboard = QApplication::clipboard();

    // Enable Print Screen hotkey
    QVERIFY(mHelperProcess->registerGlobalHotkey(VK_SNAPSHOT));
}

void PrintScreenCaptureTest::init()
{
    // Clear clipboard before each test
    mClipboard->clear();
}

void PrintScreenCaptureTest::cleanup()
{
    // Close overlay if open
    if (mOverlay) {
        mOverlay->close();
        delete mOverlay;
        mOverlay = nullptr;
    }
}

void PrintScreenCaptureTest::cleanupTestCase()
{
    delete mHelperProcess;
}

// ============================================================================
// AC1: Print Screen triggers overlay within 1 second
// ============================================================================

void PrintScreenCaptureTest::testPrintScreenTriggersOverlayWithinOneSecond()
{
    // GIVEN: Helper process is running and hotkey is registered
    QVERIFY(mHelperProcess->isHotkeyRegistered(VK_SNAPSHOT));

    // WHEN: Print Screen is pressed
    QElapsedTimer timer;
    timer.start();

    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);

    // THEN: Overlay appears within 1 second (1000ms)
    QVERIFY2(timer.elapsed() < 1000,
             "Overlay must appear within 1 second of Print Screen press");

    QVERIFY(mOverlay != nullptr);
    QVERIFY(mOverlay->isVisible());
}

void PrintScreenCaptureTest::testOverlayShowsCrosshairCursor()
{
    // GIVEN: Overlay is displayed
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    QVERIFY(mOverlay != nullptr);

    // WHEN: Overlay is active
    // THEN: Crosshair cursor is shown
    QCOMPARE(mOverlay->cursor().shape(), Qt::CrossCursor);
}

// ============================================================================
// AC2: Click-drag creates resizable selection region
// ============================================================================

void PrintScreenCaptureTest::testClickDragCreatesSelectionBox()
{
    // GIVEN: Overlay is active
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);

    // WHEN: User clicks and drags to define region
    QTest::mousePress(mOverlay, Qt::LeftButton, {}, QPoint(100, 100));
    QTest::mouseMove(mOverlay, QPoint(300, 300));
    QTest::mouseRelease(mOverlay, Qt::LeftButton, {}, QPoint(300, 300));

    // THEN: Selection box is created
    QVERIFY(mOverlay->hasSelection());
    QRect selection = mOverlay->selectionRect();
    QCOMPARE(selection.topLeft(), QPoint(100, 100));
    QCOMPARE(selection.bottomRight(), QPoint(300, 300));
}

void PrintScreenCaptureTest::testSelectionHasResizeHandles()
{
    // GIVEN: Overlay is active with selection
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    QTest::mousePress(mOverlay, Qt::LeftButton, {}, QPoint(100, 100));
    QTest::mouseMove(mOverlay, QPoint(300, 300));
    QTest::mouseRelease(mOverlay, Qt::LeftButton, {}, QPoint(300, 300));

    // WHEN: Selection exists
    // THEN: Resize handles are available
    QVERIFY(mOverlay->resizeHandlesVisible());
    QCOMPARE(mOverlay->resizeHandleCount(), 8); // 4 corners + 4 edges
}

// ============================================================================
// AC3: Enter confirms capture to clipboard
// ============================================================================

void PrintScreenCaptureTest::testEnterConfirmsCaptureToClipboard()
{
    // GIVEN: Overlay is active with selection
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    QTest::mousePress(mOverlay, Qt::LeftButton, {}, QPoint(100, 100));
    QTest::mouseMove(mOverlay, QPoint(300, 300));
    QTest::mouseRelease(mOverlay, Qt::LeftButton, {}, QPoint(300, 300));

    // WHEN: Enter is pressed
    QTest::keyClick(mOverlay, Qt::Key_Return);

    // THEN: Image is committed to clipboard
    QVERIFY2(!mClipboard->image().isNull(),
             "Clipboard should contain captured image after Enter");
    QVERIFY(!mOverlay->isVisible()); // Overlay closed
}

void PrintScreenCaptureTest::testAutosaveWhenEnabled()
{
    // GIVEN: Overlay is active with selection and autosave enabled
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    mOverlay->setAutosaveEnabled(true);
    mOverlay->setAutosavePath(u"C:\\temp\\test-screenshot.png"_s);

    QTest::mousePress(mOverlay, Qt::LeftButton, {}, QPoint(100, 100));
    QTest::mouseMove(mOverlay, QPoint(300, 300));
    QTest::mouseRelease(mOverlay, Qt::LeftButton, {}, QPoint(300, 300));

    // WHEN: Enter is pressed
    QTest::keyClick(mOverlay, Qt::Key_Return);

    // THEN: File is saved to disk
    QFile savedFile(u"C:\\temp\\test-screenshot.png"_s);
    QVERIFY2(savedFile.exists(),
             "Autosave file should exist after capture confirmed");
}

// ============================================================================
// AC4: Escape aborts capture
// ============================================================================

void PrintScreenCaptureTest::testEscapeAbortsCapture()
{
    // GIVEN: Overlay is active with selection
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    QTest::mousePress(mOverlay, Qt::LeftButton, {}, QPoint(100, 100));
    QTest::mouseMove(mOverlay, QPoint(300, 300));
    QTest::mouseRelease(mOverlay, Qt::LeftButton, {}, QPoint(300, 300));

    // WHEN: Escape is pressed
    QTest::keyClick(mOverlay, Qt::Key_Escape);

    // THEN: Overlay closes and no capture is made
    QVERIFY2(!mOverlay->isVisible(),
             "Overlay should close after Escape");
    QVERIFY2(mClipboard->image().isNull(),
             "Clipboard should be empty after abort");
}

void PrintScreenCaptureTest::testCancelButtonAbortsCapture()
{
    // GIVEN: Overlay is active
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);

    // WHEN: Cancel button is clicked
    QPushButton *cancelButton = mOverlay->findChild<QPushButton *>(u"cancelButton"_s);
    QVERIFY2(cancelButton != nullptr, "Cancel button must exist");
    QTest::mouseClick(cancelButton, Qt::LeftButton);

    // THEN: Overlay closes and no capture is made
    QVERIFY2(!mOverlay->isVisible(),
             "Overlay should close after Cancel click");
    QVERIFY2(mClipboard->image().isNull(),
             "Clipboard should be empty after abort");
}

void PrintScreenCaptureTest::testAbortDoesNotModifyClipboard()
{
    // GIVEN: Clipboard has existing content
    QImage existingImage(100, 100, QImage::Format_RGB32);
    existingImage.fill(Qt::red);
    mClipboard->setImage(existingImage);

    // WHEN: Overlay is shown and aborted
    mHelperProcess->simulateHotkeyPress(VK_SNAPSHOT);
    QTest::keyClick(mOverlay, Qt::Key_Escape);

    // THEN: Clipboard still has original content
    QCOMPARE(mClipboard->image(), existingImage);
}

QTEST_MAIN(PrintScreenCaptureTest)
#include "PrintScreenCaptureTest.moc"
```

---

### Unit Tests

**File:** `tests/Unit/SelectionGeometryTest.cpp`

```cpp
/*
 * Unit Tests for Selection Geometry Calculations
 * Story 1.1 - Print Screen Region Capture
 */

#include <QTest>
#include <QRect>

#include "Gui/Overlay/SelectionGeometry.h"

using namespace Qt::StringLiterals;

class SelectionGeometryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testSelectionRectangleCreation();
    void testNegativeCoordinatesRejected();
    void testZeroAreaSelectionRejected();
    void testHugeSelectionClampedToScreen();
    void testResizeHandlePositions();
};

void SelectionGeometryTest::testSelectionRectangleCreation()
{
    // GIVEN: Valid start and end points
    QPoint start(100, 100);
    QPoint end(300, 300);

    // WHEN: Creating selection rectangle
    SelectionGeometry geom;
    QRect rect = geom.createRectangle(start, end);

    // THEN: Rectangle is valid
    QVERIFY(rect.isValid());
    QCOMPARE(rect.topLeft(), start);
    QCOMPARE(rect.bottomRight(), end);
    QCOMPARE(rect.width(), 200);
    QCOMPARE(rect.height(), 200);
}

void SelectionGeometryTest::testNegativeCoordinatesRejected()
{
    // GIVEN: Points with negative coordinates
    QPoint start(-100, -100);
    QPoint end(300, 300);

    // WHEN: Creating selection rectangle
    SelectionGeometry geom;
    QRect rect = geom.createRectangle(start, end);

    // THEN: Rectangle is normalized to valid coordinates
    QVERIFY(rect.isValid());
    QVERIFY(rect.left() >= 0);
    QVERIFY(rect.top() >= 0);
}

void SelectionGeometryTest::testZeroAreaSelectionRejected()
{
    // GIVEN: Start and end points are the same
    QPoint start(100, 100);
    QPoint end(100, 100);

    // WHEN: Creating selection rectangle
    SelectionGeometry geom;
    QRect rect = geom.createRectangle(start, end);

    // THEN: Rectangle is invalid
    QVERIFY2(!rect.isValid() || rect.width() > 0,
             "Zero-area selections should be rejected");
}

void SelectionGeometryTest::testHugeSelectionClampedToScreen()
{
    // GIVEN: Points beyond screen bounds
    QRect screenRect = QApplication::primaryScreen()->geometry();
    QPoint start(-1000, -1000);
    QPoint end(screenRect.width() + 1000, screenRect.height() + 1000);

    // WHEN: Creating selection rectangle
    SelectionGeometry geom;
    QRect rect = geom.createRectangle(start, end);

    // THEN: Rectangle is clamped to screen
    QVERIFY(rect.left() >= screenRect.left());
    QVERIFY(rect.right() <= screenRect.right());
    QVERIFY(rect.top() >= screenRect.top());
    QVERIFY(rect.bottom() <= screenRect.bottom());
}

void SelectionGeometryTest::testResizeHandlePositions()
{
    // GIVEN: Selection rectangle
    QRect selection(100, 100, 200, 200);
    SelectionGeometry geom(selection);

    // WHEN: Calculating resize handle positions
    QList<QPoint> handles = geom.resizeHandlePositions();

    // THEN: All 8 handles are present
    QCOMPARE(handles.size(), 8);

    // Verify corner handles
    QVERIFY(handles.contains(selection.topLeft()));
    QVERIFY(handles.contains(selection.topRight()));
    QVERIFY(handles.contains(selection.bottomLeft()));
    QVERIFY(handles.contains(selection.bottomRight()));
}

QTEST_MAIN(SelectionGeometryTest)
#include "SelectionGeometryTest.moc"
```

---

## Supporting Infrastructure

### Test Fixtures

**Fixture 1: HelperProcessIPC**

```cpp
// tests/support/HelperProcessIPC.h
class HelperProcessIPC {
public:
    // Setup: Start helper process with named pipe
    void setup();

    // Provide to test
    HelperProcess* process();

    // Cleanup: Stop helper process
    void teardown();

private:
    HelperProcess *m_process;
    QProcess *m_ipcServer;
};
```

**Fixture 2: ClipboardState**

```cpp
// tests/support/ClipboardState.h
class ClipboardState {
public:
    // Setup: Clear clipboard and save initial state
    void setup();

    // Provide to test
    QClipboard* clipboard();
    QImage initialImage();

    // Cleanup: Restore initial clipboard state
    void teardown();

private:
    QClipboard *m_clipboard;
    QImage m_initialImage;
};
```

---

### Mock Requirements

**Mock 1: WGC Capture Backend**

- Required for testing without actual screen capture
- Should simulate successful capture
- Should return test image data
- Should simulate capture latency (for performance tests)

**Mock 2: Win32 Hotkey API**

- Required for testing global hotkey handling
- Should simulate hotkey press events
- Should track hotkey registration state

---

## Required data-testid Attributes

**Qt Object Names (equivalent to data-testid):**

**Overlay Window:**

- `selectionOverlay` - Main overlay window
- `selectionBox` - Rectangle selection visual
- `crosshairCursor` - Cursor indicator

**Quick Tray (floating controls):**

- `quickTray` - Floating toolbar container
- `cancelButton` - Cancel capture button
- `confirmButton` - Confirm capture button (Enter alternative)

**Selection Handles:**

- `resizeHandle-tl` - Top-left resize handle
- `resizeHandle-tr` - Top-right resize handle
- `resizeHandle-bl` - Bottom-left resize handle
- `resizeHandle-br` - Bottom-right resize handle
- `resizeHandle-t` - Top edge handle
- `resizeHandle-b` - Bottom edge handle
- `resizeHandle-l` - Left edge handle
- `resizeHandle-r` - Right edge handle

---

## Implementation Checklist

### Phase 1: Helper Process & Hotkey Infrastructure

- [ ] Create `HelperProcess` class (src/Platforms/Windows/HelperProcess.h)
- [ ] Implement Win32 hotkey registration (RegisterHotKey API)
- [ ] Add Registry Run key for auto-start
- [ ] Implement named pipe IPC server (JSON envelope)
- [ ] Add hotkey conflict detection
- [ ] Create helper process CMake target
- [ ] Add IPC message protocol (JSON schema)
- [ ] Implement hotkey simulation for tests
- [ ] Test: `testPrintScreenTriggersOverlayWithinOneSecond()` passes

**Estimated:** 4 hours

### Phase 2: Windows Graphics Capture Backend

- [ ] Create `WGCCapture` class (src/Platforms/Windows/WGCCapture.h)
- [ ] Initialize WGC capture session
- [ ] Implement frame capture with latency tracking
- [ ] Add performance benchmarking (<1s requirement)
- [ ] Cache WGC session after first use
- [ ] Add error handling for WGC failures
- [ ] Create mock backend for tests
- [ ] Test: Latency benchmark passes (P95 < 1000ms)

**Estimated:** 3 hours

### Phase 3: Selection Overlay UI

- [ ] Create `SelectionOverlay` QML component (src/Gui/Overlay/)
- [ ] Implement full-screen transparent window
- [ ] Add crosshair cursor (Qt::CrossCursor)
- [ ] Create `SelectionGeometry` class for rectangle calculations
- [ ] Implement mouse event handlers (press-drag-release)
- [ ] Add visual selection box rendering
- [ ] Create resize handle components (8 handles)
- [ ] Set Qt object names (data-testid equivalents)
- [ ] Test: `testClickDragCreatesSelectionBox()` passes
- [ ] Test: `testSelectionHasResizeHandles()` passes

**Estimated:** 4 hours

### Phase 4: Clipboard Integration

- [ ] Create clipboard manager class
- [ ] Implement image encoding (PNG/JPEG/WEBP)
- [ ] Add clipboard commit with verification
- [ ] Handle clipboard format validation (CF_DIB)
- [ ] Add error handling for commit failures
- [ ] Create clipboard state fixture
- [ ] Test: `testEnterConfirmsCaptureToClipboard()` passes
- [ ] Test: `testAbortDoesNotModifyClipboard()` passes

**Estimated:** 2 hours

### Phase 5: Confirm/Cancel Workflows

- [ ] Implement Enter key handler for confirm
- [ ] Implement Escape key handler for abort
- [ ] Create Cancel button in quick tray
- [ ] Add state cleanup on abort
- [ ] Verify overlay closes properly
- [ ] Test: `testEscapeAbortsCapture()` passes
- [ ] Test: `testCancelButtonAbortsCapture()` passes

**Estimated:** 2 hours

### Phase 6: Autosave Integration

- [ ] Read autosave settings from KConfig
- [ ] Implement file path resolution
- [ ] Add async save to avoid blocking UI
- [ ] Create test autosave directory
- [ ] Test: `testAutosaveWhenEnabled()` passes

**Estimated:** 1 hour

---

## Red-Green-Refactor Workflow

### RED Phase (Complete)

- ✅ All tests written in Given-When-Then format
- ✅ Tests cover P0 acceptance criteria
- ✅ Fixtures defined (HelperProcessIPC, ClipboardState)
- ✅ Mock requirements documented
- ✅ Qt object names documented
- ✅ Implementation checklist created

### GREEN Phase (DEV Team - Next Steps)

1. Pick one failing test
2. Implement minimal code to make it pass
3. Run test: `ctest -R PrintScreenCaptureTest`
4. Verify test passes
5. Move to next test
6. Repeat until all tests pass

**Run specific test:**
```bash
ctest -R testPrintScreenTriggersOverlayWithinOneSecond -V
```

**Run all integration tests:**
```bash
ctest -R PrintScreenCaptureTest -V
```

**Run all unit tests:**
```bash
ctest -R SelectionGeometryTest -V
```

### REFACTOR Phase (DEV Team)

1. All tests passing (green)
2. Extract duplicate code
3. Optimize performance (focus on <1s requirement)
4. Improve error messages
5. Ensure tests still pass

---

## Running Tests

### Run All Failing Tests

```bash
# From build directory
cmake --build . --target filename_test

# Run with CTest
ctest -V

# Run specific test suite
ctest -R PrintScreenCaptureTest -V
```

### Debug Specific Test

```bash
# Run in verbose mode
ctest -R testPrintScreenTriggersOverlayWithinOneSecond -VV

# Use Qt Test debug output
ctest -R PrintScreenCaptureTest --output-on-failure
```

### Performance Benchmarking

```bash
# Run latency tests with timing output
ctest -R Performance -V
```

---

## Expected Failure Messages

**Before implementation, these tests should fail with:**

```
FAIL! : PrintScreenCaptureTest::testPrintScreenTriggersOverlayWithinOneSecond()
"No overlay created within 1 second"
   Loc: [PrintScreenCaptureTest.cpp(56)]

FAIL! : PrintScreenCaptureTest::testClickDragCreatesSelectionBox()
"Selection box not created"
   Loc: [PrintScreenCaptureTest.cpp(89)]

FAIL! : PrintScreenCaptureTest::testEnterConfirmsCaptureToClipboard()
"Clipboard is empty after Enter"
   Loc: [PrintScreenCaptureTest.cpp(125)]
```

These are the **expected RED phase failures** - they indicate missing implementation, not test bugs.

---

## Next Steps for DEV Team

1. **Review this checklist** - Understand test requirements
2. **Set up build environment** - Ensure Qt 6 and KDE Frameworks are available
3. **Create test files** - Add test files to `tests/` directory
4. **Update CMakeLists.txt** - Add new test targets
5. **Run tests to confirm RED phase** - Verify tests fail as expected
6. **Start implementation** - Follow Phase 1-6 checklist above
7. **Mark tests GREEN** - One at a time, minimal implementation
8. **Refactor with confidence** - Tests provide safety net

---

## Knowledge Base References Applied

- Qt Test Framework patterns
- Test fixture architecture (setup/teardown)
- Unit test design (atomic, isolated)
- Integration test strategies (IPC, clipboard)
- Performance testing (<1s NFR validation)

---

**Generated by:** BMad TEA Agent - ATDD Workflow
**Story:** 1-1 - Print Screen Region Capture
**Framework:** Qt Test (C++)
**Date:** 2025-12-30
