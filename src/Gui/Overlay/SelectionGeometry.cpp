/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Geometry for Screen Capture
 *  Story 1.1 - Rectangle Selection
 */

#include "SelectionGeometry.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

SelectionGeometry::SelectionGeometry(QObject *parent)
    : QObject(parent)
{
}

QRect SelectionGeometry::createRectangle(const QPoint &start, const QPoint &end) const
{
    QRect rect(start, end);
    return normalizeRectangle(rect);
}

QRect SelectionGeometry::normalizeRectangle(const QRect &rect) const
{
    // Normalize to ensure width/height are positive
    QRect normalized = rect.normalized();

    // Handle negative coordinates by clamping to 0
    if (normalized.left() < 0) {
        normalized.setLeft(0);
    }
    if (normalized.top() < 0) {
        normalized.setTop(0);
    }

    return normalized;
}

QRect SelectionGeometry::clampToScreen(const QRect &rect, QScreen *screen) const
{
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    if (!screen) {
        qWarning("No screen available for clamping");
        return rect;
    }

    QRect screenGeom = screen->geometry();
    QRect clamped = rect;

    // Clamp left/top to screen bounds
    if (clamped.left() < screenGeom.left()) {
        clamped.setLeft(screenGeom.left());
    }
    if (clamped.top() < screenGeom.top()) {
        clamped.setTop(screenGeom.top());
    }

    // Clamp right/bottom to screen bounds
    if (clamped.right() > screenGeom.right()) {
        clamped.setRight(screenGeom.right());
    }
    if (clamped.bottom() > screenGeom.bottom()) {
        clamped.setBottom(screenGeom.bottom());
    }

    return clamped;
}

bool SelectionGeometry::isValidSelection(const QRect &rect) const
{
    // Must have positive area
    if (rect.width() <= 0 || rect.height() <= 0) {
        return false;
    }

    // Must be within screen bounds
    QRect screenGeom = QGuiApplication::primaryScreen()->geometry();
    if (!screenGeom.contains(rect)) {
        // Allow partial overlap for multi-monitor scenarios
        // but must have some intersection
        if (!screenGeom.intersects(rect)) {
            return false;
        }
    }

    return true;
}

QList<QPoint> SelectionGeometry::resizeHandlePositions(const QRect &rect) const
{
    QList<QPoint> handles;

    for (int i = 0; i < HANDLE_COUNT; ++i) {
        handles.append(resizeHandlePosition(rect, i));
    }

    return handles;
}

QPoint SelectionGeometry::resizeHandlePosition(const QRect &rect, int handle) const
{
    switch (handle) {
        case TopLeft:
            return rect.topLeft();
        case Top:
            return QPoint(rect.center().x(), rect.top());
        case TopRight:
            return rect.topRight();
        case Right:
            return QPoint(rect.right(), rect.center().y());
        case BottomRight:
            return rect.bottomRight();
        case Bottom:
            return QPoint(rect.center().x(), rect.bottom());
        case BottomLeft:
            return rect.bottomLeft();
        case Left:
            return QPoint(rect.left(), rect.center().y());
        default:
            qWarning("Invalid handle index: %d", handle);
            return QPoint();
    }
}

QRect SelectionGeometry::resizeFromHandle(const QRect &rect, int handle, const QPoint &newPos) const
{
    QRect result = rect;

    switch (handle) {
        case TopLeft:
            result.setTopLeft(newPos);
            break;
        case Top:
            result.setTop(newPos.y());
            break;
        case TopRight:
            result.setTopRight(newPos);
            break;
        case Right:
            result.setRight(newPos.x());
            break;
        case BottomRight:
            result.setBottomRight(newPos);
            break;
        case Bottom:
            result.setBottom(newPos.y());
            break;
        case BottomLeft:
            result.setBottomLeft(newPos);
            break;
        case Left:
            result.setLeft(newPos.x());
            break;
        default:
            qWarning("Invalid handle index for resize: %d", handle);
            break;
    }

    return normalizeRectangle(result);
}

