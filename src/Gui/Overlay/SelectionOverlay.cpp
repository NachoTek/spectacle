/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Overlay Window
 *  Story 1.1 - Full-Screen Capture Overlay
 */

#include "SelectionOverlay.h"
#include "SelectionGeometry.h"
#include "Platforms/Windows/WGCCapture.h"
#include <QScreen>
#include <QGuiApplication>
#include <QQuickItem>
#include <QQmlContext>
#include <QClipboard>
#include <QApplication>

SelectionOverlay::SelectionOverlay(QObject *parent)
    : QObject(parent)
    , m_view(nullptr)
    , m_dragging(false)
    , m_autosaveEnabled(false)
    , m_autosavePath(u"C:\\temp\\screenshot.png"_s)
{
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
    m_view->setSource(QUrl(u"qrc:/overlay/SelectionOverlay.qml"_s));
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
    context->setContextProperty(u"_overlay"_s, this);

    // Show the window
    m_view->showFullScreen();
    m_view->raise();
    m_view->activateWindow();

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

    // Commit to clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setImage(image);

    qDebug("Image committed to clipboard: %dx%d", image.width(), image.height());

    // Autosave if enabled
    if (m_autosaveEnabled) {
        image.save(m_autosavePath);
        qDebug("Image saved to: %s", qUtf8Printable(m_autosavePath));
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

#include "moc_SelectionOverlay.cpp"
