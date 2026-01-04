/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Unit Tests for Annotation Data Model
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#include <QTest>
#include <QPoint>
#include <QRect>
#include <QUuid>
#include <QColor>

#include "Gui/Annotation/Annotation.h"

using namespace Qt::StringLiterals;

class AnnotationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qInfo("Starting Annotation data model tests");
    }

    void cleanupTestCase()
    {
        qInfo("Annotation data model tests completed");
    }

    // Test 1.1: AnnotationType enum values
    void testAnnotationTypeEnumValues()
    {
        // Verify enum has correct values
        QCOMPARE(static_cast<int>(AnnotationTool::None), 0);
        QCOMPARE(static_cast<int>(AnnotationTool::FreeDraw), 1);
        QCOMPARE(static_cast<int>(AnnotationTool::Arrow), 2);
        QCOMPARE(static_cast<int>(AnnotationTool::Box), 3);
        QCOMPARE(static_cast<int>(AnnotationTool::Circle), 4);
        QCOMPARE(static_cast<int>(AnnotationTool::Text), 5);
    }

    // Test 1.2: Annotation construction and properties
    void testAnnotationConstruction()
    {
        Annotation annotation(AnnotationTool::Arrow);

        // Verify default properties
        QCOMPARE(annotation.type(), AnnotationTool::Arrow);
        QCOMPARE(annotation.color(), QColor(Qt::red));
        QCOMPARE(annotation.strokeWidth(), 2);
        QVERIFY(!annotation.uuid().isNull());
    }

    void testAnnotationProperty setters()
    {
        Annotation annotation(AnnotationTool::FreeDraw);

        // Set color
        annotation.setColor(QColor(Qt::blue));
        QCOMPARE(annotation.color(), QColor(Qt::blue));

        // Set stroke width
        annotation.setStrokeWidth(4);
        QCOMPARE(annotation.strokeWidth(), 4);

        // Set points for free draw
        QVector<QPoint> points;
        points << QPoint(10, 10) << QPoint(20, 20) << QPoint(30, 30);
        annotation.setPoints(points);
        QCOMPARE(annotation.points(), points);
    }

    void testAnnotationBoundingBox()
    {
        Annotation annotation(AnnotationTool::Box);

        // Set bounding box
        QRect bbox(10, 10, 100, 50);
        annotation.setBoundingBox(bbox);

        QCOMPARE(annotation.boundingBox(), bbox);
    }

    void testAnnotationUuidIsUnique()
    {
        Annotation annotation1(AnnotationTool::Arrow);
        Annotation annotation2(AnnotationTool::Arrow);

        // Each annotation should have unique UUID
        QVERIFY(annotation1.uuid() != annotation2.uuid());
    }

    void testAnnotationCopy()
    {
        Annotation original(AnnotationTool::Arrow);
        original.setColor(QColor(Qt::green));
        original.setStrokeWidth(5);

        Annotation copy = original;

        // Verify copy has same properties
        QCOMPARE(copy.type(), original.type());
        QCOMPARE(copy.color(), original.color());
        QCOMPARE(copy.strokeWidth(), original.strokeWidth());
    }
};

QTEST_MAIN(AnnotationTest)

#include "AnnotationTest.moc"

#endif // Q_OS_WIN
