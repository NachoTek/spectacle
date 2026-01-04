/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Post-Capture Annotation Viewer
 *  Story 1.4 - Post-Capture Annotation View
 *  Task 1: Create PostCaptureAnnotationViewer window
 */

#ifndef POSTCAPTUREANNOTATIONVIEWER_H
#define POSTCAPTUREANNOTATIONVIEWER_H

#include "AnnotationListModel.h"

#include <QImage>
#include <QObject>
#include <QQuickView>

#ifdef Q_OS_WIN

class QClipboard;
class QTimer;

/**
 * @brief Post-capture annotation viewer window
 *
 * Provides a Qt Quick window for editing annotations after capture.
 * Displays the captured image with annotation overlay and provides
 * Save As and Copy to Clipboard functionality.
 */
class PostCaptureAnnotationViewer : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(QImage capturedImage READ capturedImage CONSTANT)
    Q_PROPERTY(AnnotationListModel* annotationModel READ annotationModel CONSTANT)

public:
    /**
     * @brief Construct viewer with captured image and annotations
     * @param capturedImage The screenshot to annotate
     * @param annotations Initial annotations (will be copied)
     * @param parent Parent widget
     */
    explicit PostCaptureAnnotationViewer(const QImage &capturedImage,
                                         AnnotationListModel *annotations,
                                         QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PostCaptureAnnotationViewer() override;

    /**
     * @brief Get the captured image
     */
    QImage capturedImage() const { return m_capturedImage; }

    /**
     * @brief Get the annotation model
     */
    AnnotationListModel* annotationModel() const { return m_annotationModel; }

    /**
     * @brief Set current annotation tool
     */
    Q_INVOKABLE void setCurrentTool(int tool);

    /**
     * @brief Set current annotation color
     */
    Q_INVOKABLE void setCurrentColor(const QColor &color);

    /**
     * @brief Set current stroke width
     */
    Q_INVOKABLE void setCurrentStrokeWidth(int width);

    /**
     * @brief Get current rendered image (with annotations)
     */
    QImage renderedImage() const;

    /**
     * @brief Request window close (triggers confirmation if unsaved changes)
     */
    Q_INVOKABLE void requestClose();

protected:
    /**
     * @brief Handle close event with unsaved changes confirmation (Story 1.4 - CRITICAL #2 fix)
     */
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    /**
     * @brief Emitted when image is saved
     */
    void imageSaved(const QString &path);

public Q_SLOTS:
    /**
     * @brief Save As dialog
     */
    void saveAs();

    /**
     * @brief Copy to clipboard
     */
    void copyToClipboard();

    /**
     * @brief Update clipboard with rendered image
     */
    void updateClipboard();

private Q_SLOTS:
    /**
     * @brief Handle annotation model changes
     */
    void onAnnotationsChanged();

private:
    /**
     * @brief Setup QML context and engine
     */
    void setupQml();

    /**
     * @brief Render image with annotations
     */
    QImage renderWithAnnotations() const;

    QImage m_capturedImage;
    AnnotationListModel *m_annotationModel;
    QClipboard *m_clipboard;
    QTimer *m_clipboardUpdateTimer;
    bool m_hasUnsavedChanges;
};

#endif // Q_OS_WIN

#endif // POSTCAPTUREANNOTATIONVIEWER_H
