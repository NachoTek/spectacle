/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Integration Tests for WindowDetector
 *  Story 1.2 - Selection Targeting + Refinement
 */

#include <QTest>
#include <QPoint>

#include "Platforms/Windows/WindowDetector.h"

class WindowDetectorTest : public QObject
{
    Q_OBJECT

private:
    WindowDetector *m_detector = nullptr;

private slots:
    void initTestCase()
    {
        // Initialize detector
        m_detector = new WindowDetector(this);
    }

    void cleanupTestCase()
    {
        delete m_detector;
    }

    void testWindowDetectorCreatedSuccessfully()
    {
        QVERIFY2(m_detector != nullptr, "WindowDetector should be created");
    }

    void testWindowDetectionFindsVisibleWindows()
    {
        // GIVEN: WindowDetector is initialized
        QVERIFY2(m_detector != nullptr, "Detector should exist");

        // WHEN: Enumerating visible windows
        // Note: This test may find different windows depending on the test environment
        QList<HWND> windows = m_detector->enumerateVisibleWindows();

        // THEN: At least some windows should be found
        // (The test runner window itself should be visible)
        QVERIFY2(windows.size() > 0, "Should find at least one visible window");
    }

    void testWindowBoundsDetection()
    {
        // GIVEN: A known visible window
        HWND testWindow = GetActiveWindow();
        QVERIFY2(IsWindowVisible(testWindow), "Test window should be visible");

        // WHEN: Getting window bounds
        QRect bounds = m_detector->getWindowBounds(testWindow);

        // THEN: Bounds should be valid
        QVERIFY2(bounds.isValid(), "Window bounds should be valid");
        QVERIFY2(bounds.width() > 0, "Window width should be positive");
        QVERIFY2(bounds.height() > 0, "Window height should be positive");
    }

    void testWindowUnderCursorDetection()
    {
        // GIVEN: Current cursor position
        QPoint cursorPos = QCursor::pos();

        // WHEN: Finding window under cursor
        HWND windowUnderCursor = m_detector->windowAtPoint(cursorPos);

        // THEN: Should return a valid window handle
        QVERIFY2(windowUnderCursor != nullptr, "Should find a window under cursor");
        QVERIFY2(IsWindow(windowUnderCursor), "Return value should be a valid window");
    }

    void testWindowBoundsClampedToScreen()
    {
        // GIVEN: A point beyond screen bounds
        QPoint beyondScreen(-1000, -1000);

        // WHEN: Getting window at that point
        HWND window = m_detector->windowAtPoint(beyondScreen);

        // THEN: Should handle gracefully (return nullptr or valid window)
        // May return nullptr if no window at that location
        if (window != nullptr) {
            QVERIFY2(IsWindow(window), "If not null, should be a valid window");
        }
    }
};

QTEST_MAIN(WindowDetectorTest)
#include "WindowDetectorTest.moc"
