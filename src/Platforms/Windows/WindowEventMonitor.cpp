/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Event Monitor for Windows UI Element Tracking
 *  Story 1.8 - Window Movement Detection & Overlay Persistence
 */

#include "WindowEventMonitor.h"

#ifdef Q_OS_WIN

#include <QLoggingCategory>
#include <QCoreApplication>
#include <QMutexLocker>

Q_LOGGING_CATEGORY(LOG_WINDOWEVENTMONITOR, "spectacle.platforms.windows.windoweventmonitor")

// Global instance pointer for Win32 callback
static WindowEventMonitor* g_instance = nullptr;

WindowEventMonitor::WindowEventMonitor(QObject *parent)
    : QObject(parent)
    , m_eventHook(nullptr)
    , m_trackedHwnd(nullptr)
    , m_running(false)
    , m_debounceTimer(new QTimer(this))
{
    // Debounce timer: ignore rapid movements within 50ms
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(50);

    connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
        QMutexLocker locker(&m_mutex);
        if (!m_pendingDelta.isNull()) {
            // Get current window bounds
            RECT rect;
            if (GetWindowRect(m_trackedHwnd, &rect)) {
                Q_EMIT windowMoved(rect.left, rect.top,
                                 rect.right - rect.left,
                                 rect.bottom - rect.top);
            }
            m_pendingDelta = QPoint();
        }
    });
}

WindowEventMonitor::~WindowEventMonitor()
{
    stop();
    g_instance = nullptr;
}

bool WindowEventMonitor::start()
{
    QMutexLocker locker(&m_mutex);

    if (m_running) {
        qCWarning(LOG_WINDOWEVENTMONITOR) << "Window event monitor already running";
        return true;
    }

    g_instance = this;

    if (!initializeHook()) {
        qCWarning(LOG_WINDOWEVENTMONITOR) << "Failed to initialize event hook";
        g_instance = nullptr;
        return false;
    }

    m_running = true;
    qCInfo(LOG_WINDOWEVENTMONITOR) << "Window event monitor started";
    return true;
}

void WindowEventMonitor::stop()
{
    QMutexLocker locker(&m_mutex);

    if (!m_running) {
        return;
    }

    cleanupHook();

    m_running = false;
    m_trackedHwnd = nullptr;

    if (m_debounceTimer->isActive()) {
        m_debounceTimer->stop();
    }

    qCInfo(LOG_WINDOWEVENTMONITOR) << "Window event monitor stopped";
}

bool WindowEventMonitor::isRunning() const
{
    QMutexLocker locker(&m_mutex);
    return m_running;
}

void WindowEventMonitor::setTrackedWindow(HWND hwnd)
{
    QMutexLocker locker(&m_mutex);
    m_trackedHwnd = hwnd;
    qCDebug(LOG_WINDOWEVENTMONITOR) << "Tracking window:" << hwnd;
}

bool WindowEventMonitor::initializeHook()
{
    // Get message window thread ID (use current thread)
    DWORD threadId = GetCurrentThreadId();

    // Set WinEventHook for location change (movement/resize)
    m_eventHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,  // Event: window moved or resized
        EVENT_OBJECT_LOCATIONCHANGE,  // Min/Max range
        nullptr,                        // Hook procedure (null = use callback)
        reinterpret_cast<WINEVENTPROC>(winEventProc),
        0,                              // Process ID (0 = all processes)
        threadId,                       // Thread ID (current thread)
        WINEVENT_OUTOFCONTEXT
    );

    if (!m_eventHook) {
        DWORD error = GetLastError();
        qCWarning(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook failed:" << error;
        return false;
    }

    qCDebug(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook registered successfully";
    return true;
}

void WindowEventMonitor::cleanupHook()
{
    if (m_eventHook) {
        UnhookWinEvent(m_eventHook);
        m_eventHook = nullptr;
        qCDebug(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook unregistered";
    }
}

void CALLBACK WindowEventMonitor::winEventProc(HWINEVENTHOOK hWinEventHook,
                                                DWORD event,
                                                HWND hwnd,
                                                LONG idObject,
                                                LONG idChild,
                                                DWORD dwEventThread,
                                                DWORD dwmsEventTime)
{
    Q_UNUSED(hWinEventHook)
    Q_UNUSED(idObject)
    Q_UNUSED(idChild)
    Q_UNUSED(dwEventThread)
    Q_UNUSED(dwmsEventTime)

    if (g_instance) {
        g_instance->handleWindowEvent(event, hwnd);
    }
}

void WindowEventMonitor::handleWindowEvent(DWORD event, HWND hwnd)
{
    QMutexLocker locker(&m_mutex);

    if (!m_running || !m_trackedHwnd) {
        return;
    }

    // Check if this is our tracked window
    if (hwnd != m_trackedHwnd) {
        return;
    }

    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        // Window moved or resized - debounce rapid events
        if (m_debounceTimer->isActive()) {
            // Timer already running, will emit latest bounds when it fires
            return;
        }

        // Start debounce timer
        m_debounceTimer->start();
        qCDebug(LOG_WINDOWEVENTMONITOR) << "Window location changed (debounced)";

    } else if (event == EVENT_OBJECT_DESTROY) {
        // Window destroyed
        qCInfo(LOG_WINDOWEVENTMONITOR) << "Tracked window destroyed";
        Q_EMIT windowDestroyed();
        m_trackedHwnd = nullptr;
    }
}

#include "moc_WindowEventMonitor.cpp"

#endif // Q_OS_WIN
