/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Overlay QML Component
 *  Story 1.1 - Full-Screen Capture Overlay
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

// Transparent full-screen overlay
Rectangle {
    id: root
    color: "transparent"
    anchors.fill: parent

    // Semi-transparent dark overlay
    Rectangle {
        anchors.fill: parent
        color: "#80000000" // 50% black
        opacity: 0.3
    }

    // Target highlighter (Story 1.2 - Window Targeting)
    TargetHighlighter {
        id: targetHighlighter
        anchors.fill: parent
        z: 1  // Above dark overlay, below selection box

        // Visible when overlay is shown
        visible: true
    }

    // Selection box (shown when user has selection)
    Rectangle {
        id: selectionBox
        visible: _overlay.hasSelection

        x: _overlay.selectionRect.x
        y: _overlay.selectionRect.y
        width: _overlay.selectionRect.width
        height: _overlay.selectionRect.height

        color: "transparent"
        border.color: "#FFFFFF"
        border.width: 2

        // Dim area outside selection
        Rectangle {
            anchors.fill: parent
            color: "#00000000"
            border.color: "#00000000"
            opacity: 0.3
        }
    }

    // Resize handles (shown when selection exists)
    Repeater {
        id: handleRepeater
        model: _overlay.resizeHandlesVisible ? 8 : 0

        Rectangle {
            id: handle
            width: 10
            height: 10
            radius: 5
            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 1

            // Position will be set by C++ based on handle index
            property int handleIndex: index
            x: 0
            y: 0

            // Visual feedback on hover
            scale: handleMouseArea.containsMouse ? 1.2 : 1.0

            MouseArea {
                id: handleMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: handleIndex < 4 ?
                    (handleIndex % 2 === 0 ? Qt.SizeFDiagCursor : Qt.SizeVerCursor) :
                    (handleIndex % 2 === 0 ? Qt.SizeFDiagCursor : Qt.SizeHorCursor)
            }
        }
    }

    // Quick tray (floating controls)
    Rectangle {
        id: quickTray
        objectName: "quickTray"

        anchors.centerIn: parent
        width: cancelBtn.width + confirmBtn.width + 20
        height: 40
        color: "#E0000000" // Semi-transparent black
        radius: 8

        // Position to avoid selection (simplified)
        visible: !_overlay.hasSelection

        Row {
            anchors.centerIn: parent
            spacing: 10

            // Cancel button
            Controls.Button {
                id: cancelBtn
                objectName: "cancelButton"
                text: "Cancel"

                background: Rectangle {
                    color: cancelBtn.pressed ? "#FF0000" :
                           cancelBtn.hovered ? "#CC0000" : "#AA0000"
                    radius: 4
                }

                contentItem: Text {
                    text: cancelBtn.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Cancel clicked")
                    _overlay.escapePressed()
                }
            }

            // Refresh button (Story 1.8 - Window Movement Detection)
            Controls.Button {
                id: refreshBtn
                objectName: "refreshButton"
                text: "Refresh"
                visible: _overlay.hasSelection

                background: Rectangle {
                    color: refreshBtn.pressed ? "#0066CC" :
                           refreshBtn.hovered ? "#0088FF" : "#00AAFF"
                    radius: 4
                }

                contentItem: Text {
                    text: refreshBtn.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Refresh clicked")
                    _overlay.refreshTargets()
                }
            }

            // Confirm button (when selection exists)
            Controls.Button {
                id: confirmBtn
                objectName: "confirmButton"
                text: "Capture"
                visible: _overlay.hasSelection

                background: Rectangle {
                    color: confirmBtn.pressed ? "#00AA00" :
                           confirmBtn.hovered ? "#00CC00" : "#00FF00"
                    radius: 4
                }

                contentItem: Text {
                    text: confirmBtn.text
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Confirm clicked")
                    _overlay.enterPressed()
                }
            }
        }
    }

    // Mouse area for selection drawing
    MouseArea {
        anchors.fill: parent
        objectName: "selectionArea"

        cursorShape: Qt.CrossCursor

        onPressed: function(mouse) {
            console.log("Pressed at:", mouse.x, mouse.y)
            _overlay.mousePress(Qt.point(mouse.x, mouse.y))
        }

        onPositionChanged: function(mouse) {
            // Story 1.2: Update target highlight on hover (Windows only)
            if (!pressed && typeof(_overlay.updateTargetUnderCursor) !== "undefined") {
                _overlay.updateTargetUnderCursor(Qt.point(mouse.x, mouse.y))
            }

            if (pressed) {
                _overlay.mouseMove(Qt.point(mouse.x, mouse.y))
            }
        }

        onReleased: function(mouse) {
            console.log("Released at:", mouse.x, mouse.y)
            _overlay.mouseRelease(Qt.point(mouse.x, mouse.y))
        }
    }

    // Keyboard handling
    focus: true
    Keys.onEnterPressed: {
        console.log("Enter key pressed")
        _overlay.enterPressed()
        event.accepted = true
    }

    Keys.onReturnPressed: {
        console.log("Return key pressed")
        _overlay.enterPressed()
        event.accepted = true
    }

    Keys.onEscapePressed: {
        console.log("Escape key pressed")
        _overlay.escapePressed()
        event.accepted = true
    }
}
