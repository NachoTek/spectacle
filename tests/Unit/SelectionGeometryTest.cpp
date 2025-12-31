/*
 * Unit Tests for Selection Geometry
 * Story 1.1 - Rectangle Calculations
 */

#include <QTest>
#include <QRect>
#include <QGuiApplication>

#include "Gui/Overlay/SelectionGeometry.h"

using namespace Qt::StringLiterals;

class SelectionGeometryTest : public QObject
{
    Q_OBJECT

private:
    SelectionGeometry *m_geom;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // Rectangle creation and validation
    void testSelectionRectangleCreation();
    void testNegativeCoordinatesNormalized();
    void testZeroAreaSelectionRejected();
    void testHugeSelectionClampedToScreen();

    // Resize handles
    void testResizeHandlePositions();
    void testResizeHandleAtPosition();

    // Rectangle manipulation
    void testResizeFromHandle();
    void testResizeFromCornerHandle();
    void testResizeFromEdgeHandle();
};

void SelectionGeometryTest::initTestCase()
{
    m_geom = new SelectionGeometry(this);
}

void SelectionGeometryTest::cleanupTestCase()
{
    delete m_geom;
}

void SelectionGeometryTest::testSelectionRectangleCreation()
{
    // GIVEN: Valid start and end points
    QPoint start(100, 100);
    QPoint end(300, 300);

    // WHEN: Creating selection rectangle
    QRect rect = m_geom->createRectangle(start, end);

    // THEN: Rectangle is valid and normalized
    QVERIFY(rect.isValid());
    QCOMPARE(rect.topLeft(), start);
    QCOMPARE(rect.bottomRight(), end);
    QCOMPARE(rect.width(), 200);
    QCOMPARE(rect.height(), 200);
}

void SelectionGeometryTest::testNegativeCoordinatesNormalized()
{
    // GIVEN: Points with negative coordinates
    QPoint start(-100, -100);
    QPoint end(300, 300);

    // WHEN: Creating selection rectangle
    QRect rect = m_geom->createRectangle(start, end);

    // THEN: Rectangle is normalized to valid coordinates
    QVERIFY(rect.isValid());
    QCOMPARE(rect.left(), -100);
    QCOMPARE(rect.top(), -100);
    QCOMPARE(rect.right(), 300);
    QCOMPARE(rect.bottom(), 300);
    QCOMPARE(rect.width(), 400);
    QCOMPARE(rect.height(), 400);
}

void SelectionGeometryTest::testZeroAreaSelectionRejected()
{
    // GIVEN: Start and end points are the same
    QPoint start(100, 100);
    QPoint end(100, 100);

    // WHEN: Creating selection rectangle
    QRect rect = m_geom->createRectangle(start, end);

    // THEN: Rectangle has zero width/height (invalid for selection)
    QVERIFY2(rect.width() == 0, "Zero-area selection should have zero width");
    QVERIFY2(!m_geom->isValidSelection(rect),
             "Zero-area selections should be invalid");
}

void SelectionGeometryTest::testHugeSelectionClampedToScreen()
{
    // GIVEN: Points beyond screen bounds
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    QPoint start(-1000, -1000);
    QPoint end(screenRect.width() + 1000, screenRect.height() + 1000);

    // WHEN: Creating and clamping selection rectangle
    QRect rect = m_geom->createRectangle(start, end);
    QRect clamped = m_geom->clampToScreen(rect);

    // THEN: Rectangle is clamped to screen
    QVERIFY(clamped.left() >= screenRect.left());
    QVERIFY(clamped.right() <= screenRect.right());
    QVERIFY(clamped.top() >= screenRect.top());
    QVERIFY(clamped.bottom() <= screenRect.bottom());
}

void SelectionGeometryTest::testResizeHandlePositions()
{
    // GIVEN: Selection rectangle
    QRect selection(100, 100, 200, 200);

    // WHEN: Calculating resize handle positions
    QList<QPoint> handles = m_geom->resizeHandlePositions(selection);

    // THEN: All 8 handles are present
    QCOMPARE(handles.size(), 8);

    // Verify corner handles
    QVERIFY(handles.contains(selection.topLeft()));
    QVERIFY(handles.contains(selection.topRight()));
    QVERIFY(handles.contains(selection.bottomLeft()));
    QVERIFY(handles.contains(selection.bottomRight()));

    // Verify center positions
    QVERIFY(handles.contains(QPoint(200, 100))); // Top center
    QVERIFY(handles.contains(QPoint(300, 200))); // Right center
    QVERIFY(handles.contains(QPoint(200, 300))); // Bottom center
    QVERIFY(handles.contains(QPoint(100, 200))); // Left center
}

