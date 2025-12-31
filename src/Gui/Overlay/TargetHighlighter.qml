/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Target Highlighter for Window Selection
 *  Story 1.2 - Selection Targeting + Refinement
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

/**
 * @brief Visual highlight border for windows under cursor
 *
 * This component shows a 4px accent color border around the
 * currently hovered window to indicate it can be selected.
 * Story 1.2 - Window Targeting
 */
Rectangle {
    id: root

    // Public properties
    property bool highlighted: false
    property string targetType: "Window" // Window, Dialog, Menu

    // Highlight border styling
    color: "transparent"
    border.width: 4
    border.color: "#3DAEE9" // KDE accent color (blue)

    // Opacity for fade-in animation
    opacity: highlighted ? 1.0 : 0.0

    // Smooth fade-in animation (100ms)
    Behavior on opacity {
        OpacityAnimator {
            duration: 100
            easing.type: Easing.InOutQuad
        }
    }

    // Tooltip showing target type
    Controls.ToolTip {
        id: tooltip
        visible: root.highlighted
        text: root.targetType
        delay: 0
    }

    /**
     * @brief Highlight a target at the given bounds
     * @param bounds Rectangle in screen coordinates
     * @param type Type of target (Window, Dialog, Menu)
     */
    function highlight(bounds, type) {
        root.x = bounds.x
        root.y = bounds.y
        root.width = bounds.width
        root.height = bounds.height
        root.targetType = type
        root.highlighted = true
    }

    /**
     * @brief Clear the highlight
     */
    function clear() {
        root.highlighted = false
    }
}
