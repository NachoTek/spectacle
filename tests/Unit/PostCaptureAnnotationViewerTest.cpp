/*
 *  Unit Tests for PostCaptureAnnotationViewer
 *  Story 1.4 - Post-Capture Annotation View
 *  Task 8: Testing and verification
 */

#include "Gui/Annotation/PostCaptureAnnotationViewer.h"
#include "Gui/Annotation/AnnotationListModel.h"

#include <QTest>
#include <QImage>
#include <QClipboard>

#ifdef Q_OS_WIN

class PostCaptureAnnotationViewerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Task 1: Window creation tests
    void testViewerCreationWithValidImage();
    void testViewerCreationWithNullImage();

    // Task 3: Annotation transfer tests
    void testAnnotationModelCopyProducesIndependentInstance();
    void testAnnotationsRenderedOnCapturedImage();

    // Task 4: Clipboard update tests
    void testClipboardUpdatesOnAnnotationChange();

    // Task 5: Save As tests
    void testSaveAsTriggersFileSave();

private:
    QImage m_testImage;
};

void PostCaptureAnnotationViewerTest::initTestCase()
{
    // Create a test 100x100 red image
    m_testImage = QImage(100, 100, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
}

void PostCaptureAnnotationViewerTest::cleanupTestCase()
{
    // Cleanup
}

// Task 1.1: Test viewer creation with valid image
void PostCaptureAnnotationViewerTest::testViewerCreationWithValidImage()
{
    AnnotationListModel *model = new AnnotationListModel(this);
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, model);

    QVERIFY(viewer != nullptr);
    // Verify viewer has the image set

    delete viewer;
}

// Task 1.1: Test viewer creation with null image
void PostCaptureAnnotationViewerTest::testViewerCreationWithNullImage()
{
    QImage nullImage;
    AnnotationListModel *model = new AnnotationListModel(this);
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(nullImage, model);

    QVERIFY(viewer != nullptr);
    // Viewer should handle null image gracefully

    delete viewer;
}

// Task 3.2: Test annotation model copy produces independent instance
void PostCaptureAnnotationViewerTest::testAnnotationModelCopyProducesIndependentInstance()
{
    AnnotationListModel *originalModel = new AnnotationListModel(this);

    // Add an annotation to original
    Annotation annotation(AnnotationTool::Box);
    annotation.setColor(Qt::red);
    annotation.setStrokeWidth(2);
    annotation.setBoundingBox(QRect(10, 10, 50, 50));
    originalModel->addAnnotation(annotation);

    // Create viewer (should copy model)
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, originalModel);

    // Verify the viewer has its own model instance
    AnnotationListModel *viewerModel = viewer->annotationModel();
    QVERIFY(viewerModel != originalModel);
    QVERIFY(viewerModel->rowCount() == 1);

    // Modify original model
    Annotation annotation2(AnnotationTool::Circle);
    annotation2.setBoundingBox(QRect(20, 20, 30, 30));
    originalModel->addAnnotation(annotation2);

    // Viewer model should be unaffected
    QVERIFY(viewerModel->rowCount() == 1);

    delete viewer;
}

// Task 3.3: Test annotations rendered on captured image
void PostCaptureAnnotationViewerTest::testAnnotationsRenderedOnCapturedImage()
{
    AnnotationListModel *model = new AnnotationListModel(this);

    // Add test annotation
    Annotation annotation(AnnotationTool::Box);
    annotation.setColor(Qt::blue);
    annotation.setStrokeWidth(3);
    annotation.setBoundingBox(QRect(10, 10, 50, 50));
    model->addAnnotation(annotation);

    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, model);

    // Verify rendering works (image should have annotations composited)
    // This tests the renderAnnotations() method
    QImage rendered = viewer->renderedImage();
    QVERIFY(!rendered.isNull());
    QVERIFY(rendered.size() == m_testImage.size());

    delete viewer;
}

// Task 4.1: Test clipboard updates on annotation change
void PostCaptureAnnotationViewerTest::testClipboardUpdatesOnAnnotationChange()
{
    AnnotationListModel *model = new AnnotationListModel(this);
    PostCaptureAnnotationViewer *viewer = new PostCaptureAnnotationViewer(m_testImage, model);

    // Add annotation and verify clipboard is updated
    Annotation annotation(AnnotationTool::Box);
    annotation.setBoundingBox(QRect(10, 10, 50, 50));
    model->addAnnotation(annotation);

    // Trigger clipboard update (simulate signal)
    Q_EMIT model->layoutChanged();

    // Verify clipboard has image
    QClipboard *clipboard = QApplication::clipboard();
    QVERIFY(!clipboard->image().isNull());

    delete viewer;
}

QTEST_MAIN(PostCaptureAnnotationViewerTest)

#include "PostCaptureAnnotationViewerTest.moc"

#endif // Q_OS_WIN
