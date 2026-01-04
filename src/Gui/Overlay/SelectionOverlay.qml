/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Overlay QML Component
 *  Story 1.1 - Full-Screen Capture Overlay
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import "qrc:/overlay/Annotation"

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

    // Story 1.3: Annotation canvas (renders all annotations on top of overlay)
    AnnotationCanvas {
        id: annotationCanvas
        anchors.fill: parent
        z: 2  // Above selection box
        annotationModel: _overlay.annotationModel
        selectionRect: Qt.rect(_overlay.selectionRect.x,
                               _overlay.selectionRect.y,
                               _overlay.selectionRect.width,
                               _overlay.selectionRect.height)
        visible: _overlay.annotationModel !== null

        // Task 7: Handle annotation selection
        onAnnotationClicked: function(uuid) {
            console.log("Annotation clicked:", uuid)
            // Use Qt.invoked to call the C++ method with QUuid
            _overlay.annotationModel.selectAnnotation(uuid)
        }

        onAnnotationMoved: function(dx, dy) {
            console.log("Annotation moved:", dx, dy)
            _overlay.annotationModel.moveSelected(dx, dy)
        }
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

    // Story 1.3: Annotation toolbar (shown when selection exists)
    // Positioned above quick tray, contains tool and style pickers
    Rectangle {
        id: annotationToolbarContainer
        visible: _overlay.hasSelection

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: quickTray.top
        anchors.bottomMargin: 10

        width: toolbarColumn.implicitWidth + 20
        height: toolbarColumn.implicitHeight + 20
        color: "#E0000000" // Semi-transparent black
        radius: 8

        Column {
            id: toolbarColumn
            anchors.centerIn: parent
            spacing: 10

            // Tool picker (Free draw, Box, Circle, None)
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                Repeater {
                    model: [
                        { tool: 1, name: "✏️", tooltip: "Free Draw" },
                        { tool: 3, name: "⬜", tooltip: "Box" },
                        { tool: 4, name: "⭕", tooltip: "Circle" },
                        { tool: 0, name: "❌", tooltip: "None" }
                    ]

                    Rectangle {
                        width: 36
                        height: 36
                        color: _overlay.currentTool === modelData.tool ? "#00AAFF" : "#404040"
                        border.color: "white"
                        border.width: 2
                        radius: 6

                        Text {
                            anchors.centerIn: parent
                            text: modelData.name
                            font.pixelSize: 18
                        }

                        Controls.MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                _overlay.setCurrentTool(modelData.tool)
                            }
                        }
                    }
                }
            }

            // Color and stroke size picker
            AnnotationToolbar {
                id: annotationToolbar
                anchors.horizontalCenter: parent.horizontalCenter

                onColorPicked: function(color) {
                    _overlay.setCurrentColor(color)
                }

                onStrokeSizePicked: function(size) {
                    _overlay.setCurrentStrokeWidth(size)
                }
            }

            // Undo button
            Controls.Button {
                id: undoBtn
                anchors.horizontalCenter: parent.horizontalCenter
                text: "↩️ Undo"

                background: Rectangle {
                    color: undoBtn.pressed ? "#666666" :
                           undoBtn.hovered ? "#888888" : "#555555"
                    radius: 4
                }

                contentItem: Text {
                    text: undoBtn.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    _overlay.undoLastAnnotation()
                }
            }

            // Task 7: Delete selected button
            Controls.Button {
                id: deleteSelectedBtn
                anchors.horizontalCenter: parent.horizontalCenter
                text: "🗑️ Delete Selected"
                visible: _overlay.annotationModel &&
                         _overlay.annotationModel.selectedAnnotation.toString() !== "{00000000-0000-0000-0000-000000000000}"

                background: Rectangle {
                    color: deleteSelectedBtn.pressed ? "#990000" :
                           deleteSelectedBtn.hovered ? "#CC0000" : "#AA0000"
                    radius: 4
                }

                contentItem: Text {
                    text: deleteSelectedBtn.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Delete selected clicked")
                    _overlay.annotationModel.deleteSelected()
                }
            }
        }
    }

    // Quick tray (floating controls)
    // Design rationale: Always visible to provide Cancel button (abort capture)
    // Individual buttons show/hide based on state (Confirm/Refresh require selection)
    Rectangle {
        id: quickTray
        objectName: "quickTray"

        anchors.centerIn: parent
        width: trayRow.implicitWidth + 20
        height: 40
        color: "#E0000000" // Semi-transparent black
        radius: 8

        // Always visible: Cancel button available to abort capture at any time
        // Confirm/Refresh buttons only visible when selection exists
        visible: true

        Row {
            id: trayRow
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
    // Story 1.3: Also handles annotation drawing when tool is selected
    MouseArea {
        anchors.fill: parent
        objectName: "selectionArea"

        cursorShape: Qt.CrossCursor

        // Story 1.3: Free draw annotation state
        property var freeDrawPoints: []

        onPressed: function(mouse) {
            console.log("Pressed at:", mouse.x, mouse.y)

            // Story 1.3: Handle annotation tool press
            if (_overlay.hasSelection && _overlay.currentTool !== 0) {
                // Tool selected - start annotation
                if (_overlay.currentTool === 1) {
                    // Free draw - start collecting points
                    freeDrawPoints = [Qt.point(mouse.x, mouse.y)]
                }
                // Box and Circle will be handled in onReleased
            } else {
                // No tool - normal selection mode
                _overlay.mousePress(Qt.point(mouse.x, mouse.y))
            }
        }

        onPositionChanged: function(mouse) {
            // Story 1.2: Update target highlight on hover (Windows only)
            if (!pressed && typeof(_overlay.updateTargetUnderCursor) !== "undefined") {
                _overlay.updateTargetUnderCursor(Qt.point(mouse.x, mouse.y))
            }

            if (pressed) {
                // Story 1.3: Free draw annotation - collect points
                if (_overlay.currentTool === 1 && freeDrawPoints.length > 0) {
                    freeDrawPoints.push(Qt.point(mouse.x, mouse.y))
                    // TODO: Render preview of free draw path
                } else if (_overlay.currentTool === 0) {
                    // No tool - normal selection dragging
                    _overlay.mouseMove(Qt.point(mouse.x, mouse.y))
                }
            }
        }

        onReleased: function(mouse) {
            console.log("Released at:", mouse.x, mouse.y)

            // Story 1.3: Handle annotation tool release
            if (_overlay.hasSelection && _overlay.currentTool !== 0) {
                var startPt = freeDrawPoints.length > 0 ? freeDrawPoints[0] : null
                var endPt = Qt.point(mouse.x, mouse.y)

                if (_overlay.currentTool === 1 && freeDrawPoints.length > 1) {
                    // Free draw - add annotation with collected points
                    _overlay.addFreeDrawAnnotation(freeDrawPoints)
                    freeDrawPoints = []
                } else if (_overlay.currentTool === 3 && startPt && endPt) {
                    // Box - create from start to end points
                    var x = Math.min(startPt.x, endPt.x)
                    var y = Math.min(startPt.y, endPt.y)
                    var width = Math.abs(endPt.x - startPt.x)
                    var height = Math.abs(endPt.y - startPt.y)
                    _overlay.addBoxAnnotation(Qt.rect(x, y, width, height))
                } else if (_overlay.currentTool === 4 && startPt && endPt) {
                    // Circle - create from start to end points (bounding box)
                    var x = Math.min(startPt.x, endPt.x)
                    var y = Math.min(startPt.y, endPt.y)
                    var width = Math.abs(endPt.x - startPt.x)
                    var height = Math.abs(endPt.y - startPt.y)
                    _overlay.addCircleAnnotation(Qt.rect(x, y, width, height))
                }
            } else {
                // No tool - normal selection mode
                _overlay.mouseRelease(Qt.point(mouse.x, mouse.y))
            }
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
