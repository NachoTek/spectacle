# Story 1.4 Code Review Action Items

**Review Date**: 2025-01-04
**Reviewer**: Claude Sonnet 4.5 (Adversarial Code Review Workflow)
**Status**: ❌ INCOMPLETE - 5 CRITICAL, 5 MAJOR, 3 MINOR issues found

---

## 🔴 CRITICAL ISSUES (Must Fix)

### CRITICAL #1: Missing Integration in SpectacleCore
**Severity**: HIGH
**Impact**: Post-capture viewer will never open - signal emitted but nothing listening
**Files Affected**:
- `src/SpectacleCore.h`
- `src/SpectacleCore.cpp`

**Required Actions**:

1. **Add slot declaration to SpectacleCore.h** (after line 123, in public Q_SLOTS section):
```cpp
// Story 1.4: Open post-capture annotation viewer
Q_SLOTS:
    void openPostCaptureAnnotationViewer(const QImage &image, AnnotationListModel *annotations);
```

2. **Add include to SpectacleCore.h** (at top with other includes):
```cpp
#include "Gui/Annotation/AnnotationListModel.h"
```

3. **Add slot implementation to SpectacleCore.cpp**:
```cpp
void SpectacleCore::openPostCaptureAnnotationViewer(const QImage &image, AnnotationListModel *annotations)
{
    // Story 1.4: Open post-capture annotation viewer with captured image and annotations
    auto viewer = new PostCaptureAnnotationViewer(image, annotations, nullptr);
    viewer->show();
}
```

4. **Add include to SpectacleCore.cpp**:
```cpp
#include "Gui/Annotation/PostCaptureAnnotationViewer.h"
```

5. **Connect signal in SpectacleCore initialization** (where SelectionOverlay is created):
```cpp
// Story 1.4: Connect post-capture view signal
connect(m_selectionOverlay, &SelectionOverlay::openPostCaptureView,
        this, &SpectacleCore::openPostCaptureAnnotationViewer);
```

**Verification**:
- Add breakpoint in `openPostCaptureAnnotationViewer()` and verify it's called when Enter is pressed on overlay
- Manual test: Press Enter on overlay, confirm post-capture viewer opens

---

### CRITICAL #2: Missing Unsaved Changes Warning
**Severity**: HIGH
**Impact**: Users can lose work by pressing Escape - violates explicit AC requirement
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.qml`

**Required Actions**:

1. **Add closeEvent handler to PostCaptureAnnotationViewer.h** (in private Q_SLOTS):
```cpp
private Q_SLOTS:
    void onAnnotationsChanged();
    void closeEvent(QCloseEvent *event) override;  // ADD THIS
