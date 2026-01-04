/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation Data Model
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "AnnotationTool.h"

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QVector>
#include <QUuid>

#ifdef Q_OS_WIN

/**
 * @brief Data model for a single annotation on the capture overlay
 *
 * Represents an annotation with its type, geometry, style properties,
 * and unique identifier. Used by both pre-capture and post-capture
 * annotation workflows.
 */
class Annotation
{
public:
    /**
     * @brief Construct annotation with specified type
     * @param type The annotation tool type
     */
    explicit Annotation(AnnotationTool type);

    /**
     * @brief Copy constructor
     */
    Annotation(const Annotation &other);

    /**
     * @brief Assignment operator
     */
    Annotation &operator=(const Annotation &other);

    ~Annotation();

    /**
     * @brief Get the annotation type
     */
    AnnotationTool type() const { return m_type; }

    /**
     * @brief Get annotation color
     */
    QColor color() const { return m_color; }

    /**
     * @brief Set annotation color
     */
    void setColor(const QColor &color) { m_color = color; }

    /**
     * @brief Get stroke width
     */
    int strokeWidth() const { return m_strokeWidth; }

    /**
     * @brief Set stroke width
     */
    void setStrokeWidth(int width) { m_strokeWidth = width; }

    /**
     * @brief Get annotation points (for free draw)
     */
    QVector<QPoint> points() const { return m_points; }

    /**
     * @brief Set annotation points (for free draw paths)
     */
    void setPoints(const QVector<QPoint> &points) { m_points = points; }

    /**
     * @brief Get bounding box (for box, circle annotations)
     */
    QRect boundingBox() const { return m_boundingBox; }

    /**
     * @brief Set bounding box
     */
    void setBoundingBox(const QRect &rect) { m_boundingBox = rect; }

    /**
     * @brief Get unique identifier
     */
    QUuid uuid() const { return m_uuid; }

private:
    AnnotationTool m_type;
    QColor m_color;
    int m_strokeWidth;
    QVector<QPoint> m_points;
    QRect m_boundingBox;
    QUuid m_uuid;
};

#endif // Q_OS_WIN

#endif // ANNOTATION_H
