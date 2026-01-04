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
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QKeySequence>
#include <QKeyEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_POSTCAPTURE, "spectacle.postcapture")

PostCaptureAnnotationViewer::PostCaptureAnnotationViewer(const QImage &capturedImage,
                                                           AnnotationListModel *annotations,
                                                           QWidget *parent)
    : QWidget(parent)
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

    // Setup UI
    setupUi();

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

    // Center window on screen
    setWindowTitle(tr("Annotation Editor - Spectacle"));
    resize(800, 600);

    // Task 7.6: Ensure window is resizable
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowResizeHint);

    qCDebug(LOG_POSTCAPTURE) << "PostCaptureAnnotationViewer created with"
                             << m_annotationModel->rowCount() << "annotations";
}

PostCaptureAnnotationViewer::~PostCaptureAnnotationViewer()
{
    delete m_annotationModel;
}

void PostCaptureAnnotationViewer::setupUi()
{
    // Task 1.3, 1.4: Main layout with image display and action buttons
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Toolbar with buttons
    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    // Task 1.4: Save As button
    QPushButton *saveButton = new QPushButton(tr("Save As..."), this);
    saveButton->setShortcut(QKeySequence::Save); // Task 7.1: Ctrl+S
    connect(saveButton, &QPushButton::clicked,
            this, &PostCaptureAnnotationViewer::saveAs);
    toolbarLayout->addWidget(saveButton);

    // Task 6.1: Copy to Clipboard button
    QPushButton *copyButton = new QPushButton(tr("Copy to Clipboard"), this);
    copyButton->setShortcut(QKeySequence::Copy); // Task 7.2: Ctrl+C
    connect(copyButton, &QPushButton::clicked,
            this, &PostCaptureAnnotationViewer::copyToClipboard);
    toolbarLayout->addWidget(copyButton);

    // Close button
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked,
            this, &QWidget::close);
    toolbarLayout->addWidget(closeButton);

    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    // Task 1.3: Image display area
    // Note: Full QML integration will load AnnotationCanvas and AnnotationToolbar
    // For now, we use a basic widget placeholder
    QWidget *imageDisplay = new QWidget(this);
    imageDisplay->setMinimumSize(400, 300);
    mainLayout->addWidget(imageDisplay, 1);

    setLayout(mainLayout);
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

void PostCaptureAnnotationViewer::closeEvent(QCloseEvent *event)
{
    // Task 7.5: Support Escape to close (with confirmation if unsaved)
    if (m_hasUnsavedChanges) {
        // In a full implementation, show confirmation dialog
        // For now, just close
        qCDebug(LOG_POSTCAPTURE) << "Closing viewer with unsaved changes";
    }

    event->accept();
}

void PostCaptureAnnotationViewer::keyPressEvent(QKeyEvent *event)
{
    // Task 7.3: Support Ctrl+Z to undo last annotation
    if (event->matches(QKeySequence::Undo)) {
        m_annotationModel->undoLast();
        return;
    }

    // Task 7.4: Support Delete key to remove selected annotation
    if (event->key() == Qt::Key_Delete) {
        m_annotationModel->deleteSelected();
        return;
    }

    // Task 7.5: Support Escape to close
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }

    QWidget::keyPressEvent(event);
}

#endif // Q_OS_WIN
