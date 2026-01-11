/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Settings Manager
 *  Story 1.5 - Saving & Defaults
 *  Task 1: Design settings storage mechanism
 *  Subtask 1.3: Create SettingsManager class for reading/writing settings
 */

#include "SettingsManager.h"

#ifdef Q_OS_WIN

#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_SETTINGS, "spectacle.settings")

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, QLatin1String("Spectacle"), QLatin1String("Spectacle"))
    , m_autosave(true)
    , m_defaultSaveLocation(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QLatin1String("/Screenshots"))
    , m_defaultFormat(QLatin1String("png"))
    , m_rememberLastLocation(true)
    , m_lastSaveLocation()
    , m_lastFormat()
{
    qCDebug(LOG_SETTINGS) << "SettingsManager constructor starting";

    // Check if QSettings is writable
    if (m_settings.status() != QSettings::NoError) {
        qCWarning(LOG_SETTINGS) << "QSettings initialization error:" << m_settings.status();
    }

    // Load settings on construction
    qCDebug(LOG_SETTINGS) << "Loading settings...";
    loadSettings();

    qCDebug(LOG_SETTINGS) << "SettingsManager constructor completed";
}

void SettingsManager::loadSettings()
{
    // Load autosave setting
    m_autosave = m_settings.value(QLatin1String("autosave"), true).toBool();

    // Load default save location
    QString savedLocation = m_settings.value(QLatin1String("defaultSaveLocation")).toString();
    if (!savedLocation.isEmpty()) {
        m_defaultSaveLocation = savedLocation;
    } else {
        // Set default to Pictures/Screenshots
        m_defaultSaveLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                                + QLatin1String("/Screenshots");
    }

    // Load default format
    m_defaultFormat = m_settings.value(QLatin1String("defaultFormat"), QLatin1String("png")).toString();

    // Load remember last location setting
    m_rememberLastLocation = m_settings.value(QLatin1String("rememberLastLocation"), true).toBool();

    // Load last save location (if any)
    m_lastSaveLocation = m_settings.value(QLatin1String("lastSaveLocation")).toString();

    // Load last format (if any)
    m_lastFormat = m_settings.value(QLatin1String("lastFormat")).toString();

    qCDebug(LOG_SETTINGS) << "Settings loaded:"
                          << "autosave=" << m_autosave
                          << "defaultLocation=" << m_defaultSaveLocation
                          << "defaultFormat=" << m_defaultFormat
                          << "rememberLast=" << m_rememberLastLocation;
}

void SettingsManager::saveSettings()
{
    // Save all settings
    m_settings.setValue(QLatin1String("autosave"), m_autosave);
    m_settings.setValue(QLatin1String("defaultSaveLocation"), m_defaultSaveLocation);
    m_settings.setValue(QLatin1String("defaultFormat"), m_defaultFormat);
    m_settings.setValue(QLatin1String("rememberLastLocation"), m_rememberLastLocation);
    m_settings.setValue(QLatin1String("lastSaveLocation"), m_lastSaveLocation);
    m_settings.setValue(QLatin1String("lastFormat"), m_lastFormat);

    m_settings.sync();

    qCDebug(LOG_SETTINGS) << "Settings saved";
}

void SettingsManager::resetToDefaults()
{
    // Reset all values to defaults
    m_autosave = true;
    m_defaultSaveLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                            + QLatin1String("/Screenshots");
    m_defaultFormat = QLatin1String("png");
    m_rememberLastLocation = true;
    m_lastSaveLocation.clear();
    m_lastFormat.clear();

    // Emit change signals
    Q_EMIT autosaveChanged();
    Q_EMIT defaultSaveLocationChanged();
    Q_EMIT defaultFormatChanged();
    Q_EMIT rememberLastLocationChanged();

    qCDebug(LOG_SETTINGS) << "Settings reset to defaults";
}

// Property getters
bool SettingsManager::autosave() const
{
    return m_autosave;
}

