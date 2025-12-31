/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Geometry for Screen Capture
 *  Story 1.1 - Rectangle Selection
 */

#pragma once

#include <QObject>
#include <QRect>
#include <QPoint>
#include <QList>
#include <QScreen>

/**
 * @brief Geometry calculations for selection rectangle
 *
 * Handles creation, validation, and manipulation of
 * selection rectangles for screen capture.
 */
class SelectionGeometry : public QObject
{
    Q_OBJECT

public:
    explicit SelectionGeometry(QObject *parent = nullptr);
    ~SelectionGeometry() = default;

    /**
     * @brief Create selection rectangle from start and end points
     * @param start Starting point (mouse press)
     * @param end Ending point (mouse release)
     * @return Normalized rectangle
     */
    QRect createRectangle(const QPoint &start, const QPoint &end) const;

    /**
     * @brief Normalize rectangle (handle negative coordinates)
     * @param rect Input rectangle
     * @return Normalized rectangle with valid bounds
     */
    QRect normalizeRectangle(const QRect &rect) const;

    /**
     * @brief Clamp rectangle to screen bounds
     * @param rect Rectangle to clamp
     * @param screen Screen geometry (uses primary if null)
     * @return Clamped rectangle
     */
    QRect clampToScreen(const QRect &rect, QScreen *screen = nullptr) const;

    /**
     * @brief Check if rectangle is valid for selection
     * @param rect Rectangle to validate
     * @return true if valid (non-zero area, within screen bounds)
     */
    bool isValidSelection(const QRect &rect) const;

    /**
     * @brief Calculate resize handle positions
     * @param rect Selection rectangle
     * @return List of 8 handle positions (corners + edges)
     */
    QList<QPoint> resizeHandlePositions(const QRect &rect) const;

    /**
     * @brief Get resize handle position
     * @param rect Selection rectangle
     * @param handle Handle identifier (0-7)
     * @return Handle position
     */
    QPoint resizeHandlePosition(const QRect &rect, int handle) const;

    /**
     * @brief Resize rectangle from handle
     * @param rect Original rectangle
     * @param handle Handle being dragged
     * @param newPos New position of handle
     * @return Resized rectangle
     */
    QRect resizeFromHandle(const QRect &rect, int handle, const QPoint &newPos) const;

    /**
     * @brief Get handle at position
     * @param rect Selection rectangle
     * @param pos Point to check
     * @param hitRadius Pixel radius for hit testing
     * @return Handle index (0-7) or -1 if no hit
     */
    int handleAtPosition(const QRect &rect, const QPoint &pos, int hitRadius = 10) const;

    /**
     * @brief Resize rectangle from handle with aspect ratio constraint
     * @param rect Original rectangle
     * @param handle Handle being dragged
     * @param newPos New position of handle
     * @param maintainAspectRatio If true, preserve original aspect ratio
     * @return Resized rectangle
     */
    QRect resizeFromHandle(const QRect &rect, int handle, const QPoint &newPos, bool maintainAspectRatio) const;

    /**
     * @brief Update rectangle position by delta (Story 1.8 - Window Movement Tracking)
     * @param rect Original rectangle
     * @param deltaX X position change
     * @param deltaY Y position change
     * @return Updated rectangle (clamped to screen bounds)
     */
    QRect updatePosition(const QRect &rect, int deltaX, int deltaY) const;

    // Handle identifiers
    enum Handles {
        TopLeft = 0,
        Top = 1,
        TopRight = 2,
        Right = 3,
        BottomRight = 4,
        Bottom = 5,
        BottomLeft = 6,
        Left = 7
    };

    static const int HANDLE_COUNT = 8;
};

