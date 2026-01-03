/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Event Monitor for Windows UI Element Tracking
 *  Story 1.8 - Window Movement Detection & Overlay Persistence
 */

#pragma once

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QRect>

#ifdef Q_OS_WIN

#include <Windows.h>

// Forward declarations
class QCoreApplication;

/**
 * @brief Win32 window event monitoring using SetWinEventHook
 *
 * Monitors window movement, resize, and closure events in real-time.
 * Emits signals when tracked windows move or close, allowing the
 * overlay to update selection bounds or clear selections.
 */
class WindowEventMonitor : public QObject
{
    Q_OBJECT

public:
    explicit WindowEventMonitor(QObject *parent = nullptr);
    ~WindowEventMonitor();

    /**
     * @brief Start monitoring window events
     * @return true if monitoring started successfully
     */
    bool start();

    /**
     * @brief Stop monitoring window events
     */
    void stop();

    /**
     * @brief Check if monitor is running
     */
    bool isRunning() const;

    /**
     * @brief Set the window handle to track
     * @param hwnd Window handle to monitor for movement/closure
     */
    void setTrackedWindow(HWND hwnd);

    /**
     * @brief Get the currently tracked window handle
     * Thread-safe: locks mutex before accessing m_trackedHwnd
     */
    HWND trackedWindow() const;

Q_SIGNALS:
    /**
     * @brief Emitted when tracked window moves or resizes
     * @param newX New X coordinate
     * @param newY New Y coordinate
     * @param newWidth New width
     * @param newHeight New height
     */
    void windowMoved(int newX, int newY, int newWidth, int newHeight);

    /**
     * @brief Emitted when tracked window is destroyed
     */
    void windowDestroyed();

    /**
     * @brief Emitted when monitoring error occurs
     * @param error Error message
     */
    void errorOccurred(const QString &error);

private:
    friend class WindowEventMonitorTest;

    bool initializeHook();
    void cleanupHook();
    static void CALLBACK winEventProc(HWINEVENTHOOK hWinEventHook,
                                       DWORD event,
                                       HWND hwnd,
                                       LONG idObject,
                                       LONG idChild,
                                       DWORD dwEventThread,
                                       DWORD dwmsEventTime);

    void handleWindowEvent(DWORD event, HWND hwnd);

    HWINEVENTHOOK m_moveHook;
    HWINEVENTHOOK m_destroyHook;
    HWND m_trackedHwnd;
    bool m_running;

    // Thread-safe event dispatch
    mutable QMutex m_mutex;

    // Movement debounce timer
    QTimer *m_debounceTimer;
    QRect m_pendingRect;
    bool m_hasReceivedEvents;  // Track if any events received (distinguish from invalid rect)
};

#endif // Q_OS_WIN
