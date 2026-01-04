/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation Data Model
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#include "Annotation.h"

#ifdef Q_OS_WIN

Annotation::Annotation(AnnotationTool type)
    : m_type(type)
    , m_color(Qt::red)  // Default color: red
    , m_strokeWidth(2)   // Default stroke: 2px
    , m_uuid(QUuid::createUuid())
    , m_selected(false)  // Task 7: Not selected by default
{
}

Annotation::Annotation(const Annotation &other)
    : m_type(other.m_type)
    , m_color(other.m_color)
    , m_strokeWidth(other.m_strokeWidth)
    , m_points(other.m_points)
    , m_boundingBox(other.m_boundingBox)
    , m_uuid(other.m_uuid)  // Note: Copy shares UUID (for edit tracking)
    , m_selected(other.m_selected)  // Task 7: Copy selection state
{
}

Annotation &Annotation::operator=(const Annotation &other)
{
    if (this != &other) {
        m_type = other.m_type;
        m_color = other.m_color;
        m_strokeWidth = other.m_strokeWidth;
        m_points = other.m_points;
        m_boundingBox = other.m_boundingBox;
        m_uuid = other.m_uuid;  // Note: Assignment shares UUID (for edit tracking)
        m_selected = other.m_selected;  // Task 7: Copy selection state
    }
    return *this;
}

Annotation::~Annotation()
{
}

void Annotation::translate(int dx, int dy)
{
    // Translate bounding box
    if (!m_boundingBox.isNull()) {
        m_boundingBox.translate(dx, dy);
    }

    // Translate all points (for free draw paths)
    for (int i = 0; i < m_points.size(); ++i) {
        m_points[i] = m_points[i] + QPoint(dx, dy);
    }
}

#endif // Q_OS_WIN