```

2. **Implement closeEvent in PostCaptureAnnotationViewer.cpp**:
```cpp
void PostCaptureAnnotationViewer::closeEvent(QCloseEvent *event)
{
    // Task 7.5: Show confirmation if unsaved changes
    if (m_hasUnsavedChanges) {
        // Show confirmation dialog
        QMessageBox::StandardButton response = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved annotations. Close anyway?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (response == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();
    QQuickView::closeEvent(event);
}
```

3. **Add include to PostCaptureAnnotationViewer.cpp**:
```cpp
#include <QMessageBox>
```

4. **Remove Escape key direct close in PostCaptureAnnotationViewer.qml** (lines 175-178):
```qml
// REMOVE THIS CODE:
// else if (event.key === Qt.Key_Escape) {
//     event.accepted = true
//     viewerWindow.close()
// }

// REPLACE WITH:
else if (event.key === Qt.Key_Escape) {
    event.accepted = true
    viewerWindow.requestClose()  // This triggers closeEvent with confirmation
}
```

5. **Add Q_INVOKABLE method to PostCaptureAnnotationViewer.h**:
```cpp
Q_INVOKABLE void requestClose();  // Add after other Q_INVOKABLE methods
```

6. **Implement requestClose in PostCaptureAnnotationViewer.cpp**:
```cpp
void PostCaptureAnnotationViewer::requestClose()
{
    close();  // This will trigger closeEvent with confirmation logic
}
```

**Verification**:
- Add annotations in post-capture viewer
- Press Escape without saving
- Confirm dialog appears
- Test both Yes and No buttons

---

### CRITICAL #3: QML Image Source Will Fail
**Severity**: HIGH
**Impact**: Captured image will not display in post-capture viewer
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.qml`

**Required Actions**:

1. **Create ImageProvider class** (new file: `src/Gui/Annotation/CapturedImageProvider.h`):
```cpp
#ifndef CAPTUREDIMAGEPROVIDER_H
#define CAPTUREDIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMap>

class CapturedImageProvider : public QQuickImageProvider
{
public:
    CapturedImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    void setImage(const QString &id, const QImage &image);

private:
    QMap<QString, QImage> m_images;
};

#endif // CAPTUREDIMAGEPROVIDER_H
```

2. **Create ImageProvider implementation** (new file: `src/Gui/Annotation/CapturedImageProvider.cpp`):
```cpp
#include "CapturedImageProvider.h"

CapturedImageProvider::CapturedImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage CapturedImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (m_images.contains(id)) {
        QImage img = m_images.value(id);

        if (size) {
            *size = img.size();
        }

        if (requestedSize.width() > 0 && requestedSize.height() > 0) {
            return img.scaled(requestedSize, Qt::KeepAspectRatio);
        }

        return img;
    }

    return QImage();
}

void CapturedImageProvider::setImage(const QString &id, const QImage &image)
{
    m_images.insert(id, image);
}
```

3. **Add image provider to PostCaptureAnnotationViewer.h** (private members):
```cpp
private:
    CapturedImageProvider *m_imageProvider;  // Add this line
```

4. **Initialize image provider in PostCaptureAnnotationViewer.cpp constructor** (after setupQml()):
```cpp
// Setup image provider for QML
m_imageProvider = new CapturedImageProvider();
engine()->addImageProvider(QLatin1String("capture"), m_imageProvider);

// Register captured image with provider
QString imageId = QString::number(reinterpret_cast<quintptr>(this));
m_imageProvider->setImage(imageId, m_capturedImage);
rootContext()->setContextProperty(QStringLiteral("capturedImageId"), imageId);
```

5. **Update QML Image source** (line 36 in PostCaptureAnnotationViewer.qml):
```qml
// CHANGE FROM:
source: capturedImage !== null ? "image://capture/" + root.capturedImage : ""

// CHANGE TO:
source: capturedImage !== null ? "image://capture/" + capturedImageId : ""
```

6. **Update CMakeLists to include new files**:
```cmake
# Add to annotation sources
src/Gui/Annotation/CapturedImageProvider.cpp
```

**Verification**:
- Capture a screenshot
- Verify post-capture viewer opens with image displayed
- Check that image renders correctly with annotations overlaid

---

### CRITICAL #4: Missing AnnotationListModel Deep Copy Method
**Severity**: HIGH
**Impact**: Violates explicit requirement, error-prone manual copy
**Files Affected**:
- `src/Gui/Annotation/AnnotationListModel.h`
- `src/Gui/Annotation/AnnotationListModel.cpp`
- Story file (to update requirements)

**Option A: Add Copy Constructor (RECOMMENDED)**

1. **Add copy constructor declaration to AnnotationListModel.h**:
```cpp
// In public section, after constructor
AnnotationListModel(const AnnotationListModel &other);
```

2. **Implement copy constructor in AnnotationListModel.cpp**:
```cpp
AnnotationListModel::AnnotationListModel(const AnnotationListModel &other)
    : QAbstractListModel(other.parent())
{
    // Deep copy all annotations
    for (int i = 0; i < other.m_annotations.count(); ++i) {
        m_annotations.append(other.m_annotations.at(i));
    }
}
```

3. **Update PostCaptureAnnotationViewer.cpp constructor** (lines 39-47):
```cpp
// REPLACE current manual copy loop with:
m_annotationModel = new AnnotationListModel(*annotations, this);
```

**Option B: Update Story Requirements**

If you choose not to implement the copy constructor, update the story file to remove the requirement:

1. **Edit Story 1.4 file** (line 136):
```markdown
# REMOVE THIS LINE:
src/Gui/Annotation/AnnotationListModel.h
  - Add copy constructor or clone() method for deep copy (to avoid shared state)
```

2. **Add note explaining why**:
```markdown
# ADD:
**Note**: AnnotationListModel uses manual deep copy in PostCaptureAnnotationViewer constructor.
Copy constructor not implemented as manual iteration provides equal functionality.
```

**Verification**:
- Add test to verify deep copy (modifications to original don't affect copy)
- Verify existing `testAnnotationModelCopyProducesIndependentInstance` still passes

---

### CRITICAL #5: No Image Provider Registered
**Severity**: HIGH
**Impact**: QML cannot access captured image (same issue as CRITICAL #3)
**Status**: **Fixed as part of CRITICAL #3**

---

## ⚠️ MAJOR ISSUES (Should Fix)

### MAJOR #5: Missing Keyboard Shortcut Connections in QML
**Severity**: MEDIUM
**Impact**: Users can't use Ctrl+S or Ctrl+C shortcuts
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.qml`

**Required Actions**:

1. **Add keyboard shortcut handlers** (in Keys.onPressed, after line 173):
```qml
// Add after Undo handler:
// Task 7.1: Support Ctrl+S to trigger Save As
else if (event.modifiers & Qt.ControlModifier && event.key === Qt.Key_S) {
    event.accepted = true
    viewerWindow.saveAs()
}
// Task 7.2: Support Ctrl+C to copy to clipboard
else if (event.modifiers & Qt.ControlModifier && event.key === Qt.Key_C) {
    event.accepted = true
    viewerWindow.copyToClipboard()
}
```

**Verification**:
- Open post-capture viewer
- Press Ctrl+S - verify Save As dialog opens
- Press Ctrl+C - verify image copied to clipboard
- Paste into Paint/Word to verify

---

### MAJOR #6: Window Not Centered on Screen
**Severity**: MEDIUM
**Impact**: Window appears at random position, poor UX
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`

**Required Actions**:

1. **Add include** (at top with other includes):
```cpp
#include <QStyle>
#include <QScreen>
```

2. **Add window centering in constructor** (after line 68, replace resize call):
```cpp
// Window properties
setWindowTitle(tr("Annotation Editor - Spectacle"));
resize(800, 600);
setResizeMode(QQuickView::SizeRootObjectToView);

// Task 7.6: Center window on screen
QScreen *screen = QGuiApplication::primaryScreen();
QRect screenGeometry = screen->availableGeometry();
QRect centeredRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), screenGeometry);
setGeometry(centeredRect);
```

**Verification**:
- Trigger post-capture viewer
- Confirm window appears centered on primary screen
- Test on multi-monitor setup if available

---

### MAJOR #7: No Confirmation Message Displayed
**Severity**: MEDIUM
**Impact**: No user feedback for Save As / Copy operations
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.qml`

**Required Actions**:

1. **Add signals to PostCaptureAnnotationViewer.h** (in Q_SIGNALS section):
```cpp
Q_SIGNALS:
    void imageSaved(const QString &path);
    void imageCopied();  // ADD THIS
    void saveError(const QString &message);  // ADD THIS
```

2. **Emit signals in PostCaptureAnnotationViewer.cpp**:

In `saveAs()` method (after line 158):
```cpp
qCDebug(LOG_POSTCAPTURE) << "Image saved to" << fileName;
m_hasUnsavedChanges = false;
Q_EMIT imageSaved(fileName);  // EXISTING

// ADD AFTER SUCCESS:
Q_EMIT imageCopied();  // Reuse signal for generic success notification
```

In `saveAs()` error cases (after lines 147, 154):
```cpp
// REPLACE:
qCWarning(LOG_POSTCAPTURE) << "Failed to render image for saving";

// WITH:
Q_EMIT saveError(tr("Failed to render image for saving"));
```

3. **Connect signals in QML** (add to Component.onCompleted or root level):
```qml
// Add at bottom of file, after keyboard handler:
Connections {
    target: viewerWindow
    function onImageSaved(path) {
        messageBox.showMessage(qsTr("Image saved: ") + path)
    }
    function onImageCopied() {
        messageBox.showMessage(qsTr("Copied to clipboard"))
    }
    function onSaveError(message) {
        messageBox.showMessage(qsTr("Save failed: ") + message)
    }
}
```

**Verification**:
- Save image - verify success message appears
- Copy to clipboard - verify confirmation message appears
- Test error cases if possible (e.g., save to read-only location)

---

### MAJOR #8: No Image Provider Registered
**Severity**: MEDIUM
**Status**: **Fixed as part of CRITICAL #3**

---

### MAJOR #9: Missing Window Flags for Resizing
**Severity**: MEDIUM
**Impact**: Window may not be resizable on some platforms
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`

**Required Actions**:

1. **Add window flags in constructor** (after line 68, after setWindowTitle):
```cpp
setWindowTitle(tr("Annotation Editor - Spectacle"));

// Task 7.6: Ensure window is resizable
setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowResizeHint);

resize(800, 600);
```

**Verification**:
- Open post-capture viewer
- Verify maximize button is enabled
- Verify window can be resized by dragging edges
- Verify window can be maximized/restored

---

### MAJOR #10: Unit Tests Have Missing Implementation
**Severity**: MEDIUM
**Impact**: Tests don't verify what they claim to test
**Files Affected**:
- `tests/Unit/PostCaptureAnnotationViewerTest.cpp`

**Required Actions**:

1. **Fix testViewerCreationWithValidImage** (lines 55-64):
```cpp
void PostCaptureAnnotationViewerTest::testViewerCreationWithValidImage()
{
    AnnotationListModel *model = new AnnotationListModel(this);
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, model);

    QVERIFY(viewer != nullptr);
    // Verify viewer has the image set
    QVERIFY(!viewer->capturedImage().isNull());
    QCOMPARE(viewer->capturedImage().size(), m_testImage.size());

    delete viewer;
}
```

2. **Fix testViewerCreationWithNullImage** (lines 67-77):
```cpp
void PostCaptureAnnotationViewerTest::testViewerCreationWithNullImage()
{
    QImage nullImage;
    AnnotationListModel *model = new AnnotationListModel(this);
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(nullImage, model);

    QVERIFY(viewer != nullptr);
    // Viewer should handle null image gracefully
    QVERIFY(viewer->capturedImage().isNull());

    delete viewer;
}
```

3. **Add integration test for overlay → post-capture workflow** (new test):
```cpp
void PostCaptureAnnotationViewerTest::testPostCaptureWorkflow()
{
    // Create overlay with annotations
    AnnotationListModel *overlayModel = new AnnotationListModel(this);

    Annotation annotation(AnnotationTool::Box);
    annotation.setColor(Qt::red);
    annotation.setStrokeWidth(2);
    annotation.setBoundingBox(QRect(10, 10, 50, 50));
    overlayModel->addAnnotation(annotation);

    // Create post-capture viewer (simulates overlay transition)
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, overlayModel);

    // Verify annotations transferred
    QVERIFY(viewer->annotationModel() != overlayModel);
    QCOMPARE(viewer->annotationModel()->rowCount(), 1);

    // Verify independence
    overlayModel->addAnnotation(annotation);
    QCOMPARE(viewer->annotationModel()->rowCount(), 1);

    delete viewer;
}
```

**Verification**:
- Run all PostCaptureAnnotationViewerTest tests
- Verify all tests pass
- Check test coverage

---

## 📝 MINOR ISSUES (Nice to Fix)

### MINOR #11: Incorrect Window Type in Comment
**Severity**: LOW
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h:30`

**Required Action**:
```cpp
// CHANGE FROM:
* @brief Post-capture annotation viewer window
* Provides a QWidget-based window for editing annotations after capture.

// CHANGE TO:
* @brief Post-capture annotation viewer window
* Provides a Qt Quick window (QQuickView) for editing annotations after capture.
```

---

### MINOR #12: Missing #include for QColor
**Severity**: LOW
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h`

**Required Action**:
```cpp
// Add to includes at top of file:
#include <QColor>
```

---

### MINOR #13: Unused Parameters in C++ Methods
**Severity**: LOW
**Impact**: Tool selection state is lost, no actual tool management
**Files Affected**:
- `src/Gui/Annotation/PostCaptureAnnotationViewer.h`
- `src/Gui/Annotation/PostCaptureAnnotationViewer.cpp`

**Required Actions**:

1. **Add member variables to PostCaptureAnnotationViewer.h** (private section):
```cpp
private:
    int m_currentTool;  // Add this
    QColor m_currentColor;  // Add this
    int m_currentStrokeWidth;  // Add this
```

2. **Initialize in constructor** (PostCaptureAnnotationViewer.cpp line 37):
```cpp
, m_currentTool(static_cast<int>(AnnotationTool::None))
, m_currentColor(QColor("#FF0000"))
, m_currentStrokeWidth(2)
```

3. **Implement setter methods** (replace Q_UNUSED implementations):
```cpp
void PostCaptureAnnotationViewer::setCurrentTool(int tool)
{
    m_currentTool = tool;
    qDebug("Current tool changed to: %d", tool);
    Q_EMIT currentToolChanged();
}

void PostCaptureAnnotationViewer::setCurrentColor(const QColor &color)
{
    m_currentColor = color;
    qDebug("Current color changed to: %s", qUtf8Printable(color.name()));
    Q_EMIT currentColorChanged();
}

void PostCaptureAnnotationViewer::setCurrentStrokeWidth(int width)
{
    m_currentStrokeWidth = width;
    qDebug("Current stroke width changed to: %d", width);
    Q_EMIT currentStrokeWidthChanged();
}
```

4. **Add signals to PostCaptureAnnotationViewer.h** (Q_SIGNALS section):
```cpp
Q_SIGNALS:
    void imageSaved(const QString &path);
    void currentToolChanged();  // Add this
    void currentColorChanged();  // Add this
    void currentStrokeWidthChanged();  // Add this
```

**Verification**:
- Open post-capture viewer
- Select different tools from toolbar
- Verify tool state is maintained
- Verify annotations use correct color/width

---

## Implementation Priority Order

### Phase 1: Blockers (Must Do Before Story Can Complete)
1. **CRITICAL #1** - SpectacleCore integration (viewer won't open without this)
2. **CRITICAL #3** - Image provider setup (image won't display without this)
3. **CRITICAL #2** - Unsaved changes warning (explicit AC requirement)

### Phase 2: Core Functionality (Should Do)
4. **CRITICAL #4** - Deep copy method or update story requirements
5. **MAJOR #5** - Keyboard shortcuts (Ctrl+S, Ctrl+C)
6. **MAJOR #6** - Window centering
7. **MAJOR #7** - Success/error messages
8. **MAJOR #9** - Window flags for resizing

### Phase 3: Quality Improvements (Nice to Have)
9. **MAJOR #10** - Complete unit test implementation
10. **MINOR #11-13** - Documentation and minor fixes

---

## Testing Checklist

After implementing fixes, verify:

- [ ] Post-capture viewer opens when Enter is pressed on overlay
- [ ] Captured image displays correctly in viewer
- [ ] Pre-capture annotations are transferred and editable
- [ ] New annotations can be added in post-capture
- [ ] Annotations render correctly on saved/copied images
- [ ] Clipboard updates automatically when annotations change
- [ ] Save As dialog opens and saves with correct format
- [ ] Copy to Clipboard button works
- [ ] Ctrl+S opens Save As dialog
- [ ] Ctrl+C copies to clipboard
- [ ] Ctrl+Z undoes last annotation
- [ ] Delete key removes selected annotation
- [ ] Escape key shows confirmation if unsaved changes
- [ ] Window is centered on screen
- [ ] Window is resizable
- [ ] Success messages display for save/copy operations
- [ ] All unit tests pass
- [ ] Integration test for overlay → post-capture workflow passes

---

## Next Steps

1. **Assign each action item to a developer**
2. **Create feature branch** from `planning-windows11-conversion`
3. **Implement fixes in priority order**
4. **Run tests after each fix**
5. **Manual test on Windows 11**
6. **Submit PR for code review**
7. **Update story file** to mark tasks as complete
8. **Re-run adversarial code review** to verify all issues resolved
9. **Mark story as complete** only when all CRITICAL issues are fixed

---

**Generated**: 2025-01-04
**Workflow**: bmad:bmm:workflows:code-review
**Agent**: Claude Sonnet 4.5