QString SettingsManager::defaultSaveLocation() const
{
    return m_defaultSaveLocation;
}

QString SettingsManager::defaultFormat() const
{
    return m_defaultFormat;
}

bool SettingsManager::rememberLastLocation() const
{
    return m_rememberLastLocation;
}

// Property setters
void SettingsManager::setAutosave(bool enabled)
{
    if (m_autosave != enabled) {
        m_autosave = enabled;
        qCDebug(LOG_SETTINGS) << "Autosave changed to:" << enabled;
        Q_EMIT autosaveChanged();
    }
}

void SettingsManager::setDefaultSaveLocation(const QString &path)
{
    if (m_defaultSaveLocation != path) {
        // MEDIUM #2: Validate path before accepting it
        QDir dir(path);
        if (!path.isEmpty()) {
            // Check if path exists and is absolute
            if (!dir.exists()) {
                qCWarning(LOG_SETTINGS) << "Path does not exist:" << path;
                return;
            }
            if (!QDir::isAbsolutePath(path)) {
                qCWarning(LOG_SETTINGS) << "Path must be absolute:" << path;
                return;
            }
        }

        m_defaultSaveLocation = path;
        qCDebug(LOG_SETTINGS) << "Default save location changed to:" << path;
        Q_EMIT defaultSaveLocationChanged();
    }
}

void SettingsManager::setDefaultFormat(const QString &format)
{
    if (m_defaultFormat != format) {
        m_defaultFormat = format;
        qCDebug(LOG_SETTINGS) << "Default format changed to:" << format;
        Q_EMIT defaultFormatChanged();
    }
}

void SettingsManager::setRememberLastLocation(bool remember)
{
    if (m_rememberLastLocation != remember) {
        m_rememberLastLocation = remember;
        qCDebug(LOG_SETTINGS) << "Remember last location changed to:" << remember;
        Q_EMIT rememberLastLocationChanged();
    }
}

// Helper methods
QString SettingsManager::generateFilename(const QString &pattern) const
{
    QString patternToUse = pattern;
    if (patternToUse.isEmpty()) {
        // Use default pattern: screenshot-YYYY-MM-DD-HHMMSS
        // IMPORTANT: Single quotes around literal text to escape Qt format codes
        // Qt format codes: yyyy=year, MM=month, dd=day, HH=hour, mm=minute, ss=second
        patternToUse = QString::fromUtf8("'screenshot-'yyyy-MM-dd-HHmmss");
    }

    // Generate timestamp using Qt's date format codes
    QDateTime now = QDateTime::currentDateTime();
    QString filename = now.toString(patternToUse);

    // Add extension based on current format
    QString extension = m_defaultFormat;
    if (!extension.startsWith(QLatin1Char('.'))) {
        extension.prepend(QLatin1Char('.'));
    }

    return filename + extension;
}

QString SettingsManager::getSaveLocation() const
{
    if (m_rememberLastLocation && !m_lastSaveLocation.isEmpty()) {
        return m_lastSaveLocation;
    }

    return m_defaultSaveLocation;
}

void SettingsManager::setLastSaveLocation(const QString &path)
{
    if (m_lastSaveLocation != path) {
        m_lastSaveLocation = path;
        qCDebug(LOG_SETTINGS) << "Last save location updated to:" << path;

        // Auto-save when last location changes
        m_settings.setValue(QLatin1String("lastSaveLocation"), m_lastSaveLocation);
        m_settings.sync();
    }
}

void SettingsManager::setLastFormat(const QString &format)
{
    if (m_lastFormat != format) {
        m_lastFormat = format;
        qCDebug(LOG_SETTINGS) << "Last format updated to:" << format;

        // Auto-save when last format changes
        m_settings.setValue(QLatin1String("lastFormat"), m_lastFormat);
        m_settings.sync();
    }
}

#endif // Q_OS_WIN
