/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Named Pipe Server Tests
 *  Story 1.1 - Named Pipe IPC Server Tests
 */

#include <QObject>
#include <QTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QSignalSpy>

#ifdef Q_OS_WIN
#include "../src/Platforms/Windows/NamedPipeServer.h"
#include <windows.h>
#endif

class NamedPipeServerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testServerStartStop();
    void testServerStartWhenAlreadyRunning();
    void testClientConnects();
    void testReceiveHotkeyMessage();
    void testSendMessageToClient();
    void testMultipleMessages();
    void testInvalidJson();

private:
#ifdef Q_OS_WIN
    NamedPipeServer *m_server = nullptr;
#endif
};

void NamedPipeServerTest::initTestCase()
{
#ifdef Q_OS_WIN
    m_server = new NamedPipeServer(QLatin1String("SpectacleHelperTest"), this);
#endif
}

void NamedPipeServerTest::cleanupTestCase()
{
#ifdef Q_OS_WIN
    if (m_server) {
        m_server->stop();
        delete m_server;
        m_server = nullptr;
    }
#endif
}

void NamedPipeServerTest::testServerStartStop()
{
#ifdef Q_OS_WIN
    QVERIFY(!m_server->isRunning());

    QVERIFY(m_server->start());
    QVERIFY(m_server->isRunning());

    m_server->stop();
    QVERIFY(!m_server->isRunning());

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testServerStartWhenAlreadyRunning()
{
#ifdef Q_OS_WIN
    m_server->start();
    QVERIFY(m_server->isRunning());

    // Starting again should succeed (no-op)
    QVERIFY(m_server->start());
    QVERIFY(m_server->isRunning());

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testClientConnects()
{
#ifdef Q_OS_WIN
    m_server->start();

    QSignalSpy connectSpy(m_server, &NamedPipeServer::clientConnected);

    // Simulate client connecting in separate thread
    QThread::create([&]() {
        QString pipeName = QLatin1String("\\\\.\\pipe\\SpectacleHelperTest");
        std::wstring wPipeName = pipeName.toStdWString();

        HANDLE hPipe = CreateFileW(
            wPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            QThread::msleep(100);  // Keep connection briefly
            CloseHandle(hPipe);
        }
    })->start();

    // Wait for connection signal
    QVERIFY(connectSpy.wait(5000));
    QCOMPARE(connectSpy.count(), 1);

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testReceiveHotkeyMessage()
{
#ifdef Q_OS_WIN
    m_server->start();

    QSignalSpy connectSpy(m_server, &NamedPipeServer::clientConnected);
    QSignalSpy messageSpy(m_server, &NamedPipeServer::messageReceived);

    // Simulate client sending hotkey message
    QThread::create([&]() {
        QString pipeName = QLatin1String("\\\\.\\pipe\\SpectacleHelperTest");
        std::wstring wPipeName = pipeName.toStdWString();

        HANDLE hPipe = CreateFileW(
            wPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            // Wait for connection
            QThread::msleep(100);

            // Create hotkey message
            QJsonObject data;
            data[QLatin1String("keyCode")] = 44;  // Print Screen key
            QJsonDocument msg = IPCProtocol::hotkeyPressedMessage(44);

            // Send message
            QByteArray jsonBytes = msg.toJson(QJsonDocument::Compact);
            DWORD bytesWritten = 0;
            WriteFile(hPipe, jsonBytes.constData(), jsonBytes.size(), &bytesWritten, nullptr);

            QThread::msleep(100);
            CloseHandle(hPipe);
        }
    })->start();

    // Wait for connection and message
    QVERIFY(connectSpy.wait(5000));
    QVERIFY(messageSpy.wait(5000));

    QCOMPARE(messageSpy.count(), 1);

    // Verify message content
    QList<QVariant> args = messageSpy.takeFirst();
    IPCMessageType type = static_cast<IPCMessageType>(args.at(0).toInt());
    QJsonObject data = args.at(1).toJsonObject();

    QCOMPARE(type, IPCMessageType::HotkeyPressed);
    QCOMPARE(data[QLatin1String("keyCode")].toInt(), 44);

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testSendMessageToClient()
{
#ifdef Q_OS_WIN
    m_server->start();

    QSignalSpy connectSpy(m_server, &NamedPipeServer::clientConnected);

    // Simulate client receiving message
    QThread::create([&]() {
        QString pipeName = QLatin1String("\\\\.\\pipe\\SpectacleHelperTest");
        std::wstring wPipeName = pipeName.toStdWString();

        HANDLE hPipe = CreateFileW(
            wPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            // Wait for connection
            QThread::msleep(200);

            // Read message
            char buffer[5120];
            DWORD bytesRead = 0;
            BOOL success = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);

            QVERIFY(success);
            QVERIFY(bytesRead > 0);

            buffer[bytesRead] = '\0';

            // Verify JSON
            QJsonDocument json = QJsonDocument::fromJson(QByteArray(buffer, bytesRead));
            QVERIFY(json.isObject());
            QCOMPARE(json.object()[QLatin1String("type")].toString(), QLatin1String("Pong"));

            CloseHandle(hPipe);
        }
    })->start();

    QVERIFY(connectSpy.wait(5000));

    // Send pong message
    QJsonDocument pongMsg = IPCProtocol::createMessage(IPCMessageType::Pong);
    QVERIFY(m_server->sendMessage(pongMsg));

    QThread::msleep(500);  // Wait for client to receive

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testMultipleMessages()
{
#ifdef Q_OS_WIN
    m_server->start();

    QSignalSpy messageSpy(m_server, &NamedPipeServer::messageReceived);

    // Simulate client sending multiple messages
    QThread::create([&]() {
        QString pipeName = QLatin1String("\\\\.\\pipe\\SpectacleHelperTest");
        std::wstring wPipeName = pipeName.toStdWString();

        HANDLE hPipe = CreateFileW(
            wPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            QThread::msleep(100);

            // Send multiple messages
            for (int i = 0; i < 3; ++i) {
                QJsonObject data;
                data[QLatin1String("keyCode")] = i;
                QJsonDocument msg = IPCProtocol::hotkeyPressedMessage(i);

                QByteArray jsonBytes = msg.toJson(QJsonDocument::Compact);
                DWORD bytesWritten = 0;
                WriteFile(hPipe, jsonBytes.constData(), jsonBytes.size(), &bytesWritten, nullptr);

                QThread::msleep(50);
            }

            QThread::msleep(100);
            CloseHandle(hPipe);
        }
    })->start();

    // Wait for all messages
    QElapsedTimer timer;
    timer.start();

    while (messageSpy.count() < 3 && timer.elapsed() < 5000) {
        QTest::qWait(100);
    }

    QCOMPARE(messageSpy.count(), 3);

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

void NamedPipeServerTest::testInvalidJson()
{
#ifdef Q_OS_WIN
    m_server->start();

    QSignalSpy connectSpy(m_server, &NamedPipeServer::clientConnected);
    QSignalSpy errorSpy(m_server, &NamedPipeServer::errorOccurred);

    // Simulate client sending invalid JSON
    QThread::create([&]() {
        QString pipeName = QLatin1String("\\\\.\\pipe\\SpectacleHelperTest");
        std::wstring wPipeName = pipeName.toStdWString();

        HANDLE hPipe = CreateFileW(
            wPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            QThread::msleep(100);

            // Send invalid JSON
            const char *invalidJson = "{invalid json}";
            DWORD bytesWritten = 0;
            WriteFile(hPipe, invalidJson, static_cast<DWORD>(strlen(invalidJson)), &bytesWritten, nullptr);

            QThread::msleep(100);
            CloseHandle(hPipe);
        }
    })->start();

    QVERIFY(connectSpy.wait(5000));
    QThread::msleep(500);  // Give server time to process

    // Invalid JSON should not crash the server
    // Server should log a warning but remain running
    QVERIFY(m_server->isRunning());

    m_server->stop();

#else
    QSKIP("Named pipes are Windows-only");
#endif
}

QTEST_MAIN(NamedPipeServerTest)
#include "NamedPipeServerTest.moc"
