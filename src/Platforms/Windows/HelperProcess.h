/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Helper Process for Windows Global Hotkey Handling
 *  Story 1.1 - ATDD Implementation
 */

#pragma once

#include <QObject>
#include <QProcess>
#include <QSet>
#include <QMap>
#include <windows.h>

/**
 * @brief Helper process for reliable global hotkey handling on Windows
 *
 * This process runs always-on (via Registry Run key) and handles:
 * - Global hotkey registration (Win32 RegisterHotKey API)
 * - IPC via named pipes with JSON envelope
 * - Hotkey conflict detection
 */
class HelperProcess : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     * @return HelperProcess instance
     */
    static HelperProcess* instance();

    /**
     * @brief Register a global hotkey
     * @param keyCode Virtual key code (e.g., VK_SNAPSHOT for Print Screen)
     * @return true if registration successful, false otherwise
     */
    bool registerGlobalHotkey(int keyCode);

    /**
     * @brief Check if a hotkey is registered
     * @param keyCode Virtual key code
     * @return true if registered
     */
    bool isHotkeyRegistered(int keyCode) const;

    /**
     * @brief Simulate hotkey press (for testing)
     * @param keyCode Virtual key code to simulate
     */
    void simulateHotkeyPress(int keyCode);

    /**
     * @brief Start the helper process
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop the helper process
     */
    void stop();

    /**
     * @brief Check if process is running
     * @return true if running
     */
    bool isRunning() const;

    /**
     * @brief Get HWND of the message window (for hotkey processing)
     * @return Window handle
     */
    HWND messageWindow() const { return m_messageWindow; }

Q_SIGNALS:
    /**
     * @brief Emitted when a registered hotkey is pressed
     * @param keyCode The virtual key code that was pressed
     */
    void hotkeyPressed(int keyCode);

private:
    explicit HelperProcess(QObject *parent = nullptr);
    ~HelperProcess();
    Q_DISABLE_COPY(HelperProcess)

    bool m_running;
    QSet<int> m_registeredKeys;
    QMap<int, int> m_hotkeyAtoms; // keyCode -> atom mapping
    HWND m_messageWindow;          // Hidden window for message processing
    HINSTANCE m_instance;

    /**
     * @brief Create hidden message window for hotkey processing
     * @return true if successful
     */
    bool createMessageWindow();

    /**
     * @brief Window procedure for message processing
     */
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Register for auto-start via Registry Run key
     * @return true if successful
     */
    bool registerAutoStart();

    /**
     * @brief Unregister from auto-start
     * @return true if successful
     */
    bool unregisterAutoStart();
};

