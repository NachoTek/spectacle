/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Overlay Window
 *  Story 1.1 - Full-Screen Capture Overlay
 *  Story 1.2 - Selection Targeting + Refinement
 *  Story 1.3 - Pre-Capture Annotation Tools
 *  Story 1.8 - Window Movement Detection & Overlay Persistence
 */

#include "SelectionOverlay.h"
#include "SelectionGeometry.h"
#include "Platforms/Windows/WGCCapture.h"
#include "Gui/Annotation/AnnotationListModel.h"
#include "Gui/Annotation/Annotation.h"
#include "Gui/Annotation/AnnotationTool.h"
#include "Gui/Annotation/AnnotationRenderer.h"  // Task 8: Render annotations
#include "Gui/Annotation/PostCaptureAnnotationViewer.h"  // Story 1.4: Post-capture annotation viewer

#ifdef Q_OS_WIN
#include "Platforms/Windows/WindowDetector.h"
#include "Gui/Overlay/SelectionSnapper.h"
#include "Platforms/Windows/WindowEventMonitor.h"
#endif

#include <QScreen>
#include <QGuiApplication>
#include <QQuickItem>
#include <QQmlContext>
#include <QClipboard>
#include <QApplication>
#include <QStandardPaths>
#include <QMetaObject>

SelectionOverlay::SelectionOverlay(QObject *parent)
    : QObject(parent)
    , m_view(nullptr)
    , m_dragging(false)
    , m_autosaveEnabled(false)
    , m_autosavePath(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1String("/screenshot.png"))
#ifdef Q_OS_WIN
    , m_windowDetector(nullptr)
    , m_selectionSnapper(nullptr)
    , m_eventMonitor(nullptr)
    , m_targetingEnabled(true)  // Enable click-to-select targeting by default
#endif
    // Story 1.3: Initialize annotation system
    , m_annotationModel(new AnnotationListModel(this))
    , m_currentTool(static_cast<int>(AnnotationTool::None))
    , m_currentColor(QColor("#FF0000"))  // Default: red
    , m_currentStrokeWidth(2)  // Default: 2px
{
#ifdef Q_OS_WIN
    // Initialize Windows-specific targeting components
    m_windowDetector = new WindowDetector(this);
    m_selectionSnapper = new SelectionSnapper(this);
    m_eventMonitor = new WindowEventMonitor(this);

    // Connect snap completion signal
    connect(m_selectionSnapper, &SelectionSnapper::snapCompleted,
            this, [this](const QRect &finalBounds) {
        m_selectionRect = finalBounds;
        Q_EMIT selectionChanged();
    });

    // Connect bounds changed signal for animation
    connect(m_selectionSnapper, &SelectionSnapper::boundsChanged,
            this, [this](const QRect &newBounds) {
        m_selectionRect = newBounds;
        Q_EMIT selectionChanged();
    });

    // Story 1.8: Connect window event monitoring signals
    connect(m_eventMonitor, &WindowEventMonitor::windowMoved,
            this, [this](int newX, int newY, int newWidth, int newHeight) {
        // Update selection bounds when tracked window moves
        if (!hasSelection()) {
            return;
        }

        // Calculate new selection bounds
        SelectionGeometry geom;
        QRect updated = QRect(newX, newY, newWidth, newHeight);
        updated = geom.clampToScreen(updated);
        if (!geom.isValidSelection(updated)) {
            m_selectionRect = QRect();
        } else {
            m_selectionRect = updated;
        }
        Q_EMIT selectionChanged();
        qDebug("Selection updated to window movement: %dx%d at (%d, %d)",
               newWidth, newHeight, newX, newY);
    });

    connect(m_eventMonitor, &WindowEventMonitor::windowDestroyed,
            this, [this]() {
        // Clear selection when tracked window closes
        qDebug("Tracked window destroyed, clearing selection");
        m_selectionRect = QRect();
        Q_EMIT selectionChanged();
    });

    connect(m_eventMonitor, &WindowEventMonitor::errorOccurred,
            this, [](const QString &error) {
        qWarning("WindowEventMonitor error: %s", qUtf8Printable(error));
    });

    // Story 1.4: Connect post-capture view signal to slot
    connect(this, &SelectionOverlay::openPostCaptureView,
            this, &SelectionOverlay::openPostCaptureAnnotationViewer);
#endif
}

SelectionOverlay::~SelectionOverlay()
{
    close();
}

