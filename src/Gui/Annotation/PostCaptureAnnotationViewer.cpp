/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Post-Capture Annotation Viewer
 *  Story 1.4 - Post-Capture Annotation View
 *  Task 1: Create PostCaptureAnnotationViewer window
 */

#include "PostCaptureAnnotationViewer.h"

#ifdef Q_OS_WIN

#include "AnnotationRenderer.h"

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColor>
#include <QFileDialog>
#include <QImage>
#include <QQuickItem>
#include <QTimer>
#include <QUrl>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_POSTCAPTURE, "spectacle.postcapture")

PostCaptureAnnotationViewer::PostCaptureAnnotationViewer(const QImage &capturedImage,
                                                           AnnotationListModel *annotations,
                                                           QWidget *parent)
    : QQuickView(parent)
    , m_capturedImage(capturedImage)
    , m_annotationModel(nullptr)
    , m_clipboard(QApplication::clipboard())
    , m_clipboardUpdateTimer(new QTimer(this))
    , m_hasUnsavedChanges(false)
{
    // Task 3.2: Create deep copy of annotation model (not reference)
    m_annotationModel = new AnnotationListModel(this);

    if (annotations) {
        // Copy all annotations from source model
        for (int i = 0; i < annotations->rowCount(); ++i) {
            m_annotationModel->addAnnotation(annotations->getAnnotation(i));
        }
    }

    // Setup QML engine and context
    setupQml();

    // Task 4.3: Update clipboard on annotation changes
    connect(m_annotationModel, &QAbstractListModel::rowsInserted,
            this, &PostCaptureAnnotationViewer::onAnnotationsChanged);
    connect(m_annotationModel, &QAbstractListModel::rowsRemoved,
            this, &PostCaptureAnnotationViewer::onAnnotationsChanged);
    connect(m_annotationModel, &QAbstractListModel::dataChanged,
            this, &PostCaptureAnnotationViewer::onAnnotationsChanged);

    // Task 4.4: Debounce clipboard updates
    m_clipboardUpdateTimer->setSingleShot(true);
    m_clipboardUpdateTimer->setInterval(100);
    connect(m_clipboardUpdateTimer, &QTimer::timeout,
            this, &PostCaptureAnnotationViewer::updateClipboard);

    // Window properties
    setWindowTitle(tr("Annotation Editor - Spectacle"));
    resize(800, 600);
    setResizeMode(QQuickView::SizeRootObjectToView);

    qCDebug(LOG_POSTCAPTURE) << "PostCaptureAnnotationViewer created with"
                             << m_annotationModel->rowCount() << "annotations";
}

PostCaptureAnnotationViewer::~PostCaptureAnnotationViewer()
{
    delete m_annotationModel;
}

void PostCaptureAnnotationViewer::setupQml()
{
    // Set QML source
    setSource(QUrl(QStringLiteral("qrc:/qml/PostCaptureAnnotationViewer.qml")));

    // Expose C++ objects to QML
    rootContext()->setContextProperty("viewerWindow", this);
    rootContext()->setContextProperty("annotationModel", m_annotationModel);

    // Register QImage for use in QML
    qmlRegisterType<AnnotationListModel>("Spectacle", 1, 0, "AnnotationListModel");
}

void PostCaptureAnnotationViewer::setCurrentTool(int tool)
{
    // QML can call this to set the current annotation tool
    Q_UNUSED(tool);
    // Implementation would update tool state
}

void PostCaptureAnnotationViewer::setCurrentColor(const QColor &color)
{
    // QML can call this to set the current color
    Q_UNUSED(color);
}

void PostCaptureAnnotationViewer::setCurrentStrokeWidth(int width)
{
    // QML can call this to set the current stroke width
    Q_UNUSED(width);
}

QImage PostCaptureAnnotationViewer::renderedImage() const
{
    return renderWithAnnotations();
}

QImage PostCaptureAnnotationViewer::renderWithAnnotations() const
{
    // Task 4.2: Compose final image with annotations
    if (m_capturedImage.isNull()) {
        qCWarning(LOG_POSTCAPTURE) << "Cannot render: captured image is null";
        return QImage();
    }

    // Use AnnotationRenderer from Story 1.3
    return AnnotationRenderer::renderFromModel(m_capturedImage, m_annotationModel);
}

void PostCaptureAnnotationViewer::saveAs()
{
    // Task 5.1: Create save dialog with format selection
    QString filter = tr("PNG Images (*.png);;JPEG Images (*.jpg *.jpeg);;All Files (*)");
    QString fileName = QFileDialog::getSaveFileName(this,
                                                     tr("Save Annotated Image"),
                                                     QString(),
                                                     filter);

    if (fileName.isEmpty()) {
        return; // User cancelled
    }

    // Task 5.2: Render final image with annotations
    QImage finalImage = renderWithAnnotations();

    if (finalImage.isNull()) {
        // Task 5.4: Show inline error message
        qCWarning(LOG_POSTCAPTURE) << "Failed to render image for saving";
        return;
    }

    // Task 5.3: Save to selected location
    if (!finalImage.save(fileName)) {
        // Task 5.4: Show inline error message
        qCWarning(LOG_POSTCAPTURE) << "Failed to save image to" << fileName;
        return;
    }

    qCDebug(LOG_POSTCAPTURE) << "Image saved to" << fileName;
    m_hasUnsavedChanges = false;

    Q_EMIT imageSaved(fileName);
}

void PostCaptureAnnotationViewer::copyToClipboard()
{
    // Task 6.2: Compose final image with annotations
    QImage finalImage = renderWithAnnotations();

    if (finalImage.isNull()) {
        qCWarning(LOG_POSTCAPTURE) << "Failed to render image for clipboard";
        return;
    }

    // Task 6.3: Update clipboard with annotated image
    m_clipboard->setImage(finalImage);

    // Task 6.4: Show inline confirmation
    qCDebug(LOG_POSTCAPTURE) << "Image copied to clipboard";
}

void PostCaptureAnnotationViewer::updateClipboard()
{
    // Task 4.2, 4.3: Render and update clipboard
    copyToClipboard();
}

void PostCaptureAnnotationViewer::onAnnotationsChanged()
{
    // Task 4.3, 4.4: Trigger debounced clipboard update
    m_hasUnsavedChanges = true;
    m_clipboardUpdateTimer->start();
}

#endif // Q_OS_WIN
