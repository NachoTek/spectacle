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
#include <QImage>
#include <QPainter>

#include "Gui/Annotation/Annotation.h"
#include "Gui/Annotation/AnnotationListModel.h"
#include "Gui/Annotation/AnnotationRenderer.h"

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

    // Task 7: Test selection state
    void testAnnotationSelectionState()
    {
        Annotation annotation(AnnotationTool::Box);

        // Default: not selected
        QVERIFY(!annotation.isSelected());

        // Set selected
        annotation.setSelected(true);
        QVERIFY(annotation.isSelected());

        // Clear selection
        annotation.setSelected(false);
        QVERIFY(!annotation.isSelected());
    }

    // Task 7: Test translate method
    void testAnnotationTranslate()
    {
        // Test box annotation translation
        Annotation box(AnnotationTool::Box);
        QRect bbox(10, 20, 100, 50);
        box.setBoundingBox(bbox);
        box.translate(5, 10);

        QRect expected(15, 30, 100, 50);
        QCOMPARE(box.boundingBox(), expected);

        // Test free draw translation
        Annotation freeDraw(AnnotationTool::FreeDraw);
        QVector<QPoint> points;
        points << QPoint(10, 10) << QPoint(20, 20) << QPoint(30, 30);
        freeDraw.setPoints(points);
        freeDraw.translate(5, 10);

        QVector<QPoint> expectedPoints;
        expectedPoints << QPoint(15, 20) << QPoint(25, 30) << QPoint(35, 40);
        QCOMPARE(freeDraw.points(), expectedPoints);
    }

    // Task 7: Test copy preserves selection state
    void testAnnotationCopyPreservesSelection()
    {
        Annotation original(AnnotationTool::Circle);
        original.setSelected(true);

        Annotation copy = original;
        QVERIFY(copy.isSelected());
    }

    // Task 7: Test AnnotationListModel selection
    void testAnnotationListModelSelection()
    {
        AnnotationListModel model;

        // Add annotations
        Annotation ann1(AnnotationTool::Box);
        ann1.setBoundingBox(QRect(10, 10, 50, 50));
        model.addAnnotation(ann1);

        Annotation ann2(AnnotationTool::Circle);
        ann2.setBoundingBox(QRect(100, 100, 50, 50));
        model.addAnnotation(ann2);

        QCOMPARE(model.rowCount(), 2);

        // Select first annotation
        model.selectAnnotation(ann1.uuid());
        QCOMPARE(model.selectedAnnotation(), ann1.uuid());
        QVERIFY(model.getAnnotation(0).isSelected());
        QVERIFY(!model.getAnnotation(1).isSelected());

        // Select second annotation (first should be deselected)
        model.selectAnnotation(ann2.uuid());
        QCOMPARE(model.selectedAnnotation(), ann2.uuid());
        QVERIFY(!model.getAnnotation(0).isSelected());
        QVERIFY(model.getAnnotation(1).isSelected());

        // Clear selection
        model.clearSelection();
        QVERIFY(model.selectedAnnotation().isNull());
        QVERIFY(!model.getAnnotation(0).isSelected());
        QVERIFY(!model.getAnnotation(1).isSelected());
    }

    // Task 7: Test delete selected
    void testAnnotationListModelDeleteSelected()
    {
        AnnotationListModel model;

        Annotation ann1(AnnotationTool::Box);
        ann1.setBoundingBox(QRect(10, 10, 50, 50));
        model.addAnnotation(ann1);

        Annotation ann2(AnnotationTool::Circle);
        ann2.setBoundingBox(QRect(100, 100, 50, 50));
        model.addAnnotation(ann2);

        QCOMPARE(model.rowCount(), 2);

        // Select and delete first annotation
        model.selectAnnotation(ann1.uuid());
        model.deleteSelected();

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.getAnnotation(0).uuid(), ann2.uuid());
    }

    // Task 7: Test move selected
    void testAnnotationListModelMoveSelected()
    {
        AnnotationListModel model;

        Annotation ann1(AnnotationTool::Box);
        ann1.setBoundingBox(QRect(10, 10, 50, 50));
        model.addAnnotation(ann1);

        Annotation ann2(AnnotationTool::Circle);
        ann2.setBoundingBox(QRect(100, 100, 50, 50));
        model.addAnnotation(ann2);

        // Select first and move it
        model.selectAnnotation(ann1.uuid());
        model.moveSelected(10, 20);

        QRect expected(20, 30, 50, 50);
        QCOMPARE(model.getAnnotation(0).boundingBox(), expected);

        // Second annotation should not move
        QCOMPARE(model.getAnnotation(1).boundingBox(), QRect(100, 100, 50, 50));
    }

    // Task 8: Test AnnotationRenderer with box
    void testAnnotationRendererBox()
    {
        // Create test image
        QImage testImage(200, 200, QImage::Format_RGB32);
        testImage.fill(Qt::white);

        // Create box annotation
        Annotation box(AnnotationTool::Box);
        box.setColor(QColor(Qt::red));
        box.setStrokeWidth(3);
        box.setBoundingBox(QRect(50, 50, 100, 80));

        QVector<Annotation> annotations;
        annotations << box;

        // Render annotations
        QImage result = AnnotationRenderer::renderAnnotations(testImage, annotations);

        // Verify image is not null
        QVERIFY(!result.isNull());
        QCOMPARE(result.size(), testImage.size());
    }

    // Task 8: Test AnnotationRenderer with free draw
    void testAnnotationRendererFreeDraw()
    {
        // Create test image
        QImage testImage(200, 200, QImage::Format_RGB32);
        testImage.fill(Qt::white);

        // Create free draw annotation
        Annotation freeDraw(AnnotationTool::FreeDraw);
        freeDraw.setColor(QColor(Qt::blue));
        freeDraw.setStrokeWidth(2);

        QVector<QPoint> points;
        points << QPoint(10, 10) << QPoint(50, 50) << QPoint(100, 10);
        freeDraw.setPoints(points);

        QVector<Annotation> annotations;
        annotations << freeDraw;

        // Render annotations
        QImage result = AnnotationRenderer::renderAnnotations(testImage, annotations);

        // Verify image is not null
        QVERIFY(!result.isNull());
        QCOMPARE(result.size(), testImage.size());
    }

    // Task 8: Test AnnotationRenderer with circle
    void testAnnotationRendererCircle()
    {
        // Create test image
        QImage testImage(200, 200, QImage::Format_RGB32);
        testImage.fill(Qt::white);

        // Create circle annotation
        Annotation circle(AnnotationTool::Circle);
        circle.setColor(QColor(Qt::green));
        circle.setStrokeWidth(4);
        circle.setBoundingBox(QRect(50, 50, 100, 100));

        QVector<Annotation> annotations;
        annotations << circle;

        // Render annotations
        QImage result = AnnotationRenderer::renderAnnotations(testImage, annotations);

        // Verify image is not null
        QVERIFY(!result.isNull());
        QCOMPARE(result.size(), testImage.size());
    }

    // Task 8: Test AnnotationRenderer with multiple annotations
    void testAnnotationRendererMultiple()
    {
        // Create test image
        QImage testImage(300, 300, QImage::Format_RGB32);
        testImage.fill(Qt::white);

        // Create multiple annotations
        Annotation box(AnnotationTool::Box);
        box.setColor(Qt::red);
        box.setStrokeWidth(2);
        box.setBoundingBox(QRect(10, 10, 80, 60));

        Annotation circle(AnnotationTool::Circle);
        circle.setColor(Qt::blue);
        circle.setStrokeWidth(3);
        circle.setBoundingBox(QRect(100, 100, 80, 80));

        Annotation freeDraw(AnnotationTool::FreeDraw);
        freeDraw.setColor(Qt::green);
        freeDraw.setStrokeWidth(2);
        QVector<QPoint> points;
        points << QPoint(200, 200) << QPoint(250, 250) << QPoint(280, 220);
        freeDraw.setPoints(points);

        QVector<Annotation> annotations;
        annotations << box << circle << freeDraw;

        // Render annotations
        QImage result = AnnotationRenderer::renderAnnotations(testImage, annotations);

        // Verify image is not null
        QVERIFY(!result.isNull());
        QCOMPARE(result.size(), testImage.size());
    }

    // Task 8: Test AnnotationRenderer from model
    void testAnnotationRendererFromModel()
    {
        // Create test image
        QImage testImage(200, 200, QImage::Format_RGB32);
        testImage.fill(Qt::white);

        // Create model with annotations
        AnnotationListModel model;

        Annotation box(AnnotationTool::Box);
        box.setColor(Qt::red);
        box.setStrokeWidth(2);
        box.setBoundingBox(QRect(10, 10, 50, 50));
        model.addAnnotation(box);

        // Render from model
        QImage result = AnnotationRenderer::renderFromModel(testImage, &model);

        // Verify image is not null
        QVERIFY(!result.isNull());
        QCOMPARE(result.size(), testImage.size());
    }
};

QTEST_MAIN(AnnotationTest)

#include "AnnotationTest.moc"

#endif // Q_OS_WIN
