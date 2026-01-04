/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation Renderer
 *  Story 1.3 - Render annotations in captured image
 *  Task 8: Burn-in annotations to final screenshot
 */

#include "AnnotationRenderer.h"

#ifdef Q_OS_WIN

#include <QPen>
#include <QBrush>
#include <QPainterPath>

QImage AnnotationRenderer::renderAnnotations(const QImage &sourceImage,
                                             const QVector<Annotation> &annotations)
{
    if (sourceImage.isNull()) {
        qWarning("Cannot render annotations on null image");
        return sourceImage;
    }

    // Create a copy of the source image
    QImage result = sourceImage.copy();
    QPainter painter(&result);

    // Enable antialiasing for smooth rendering
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Render each annotation
    for (const Annotation &annotation : annotations) {
        renderAnnotation(painter, annotation);
    }

    painter.end();

    qDebug("Rendered %d annotations onto %dx%d image",
           annotations.count(), result.width(), result.height());

    return result;
}

QImage AnnotationRenderer::renderFromModel(const QImage &sourceImage,
                                           AnnotationListModel *model)
{
    if (!model) {
        qWarning("Cannot render annotations: null model");
        return sourceImage;
    }

    QVector<Annotation> annotations;

    // Extract all annotations from the model
    for (int i = 0; i < model->rowCount(); ++i) {
        Annotation annotation = model->getAnnotation(i);
        if (annotation.type() != AnnotationTool::None) {
            annotations.append(annotation);
        }
    }

    return renderAnnotations(sourceImage, annotations);
}

void AnnotationRenderer::renderAnnotation(QPainter &painter,
                                          const Annotation &annotation)
{
    // Set up pen with annotation color and stroke width
    QPen pen(annotation.color());
    pen.setWidth(annotation.strokeWidth());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    switch (annotation.type()) {
    case AnnotationTool::FreeDraw:
        // Render free draw path
        if (annotation.points().count() > 1) {
            QPainterPath path;
            const QVector<QPoint> &points = annotation.points();

            path.moveTo(points[0]);
            for (int i = 1; i < points.count(); ++i) {
                path.lineTo(points[i]);
            }

            painter.drawPath(path);
        }
        break;

    case AnnotationTool::Box:
        // Render box/rectangle annotation
        if (!annotation.boundingBox().isNull()) {
            painter.drawRect(annotation.boundingBox());
        }
        break;

    case AnnotationTool::Circle:
        // Render circle/ellipse annotation
        if (!annotation.boundingBox().isNull()) {
            painter.drawEllipse(annotation.boundingBox());
        }
        break;

    case AnnotationTool::Arrow:
        // TODO: Implement arrow rendering
        qWarning("Arrow annotation rendering not yet implemented");
        break;

    case AnnotationTool::Text:
        // TODO: Implement text rendering
        qWarning("Text annotation rendering not yet implemented");
        break;

    case AnnotationTool::None:
        // Skip annotations with no type
        break;

    default:
        qWarning("Unknown annotation type: %d", static_cast<int>(annotation.type()));
        break;
    }
}

#endif // Q_OS_WIN
