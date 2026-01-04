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
{
}

Annotation::Annotation(const Annotation &other)
    : m_type(other.m_type)
    , m_color(other.m_color)
    , m_strokeWidth(other.m_strokeWidth)
    , m_points(other.m_points)
    , m_boundingBox(other.m_boundingBox)
    , m_uuid(other.m_uuid)  // Note: Copy shares UUID (for edit tracking)
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
    }
    return *this;
}

Annotation::~Annotation()
{
}

#endif // Q_OS_WIN
