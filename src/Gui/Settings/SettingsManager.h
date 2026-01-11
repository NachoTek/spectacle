/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Settings Manager
 *  Story 1.5 - Saving & Defaults
 *  Task 1: Design settings storage mechanism
 *  Subtask 1.3: Create SettingsManager class for reading/writing settings
 */

#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

#ifdef Q_OS_WIN

/**
 * @brief Settings manager for Spectacle capture preferences
 *
 * Manages application settings using QSettings with INI format on Windows.
 * All settings are exposed via Q_PROPERTY for QML binding.
 */
class SettingsManager : public QObject
{
    Q_OBJECT

    // Properties for QML binding
    Q_PROPERTY(bool autosave READ autosave WRITE setAutosave NOTIFY autosaveChanged)
    Q_PROPERTY(QString defaultSaveLocation READ defaultSaveLocation WRITE setDefaultSaveLocation NOTIFY defaultSaveLocationChanged)
    Q_PROPERTY(QString defaultFormat READ defaultFormat WRITE setDefaultFormat NOTIFY defaultFormatChanged)
    Q_PROPERTY(bool rememberLastLocation READ rememberLastLocation WRITE setRememberLastLocation NOTIFY rememberLastLocationChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager() override = default;

    // Settings management
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

    // Property getters
    bool autosave() const;
    QString defaultSaveLocation() const;
    QString defaultFormat() const;
    bool rememberLastLocation() const;

    // Helper methods
    /**
     * @brief Generate filename with timestamp pattern
     * @param pattern Custom pattern (optional). Default: "screenshot-%Y-%m-%d-%H%M%S"
     * @return Generated filename with extension
     */
    QString generateFilename(const QString &pattern = QString()) const;

    /**
     * @brief Get save location based on settings
     * @return Absolute path to save location (default or last used)
     */
    QString getSaveLocation() const;

    /**
     * @brief Set the last save location (for "remember last location" feature)
     * @param path The path where user last saved a file
     */
    void setLastSaveLocation(const QString &path);

    /**
     * @brief Set the last format used (for "remember last format" feature)
     * @param format The format user last saved with
     */
    void setLastFormat(const QString &format);

public Q_SLOTS:
    // Property setters
    void setAutosave(bool enabled);
    void setDefaultSaveLocation(const QString &path);
    void setDefaultFormat(const QString &format);
    void setRememberLastLocation(bool remember);

Q_SIGNALS:
    void autosaveChanged();
    void defaultSaveLocationChanged();
    void defaultFormatChanged();
    void rememberLastLocationChanged();

private:
    QSettings m_settings;

    // Cached values
    bool m_autosave;
    QString m_defaultSaveLocation;
    QString m_defaultFormat;
    bool m_rememberLastLocation;
    QString m_lastSaveLocation;
    QString m_lastFormat;
};

#endif // Q_OS_WIN