void SelectionOverlay::show()
{
    if (m_view) {
        m_view->show();
        m_view->raise();
        m_view->activateWindow();
        return;
    }

    // Create full-screen transparent view
    m_view = new QQuickView();

    // Configure view properties
    m_view->setSource(QUrl(QLatin1String("qrc:/overlay/SelectionOverlay.qml")));
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->setClearColor(Qt::transparent);

    // Make full-screen
    QScreen *screen = QGuiApplication::primaryScreen();
    m_view->setGeometry(screen->geometry());

    // Set window flags for frameless full-screen window
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool;
    m_view->setWindowFlags(flags);

    // Expose this object to QML context
    QQmlContext *context = m_view->rootContext();
    context->setContextProperty(QLatin1String("_overlay"), this);

    // Show the window
    m_view->showFullScreen();
    m_view->raise();
    m_view->activateWindow();

#ifdef Q_OS_WIN
    // Story 1.8: Start window event monitoring
    if (m_eventMonitor && !m_eventMonitor->isRunning()) {
        if (m_eventMonitor->start()) {
            qDebug("Window event monitoring started");
        } else {
            qWarning("Failed to start window event monitoring");
        }
    }
#endif

    qDebug("Selection overlay shown (fullscreen transparent window)");
}

void SelectionOverlay::hide()
{
    if (m_view) {
        m_view->hide();
    }
}

void SelectionOverlay::close()
{
    if (m_view) {
        m_view->close();
        delete m_view;
        m_view = nullptr;
    }
    m_selectionRect = QRect();
    m_dragging = false;

#ifdef Q_OS_WIN
    // Story 1.8: Stop window event monitoring
    if (m_eventMonitor && m_eventMonitor->isRunning()) {
        m_eventMonitor->stop();
        qDebug("Window event monitoring stopped");
    }
#endif
}

bool SelectionOverlay::isVisible() const
{
    return m_view && m_view->isVisible();
}

void SelectionOverlay::setAutosaveEnabled(bool enabled)
{
    if (m_autosaveEnabled != enabled) {
        m_autosaveEnabled = enabled;
        Q_EMIT autosaveChanged();
    }
}

void SelectionOverlay::setAutosavePath(const QString &path)
{
    if (m_autosavePath != path) {
        m_autosavePath = path;
        Q_EMIT autosaveChanged();
    }
}

// Story 1.3: Annotation property setters

void SelectionOverlay::setCurrentTool(int tool)
{
    if (m_currentTool != tool) {
        m_currentTool = tool;
        qDebug("Current annotation tool changed to: %d", tool);
        Q_EMIT currentToolChanged();
    }
}

void SelectionOverlay::setCurrentColor(const QColor &color)
{
    if (m_currentColor != color) {
        m_currentColor = color;
        qDebug("Current annotation color changed to: %s", qUtf8Printable(color.name()));
        Q_EMIT currentColorChanged();
    }
}

void SelectionOverlay::setCurrentStrokeWidth(int width)
{
    if (m_currentStrokeWidth != width) {
        m_currentStrokeWidth = width;
        qDebug("Current annotation stroke width changed to: %d", width);
        Q_EMIT currentStrokeWidthChanged();
    }
}

// Story 1.3: Annotation creation methods

void SelectionOverlay::addFreeDrawAnnotation(const QVector<QPoint> &points)
{
    if (points.isEmpty()) {
        qWarning("Cannot add free draw annotation: no points");
        return;
    }

    Annotation annotation(AnnotationTool::FreeDraw);
    annotation.setColor(m_currentColor);
    annotation.setStrokeWidth(m_currentStrokeWidth);
    annotation.setPoints(points);

    // Calculate bounding box from points
    QRect bbox;
    for (const QPoint &pt : points) {
        if (bbox.isNull()) {
            bbox = QRect(pt, QSize(1, 1));
        } else {
            bbox = bbox.united(QRect(pt, QSize(1, 1)));
        }
    }
    annotation.setBoundingBox(bbox);

    m_annotationModel->addAnnotation(annotation);
    qDebug("Added free draw annotation with %d points, bbox: %dx%d at (%d, %d)",
           points.count(), bbox.width(), bbox.height(), bbox.x(), bbox.y());
}

void SelectionOverlay::addBoxAnnotation(const QRect &bounds)
{
    if (bounds.isEmpty()) {
        qWarning("Cannot add box annotation: empty bounds");
        return;
    }

    Annotation annotation(AnnotationTool::Box);
    annotation.setColor(m_currentColor);
    annotation.setStrokeWidth(m_currentStrokeWidth);
    annotation.setBoundingBox(bounds);

    m_annotationModel->addAnnotation(annotation);
    qDebug("Added box annotation: %dx%d at (%d, %d)",
           bounds.width(), bounds.height(), bounds.x(), bounds.y());
}

