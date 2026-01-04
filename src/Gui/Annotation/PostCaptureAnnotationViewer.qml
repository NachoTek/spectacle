/*
 *  Post-Capture Annotation Viewer UI
 *  Story 1.4 - Post-Capture Annotation View
 *  Task 1.2: Create PostCaptureAnnotationViewer.qml for UI layout
 *  Task 2: Reuse annotation components from Story 1.3
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Task 1.3: Main window layout
ApplicationWindow {
    id: root
    width: 800
    height: 600
    title: qsTr("Annotation Editor - Spectacle")

    // Task 7.6: Ensure window is resizable
    minimumWidth: 640
    minimumHeight: 480

    // Properties to be set from C++
    property var annotationModel: null

    // Task 1.3: Image display area with annotation overlay
    Item {
        id: imageDisplay
        anchors.fill: parent
        anchors.margins: 80 // Leave space for toolbar at bottom

        // Captured image (CRITICAL #3 fix - uses image provider with capturedImageId)
        Image {
            id: capturedImageDisplay
            source: "image://capture/" + capturedImageId
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

            // Task 1.3: Scrollable if large
            property real zoomLevel: 1.0

            // Task 2.1: Load AnnotationCanvas component (from Story 1.3)
            // This overlays annotations on the image
            AnnotationCanvas {
                id: annotationCanvas
                anchors.fill: parent

                // Pass annotation model from viewer
                annotationModel: root.annotationModel

                // Selection bounds for clipping to image area
                selectionRect: Qt.rect(0, 0,
                                      capturedImageDisplay.width * capturedImageDisplay.zoomLevel,
                                      capturedImageDisplay.height * capturedImageDisplay.zoomLevel)

                // Annotation interaction callbacks
                onAnnotationClicked: function(uuid) {
                    annotationModel.selectAnnotation(uuid)
                }

                onAnnotationMoved: function(dx, dy) {
                    annotationModel.moveSelected(dx, dy)
                }
            }
        }
    }

    // Task 1.4: Action buttons toolbar
    RowLayout {
        id: toolbar
        anchors.bottom: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        spacing: 10

        // Task 1.4: Save As button
        Button {
            text: qsTr("Save As...")
            // Task 7.1: Support Ctrl+S shortcut
            focusPolicy: Qt.NoFocus
            onClicked: viewerWindow.saveAs()
        }

        // Task 6.1: Copy to Clipboard button
        Button {
            text: qsTr("Copy to Clipboard")
            // Task 7.2: Support Ctrl+C shortcut
            focusPolicy: Qt.NoFocus
            onClicked: viewerWindow.copyToClipboard()
        }

        // Close button
        Button {
            text: qsTr("Close")
            focusPolicy: Qt.NoFocus
            // Task 7.5: Support Escape to close
            onClicked: viewerWindow.close()
        }
    }

    // Task 2.2: Load AnnotationToolbar component (from Story 1.3)
    // This provides color and stroke size picker
    AnnotationToolbar {
        id: annotationToolbar
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20

        // Connect to annotation model for tool selection
        onToolSelected: function(toolType) {
            viewerWindow.setCurrentTool(toolType)
        }

        onColorSelected: function(color) {
            viewerWindow.setCurrentColor(color)
        }

        onStrokeWidthSelected: function(width) {
            viewerWindow.setCurrentStrokeWidth(width)
        }
    }

    // Component for displaying inline messages
    Rectangle {
        id: messageBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: toolbar.top
        anchors.bottomMargin: 20
        width: messageText.width + 40
        height: messageText.height + 20
        color: "#333333"
        radius: 5
        opacity: 0

        // Task 5.4, 6.4: Show inline messages (no toasts)
        function showMessage(text) {
            messageText.text = text
            opacity = 1
            messageTimer.restart()
        }

        Timer {
            id: messageTimer
            interval: 3000
            onTriggered: messageBox.opacity = 0
        }

        Text {
            id: messageText
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 12
        }

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    // Keyboard shortcuts
    focus: true
    Keys.onPressed: function(event) {
        // Task 7.3: Support Ctrl+Z to undo
        if (event.matches(StandardKey.Undo)) {
            event.accepted = true
            annotationModel.undoLast()
        }
        // MAJOR #5: Support Ctrl+S to trigger Save As
        else if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_S) {
            event.accepted = true
            viewerWindow.saveAs()
        }
        // MAJOR #5: Support Ctrl+C to copy to clipboard
        else if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_C) {
            event.accepted = true
            viewerWindow.copyToClipboard()
        }
        // Task 7.4: Support Delete key
        else if (event.key === Qt.Key_Delete) {
            event.accepted = true
            annotationModel.deleteSelected()
        }
        // Task 7.5: Support Escape to close (CRITICAL #2 fix - uses requestClose for confirmation)
        else if (event.key === Qt.Key_Escape) {
            event.accepted = true
            viewerWindow.requestClose()
        }
    }
}
