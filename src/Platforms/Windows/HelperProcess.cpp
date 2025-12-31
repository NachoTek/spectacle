/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Helper Process for Windows Global Hotkey Handling
 *  Story 1.1 - ATDD Implementation - GREEN Phase
 */

#include "HelperProcess.h"
#include <QCoreApplication>

// Global pointer for window procedure
static HelperProcess* g_instance = nullptr;

HelperProcess* HelperProcess::instance()
{
    static HelperProcess instance;
    return &instance;
}

HelperProcess::HelperProcess(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_messageWindow(nullptr)
    , m_instance(GetModuleHandle(nullptr))
{
    g_instance = this;
}

HelperProcess::~HelperProcess()
{
    stop();
    g_instance = nullptr;
}

bool HelperProcess::createMessageWindow()
{
    // Register window class
    static const QString className = QLatin1String("SpectacleHotkeyWindow");

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_instance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = (LPCWSTR)className.utf16();
    wc.hIconSm = nullptr;

    if (!RegisterClassExW(&wc)) {
        qWarning("Failed to register window class: %lu", GetLastError());
        return false;
    }

    // Create hidden message window
    m_messageWindow = CreateWindowExW(
        0,                              // dwExStyle
        (LPCWSTR)className.utf16(),      // lpClassName
        L"SpectacleHotkey",              // lpWindowName
        0,                               // dwStyle
        0, 0, 0, 0,                     // x, y, width, height
        nullptr,                         // hWndParent
        nullptr,                         // hMenu
        m_instance,                      // hInstance
        nullptr                          // lpParam
    );

    if (!m_messageWindow) {
        qWarning("Failed to create message window: %lu", GetLastError());
        return false;
    }

    return true;
}

LRESULT CALLBACK HelperProcess::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_HOTKEY && g_instance) {
        int keyCode = HIWORD(lParam); // Extended key information
        g_instance->Q_EMIT hotkeyPressed(keyCode);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool HelperProcess::registerGlobalHotkey(int keyCode)
{
    if (!m_messageWindow && !createMessageWindow()) {
        return false;
    }

    // Check if already registered
    if (m_registeredKeys.contains(keyCode)) {
        qWarning("Hotkey 0x%X already registered", keyCode);
        return true;
    }

    // Register with unique atom ID
    int atom = GlobalAddAtomW((LPCWSTR)QString::fromUtf8("ATOM_0x%1").arg(keyCode, 0, 16).utf16());
    if (atom == 0) {
        qWarning("Failed to create atom: %lu", GetLastError());
        return false;
    }

    BOOL result = RegisterHotKey(m_messageWindow, atom, 0, keyCode);
    if (!result) {
        DWORD error = GetLastError();
        qWarning("Failed to register hotkey 0x%X: %lu", keyCode, error);

        // Check for conflicts (ERROR_HOTKEY_ALREADY_REGISTERED = 1409)
        if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
            qWarning("Hotkey 0x%X conflicts with existing application", keyCode);
        }

        GlobalDeleteAtom(atom);
        return false;
    }

    m_registeredKeys.insert(keyCode);
    m_hotkeyAtoms.insert(keyCode, atom);

    qDebug("Successfully registered hotkey 0x%X with atom %d", keyCode, atom);
    return true;
}

bool HelperProcess::isHotkeyRegistered(int keyCode) const
{
    return m_registeredKeys.contains(keyCode);
}

void HelperProcess::simulateHotkeyPress(int keyCode)
{
    if (m_registeredKeys.contains(keyCode)) {
        Q_EMIT hotkeyPressed(keyCode);
    }
}

bool HelperProcess::registerAutoStart()
{
    // Open Registry Run key
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        qWarning("Failed to open Registry Run key: %ld", result);
        return false;
    }

    // Get application path
    QString appPath = QCoreApplication::applicationFilePath();
    std::wstring appPathW = appPath.toStdWString();

    // Set registry value
    result = RegSetValueExW(
        hKey,
        L"Spectacle",                            // Value name
        0,                                       // Reserved
        REG_SZ,                                  // Type (string)
        (const BYTE*)appPathW.c_str(),           // Data
        (appPath.length() + 1) * sizeof(wchar_t) // Data size
    );

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        qWarning("Failed to set Registry Run value: %ld", result);
        return false;
    }

    qDebug("Registered for auto-start: %s", qUtf8Printable(appPath));
    return true;
}

bool HelperProcess::unregisterAutoStart()
{
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        qWarning("Failed to open Registry Run key: %ld", result);
        return false;
    }

    // Delete registry value
    result = RegDeleteValueW(hKey, L"Spectacle");
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        qWarning("Failed to delete Registry Run value: %ld", result);
        return false;
    }

    qDebug("Unregistered from auto-start");
    return true;
}

bool HelperProcess::start()
{
    if (m_running) {
        qWarning("Helper process already running");
        return true;
    }

    // Create message window for hotkey processing
    if (!createMessageWindow()) {
        return false;
    }

    // Register for auto-start via Registry
    if (!registerAutoStart()) {
        // Non-fatal - helper can still run
        qWarning("Auto-start registration failed, but continuing");
    }

    m_running = true;
    qDebug("Helper process started");
    return true;
}

void HelperProcess::stop()
{
    if (!m_running) {
        return;
    }

    // Unregister all hotkeys
    for (auto it = m_hotkeyAtoms.constBegin(); it != m_hotkeyAtoms.constEnd(); ++it) {
        UnregisterHotKey(m_messageWindow, it.value());
        GlobalDeleteAtom(it.value());
    }

    m_registeredKeys.clear();
    m_hotkeyAtoms.clear();

    // Destroy message window
    if (m_messageWindow) {
        DestroyWindow(m_messageWindow);
        m_messageWindow = nullptr;
    }

    // Unregister from auto-start
    unregisterAutoStart();

    m_running = false;
    qDebug("Helper process stopped");
}

bool HelperProcess::isRunning() const
{
    return m_running;
}

#include "moc_HelperProcess.cpp"
