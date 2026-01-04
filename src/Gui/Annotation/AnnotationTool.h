/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation Tool Enum
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#ifndef ANNOTATIONTOOL_H
#define ANNOTATIONTOOL_H

#include <QtGlobal>

#ifdef Q_OS_WIN

/**
 * @brief Enum representing available annotation tools
 */
enum class AnnotationTool
{
    None = 0,      ///< No tool selected
    FreeDraw = 1,  ///< Freehand drawing tool
    Arrow = 2,     ///< Arrow/line tool
    Box = 3,       ///< Rectangle box tool
    Circle = 4,    ///< Circle/ellipse tool
    Text = 5       ///< Text annotation tool
};

#endif // Q_OS_WIN

#endif // ANNOTATIONTOOL_H
