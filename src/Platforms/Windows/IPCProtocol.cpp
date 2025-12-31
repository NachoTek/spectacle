/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  IPC Protocol for Helper Process Communication
 *  Story 1.1 - Named Pipe JSON Envelope
 */

#include "IPCProtocol.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

QJsonDocument IPCProtocol::createMessage(IPCMessageType type, const QJsonObject &data)
{
    QJsonObject msg;
    msg[QLatin1String("type")] = messageTypeToString(type);
    msg[QLatin1String("timestamp")] = QDateTime::currentMSecsSinceEpoch();

    // Merge data into message
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        msg[it.key()] = it.value();
    }

    return QJsonDocument(msg);
}

QPair<IPCMessageType, QJsonObject> IPCProtocol::parseMessage(const QJsonDocument &json)
{
    if (!json.isObject()) {
        qWarning("Invalid IPC message: not an object");
        return qMakePair(IPCMessageType::Error, QJsonObject());
    }

    QJsonObject obj = json.object();
    QString typeStr = obj.value(QLatin1String("type")).toString();
    IPCMessageType type = stringToMessageType(typeStr);

    // Remove metadata fields
    QJsonObject data;
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        if (it.key() != QLatin1String("type") && it.key() != QLatin1String("timestamp")) {
            data[it.key()] = it.value();
        }
    }

    return qMakePair(type, data);
}

QJsonDocument IPCProtocol::hotkeyPressedMessage(int keyCode)
{
    QJsonObject data;
    data[QLatin1String("keyCode")] = keyCode;
    return createMessage(IPCMessageType::HotkeyPressed, data);
}

QJsonDocument IPCProtocol::registerHotkeyRequest(int keyCode)
{
    QJsonObject data;
    data[QLatin1String("keyCode")] = keyCode;
    return createMessage(IPCMessageType::RegisterHotkey, data);
}

QJsonDocument IPCProtocol::errorMessage(const QString &errorMessage)
{
    QJsonObject data;
    data[QLatin1String("error")] = errorMessage;
    return createMessage(IPCMessageType::Error, data);
}

QString IPCProtocol::messageTypeToString(IPCMessageType type)
{
    switch (type) {
        case IPCMessageType::HotkeyPressed: return QLatin1String("HotkeyPressed");
        case IPCMessageType::RegisterHotkey: return QLatin1String("RegisterHotkey");
        case IPCMessageType::UnregisterHotkey: return QLatin1String("UnregisterHotkey");
        case IPCMessageType::Ping: return QLatin1String("Ping");
        case IPCMessageType::Pong: return QLatin1String("Pong");
        case IPCMessageType::Error: return QLatin1String("Error");
        default: return QLatin1String("Unknown");
    }
}

IPCMessageType IPCProtocol::stringToMessageType(const QString &str)
{
    if (str == QLatin1String("HotkeyPressed")) return IPCMessageType::HotkeyPressed;
    if (str == QLatin1String("RegisterHotkey")) return IPCMessageType::RegisterHotkey;
    if (str == QLatin1String("UnregisterHotkey")) return IPCMessageType::UnregisterHotkey;
    if (str == QLatin1String("Ping")) return IPCMessageType::Ping;
    if (str == QLatin1String("Pong")) return IPCMessageType::Pong;
    if (str == QLatin1String("Error")) return IPCMessageType::Error;
    return IPCMessageType::Error; // Default to error for unknown types
}

