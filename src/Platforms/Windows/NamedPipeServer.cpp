/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Named Pipe IPC Server for Helper Process Communication
 *  Story 1.1 - Win32 Named Pipe Server with JSON Envelope
 */

#include "NamedPipeServer.h"
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QByteArray>

Q_LOGGING_CATEGORY(NAMEDPIPE_SERVER, "spectacle.platforms.windows.namedpipeserver")

NamedPipeServer::NamedPipeServer(const QString &pipeName, QObject *parent)
    : QObject(parent)
    , m_pipeName(pipeName)
{
    m_worker = std::make_unique<PipeWorker>(this, this);
}

NamedPipeServer::~NamedPipeServer()
{
    stop();
}

bool NamedPipeServer::start()
{
    QMutexLocker locker(&m_mutex);

    if (m_running) {
        qCWarning(NAMEDPIPE_SERVER) << "Named pipe server already running";
        return true;
    }

    qCInfo(NAMEDPIPE_SERVER) << "Starting named pipe server:" << m_pipeName;

    // Start worker thread
    m_worker->start();
    m_running = true;

    return true;
}

void NamedPipeServer::stop()
{
    QMutexLocker locker(&m_mutex);

    if (!m_running) {
        return;
    }

    qCInfo(NAMEDPIPE_SERVER) << "Stopping named pipe server";

    m_worker->stop();
    m_worker->wait();

    m_running = false;
}

bool NamedPipeServer::isRunning() const
{
    QMutexLocker locker(&m_mutex);
    return m_running;
}

bool NamedPipeServer::sendMessage(const QJsonDocument &message)
{
    if (!m_running) {
        qCWarning(NAMEDPIPE_SERVER) << "Cannot send message: server not running";
        return false;
    }

    return m_worker->sendMessage(message);
}

// ============================================================================
// PipeWorker Implementation
// ============================================================================

NamedPipeServer::PipeWorker::PipeWorker(NamedPipeServer *server, QObject *parent)
    : QThread(parent)
    , m_server(server)
{
#ifdef Q_OS_WIN
    m_hPipe = INVALID_HANDLE_VALUE;
#endif
}

NamedPipeServer::PipeWorker::~PipeWorker()
{
#ifdef Q_OS_WIN
    if (m_hPipe != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(m_hPipe);
        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }
#endif
}

bool NamedPipeServer::PipeWorker::sendMessage(const QJsonDocument &message)
{
#ifdef Q_OS_WIN
    if (m_hPipe == INVALID_HANDLE_VALUE) {
        qCWarning(NAMEDPIPE_SERVER) << "Cannot send message: no client connected";
        return false;
    }

    // Convert JSON to UTF-8 bytes
    QByteArray data = message.toJson(QJsonDocument::Compact);
    DWORD bytesWritten = 0;

    // Write message to pipe
    BOOL success = WriteFile(
        m_hPipe,
        data.constData(),
        static_cast<DWORD>(data.size()),
        &bytesWritten,
        nullptr
    );

    if (!success || bytesWritten != static_cast<DWORD>(data.size())) {
        DWORD error = GetLastError();
        qCWarning(NAMEDPIPE_SERVER) << "Failed to write to pipe:" << error;
        return false;
    }

    qCDebug(NAMEDPIPE_SERVER) << "Sent message:" << data.size() << "bytes";
    return true;

#else
    Q_UNUSED(message)
    qCWarning(NAMEDPIPE_SERVER) << "Named pipes are Windows-only";
    return false;
#endif
}

void NamedPipeServer::PipeWorker::run()
{
#ifdef Q_OS_WIN
    qCInfo(NAMEDPIPE_SERVER) << "Pipe worker thread started";

    while (!m_stopRequested) {
        // Create and connect to pipe (blocking)
        if (!createAndConnectPipe()) {
            if (m_stopRequested) break;

            // Wait before retry
            QThread::msleep(100);
            continue;
        }

        Q_EMIT m_server->clientConnected();

        // Process incoming messages
        processIncomingMessages();

        // Client disconnected
        Q_EMIT m_server->clientDisconnected();

        // Clean up pipe
        if (m_hPipe != INVALID_HANDLE_VALUE) {
            DisconnectNamedPipe(m_hPipe);
            CloseHandle(m_hPipe);
            m_hPipe = INVALID_HANDLE_VALUE;
        }

        if (m_stopRequested) break;
    }

    qCInfo(NAMEDPIPE_SERVER) << "Pipe worker thread stopped";

#else
    qCWarning(NAMEDPIPE_SERVER) << "Named pipes are Windows-only";
#endif
}