void SelectionGeometryTest::testResizeHandleAtPosition()
{
    // GIVEN: Selection rectangle
    QRect selection(100, 100, 200, 200);

    // WHEN: Checking for handle at exact position
    int hitHandle = m_geom->handleAtPosition(selection, selection.topLeft(), 10);

    // THEN: Should return top-left handle index
    QCOMPARE(hitHandle, static_cast<int>(SelectionGeometry::TopLeft));

    // WHEN: Checking for handle at non-handle position
    int noHit = m_geom->handleAtPosition(selection, QPoint(50, 50), 10);

    // THEN: Should return -1 (no hit)
    QCOMPARE(noHit, -1);
}

void SelectionGeometryTest::testResizeFromHandle()
{
    // GIVEN: Selection rectangle
    QRect original(100, 100, 200, 200);

    // WHEN: Resizing from bottom-right handle
    QPoint newPos(400, 400);
    QRect resized = m_geom->resizeFromHandle(original,
                                              SelectionGeometry::BottomRight,
                                              newPos);

    // THEN: Rectangle should be resized
    QCOMPARE(resized.topLeft(), original.topLeft());
    QCOMPARE(resized.bottomRight(), newPos);
    QCOMPARE(resized.width(), 300);
    QCOMPARE(resized.height(), 300);
}

void SelectionGeometryTest::testResizeFromCornerHandle()
{
    // GIVEN: Selection rectangle
    QRect original(100, 100, 200, 200);

    // WHEN: Resizing from top-left handle
    QPoint newPos(50, 50);
    QRect resized = m_geom->resizeFromHandle(original,
                                              SelectionGeometry::TopLeft,
                                              newPos);

    // THEN: Rectangle should be resized
    QCOMPARE(resized.topLeft(), newPos);
    QCOMPARE(resized.bottomRight(), original.bottomRight());
}

void SelectionGeometryTest::testResizeFromEdgeHandle()
{
    // GIVEN: Selection rectangle
    QRect original(100, 100, 200, 200);

    // WHEN: Resizing from top edge handle
    QPoint newPos(150, 50); // x should be ignored for edge
    QRect resized = m_geom->resizeFromHandle(original,
                                              SelectionGeometry::Top,
                                              newPos);

    // THEN: Only top coordinate should change
    QCOMPARE(resized.top(), newPos.y());
    QCOMPARE(resized.bottom(), original.bottom());
    QCOMPARE(resized.left(), original.left());
    QCOMPARE(resized.right(), original.right());
}

void SelectionGeometryTest::testShiftResizingMaintainsAspectRatio()
{
    // GIVEN: Selection rectangle with 2:1 aspect ratio
    QRect original(100, 100, 300, 200); // 200x100 = 2:1 ratio
    double originalAspect = static_cast<double>(original.width()) / original.height();

    // WHEN: Resizing from bottom-right corner with aspect ratio constraint
    QPoint newPos(400, 250); // Move to 300x150 area
    QRect resized = m_geom->resizeFromHandle(original,
                                              SelectionGeometry::BottomRight,
                                              newPos,
                                              true); // maintain aspect ratio

    // THEN: Aspect ratio should be preserved
    double newAspect = static_cast<double>(resized.width()) / resized.height();
    QVERIFY2(qAbs(newAspect - originalAspect) < 0.01,
              "Aspect ratio should be maintained (within 1% tolerance)");

    // AND: Result should be clamped to screen
    QVERIFY2(m_geom->isValidSelection(resized), "Resized rectangle should be valid");
}

void SelectionGeometryTest::testResizeConstrainedToScreen()
{
    // GIVEN: Selection rectangle
    QRect original(100, 100, 200, 200);

    // WHEN: Resizing beyond screen bounds
    QPoint newPos(5000, 5000); // Way beyond screen
    QRect resized = m_geom->resizeFromHandle(original,
                                              SelectionGeometry::BottomRight,
                                              newPos);

    // THEN: Should be clamped to screen bounds
    QVERIFY2(m_geom->isValidSelection(resized), "Should be clamped to screen");

    QRect screenGeom = QGuiApplication::primaryScreen()->geometry();
    QVERIFY2(resized.right() <= screenGeom.right(), "Should not exceed screen right");
    QVERIFY2(resized.bottom() <= screenGeom.bottom(), "Should not exceed screen bottom");
}

QTEST_MAIN(SelectionGeometryTest)
#include "SelectionGeometryTest.moc"
