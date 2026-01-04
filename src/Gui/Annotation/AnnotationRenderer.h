/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation Renderer
 *  Story 1.3 - Render annotations in captured image
 *  Task 8: Burn-in annotations to final screenshot
 */

#ifndef ANNOTATIONRENDERER_H
#define ANNOTATIONRENDERER_H

#include "Annotation.h"
#include "AnnotationListModel.h"

#include <QImage>
#include <QPainter>
#include <QVector>

#ifdef Q_OS_WIN

/**
 * @brief Renders annotations onto a QImage for capture output
 *
 * Takes a source image and a list of annotations, then renders
 * each annotation onto the image using QPainter. This is used to
 * "burn in" annotations to the final captured screenshot.
 */
class AnnotationRenderer
{
public:
    /**
     * @brief Render annotations onto image
     * @param sourceImage The captured screenshot
     * @param annotations List of annotations to render
     * @return Image with annotations rendered
     */
    static QImage renderAnnotations(const QImage &sourceImage,
                                    const QVector<Annotation> &annotations);

    /**
     * @brief Render annotations from model onto image
     * @param sourceImage The captured screenshot
     * @param model The annotation list model
     * @return Image with annotations rendered
     */
    static QImage renderFromModel(const QImage &sourceImage,
                                  AnnotationListModel *model);

private:
    /**
     * @brief Render a single annotation
     * @param painter The QPainter to use
     * @param annotation The annotation to render
     */
    static void renderAnnotation(QPainter &painter,
                                 const Annotation &annotation);
};

#endif // Q_OS_WIN

#endif // ANNOTATIONRENDERER_H
