/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Overlay Window
 *  Story 1.1 - Full-Screen Capture Overlay
 */

#pragma once

#include <QObject>
#include <QQuickView>
#include <QRect>
#include <QImage>

/**
 * @brief Full-screen overlay for screen capture selection
 *
 * Manages the transparent overlay window that appears when
 * Print Screen is pressed, allowing the user to select a region.
 */
class SelectionOverlay : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect selectionRect READ selectionRect NOTIFY selectionChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged)
    Q_PROPERTY(bool resizeHandlesVisible READ resizeHandlesVisible NOTIFY selectionChanged)
    Q_PROPERTY(bool autosaveEnabled READ autosaveEnabled WRITE setAutosaveEnabled NOTIFY autosaveChanged)
    Q_PROPERTY(QString autosavePath READ autosavePath WRITE setAutosavePath NOTIFY autosaveChanged)

public:
    explicit SelectionOverlay(QObject *parent = nullptr);
    ~SelectionOverlay();

    /**
     * @brief Show the overlay window
     */
    void show();

    /**
     * @brief Hide the overlay window
     */
    void hide();

    /**
     * @brief Close overlay and cleanup
     */
    void close();

    /**
     * @brief Check if overlay is visible
     */
    bool isVisible() const;

    /**
     * @brief Get current selection rectangle
     */
    QRect selectionRect() const { return m_selectionRect; }

    /**
     * @brief Check if user has made a selection
     */
    bool hasSelection() const { return !m_selectionRect.isEmpty(); }

    /**
     * @brief Check if resize handles should be visible
     */
    bool resizeHandlesVisible() const { return hasSelection(); }

    /**
     * @brief Get number of resize handles
     */
    int resizeHandleCount() const { return 8; }

    /**
     * @brief Get crosshair cursor state
     */
    Qt::CursorShape cursor() const { return Qt::CrossCursor; }

    // Autosave properties
    bool autosaveEnabled() const { return m_autosaveEnabled; }
    void setAutosaveEnabled(bool enabled);
    QString autosavePath() const { return m_autosavePath; }
    void setAutosavePath(const QString &path);

Q_SIGNALS:
    void selectionChanged();
    void autosaveChanged();
    void captureConfirmed(const QImage &image);
    void captureAborted();

public Q_SLOTS:
    /**
     * @brief Handle mouse press (start selection)
     */
    void mousePress(const QPoint &pos);

    /**
     * @brief Handle mouse move (update selection)
     */
    void mouseMove(const QPoint &pos);

    /**
     * @brief Handle mouse release (end selection)
     */
    void mouseRelease(const QPoint &pos);

    /**
     * @brief Handle Enter key (confirm capture)
     */
    void enterPressed();

    /**
     * @brief Handle Escape key (abort capture)
     */
    void escapePressed();

private:
    QQuickView *m_view;
    QRect m_selectionRect;
    QPoint m_dragStart;
    bool m_dragging;
    bool m_autosaveEnabled;
    QString m_autosavePath;
};

