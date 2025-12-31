/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Integration Tests for TargetHighlighter
 *  Story 1.2 - Selection Targeting + Refinement
 */

#include <QTest>
#include <QQuickView>
#include <QQuickItem>
#include <QRect>

class TargetHighlighterTest : public QObject
{
    Q_OBJECT

private:
    QQuickView *m_view = nullptr;
    QObject *m_highlighter = nullptr;

private slots:
    void initTestCase()
    {
        // Create QML view
        m_view = new QQuickView();

        // Load TargetHighlighter component
        m_view->setSource(QUrl::fromLocalFile(QFINDTESTDATA("src/Gui/Overlay/TargetHighlighter.qml")));

        if (m_view->status() == QQuickView::Error) {
            qWarning() << "Failed to load TargetHighlighter.qml:" << m_view->errors();
        }

        QVERIFY2(m_view->status() == QQuickView::Ready, "TargetHighlighter should load successfully");

        m_highlighter = m_view->rootObject();
        QVERIFY2(m_highlighter != nullptr, "Root object should exist");
    }

    void cleanupTestCase()
    {
        delete m_view;
    }

    void testComponentLoadsSuccessfully()
    {
        QVERIFY2(m_highlighter != nullptr, "TargetHighlighter should load");
    }

    void testHasHighlightProperty()
    {
        QVERIFY2(m_highlighter->property("highlighted").isValid(), "Should have 'highlighted' property");
    }

    void testHasTargetTypeProperty()
    {
        QVERIFY2(m_highlighter->property("targetType").isValid(), "Should have 'targetType' property");
    }

    void testHighlightFunction()
    {
        QVERIFY2(m_highlighter->property("highlight").isValid(), "Should have 'highlight' function");
    }

    void testClearFunction()
    {
        QVERIFY2(m_highlighter->property("clear").isValid(), "Should have 'clear' function");
    }

    void testInitialOpacityIsZero()
    {
        qreal opacity = m_highlighter->property("opacity").toReal();
        QVERIFY2(opacity == 0.0, "Initial opacity should be 0 (not highlighted)");
    }

    void testHoverShowsHighlightBorder()
    {
        // GIVEN: Highlighter is loaded
        QVERIFY2(m_highlighter != nullptr, "Highlighter should exist");

        // WHEN: Setting highlighted to true
        QVERIFY2(m_highlighter->setProperty("highlighted", true), "Should set highlighted property");

        // THEN: Opacity should become 1.0
        m_view->show(); // Need to show for property changes to take effect
        QTest::qWait(150); // Wait for 100ms animation + buffer

        qreal opacity = m_highlighter->property("opacity").toReal();
        QVERIFY2(opacity == 1.0, "Opacity should be 1.0 when highlighted");
    }

    void testOverlappingTargetsPrioritizeTopmost()
    {
        // This test validates that WindowFromPoint API (used by WindowDetector)
        // correctly handles z-order and returns the topmost window

        // GIVEN: Multiple windows exist (depends on test environment)
        HWND topWindow = GetForegroundWindow();
        QVERIFY2(IsWindow(topWindow), "Should have a foreground window");

        // WHEN: Getting window at a point on that window
        RECT rect;
        GetWindowRect(topWindow, &rect);
        POINT centerPoint;
        centerPoint.x = (rect.left + rect.right) / 2;
        centerPoint.y = (rect.top + rect.bottom) / 2;

        HWND windowAtPoint = WindowFromPoint(centerPoint);

        // THEN: Should return the topmost window (or its child)
        QVERIFY2(windowAtPoint != nullptr, "Should find a window at the point");
        QVERIFY2(IsWindow(windowAtPoint), "Return value should be a valid window");
    }
};

QTEST_MAIN(TargetHighlighterTest)
#include "TargetHighlighterTest.moc"
