/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Detector for Windows UI Element Detection
 *  Story 1.2 - Selection Targeting + Refinement
 */

#include "WindowDetector.h"

#ifdef Q_OS_WIN

#include <QCursor>
#include <QGuiApplication>
#include <QLoggingCategory>

// Windows API
#include <dwmapi.h>

Q_LOGGING_CATEGORY(LOG_WINDOWDETECTOR, "spectacle.platforms.windows.windowdetector")

// Callback for EnumWindows - store visible windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    // Filter: Only visible windows
    if (!IsWindowVisible(hwnd)) {
        return TRUE; // Continue enumeration
    }

    // Filter: Skip cloaked windows (Windows 8+)
    DWORD cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
        return TRUE; // Continue enumeration
    }

    // Filter: Only top-level windows (no child windows)
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (style & WS_CHILD) {
        return TRUE; // Continue enumeration
    }

    // Add to list
    QList<HWND> *windows = reinterpret_cast<QList<HWND>*>(lParam);
    windows->append(hwnd);

    return TRUE; // Continue enumeration
}

WindowDetector::WindowDetector(QObject *parent)
    : QObject(parent)
{
}

WindowDetector::~WindowDetector()
{
}

QList<HWND> WindowDetector::enumerateVisibleWindows()
{
    QList<HWND> windows;

    // Use Win32 EnumWindows API
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

    qCDebug(LOG_WINDOWDETECTOR) << "Found" << windows.size() << "visible windows";

    return windows;
}

QRect WindowDetector::getWindowBounds(HWND hwnd)
{
    if (!IsWindow(hwnd)) {
        qCWarning(LOG_WINDOWDETECTOR) << "Invalid window handle";
        return QRect();
    }

    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        return QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    }

    qCWarning(LOG_WINDOWDETECTOR) << "Failed to get window rect";
    return QRect();
}

HWND WindowDetector::windowAtPoint(const QPoint &point)
{
    // Convert Qt point to Win32 POINT
    POINT winPoint;
    winPoint.x = point.x();
    winPoint.y = point.y();

    // Use WindowFromPoint API
    HWND hwnd = WindowFromPoint(winPoint);

    return hwnd;
}

bool WindowDetector::isWindowVisible(HWND hwnd)
{
    if (!IsWindow(hwnd)) {
        return false;
    }

    return IsWindowVisible(hwnd);
}

QPoint WindowDetector::translateToScreen(const QPoint &point)
{
    // Use Qt's multi-monitor support
    QScreen *screen = QGuiApplication::screenAt(point);

    if (!screen) {
        // Point is outside all screens, clamp to primary screen
        screen = QGuiApplication::primaryScreen();
    }

    if (!screen) {
        // No screens available (shouldn't happen)
        qCWarning(LOG_WINDOWDETECTOR) << "No screens available for point translation";
        return point;
    }

    QRect screenGeometry = screen->geometry();

    // Validate screen geometry before clamping
    if (screenGeometry.isEmpty() || screenGeometry.width() <= 1 || screenGeometry.height() <= 1) {
        qCWarning(LOG_WINDOWDETECTOR) << "Invalid screen geometry";
        return point;
    }

    // Clamp point to screen bounds (ensure right() - 1 doesn't underflow)
    int maxX = qMax(screenGeometry.left(), screenGeometry.right() - 1);
    int maxY = qMax(screenGeometry.top(), screenGeometry.bottom() - 1);

    int x = qBound(screenGeometry.left(), point.x(), maxX);
    int y = qBound(screenGeometry.top(), point.y(), maxY);

    return QPoint(x, y);
}

#endif // Q_OS_WIN