void SelectionOverlay::addCircleAnnotation(const QRect &bounds)
{
    if (bounds.isEmpty()) {
        qWarning("Cannot add circle annotation: empty bounds");
        return;
    }

    Annotation annotation(AnnotationTool::Circle);
    annotation.setColor(m_currentColor);
    annotation.setStrokeWidth(m_currentStrokeWidth);
    annotation.setBoundingBox(bounds);

    m_annotationModel->addAnnotation(annotation);
    qDebug("Added circle annotation: %dx%d at (%d, %d)",
           bounds.width(), bounds.height(), bounds.x(), bounds.y());
}

void SelectionOverlay::undoLastAnnotation()
{
    if (m_annotationModel) {
        m_annotationModel->undoLast();
        qDebug("Undo last annotation");
    }
}

void SelectionOverlay::mousePress(const QPoint &pos)
{
    qDebug("Mouse press at: (%d, %d)", pos.x(), pos.y());
    m_dragStart = pos;
    m_dragging = true;
    m_selectionRect = QRect();
    Q_EMIT selectionChanged();
}

void SelectionOverlay::mouseMove(const QPoint &pos)
{
    if (!m_dragging) {
        return;
    }

    qDebug("Mouse move to: (%d, %d)", pos.x(), pos.y());

    // Update selection rectangle
    SelectionGeometry geom;
    m_selectionRect = geom.createRectangle(m_dragStart, pos);

    // Clamp to screen bounds
    m_selectionRect = geom.clampToScreen(m_selectionRect);

    Q_EMIT selectionChanged();
}

void SelectionOverlay::mouseRelease(const QPoint &pos)
{
    if (!m_dragging) {
        return;
    }

    qDebug("Mouse release at: (%d, %d)", pos.x(), pos.y());

    SelectionGeometry geom;
    m_selectionRect = geom.createRectangle(m_dragStart, pos);
    m_selectionRect = geom.clampToScreen(m_selectionRect);

    m_dragging = false;

    // Validate selection
    if (geom.isValidSelection(m_selectionRect)) {
        qDebug("Valid selection: %dx%d at (%d, %d)",
               m_selectionRect.width(), m_selectionRect.height(),
               m_selectionRect.x(), m_selectionRect.y());
    } else {
        qDebug("Invalid selection, clearing");
        m_selectionRect = QRect();
    }

    Q_EMIT selectionChanged();
}

void SelectionOverlay::enterPressed()
{
    if (!hasSelection()) {
        qWarning("Enter pressed but no selection");
        return;
    }

    qDebug("Enter pressed - confirming capture");

    // Capture screen using WGC backend
    WGCCapture *capture = WGCCapture::instance();
    QImage image = capture->captureFrame();

    if (image.isNull()) {
        qWarning("Capture failed - image is null");
        Q_EMIT captureAborted();
        close();
        return;
    }

    // Crop to selection region
    image = image.copy(m_selectionRect);

    // Story 1.4: Open post-capture annotation view
    // Pass the raw image and annotation model (will be deep copied in viewer)
    qDebug("Opening post-capture annotation view with %d annotations",
           m_annotationModel ? m_annotationModel->rowCount() : 0);
    Q_EMIT openPostCaptureView(image, m_annotationModel);

    // Task 8: Render annotations onto the captured image for clipboard/autosave
    if (m_annotationModel && m_annotationModel->rowCount() > 0) {
        qDebug("Rendering %d annotations onto captured image", m_annotationModel->rowCount());
        QImage renderedImage = AnnotationRenderer::renderFromModel(image, m_annotationModel);

        // Commit to clipboard
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setImage(renderedImage);

        // Autosave if enabled
        if (m_autosaveEnabled) {
            renderedImage.save(m_autosavePath);
            qDebug("Rendered image saved to: %s", qUtf8Printable(m_autosavePath));
        }

        qDebug("Rendered image committed to clipboard: %dx%d", renderedImage.width(), renderedImage.height());
    } else {
        // No annotations - use original image
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setImage(image);

        // Autosave if enabled
        if (m_autosaveEnabled) {
            image.save(m_autosavePath);
            qDebug("Image saved to: %s", qUtf8Printable(m_autosavePath));
        }

        qDebug("Image committed to clipboard: %dx%d", image.width(), image.height());
    }

    Q_EMIT captureConfirmed(image);
    close();
}

void SelectionOverlay::escapePressed()
{
    qDebug("Escape pressed - aborting capture");
    m_selectionRect = QRect();
    m_dragging = false;
    Q_EMIT selectionChanged();
    Q_EMIT captureAborted();
    close();
}

