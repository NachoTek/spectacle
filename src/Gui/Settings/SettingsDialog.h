/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Settings Dialog
 *  Story 1.5 - Saving & Defaults
 *  Task 2: Create settings UI
 *  Subtask 2.1: Create SettingsDialog or SettingsPage QML component
 */

#pragma once

#include <QQuickView>

#ifdef Q_OS_WIN

class SettingsManager;

/**
 * @brief Settings dialog window for Spectacle preferences
 *
 * Provides a Windows 11 Fluent-styled interface for configuring
 * capture save settings including autosave, default location, and format.
 */
class SettingsDialog : public QQuickView
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWindow *parent = nullptr);
    ~SettingsDialog() override = default;

    /**
     * @brief Set the SettingsManager instance for this dialog
     * @param manager The settings manager (ownership is not transferred)
     */
    void setSettingsManager(SettingsManager *manager);

    /**
     * @brief Show the settings dialog modally
     */
    void showSettings();

private:
    void setupQml();

    SettingsManager *m_settingsManager;
};

#endif // Q_OS_WIN