QRect SelectionGeometry::resizeFromHandle(const QRect &rect, int handle, const QPoint &newPos, bool maintainAspectRatio) const
{
    if (!maintainAspectRatio) {
        // No aspect ratio constraint, use simple resize
        return resizeFromHandle(rect, handle, newPos);
    }

    // Preserve original aspect ratio
    double aspectRatio = static_cast<double>(rect.width()) / rect.height();
    QRect result = rect;

    switch (handle) {
        case TopLeft:
            // Adjust both top and left, preserving aspect ratio
            {
                int dx = newPos.x() - rect.left();
                int dy = newPos.y() - rect.top();

                // Use the dimension that changed more
                if (qAbs(dx) > qAbs(dy)) {
                    // Horizontal change dominant, constrain vertical
                    int newHeight = static_cast<int>((rect.right() - newPos.x()) / aspectRatio);
                    result.setTopLeft(QPoint(newPos.x(), rect.bottom() - newHeight));
                } else {
                    // Vertical change dominant, constrain horizontal
                    int newWidth = static_cast<int>((rect.bottom() - newPos.y()) * aspectRatio);
                    result.setTopLeft(QPoint(rect.right() - newWidth, newPos.y()));
                }
            }
            break;

        case TopRight:
            {
                int dx = rect.right() - newPos.x();
                int dy = newPos.y() - rect.top();

                if (qAbs(dx) > qAbs(dy)) {
                    int newHeight = static_cast<int>((newPos.x() - rect.left()) / aspectRatio);
                    result.setTopRight(QPoint(newPos.x(), rect.bottom() - newHeight));
                } else {
                    int newWidth = static_cast<int>((rect.bottom() - newPos.y()) * aspectRatio);
                    result.setTopRight(QPoint(rect.left() + newWidth, newPos.y()));
                }
            }
            break;

        case BottomLeft:
            {
                int dx = newPos.x() - rect.left();
                int dy = rect.bottom() - newPos.y();

                if (qAbs(dx) > qAbs(dy)) {
                    int newHeight = static_cast<int>((rect.right() - newPos.x()) / aspectRatio);
                    result.setBottomLeft(QPoint(newPos.x(), newPos.y() - newHeight));
                } else {
                    int newWidth = static_cast<int>((newPos.y() - rect.top()) * aspectRatio);
                    result.setBottomLeft(QPoint(rect.right() - newWidth, newPos.y()));
                }
            }
            break;

        case BottomRight:
            {
                int dx = rect.right() - newPos.x();
                int dy = rect.bottom() - newPos.y();

                if (qAbs(dx) > qAbs(dy)) {
                    int newHeight = static_cast<int>((newPos.x() - rect.left()) / aspectRatio);
                    result.setBottomRight(QPoint(newPos.x(), rect.top() + newHeight));
                } else {
                    int newWidth = static_cast<int>((newPos.y() - rect.top()) * aspectRatio);
                    result.setBottomRight(QPoint(rect.left() + newWidth, newPos.y()));
                }
            }
            break;

        case Top:
            {
                // Moving top edge: adjust height, then width based on aspect ratio
                int newHeight = rect.bottom() - newPos.y();
                qint64 newWidth64 = static_cast<qint64>(newHeight) * static_cast<qint64>(aspectRatio);
                int newWidth = static_cast<int>(qBound<qint64>(0, newWidth64, INT_MAX));
                int widthDelta = newWidth - rect.width();

                result.setTop(newPos.y());
                result.setLeft(rect.left() - widthDelta / 2);
                result.setRight(rect.right() + widthDelta / 2);
            }
            break;

        case Bottom:
            {
                int newHeight = newPos.y() - rect.top();
                qint64 newWidth64 = static_cast<qint64>(newHeight) * static_cast<qint64>(aspectRatio);
                int newWidth = static_cast<int>(qBound<qint64>(0, newWidth64, INT_MAX));
                int widthDelta = newWidth - rect.width();

                result.setBottom(newPos.y());
                result.setLeft(rect.left() - widthDelta / 2);
                result.setRight(rect.right() + widthDelta / 2);
            }
            break;

        case Left:
            {
                int newWidth = rect.right() - newPos.x();
                qint64 newHeight64 = static_cast<qint64>(newWidth) * 1000 / static_cast<qint64>(aspectRatio * 1000);
                int newHeight = static_cast<int>(qBound<qint64>(0, newHeight64, INT_MAX));
                int heightDelta = newHeight - rect.height();

                result.setLeft(newPos.x());
                result.setTop(rect.top() - heightDelta / 2);
                result.setBottom(rect.bottom() + heightDelta / 2);
            }
            break;

        case Right:
            {
                int newWidth = newPos.x() - rect.left();
                qint64 newHeight64 = static_cast<qint64>(newWidth) * 1000 / static_cast<qint64>(aspectRatio * 1000);
                int newHeight = static_cast<int>(qBound<qint64>(0, newHeight64, INT_MAX));
                int heightDelta = newHeight - rect.height();

                result.setRight(newPos.x());
                result.setTop(rect.top() - heightDelta / 2);
                result.setBottom(rect.bottom() + heightDelta / 2);
            }
            break;

        default:
            qWarning("Invalid handle index for resize: %d", handle);
            break;
    }

    return normalizeRectangle(result);
}

int SelectionGeometry::handleAtPosition(const QRect &rect, const QPoint &pos, int hitRadius) const
{
    QList<QPoint> handles = resizeHandlePositions(rect);

    for (int i = 0; i < handles.size(); ++i) {
        int dx = handles[i].x() - pos.x();
        int dy = handles[i].y() - pos.y();
        int distance = static_cast<int>(qSqrt(dx * dx + dy * dy));

        if (distance <= hitRadius) {
            return i;
        }
    }

    return -1; // No handle hit
}

QRect SelectionGeometry::updatePosition(const QRect &rect, int deltaX, int deltaY) const
{
    // Create new rectangle shifted by delta
    QRect updated = rect.translated(deltaX, deltaY);

    // Clamp to screen bounds to prevent selection from going off-screen
    updated = clampToScreen(updated);

    return updated;
}

