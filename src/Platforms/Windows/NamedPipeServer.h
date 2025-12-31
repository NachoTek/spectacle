/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Named Pipe IPC Server for Helper Process Communication
 *  Story 1.1 - Win32 Named Pipe Server with JSON Envelope
 */

#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <memory>
#include "IPCProtocol.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

/**
 * @brief Named pipe server for Win32 IPC communication
 *
 * This class implements a named pipe server using Win32 API to communicate
 * with the helper process. It uses JSON envelope protocol for message serialization.
 *
 * Pipe name: \\\\.\\pipe\\SpectacleHelper
 * Protocol: JSON envelope (IPCProtocol)
 */
class NamedPipeServer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Create a named pipe server
     * @param pipeName Name of the pipe (default: "SpectacleHelper")
     * @param parent Parent object
     */
    explicit NamedPipeServer(const QString &pipeName = QLatin1String("SpectacleHelper"), QObject *parent = nullptr);

    /**
     * @brief Destroy the named pipe server and stop listening
     */
    ~NamedPipeServer() override;

    /**
     * @brief Start the named pipe server
     * @return true if server started successfully
     */
    bool start();

    /**
     * @brief Stop the named pipe server
     */
    void stop();

    /**
     * @brief Check if server is running
     * @return true if server is listening
     */
    bool isRunning() const;

    /**
     * @brief Send a message to the connected client
     * @param message JSON message to send
     * @return true if message sent successfully
     */
    bool sendMessage(const QJsonDocument &message);

    /**
     * @brief Get the pipe name
     * @return Pipe name
     */
    QString pipeName() const { return m_pipeName; }

Q_SIGNALS:
    /**
     * @brief Emitted when a message is received from the helper process
     * @param type Message type
     * @param data Message data
     */
    void messageReceived(IPCMessageType type, const QJsonObject &data);

    /**
     * @brief Emitted when a client connects
     */
    void clientConnected();

    /**
     * @brief Emitted when a client disconnects
     */
    void clientDisconnected();

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void errorOccurred(const QString &error);

private:
    /**
     * @brief Worker thread for blocking pipe operations
     */
    class PipeWorker;
    std::unique_ptr<PipeWorker> m_worker;

    QString m_pipeName;
    bool m_running = false;
    QMutex m_mutex;
};

/**
 * @brief Worker thread for named pipe operations
 *
 * This worker runs in a separate thread to handle blocking Win32 pipe operations
 * without blocking the main UI thread.
 */
class NamedPipeServer::PipeWorker : public QThread
{
    Q_OBJECT

public:
    explicit PipeWorker(NamedPipeServer *server, QObject *parent = nullptr);
    ~PipeWorker() override;

    void stop() { m_stopRequested = true; }

    /**
     * @brief Send a message to the connected client
     * @param message JSON message to send
     * @return true if message sent successfully
     */
    bool sendMessage(const QJsonDocument &message);

protected:
    void run() override;

private:
    NamedPipeServer *m_server;
    bool m_stopRequested = false;

#ifdef Q_OS_WIN
    HANDLE m_hPipe = INVALID_HANDLE_VALUE;
#endif

    bool createAndConnectPipe();
    void processIncomingMessages();
};
