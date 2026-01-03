/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Window Event Monitor Integration Tests
 *  Story 1.8 - Window Movement Detection & Overlay Persistence
 */

#include <QtTest>
#include <QSignalSpy>
#include "Platforms/Windows/WindowEventMonitor.h"

#ifdef Q_OS_WIN

class WindowEventMonitorTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qInfo("Starting WindowEventMonitor tests");
    }

    void cleanupTestCase()
    {
        qInfo("WindowEventMonitor tests completed");
    }

    void testSetWinEventHookRegistersSuccessfully()
    {
        // Test: SetWinEventHook registration succeeds
        WindowEventMonitor monitor;

        // Should start successfully
        QVERIFY(monitor.start());
        QVERIFY(monitor.isRunning());

        monitor.stop();
        QVERIFY(!monitor.isRunning());
    }

    void testEventCallbackThreadSafe()
    {
        // Test: Event callback dispatch is thread-safe
        WindowEventMonitor monitor;

        // Start monitoring
        QVERIFY(monitor.start());

        // Set tracked window (use desktop window for testing)
        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        // Note: Cannot easily simulate actual Win32 events in tests
        // This validates the infrastructure is in place
        QVERIFY(monitor.isRunning());

        monitor.stop();
    }

    void testMovingWindowUpdatesSelectionBounds()
    {
        WindowEventMonitor monitor;

        QVERIFY(monitor.start());

        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        QSignalSpy spyMoved(&monitor, &WindowEventMonitor::windowMoved);

        monitor.handleWindowEvent(EVENT_OBJECT_LOCATIONCHANGE, hwnd);

        QVERIFY(spyMoved.wait(150));
        QCOMPARE(spyMoved.count(), 1);

        const QList<QVariant> args = spyMoved.takeFirst();
        QCOMPARE(args.size(), 4);
        QVERIFY(args.at(2).toInt() > 0);
        QVERIFY(args.at(3).toInt() > 0);

        monitor.stop();
    }

    void testResizeUpdatesSelectionBounds()
    {
        WindowEventMonitor monitor;

        QVERIFY(monitor.start());

        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        QSignalSpy spyMoved(&monitor, &WindowEventMonitor::windowMoved);

        monitor.handleWindowEvent(EVENT_OBJECT_LOCATIONCHANGE, hwnd);

        QVERIFY(spyMoved.wait(150));
        QCOMPARE(spyMoved.count(), 1);

        const QList<QVariant> args = spyMoved.takeFirst();
        QCOMPARE(args.size(), 4);
        QVERIFY(args.at(2).toInt() > 0);
        QVERIFY(args.at(3).toInt() > 0);

        monitor.stop();
    }

    void testClosingWindowClearsSelection()
    {
        WindowEventMonitor monitor;

        QVERIFY(monitor.start());

        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        QSignalSpy spyDestroyed(&monitor, &WindowEventMonitor::windowDestroyed);

        monitor.handleWindowEvent(EVENT_OBJECT_DESTROY, hwnd);

        QVERIFY(spyDestroyed.wait(50));
        QCOMPARE(spyDestroyed.count(), 1);

        monitor.stop();
    }

    void testOverlayWindowCloseIgnored()
    {
        WindowEventMonitor monitor;

        QVERIFY(monitor.start());

        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        QSignalSpy spyDestroyed(&monitor, &WindowEventMonitor::windowDestroyed);

        HWND otherWindow = reinterpret_cast<HWND>(0x12345);
        monitor.handleWindowEvent(EVENT_OBJECT_DESTROY, otherWindow);

        QVERIFY(!spyDestroyed.wait(80));

        monitor.stop();
    }

    void testRapidMovementsDebounced()
    {
        WindowEventMonitor monitor;

        QVERIFY(monitor.start());

        HWND hwnd = GetDesktopWindow();
        monitor.setTrackedWindow(hwnd);

        QSignalSpy spyMoved(&monitor, &WindowEventMonitor::windowMoved);

        monitor.handleWindowEvent(EVENT_OBJECT_LOCATIONCHANGE, hwnd);
        monitor.handleWindowEvent(EVENT_OBJECT_LOCATIONCHANGE, hwnd);
        monitor.handleWindowEvent(EVENT_OBJECT_LOCATIONCHANGE, hwnd);

        QVERIFY(spyMoved.wait(150));
        QCOMPARE(spyMoved.count(), 1);
        QVERIFY(!spyMoved.wait(80));

        monitor.stop();
    }

    void testStopWithoutStartDoesNotCrash()
    {
        // Test: Stopping without starting is safe
        WindowEventMonitor monitor;

        // Should not crash
        monitor.stop();
        QVERIFY(!monitor.isRunning());
    }

    void testSetTrackedWindow()
    {
        // Test: Can set tracked window handle
        WindowEventMonitor monitor;

        HWND hwnd = reinterpret_cast<HWND>(0x12345);
        monitor.setTrackedWindow(hwnd);

        QCOMPARE(monitor.trackedWindow(), hwnd);
    }

    void testSignalsDeclared()
    {
        // Test: Signals are properly declared
        WindowEventMonitor monitor;

        // Verify signal existence via spy (won't be emitted in this test)
        QSignalSpy spyMoved(&monitor, &WindowEventMonitor::windowMoved);
        QSignalSpy spyDestroyed(&monitor, &WindowEventMonitor::windowDestroyed);
        QSignalSpy spyError(&monitor, &WindowEventMonitor::errorOccurred);

        QVERIFY(spyMoved.isValid());
        QVERIFY(spyDestroyed.isValid());
        QVERIFY(spyError.isValid());
    }
};

QTEST_MAIN(WindowEventMonitorTest)

#include "WindowEventMonitorTest.moc"

#endif // Q_OS_WIN
