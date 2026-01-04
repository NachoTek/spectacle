# Story 1.3: Pre-Capture Annotation Tools

Status: ready-for-dev

## Story

As a Windows 11 user,
I want to annotate before capture using drawing tools,
So that I can add callouts and markup before finalizing the screenshot.

## Acceptance Criteria

**Given** Spectacle is running and the overlay is active with a selection
**When** I choose free draw, arrow, box, or circle annotation tool
**Then** I can draw that annotation within the selection bounds

**Given** I add an annotation to the overlay
**When** I continue editing
**Then** the annotation stays editable until capture is confirmed (Enter pressed)

**Given** I change annotation color or stroke size from the quick tray
**When** I draw new annotations
**Then** the new style is applied to subsequent annotations

**Given** I have annotations on the overlay
**When** I press Enter to confirm capture
**Then** the annotations are included in the captured image

## Tasks / Subtasks

- [ ] Task 1: Design annotation data structures (AC: #1, #2)
  - [ ] Subtask 1.1: Create AnnotationType enum (FreeDraw, Arrow, Box, Circle, Text)
  - [ ] Subtask 1.2: Create Annotation C++ class with properties (type, points, color, strokeWidth, uuid)
  - [ ] Subtask 1.3: Create AnnotationListModel QML model for managing annotations

- [ ] Task 2: Implement free draw annotation (AC: #1)
  - [ ] Subtask 2.1: Add free draw tool button to quick tray
  - [ ] Subtask 2.2: Create FreeDrawAnnotation QML component for capturing mouse path
  - [ ] Subtask 2.3: Store free draw points as QVector<QPoint> in Annotation object
  - [ ] Subtask 2.4: Render free draw path on overlay using Canvas or PathView

- [ ] Task 3: Implement arrow annotation (AC: #1)
  - [ ] Subtask 3.1: Add arrow tool button to quick tray
  - [ ] Subtask 3.2: Create ArrowAnnotation QML component (drag start to end)
  - [ ] Subtask 3.3: Calculate arrow head geometry from start/end points
  - [ ] Subtask 3.4: Render arrow with line and triangular head on overlay

- [ ] Task 4: Implement box/rectangle annotation (AC: #1)
  - [ ] Subtask 4.1: Add box tool button to quick tray
  - [ ] Subtask 4.2: Create BoxAnnotation QML component (drag diagonal corners)
  - [ ] Subtask 4.3: Store box as QRect with stroke properties
  - [ ] Subtask 4.4: Render rectangle outline on overlay

- [ ] Task 5: Implement circle/ellipse annotation (AC: #1)
  - [ ] Subtask 5.1: Add circle tool button to quick tray
  - [ ] Subtask 5.2: Create CircleAnnotation QML component (drag bounding box)
  - [ ] Subtask 5.3: Calculate ellipse from bounding rect or center+radius
  - [ ] Subtask 5.4: Render ellipse outline on overlay

- [ ] Task 6: Add color and stroke size picker (AC: #3)
  - [ ] Subtask 6.1: Add color picker button(s) to quick tray (preset colors: Red, Blue, Green, Yellow, Black, White)
  - [ ] Subtask 6.2: Add stroke size slider or buttons (1px, 2px, 4px, 8px)
  - [ ] Subtask 6.3: Store current annotation style in SelectionOverlay C++ class
  - [ ] Subtask 6.4: Apply selected style to new annotations

- [ ] Task 7: Make annotations editable (AC: #2)
  - [ ] Subtask 7.1: Add selection mode for annotations (click to select)
  - [ ] Subtask 7.2: Show resize/delete handles for selected annotation
  - [ ] Subtask 7.3: Support Delete key to remove selected annotation
  - [ ] Subtask 7.4: Support Undo (Ctrl+Z) to remove last annotation

- [ ] Task 8: Render annotations in captured image (AC: #4)
  - [ ] Subtask 8.1: Modify capture pipeline to composite annotations before WGC capture
  - [ ] Subtask 8.2: Create QImage render function that draws selection + annotations
  - [ ] Subtask 8.3: Ensure annotations appear in clipboard image data
  - [ ] Subtask 8.4: Ensure annotations appear in autosaved file

- [ ] Task 9: Testing (All AC)
  - [ ] Subtask 9.1: Test free draw creates smooth path
  - [ ] Subtask 9.2: Test arrow renders with correct head orientation
  - [ ] Subtask 9.3: Test box and circle annotations render correctly
  - [ ] Subtask 9.4: Test color and stroke size changes work
  - [ ] Subtask 9.5: Test annotations are editable (select, move, delete, undo)
  - [ ] Subtask 9.6: Test annotations appear in captured image (clipboard and save)
  - [ ] Subtask 9.7: Test multiple annotations can coexist
  - [ ] Subtask 9.8: Unit tests for Annotation data model
  - [ ] Subtask 9.9: Integration test for annotation rendering pipeline

## Dev Notes

### Epic Context
This is Story 1.3 in Epic 1 (Screenshot Capture & Annotation Flow). Stories 1.1 and 1.2 (overlay + selection) are complete. This story introduces pre-capture annotation tools that will be REUSED in Story 1.4 (post-capture annotation).

**Critical Dependency:** The same annotation components created here MUST be reusable in Story 1.4 without duplication. Design annotations as a standalone QML module that can be loaded into both pre-capture overlay and post-capture view.

### Architecture Requirements

**From Architecture:**
- UI stack: Qt Quick/QML with Windows 11 Fluent styling
- Keep UI in QML under `src/Gui/`; platform-specific logic in C++
- Use Qt signals/slots to bridge core state into QML
- Overlay must remain responsive (<1s); avoid long blocking calls
- Clipboard must update on annotation changes

**From Project Context:**
- Use C++20; member variables use `mCamelCase`
- Access modifier order: public, signals, public slots, protected, private
- Source file names match class names
- Use QLoggingCategory for logging (no printf/stdout)
- Windows-specific code behind `#ifdef Q_OS_WIN`
- Keep commits small and atomic

**Key Architecture Decision:** Use `KQuickImageEditor` if available (mentioned in project context), otherwise build custom annotation components. KQuickImageEditor is a Qt Quick component library for image annotation that provides free draw, arrow, box, circle tools out of the box.

**Implementation Options:**
1. **Option A: Use KQuickImageEditor (RECOMMENDED)**
   - Pros: Battle-tested, handles edge cases, consistent with KDE ecosystem
   - Cons: May need to package/build library, version compatibility
   - Source: https://invent.kde.org/libraries/kquickimageeditor

2. **Option B: Custom annotation components**
   - Pros: Full control, no external dependency
   - Cons: Reinvent wheel, edge cases, maintenance burden

**Recommendation:** Attempt KQuickImageEditor first. If it's not viable for Windows/Qt 6.9, build custom components using the same API structure so we can switch later if needed.

### File Structure Requirements

**Create new files:**
```
src/Gui/Annotation/
  Annotation.h (data model)
  Annotation.cpp
  AnnotationListModel.h (QAbstractListModel)
  AnnotationListModel.cpp
  AnnotationTool.h (enum)
  AnnotationTool.cpp
  AnnotationCanvas.qml (renders all annotations)
  FreeDrawAnnotation.qml
  ArrowAnnotation.qml
  BoxAnnotation.qml
  CircleAnnotation.qml
  AnnotationToolbar.qml (color/stroke picker)
```

**Modify existing files:**
```
src/Gui/Overlay/SelectionOverlay.h
  - Add annotation model, current tool, current style properties
  - Add signals: annotationAdded, annotationRemoved, annotationModified
  - Add methods: addAnnotation(), removeAnnotation(), clearAnnotations()
  - Add method: renderAnnotationsToImage() for capture pipeline

src/Gui/Overlay/SelectionOverlay.qml
  - Load AnnotationCanvas above selection box (z-index)
  - Load AnnotationToolbar in quick tray
  - Route mouse events to annotation tools when tool is active
  - Handle Delete key, Ctrl+Z for undo
```

**Do NOT modify:**
- `src/Platforms/Windows/*` (annotation is platform-agnostic UI logic)
- Core capture pipeline in WGC (annotations composited before capture)

### Technical Requirements

**Qt Types to Use:**
- `QVector<QPoint>` for free draw paths
- `QLineF` for arrow geometry calculations
- `QRectF` for box/circle bounding rects
- `QColor` for annotation colors
- `QAbstractListModel` for annotation list model
- `QPainter` for rendering annotations to QImage
- `QQmlEngine` for loading QML components dynamically

**QML Components:**
- Use `Canvas` or `PainterPath` for drawing annotations (QML has Canvas element)
- Use `MouseArea` for capturing drag gestures
- Use `Keys.onPressed` for Delete, Ctrl+Z shortcuts
- Use `Repeater` to render annotation list
- Use `Qt.rgba()` for color values

**Annotation State Management:**
```cpp
// In SelectionOverlay.h
enum class AnnotationTool {
    None,
    FreeDraw,
    Arrow,
    Box,
    Circle
};

struct AnnotationStyle {
    QColor color = Qt::red;
    int strokeWidth = 2;
};

class SelectionOverlay {
    // Existing members...

    // Annotation state
    AnnotationTool m_currentTool = AnnotationTool::None;
    AnnotationStyle m_currentStyle;
    AnnotationListModel* m_annotationModel;  // Owned

Q_SIGNALS:
    void annotationAdded(const Annotation& annotation);
    void annotationRemoved(const QUuid& uuid);
    void currentToolChanged(AnnotationTool tool);
    void currentStyleChanged(const AnnotationStyle& style);

public:
    void setAnnotationTool(AnnotationTool tool);
    void setAnnotationColor(const QColor& color);
    void setAnnotationStrokeWidth(int width);
    void addAnnotation(const Annotation& annotation);
    void removeAnnotation(const QUuid& uuid);
    void undoLastAnnotation();
    QImage renderAnnotations(const QRect& selection, const QImage& capture);
};
```

### Testing Requirements

**Unit Tests (tests/Unit/):**
- `AnnotationTest.cpp`: Test Annotation data model serialization
- `AnnotationListModelTest.cpp`: Test add/remove/undo operations
- `AnnotationGeometryTest.cpp`: Test arrow head, ellipse calculations

**Integration Tests (tests/Integration/):**
- `AnnotationPipelineTest.cpp`: Test annotations render correctly in captured image
- `AnnotationUndoRedoTest.cpp`: Test undo/redo of annotations
- `AnnotationStyleTest.cpp`: Test color and stroke size changes

**Manual Testing Checklist:**
- [ ] Free draw creates smooth connected line
- [ ] Arrow head points in correct direction (end - start vector)
- [ ] Box and circle render with correct aspect ratio
- [ ] Color picker changes color immediately
- [ ] Stroke size changes thickness immediately
- [ ] Select annotation shows resize handles
- [ ] Delete key removes selected annotation
- [ ] Ctrl+Z removes last annotation
- [ ] Annotations appear in captured screenshot
- [ ] Annotations appear in clipboard paste
- [ ] Multiple annotations can coexist
- [ ] Annotations persist across selection resize
- [ ] Clearing selection clears annotations OR warns user

### Previous Story Intelligence

**From Story 1.1 (Overlay + Confirm/Cancel):**
- Learned: Overlay is full-screen QML Rectangle with MouseArea
- Learned: Quick tray uses Row layout with Buttons
- Learned: Selection bounds stored in SelectionOverlay C++ class
- Learned: Enter key triggers `enterPressed()`, Escape triggers `escapePressed()`
- **Critical Pattern:** Overlay must remain lightweight; defer heavy operations

**From Story 1.2 (Selection Targeting):**
- Learned: Window detection uses WindowDetector C++ class
- Learned: TargetHighlighter QML component shows window under cursor
- Learned: Selection updates use `setSelectionRect()` method
- **Critical Pattern:** Separate "selection" from "annotations" - selection bounds the capture area, annotations decorate it

**From Story 1.8 (Window Movement):**
- Learned: WindowEventMonitor tracks window movement
- Learned: Overlay must persist during window movement
- **Important:** Annotations must NOT interfere with window tracking - they're independent features

### Git Intelligence Summary

**Recent commits (last 5):**
- `5b8c2ad2` - Fix clang-tidy workflow (CI improvements)
- `e988b70e` - Add code linting workflows (clang-tidy, qmllint)
- `96ad8073` - Fix CI build errors (AutoMoc, qBound issues)
- `6cf6d083` - Fix code review issues for Story 1.8 (thread-safety, validation)
- `5b265773` - Implement Stories 1.2 and 1.8 (window targeting, movement tracking)

**Code patterns observed:**
- Use QLoggingCategory: `qCDebug(LOG_CATEGORY) << "message"`
- Use `QMutexLocker` for thread safety
- Use `Q_SIGNALS` macro for signal declarations
- QML components use PascalCase filenames, lowerCamelCase ids
- Use `objectName` property for test object identification

**Library dependencies:**
- Qt 6.9.0 (Core, Gui, Qml, Quick, Widgets, Multimedia)
- C++20 features (concepts, ranges, etc.)
- Windows-specific: Win32 API, WGC

### Latest Tech Information

**KQuickImageEditor Status (2025):**
- KQuickImageEditor is maintained by KDE
- Latest version: 0.5.0 (as of 2024)
- Qt 6 support: Yes (requires Qt >= 6.0)
- License: LGPL v2.1
- Repository: https://invent.kde.org/libraries/kquickimageeditor
- Provides: FreeDraw, Arrow, Box, Circle, Text, Eraser, Undo/Redo
- **Note:** May need CMake integration; check if it's in KDE Frameworks 6.18

**Alternative: Qt Quick PaintedItem:**
- Qt Quick 3D `PaintedItem` provides 2D painting
- Good fallback if KQuickImageEditor isn't viable

**Qt Canvas vs. QQuickPaintedItem:**
- QML `Canvas` element: Software-rendered, simpler API
- `QQuickPaintedItem`: Hardware-accelerated, more control
- **Recommendation:** Use Canvas for simplicity unless performance issues

### Performance Considerations

- Annotations must not block the UI thread during rendering
- Use `QImage` for rasterizing annotations (fast for 2D)
- Limit free draw point sampling (e.g., capture point every 5 pixels)
- Use QRect/QPoint for geometry (not float-based QPointF unless needed)
- Cache rendered annotations if complex (but keep simple for this story)

### Accessibility

- Annotation tools must be keyboard-accessible (Tab to cycle tools, 1-5 for tool shortcuts)
- Color picker should have high contrast options
- Stroke size should have visible indicators
- Screen reader should announce tool changes

### References

- Story Requirements: [_bmad-output/planning-artifacts/epics.md](epics.md#Story-13)
- FR8-FR11: Annotation functional requirements
- Architecture: [_bmad-output/planning-artifacts/architecture.md](architecture.md)
- Project Context: [_bmad-output/project-context.md](project-context.md)
- Previous Story: [1-1-print-screen-region-capture-overlay-confirm-cancel](1-1-print-screen-region-capture-overlay-confirm-cancel.md)
- Previous Story: [1-2-selection-targeting-refinement](1-2-selection-targeting-refinement.md)

## Dev Agent Record

### Agent Model Used

Claude Sonnet 4.5 (claude-sonnet-4-5-20251101)

### Debug Log References

None yet - story not started

### Completion Notes List

Story created with comprehensive context from epics, architecture, project context, and previous stories. Ready for dev-story workflow implementation.

### File List

**To Create:**
- src/Gui/Annotation/Annotation.h
- src/Gui/Annotation/Annotation.cpp
- src/Gui/Annotation/AnnotationListModel.h
- src/Gui/Annotation/AnnotationListModel.cpp
- src/Gui/Annotation/AnnotationCanvas.qml
- src/Gui/Annotation/FreeDrawAnnotation.qml
- src/Gui/Annotation/ArrowAnnotation.qml
- src/Gui/Annotation/BoxAnnotation.qml
- src/Gui/Annotation/CircleAnnotation.qml
- src/Gui/Annotation/AnnotationToolbar.qml
- tests/Unit/AnnotationTest.cpp
- tests/Unit/AnnotationListModelTest.cpp
- tests/Integration/AnnotationPipelineTest.cpp

**To Modify:**
- src/Gui/Overlay/SelectionOverlay.h
- src/Gui/Overlay/SelectionOverlay.cpp
- src/Gui/Overlay/SelectionOverlay.qml
- tests/CMakeLists.txt (add new tests)

**To NOT Modify:**
- src/Platforms/Windows/* (keep platform-specific)
- WGC capture pipeline (annotations composited before capture)
