/*
 * ATDD Test for Story 1.1 - Helper Process
 * RED Phase: Tests will fail until implementation is complete
 */

#include <QTest>
#include <QElapsedTimer>

#include "Platforms/Windows/HelperProcess.h"

using namespace Qt::StringLiterals;

class HelperProcessTest : public QObject
{
    Q_OBJECT

private:
    HelperProcess *m_helper;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test hotkey registration
    void testRegisterGlobalHotkey();
    void testIsHotkeyRegistered();

    // Test hotkey simulation
    void testSimulateHotkeyPressEmitsSignal();

    // Test process lifecycle
    void testStartHelperProcess();
    void testStopHelperProcess();
    void testIsRunning();

    // End-to-end test (NFR1: < 1s overlay latency)
    void testPrintScreenTriggersOverlayWithinOneSecond();
};

void HelperProcessTest::initTestCase()
{
    m_helper = HelperProcess::instance();
}

void HelperProcessTest::init()
{
    // Clean state before each test
    if (m_helper->isRunning()) {
        m_helper->stop();
    }
}

void HelperProcessTest::cleanup()
{
    // Clean up after each test
}

void HelperProcessTest::cleanupTestCase()
{
    if (m_helper->isRunning()) {
        m_helper->stop();
    }
}

void HelperProcessTest::testRegisterGlobalHotkey()
{
    // GIVEN: Helper process is ready
    QVERIFY(m_helper != nullptr);

    // WHEN: Registering Print Screen hotkey (VK_SNAPSHOT = 0x2C)
    bool registered = m_helper->registerGlobalHotkey(0x2C);

    // THEN: Hotkey is registered successfully
    QVERIFY2(registered, "Print Screen hotkey should register successfully");
    QVERIFY2(m_helper->isHotkeyRegistered(0x2C), "isHotkeyRegistered should return true");
}

void HelperProcessTest::testIsHotkeyRegistered()
{
    // GIVEN: Helper process
    QVERIFY(m_helper != nullptr);

    // WHEN: Hotkey not yet registered
    // THEN: Should return false
    QVERIFY2(!m_helper->isHotkeyRegistered(0x2C),
             "Unregistered hotkey should return false");

    // WHEN: Register hotkey
    m_helper->registerGlobalHotkey(0x2C);

    // THEN: Should return true
    QVERIFY2(m_helper->isHotkeyRegistered(0x2C),
             "Registered hotkey should return true");
}

void HelperProcessTest::testSimulateHotkeyPressEmitsSignal()
{
    // GIVEN: Helper process with registered hotkey
    m_helper->registerGlobalHotkey(0x2C);

    bool signalEmitted = false;
    int receivedKeyCode = 0;

    // Connect to signal
    QObject::connect(m_helper, &HelperProcess::hotkeyPressed,
                     [&](int keyCode) {
                         signalEmitted = true;
                         receivedKeyCode = keyCode;
                     });

    // WHEN: Simulating hotkey press
    m_helper->simulateHotkeyPress(0x2C);

    // THEN: Signal should be emitted with correct key code
    QVERIFY2(signalEmitted, "hotkeyPressed signal should be emitted");
    QCOMPARE(receivedKeyCode, 0x2C);
}

void HelperProcessTest::testStartHelperProcess()
{
    // GIVEN: Helper process instance
    QVERIFY(m_helper != nullptr);

    // WHEN: Starting the process
    bool started = m_helper->start();

    // THEN: Process should start successfully
    QVERIFY2(started, "Helper process should start successfully");
    QVERIFY2(m_helper->isRunning(), "isRunning should return true after start");
}

void HelperProcessTest::testStopHelperProcess()
{
    // GIVEN: Running helper process
    m_helper->start();
    QVERIFY(m_helper->isRunning());

    // WHEN: Stopping the process
    m_helper->stop();

    // THEN: Process should not be running
    QVERIFY2(!m_helper->isRunning(), "isRunning should return false after stop");
}

void HelperProcessTest::testIsRunning()
{
    // GIVEN: Helper process instance
    QVERIFY(m_helper != nullptr);

    // WHEN: Not started
    // THEN: Should not be running
    QVERIFY2(!m_helper->isRunning(), "Should not be running initially");

    // WHEN: Started
    m_helper->start();

    // THEN: Should be running
    QVERIFY2(m_helper->isRunning(), "Should be running after start");
}

void HelperProcessTest::testPrintScreenTriggersOverlayWithinOneSecond()
{
    // AC1: Given Spectacle is running and the Print Screen hotkey is enabled
    // WHEN: I press Print Screen
    // THEN: a full-screen overlay appears within 1 second and shows a crosshair cursor

    // GIVEN: Helper process is running with Print Screen hotkey registered
    m_helper->registerGlobalHotkey(0x2C);  // VK_SNAPSHOT
    m_helper->start();
    QVERIFY2(m_helper->isRunning(), "Helper process should be running");

    // Track signal emission
    bool signalReceived = false;
    QElapsedTimer latencyTimer;

    // Connect to hotkey signal
    QObject::connect(m_helper, &HelperProcess::hotkeyPressed,
                     [&](int keyCode) {
                         if (keyCode == 0x2C) {  // VK_SNAPSHOT
                             signalReceived = true;
                             latencyTimer.elapsed();  // Stop timer
                         }
                     });

    // WHEN: Print Screen is pressed
    latencyTimer.start();
    m_helper->simulateHotkeyPress(0x2C);

    // THEN: Signal should be received
    QVERIFY2(signalReceived, "Hotkey signal should be received");

    // AND: Overlay should appear within 1 second (NFR1 requirement)
    qint64 latencyMs = latencyTimer.elapsed();
    QVERIFY2(latencyMs < 1000,
             QString("Overlay latency (%1ms) should be < 1000ms (NFR1)").arg(latencyMs).toUtf8().constData());

    // Cleanup
    m_helper->stop();
}

QTEST_MAIN(HelperProcessTest)
#include "HelperProcessTest.moc"