bool NamedPipeServer::PipeWorker::createAndConnectPipe()
{
#ifdef Q_OS_WIN
    // Build full pipe name: \\\\.\\pipe\\SpectacleHelper
    QString fullPipeName = u"\\\\.\\pipe\\%1"_s.arg(m_server->pipeName());

    // Convert to wchar_t for Win32 API
    std::wstring wPipeName = fullPipeName.toStdWString();

    // Create named pipe
    m_hPipe = CreateNamedPipeW(
        wPipeName.c_str(),                  // Pipe name
        PIPE_ACCESS_DUPLEX,                 // Read/write access
        PIPE_TYPE_MESSAGE |                 // Message-type pipe
        PIPE_READMODE_MESSAGE |             // Message-read mode
        PIPE_WAIT,                          // Blocking mode
        1,                                  // Max instances
        5120,                               // Output buffer size
        5120,                               // Input buffer size
        0,                                  // Default timeout
        nullptr                            // Default security attributes
    );

    if (m_hPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        qCWarning(NAMEDPIPE_SERVER) << "Failed to create named pipe:" << error;
        Q_EMIT m_server->errorOccurred(QString("Failed to create pipe: %1").arg(error));
        return false;
    }

    qCInfo(NAMEDPIPE_SERVER) << "Named pipe created, waiting for connection...";

    // Wait for client connection (blocking)
    BOOL connected = ConnectNamedPipe(m_hPipe, nullptr);

    if (!connected) {
        DWORD error = GetLastError();
        if (error == ERROR_PIPE_CONNECTED) {
            // Client already connected (race condition)
            connected = TRUE;
        } else {
            qCWarning(NAMEDPIPE_SERVER) << "Failed to connect pipe:" << error;
            CloseHandle(m_hPipe);
            m_hPipe = INVALID_HANDLE_VALUE;
            Q_EMIT m_server->errorOccurred(QString("Failed to connect pipe: %1").arg(error));
            return false;
        }
    }

    qCInfo(NAMEDPIPE_SERVER) << "Client connected to named pipe";
    return true;

#else
    return false;
#endif
}

void NamedPipeServer::PipeWorker::processIncomingMessages()
{
#ifdef Q_OS_WIN
    constexpr DWORD BUFFER_SIZE = 5120;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead = 0;

    while (!m_stopRequested) {
        // Read message from pipe (blocking)
        BOOL success = ReadFile(
            m_hPipe,
            buffer,
            BUFFER_SIZE - 1,  // Leave space for null terminator
            &bytesRead,
            nullptr
        );

        if (!success) {
            DWORD error = GetLastError();

            if (error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA) {
                // Client disconnected
                qCInfo(NAMEDPIPE_SERVER) << "Client disconnected";
                break;
            } else {
                qCWarning(NAMEDPIPE_SERVER) << "Read error:" << error;
                Q_EMIT m_server->errorOccurred(QString("Read error: %1").arg(error));
                break;
            }
        }

        if (bytesRead == 0) {
            // No data (client may have disconnected)
            continue;
        }

        // Null-terminate buffer
        buffer[bytesRead] = '\0';

        // Parse JSON message
        QJsonParseError parseError;
        QJsonDocument json = QJsonDocument::fromJson(QByteArray(buffer, bytesRead), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qCWarning(NAMEDPIPE_SERVER) << "JSON parse error:" << parseError.errorString();
            continue;
        }

        // Parse IPC message
        auto [type, data] = IPCProtocol::parseMessage(json);

        qCDebug(NAMEDPIPE_SERVER) << "Received message:" << IPCProtocol::messageTypeToString(type);

        // Emit signal with parsed message
        Q_EMIT m_server->messageReceived(type, data);
    }

#else
    Q_UNUSED(m_hPipe)
#endif
}

#include "moc_NamedPipeServer.cpp"
