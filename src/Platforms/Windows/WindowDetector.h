/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Detector for Windows UI Element Detection
 *  Story 1.2 - Selection Targeting + Refinement
 */

#pragma once

#ifdef Q_OS_WIN

#include <QObject>
#include <QRect>
#include <QList>
#include <QPoint>
#include <windows.h>

/**
 * @brief Detects and queries windows on Windows for selection targeting
 *
 * This class provides window enumeration and detection capabilities:
 * - Enumerate all visible top-level windows
 * - Get window bounds
 * - Find window under cursor
 * - Filter by visibility and window styles
 */
class WindowDetector : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit WindowDetector(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~WindowDetector();

    /**
     * @brief Enumerate all visible top-level windows
     * @return List of window handles (HWNDs)
     */
    QList<HWND> enumerateVisibleWindows();

    /**
     * @brief Get the bounding rectangle of a window
     * @param hwnd Window handle
     * @return Bounding rectangle in screen coordinates
     */
    QRect getWindowBounds(HWND hwnd);

    /**
     * @brief Find window under a specific point
     * @param point Screen coordinates
     * @return Window handle at that point (or nullptr if none)
     */
    HWND windowAtPoint(const QPoint &point);

    /**
     * @brief Check if a window is visible
     * @param hwnd Window handle
     * @return true if window is visible
     */
    bool isWindowVisible(HWND hwnd);

    /**
     * @brief Translate point to correct screen (multi-monitor support)
     * @param point Screen coordinates
     * @return Point clamped to valid screen bounds
     */
    QPoint translateToScreen(const QPoint &point);

private:
    Q_DISABLE_COPY(WindowDetector)
};

#endif // Q_OS_WIN
