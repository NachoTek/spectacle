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
    msg[u"type"_s] = messageTypeToString(type);
    msg[u"timestamp"_s] = QDateTime::currentMSecsSinceEpoch();

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
    QString typeStr = obj.value(u"type"_s).toString();
    IPCMessageType type = stringToMessageType(typeStr);

    // Remove metadata fields
    QJsonObject data;
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        if (it.key() != u"type"_s && it.key() != u"timestamp"_s) {
            data[it.key()] = it.value();
        }
    }

    return qMakePair(type, data);
}

QJsonDocument IPCProtocol::hotkeyPressedMessage(int keyCode)
{
    QJsonObject data;
    data[u"keyCode"_s] = keyCode;
    return createMessage(IPCMessageType::HotkeyPressed, data);
}

QJsonDocument IPCProtocol::registerHotkeyRequest(int keyCode)
{
    QJsonObject data;
    data[u"keyCode"_s] = keyCode;
    return createMessage(IPCMessageType::RegisterHotkey, data);
}

QJsonDocument IPCProtocol::errorMessage(const QString &errorMessage)
{
    QJsonObject data;
    data[u"error"_s] = errorMessage;
    return createMessage(IPCMessageType::Error, data);
}

QString IPCProtocol::messageTypeToString(IPCMessageType type)
{
    switch (type) {
        case IPCMessageType::HotkeyPressed: return u"HotkeyPressed"_s;
        case IPCMessageType::RegisterHotkey: return u"RegisterHotkey"_s;
        case IPCMessageType::UnregisterHotkey: return u"UnregisterHotkey"_s;
        case IPCMessageType::Ping: return u"Ping"_s;
        case IPCMessageType::Pong: return u"Pong"_s;
        case IPCMessageType::Error: return u"Error"_s;
        default: return u"Unknown"_s;
    }
}

IPCMessageType IPCProtocol::stringToMessageType(const QString &str)
{
    if (str == u"HotkeyPressed"_s) return IPCMessageType::HotkeyPressed;
    if (str == u"RegisterHotkey"_s) return IPCMessageType::RegisterHotkey;
    if (str == u"UnregisterHotkey"_s) return IPCMessageType::UnregisterHotkey;
    if (str == u"Ping"_s) return IPCMessageType::Ping;
    if (str == u"Pong"_s) return IPCMessageType::Pong;
    if (str == u"Error"_s) return IPCMessageType::Error;
    return IPCMessageType::Error; // Default to error for unknown types
}

