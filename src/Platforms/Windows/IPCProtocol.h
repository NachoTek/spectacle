/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  IPC Protocol for Helper Process Communication
 *  Story 1.1 - Named Pipe JSON Envelope
 */

#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @brief IPC message types for helper process communication
 */
enum class IPCMessageType {
    HotkeyPressed,        ///< User pressed a registered hotkey
    RegisterHotkey,       ///< Request to register a new hotkey
    UnregisterHotkey,     ///< Request to unregister a hotkey
    Ping,                 ///< Health check ping
    Pong,                 ///< Health check pong response
    Error                 ///< Error message
};

/**
 * @brief IPC protocol handler for JSON envelope
 *
 * Message format (JSON):
 * {
 *   "type": "HotkeyPressed",
 *   "keyCode": 44,
 *   "timestamp": 1234567890
 * }
 */
class IPCProtocol
{
public:
    /**
     * @brief Create an IPC message
     * @param type Message type
     * @param data Message data
     * @return JSON document
     */
    static QJsonDocument createMessage(IPCMessageType type, const QJsonObject &data = QJsonObject());

    /**
     * @brief Parse an IPC message
     * @param json JSON document
     * @return Message type and data
     */
    static QPair<IPCMessageType, QJsonObject> parseMessage(const QJsonDocument &json);

    /**
     * @brief Create hotkey pressed message
     * @param keyCode Virtual key code
     * @return JSON document
     */
    static QJsonDocument hotkeyPressedMessage(int keyCode);

    /**
     * @brief Create register hotkey request
     * @param keyCode Virtual key code
     * @return JSON document
     */
    static QJsonDocument registerHotkeyRequest(int keyCode);

    /**
     * @brief Create error message
     * @param errorMessage Error description
     * @return JSON document
     */
    static QJsonDocument errorMessage(const QString &errorMessage);

    /**
     * @brief Convert message type to string
     * @param type Message type
     * @return String representation
     */
    static QString messageTypeToString(IPCMessageType type);

    /**
     * @brief Convert string to message type
     * @param str String representation
     * @return Message type
     */
    static IPCMessageType stringToMessageType(const QString &str);
};

