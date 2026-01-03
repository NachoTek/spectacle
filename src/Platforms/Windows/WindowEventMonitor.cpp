/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Event Monitor for Windows UI Element Tracking
 *  Story 1.8 - Window Movement Detection & Overlay Persistence
 */

#include "WindowEventMonitor.h"

#ifdef Q_OS_WIN

#include <QLoggingCategory>
#include <QMetaObject>
#include <QMutexLocker>
#include <QPointer>

Q_LOGGING_CATEGORY(LOG_WINDOWEVENTMONITOR, "spectacle.platforms.windows.windoweventmonitor")

// Global instance pointer for Win32 callback
static QPointer<WindowEventMonitor> g_instance;

WindowEventMonitor::WindowEventMonitor(QObject *parent)
    : QObject(parent)
    , m_moveHook(nullptr)
    , m_destroyHook(nullptr)
    , m_trackedHwnd(nullptr)
    , m_running(false)
    , m_hasReceivedEvents(false)
    , m_debounceTimer(new QTimer(this))
{
    // Debounce timer: ignore rapid movements within 50ms
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(50);

    connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
        QRect pendingBounds;
        bool hasEvents;
        {
            QMutexLocker locker(&m_mutex);
            pendingBounds = m_pendingRect;
            hasEvents = m_hasReceivedEvents;
            m_pendingRect = QRect();
        }

        // Only emit if we've received at least one valid event
        // This distinguishes "no events yet" (false) from "invalid rect" (true but invalid)
        if (!hasEvents || !pendingBounds.isValid()) {
            return;
        }

        Q_EMIT windowMoved(pendingBounds.x(),
                           pendingBounds.y(),
                           pendingBounds.width(),
                           pendingBounds.height());
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
    m_pendingRect = QRect();
    m_hasReceivedEvents = false;  // Reset event tracking

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

HWND WindowEventMonitor::trackedWindow() const
{
    QMutexLocker locker(&m_mutex);
    return m_trackedHwnd;
}

void WindowEventMonitor::setTrackedWindow(HWND hwnd)
{
    QMutexLocker locker(&m_mutex);

    if (hwnd) {
        // Validate that hwnd is a top-level window, not a child window or control
        // GetAncestor with GA_ROOT returns the root parent window
        HWND rootHwnd = GetAncestor(hwnd, GA_ROOT);
        if (rootHwnd != hwnd) {
            qCWarning(LOG_WINDOWEVENTMONITOR) << "Refusing to track child window or control:"
                                               << hwnd << "(root:" << rootHwnd << ")";
            Q_EMIT errorOccurred(QStringLiteral("Cannot track child windows or controls"));
            return;
        }
    }

    m_trackedHwnd = hwnd;
    m_pendingRect = QRect();
    m_hasReceivedEvents = false;  // Reset event tracking for new window
    qCDebug(LOG_WINDOWEVENTMONITOR) << "Tracking window:" << hwnd;
}

bool WindowEventMonitor::initializeHook()
{
    // Monitor all threads in the current desktop
    DWORD threadId = 0;

    // Set WinEventHook for location change (movement/resize)
    m_moveHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,  // Event: window moved or resized
        EVENT_OBJECT_LOCATIONCHANGE,  // Min/Max range
        nullptr,                        // Hook procedure (null = use callback)
        reinterpret_cast<WINEVENTPROC>(winEventProc),
        0,                              // Process ID (0 = all processes)
        threadId,                       // Thread ID (current thread)
        WINEVENT_OUTOFCONTEXT
    );

    if (!m_moveHook) {
        DWORD error = GetLastError();
        qCWarning(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook failed:" << error;
        Q_EMIT errorOccurred(QStringLiteral("SetWinEventHook failed: %1").arg(error));
        return false;
    }

    m_destroyHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY,          // Event: window destroyed
        EVENT_OBJECT_DESTROY,          // Min/Max range
        nullptr,                        // Hook procedure (null = use callback)
        reinterpret_cast<WINEVENTPROC>(winEventProc),
        0,                              // Process ID (0 = all processes)
        threadId,                       // Thread ID (all threads)
        WINEVENT_OUTOFCONTEXT
    );

    if (!m_destroyHook) {
        DWORD error = GetLastError();
        qCWarning(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook failed for destroy:" << error;
        Q_EMIT errorOccurred(QStringLiteral("SetWinEventHook failed for destroy: %1").arg(error));
        cleanupHook();
        return false;
    }

    qCDebug(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook registered successfully";
    return true;
}

void WindowEventMonitor::cleanupHook()
{
    if (m_moveHook) {
        UnhookWinEvent(m_moveHook);
        m_moveHook = nullptr;
        qCDebug(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook (move) unregistered";
    }

    if (m_destroyHook) {
        UnhookWinEvent(m_destroyHook);
        m_destroyHook = nullptr;
        qCDebug(LOG_WINDOWEVENTMONITOR) << "SetWinEventHook (destroy) unregistered";
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

    if (!g_instance) {
        return;
    }

    QPointer<WindowEventMonitor> instance = g_instance;
    QMetaObject::invokeMethod(instance, [instance, event, hwnd]() {
        if (!instance) {
            return;
        }
        instance->handleWindowEvent(event, hwnd);
    }, Qt::QueuedConnection);
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

    // Filter out minimized or hidden windows
    // IsIconic: Returns TRUE if window is minimized
    // IsWindowVisible: Returns FALSE if window is hidden (WS_VISIBLE not set)
    if (IsIconic(hwnd) || !IsWindowVisible(hwnd)) {
        qCDebug(LOG_WINDOWEVENTMONITOR) << "Ignoring event for minimized or hidden window";
        return;
    }

    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        // Window moved or resized - debounce rapid events
        RECT rect;
        if (!GetWindowRect(hwnd, &rect)) {
            DWORD error = GetLastError();
            qCWarning(LOG_WINDOWEVENTMONITOR) << "GetWindowRect failed for tracked window:" << error;
            Q_EMIT errorOccurred(QStringLiteral("GetWindowRect failed: %1").arg(error));
            return;
        }

        m_pendingRect = QRect(rect.left,
                              rect.top,
                              rect.right - rect.left,
                              rect.bottom - rect.top);
        m_hasReceivedEvents = true;  // Mark that we've received at least one event

        if (!m_debounceTimer->isActive()) {
            // Start debounce timer
            m_debounceTimer->start();
        }
        qCDebug(LOG_WINDOWEVENTMONITOR) << "Window location changed (debounced)";

    } else if (event == EVENT_OBJECT_DESTROY) {
        // Window destroyed
        qCInfo(LOG_WINDOWEVENTMONITOR) << "Tracked window destroyed";
        m_pendingRect = QRect();
        Q_EMIT windowDestroyed();
        m_trackedHwnd = nullptr;
    }
}

#include "moc_WindowEventMonitor.cpp"

#endif // Q_OS_WIN
