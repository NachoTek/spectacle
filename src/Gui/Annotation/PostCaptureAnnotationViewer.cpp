/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Post-Capture Annotation Viewer
 *  Story 1.4 - Post-Capture Annotation View
 *  Task 1: Create PostCaptureAnnotationViewer window
 *  Story 1.5 - Saving & Defaults (Save As integration)
 */

#include "PostCaptureAnnotationViewer.h"

#ifdef Q_OS_WIN

#include "AnnotationRenderer.h"
#include "CapturedImageProvider.h"
#include "../Settings/SettingsManager.h"  // Story 1.5: Settings manager

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColor>
#include <QFileDialog>
#include <QQmlContext>  // For QQmlContext methods in setupQml()
#include <QGuiApplication>
#include <QImage>
#include <QQuickItem>
#include <QMessageBox>
#include <QScreen>
#include <QStyle>
#include <QTimer>
#include <QUrl>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_POSTCAPTURE, "spectacle.postcapture")

PostCaptureAnnotationViewer::PostCaptureAnnotationViewer(const QImage &capturedImage,
                                                           AnnotationListModel *annotations,
                                                           QWindow *parent)
    : QQuickView(parent)
    , m_capturedImage(capturedImage)
    , m_annotationModel(nullptr)
    , m_clipboard(QGuiApplication::clipboard())  // Use QGuiApplication for Qt Quick
    , m_clipboardUpdateTimer(new QTimer(this))
    , m_hasUnsavedChanges(false)
    , m_imageProvider(nullptr)  // CRITICAL #3: Will be initialized in setupQml
    , m_currentTool(static_cast<int>(AnnotationTool::None))  // MINOR #13: Initialize tool state
    , m_currentColor(QColor("#FF0000"))  // MINOR #13: Default red color
    , m_currentStrokeWidth(2)  // MINOR #13: Default 2px stroke
    , m_settingsManager(new SettingsManager(this))  // Story 1.5: Initialize settings manager
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
    setTitle(tr("Annotation Editor - Spectacle"));
    resize(800, 600);
    setResizeMode(QQuickView::SizeRootObjectToView);

    // MAJOR #9: QQuickView windows are resizable by default
    // No need to set window flags like with QWidget

    // MAJOR #6: Center window on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    QRect centeredRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), screenGeometry);
    setGeometry(centeredRect);

    qCDebug(LOG_POSTCAPTURE) << "PostCaptureAnnotationViewer created with"
                             << m_annotationModel->rowCount() << "annotations";
}

PostCaptureAnnotationViewer::~PostCaptureAnnotationViewer()
{
    delete m_annotationModel;
}

void PostCaptureAnnotationViewer::setupQml()
{
    // CRITICAL #3: Create and register image provider for QML
    m_imageProvider = new CapturedImageProvider();
    engine()->addImageProvider(QLatin1String("capture"), m_imageProvider);

    // Register captured image with provider using unique ID
    QString imageId = QString::number(reinterpret_cast<quintptr>(this));
    m_imageProvider->setImage(imageId, m_capturedImage);
    rootContext()->setContextProperty(QStringLiteral("capturedImageId"), imageId);

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
    // MINOR #13: Set the current annotation tool
    if (m_currentTool != tool) {
        m_currentTool = tool;
        qCDebug(LOG_POSTCAPTURE) << "Current tool changed to:" << tool;
        Q_EMIT currentToolChanged();
    }
}

void PostCaptureAnnotationViewer::setCurrentColor(const QColor &color)
{
    // MINOR #13: Set the current annotation color
    if (m_currentColor != color) {
        m_currentColor = color;
        qCDebug(LOG_POSTCAPTURE) << "Current color changed to:" << color.name();
        Q_EMIT currentColorChanged();
    }
}

void PostCaptureAnnotationViewer::setCurrentStrokeWidth(int width)
{
    // MINOR #13: Set the current stroke width
    if (m_currentStrokeWidth != width) {
        m_currentStrokeWidth = width;
        qCDebug(LOG_POSTCAPTURE) << "Current stroke width changed to:" << width;
        Q_EMIT currentStrokeWidthChanged();
    }
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
    // Task 4.2: Default to last used location or default location from SettingsManager
    QString defaultLocation = m_settingsManager->getSaveLocation();

    // Task 4.3: Default to last used format or default format from SettingsManager
    QString defaultFormat = m_settingsManager->defaultFormat();

    // Task 4.1: Create save dialog with format selection
    QString filter = tr("PNG Images (*.png);;JPEG Images (*.jpg *.jpeg);;All Files (*)");

    // Set default directory to the configured save location
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                     tr("Save Annotated Image"),
                                                     defaultLocation,
                                                     filter);

    if (fileName.isEmpty()) {
        return; // User cancelled
    }

    // Render final image with annotations
    QImage finalImage = renderWithAnnotations();

    if (finalImage.isNull()) {
        // MAJOR #7: Emit error signal for rendering failure
        qCWarning(LOG_POSTCAPTURE) << "Failed to render image for saving";
        Q_EMIT saveError(tr("Failed to render image"));
        return;
    }

    // Save to selected location
    if (!finalImage.save(fileName)) {
        // MAJOR #7: Emit error signal for save failure
        qCWarning(LOG_POSTCAPTURE) << "Failed to save image to" << fileName;
        Q_EMIT saveError(tr("Failed to save image to ") + fileName);
        return;
    }

    // Task 4.4: Remember user's manual save choice for next time
    QFileInfo fileInfo(fileName);
    m_settingsManager->setLastSaveLocation(fileInfo.absolutePath());

    // Extract and remember the format
    QString extension = fileInfo.suffix().toLower();
    if (extension.isEmpty()) {
        extension = QLatin1String("png");  // Default to PNG if no extension
    }
    m_settingsManager->setLastFormat(extension);

    qCDebug(LOG_POSTCAPTURE) << "Image saved to" << fileName;
    m_hasUnsavedChanges = false;

    Q_EMIT imageSaved(fileName);
    Q_EMIT imageCopied();  // Reuse signal for generic success notification
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

    // Task 6.4: Show inline confirmation (MAJOR #7)
    qCDebug(LOG_POSTCAPTURE) << "Image copied to clipboard";
    Q_EMIT imageCopied();
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

// Story 1.4: CRITICAL #2 Fix - Unsaved changes warning
void PostCaptureAnnotationViewer::requestClose()
{
    close();  // This will trigger closeEvent with confirmation logic
}

void PostCaptureAnnotationViewer::closeEvent(QCloseEvent *event)
{
    // Task 7.5: Show confirmation if unsaved changes
    if (m_hasUnsavedChanges) {
        QMessageBox::StandardButton response = QMessageBox::question(
            nullptr,
            tr("Unsaved Changes"),
            tr("You have unsaved annotations. Close anyway?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (response == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();
    QQuickView::closeEvent(event);
}

#endif // Q_OS_WIN