#ifdef Q_OS_WIN
void SelectionOverlay::updateTargetUnderCursor(const QPoint &pos)
{
    if (!m_windowDetector || !m_targetingEnabled || !m_view) {
        return;
    }

    // Find window under cursor
    HWND hwnd = m_windowDetector->windowAtPoint(pos);
    if (!hwnd) {
        // No window under cursor, clear highlight
        updateTargetHighlight(QRect());
        return;
    }

    // Get window bounds
    QRect windowBounds = m_windowDetector->getWindowBounds(hwnd);
    if (windowBounds.isEmpty()) {
        return;  // Invalid bounds
    }

    // Update QML TargetHighlighter to show the window bounds
    updateTargetHighlight(windowBounds);
}

void SelectionOverlay::updateTargetHighlight(const QRect &bounds)
{
    if (!m_view) {
        return;
    }

    // Access QML root object
    QQuickItem *rootItem = m_view->rootObject();
    if (!rootItem) {
        return;
    }

    // Find TargetHighlighter by object name
    QQuickItem *highlighter = rootItem->findChild<QQuickItem*>(QLatin1String("targetHighlighter"));
    if (!highlighter) {
        return;
    }

    if (bounds.isEmpty()) {
        // Clear highlight
        QMetaObject::invokeMethod(highlighter, "clear");
    } else {
        // Show highlight with window bounds
        QMetaObject::invokeMethod(highlighter, "highlight",
                                  Q_ARG(QVariant, QVariant::fromValue(bounds)),
                                  Q_ARG(QVariant, QVariant::fromValue(QLatin1String("Window"))));
    }
}

void SelectionOverlay::snapToWindowBounds(const QRect &windowBounds)
{
    if (!m_selectionSnapper || !m_targetingEnabled) {
        return;
    }

    // Use the current selection or create a default one
    QRect currentBounds = m_selectionRect;
    if (currentBounds.isEmpty()) {
        // Start from center of target window
        QPoint center = windowBounds.center();
        currentBounds = QRect(center, center);
    }

    // Animate snap to window bounds
    m_selectionSnapper->snapTo(currentBounds, windowBounds);

#ifdef Q_OS_WIN
    // Story 1.8: Start monitoring the selected window for movement
    if (m_eventMonitor && m_eventMonitor->isRunning()) {
        // Get the HWND from the window bounds (if we have WindowDetector)
        if (m_windowDetector) {
            HWND hwnd = m_windowDetector->windowAtPoint(windowBounds.center());
            if (hwnd) {
                m_eventMonitor->setTrackedWindow(hwnd);
                qDebug("Now tracking window movement for HWND %p", hwnd);
            }
        }
    }
#endif
}

void SelectionOverlay::refreshTargets()
{
#ifdef Q_OS_WIN
    if (!m_windowDetector) {
        return;
    }

    qDebug("Refreshing target windows");

    // If we have a selection, try to re-detect the target
    if (hasSelection()) {
        QPoint center = m_selectionRect.center();

        // Find window at selection center
        HWND hwnd = m_windowDetector->windowAtPoint(center);
        if (hwnd) {
            // Get current window bounds
            QRect newBounds = m_windowDetector->getWindowBounds(hwnd);
            if (!newBounds.isEmpty()) {
                // Update selection to current window position
                m_selectionRect = newBounds;
                Q_EMIT selectionChanged();
                qDebug("Selection refreshed to new window bounds: %dx%d at (%d, %d)",
                       newBounds.width(), newBounds.height(), newBounds.x(), newBounds.y());

                // Update tracked window in monitor
                if (m_eventMonitor && m_eventMonitor->isRunning()) {
                    m_eventMonitor->setTrackedWindow(hwnd);
                }
                return;
            }
        }

        // Window no longer exists - clear selection
        qDebug("Target window no longer found, clearing selection");
        m_selectionRect = QRect();
        Q_EMIT selectionChanged();
    }
#endif
}

// Story 1.4: CRITICAL #1 Fix - Open post-capture annotation viewer
void SelectionOverlay::openPostCaptureAnnotationViewer(const QImage &image, AnnotationListModel *annotations)
{
#ifdef Q_OS_WIN
    if (image.isNull()) {
        qWarning("Cannot open post-capture viewer: image is null");
        return;
    }

    qDebug("Opening post-capture annotation viewer with %d annotations",
           annotations ? annotations->rowCount() : 0);

    // Create and show post-capture viewer
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(image, annotations, nullptr);
    viewer->show();
    viewer->raise();
    viewer->activateWindow();

    qDebug("Post-capture annotation viewer opened successfully");
#endif
}

#include "moc_SelectionOverlay.cpp"
