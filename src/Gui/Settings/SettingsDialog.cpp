/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Settings Dialog
 *  Story 1.5 - Saving & Defaults
 *  Task 2: Create settings UI
 *  Subtask 2.1: Create SettingsDialog or SettingsPage QML component
 */

#include "SettingsDialog.h"

#ifdef Q_OS_WIN

#include "SettingsManager.h"

#include <QScreen>
#include <QStyle>
#include <QQmlContext>

SettingsDialog::SettingsDialog(QWindow *parent)
    : QQuickView(parent)
    , m_settingsManager(nullptr)
{
    // Setup QML engine and context
    setupQml();

    // Window properties
    setTitle(tr("Spectacle Settings"));
    resize(600, 500);
    setResizeMode(QQuickView::SizeRootObjectToView);

    // Center window on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    QRect centeredRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), screenGeometry);
    setGeometry(centeredRect);
}

void SettingsDialog::setSettingsManager(SettingsManager *manager)
{
    m_settingsManager = manager;

    // Expose SettingsManager to QML
    rootContext()->setContextProperty("settingsManager", m_settingsManager);
}

void SettingsDialog::showSettings()
{
    // Show the dialog modally
    show();
    raise();
    requestActivate();
}

void SettingsDialog::setupQml()
{
    // Load QML from file system (no qrc resources configured)
    // In production, this should use qrc:/qml/SettingsDialog.qml
    QString qmlPath = QCoreApplication::applicationDirPath() + QLatin1String("/../qml/SettingsDialog.qml");
    setSource(QUrl::fromLocalFile(qmlPath));
}

#endif // Q_OS_WIN
