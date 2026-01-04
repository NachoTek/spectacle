/*
 *  Annotation Canvas
 *  Renders all annotations on the overlay
 *  Story 1.3 - Pre-Capture Annotation Tools
 *  Task 7: Selection and editing support
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Item {
    id: root

    // List of annotations to render
    property var annotationModel: null

    // Selection bounds for clipping
    property rect selectionRect: Qt.rect(0, 0, 0, 0)

    // Callbacks for selection editing
    signal annotationClicked(var uuid)
    signal annotationMoved(int dx, int dy)

    // Track drag state for moving annotations
    property var dragStartPos: null
    property bool isDragging: false

    // Render all annotations
    Repeater {
        model: root.annotationModel

        // Free draw annotation
        Item {
            visible: model.type === 1  // FreeDraw
            x: root.selectionRect.x
            y: root.selectionRect.y

            // Task 7: Clickable area for free draw paths
            MouseArea {
                anchors.fill: parent
                enabled: root.annotationModel !== null
                onClicked: {
                    root.annotationClicked(model.uuid)
                }
            }

            Canvas {
                id: canvas
                width: root.selectionRect.width
                height: root.selectionRect.height

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    if (model.points && model.points.length > 1) {
                        ctx.strokeStyle = model.color;
                        ctx.lineWidth = model.strokeWidth;
                        ctx.lineCap = "round";
                        ctx.lineJoin = "round";

                        ctx.beginPath();
                        ctx.moveTo(model.points[0].x - root.selectionRect.x,
                                    model.points[0].y - root.selectionRect.y);

                        for (var i = 1; i < model.points.length; i++) {
                            ctx.lineTo(model.points[i].x - root.selectionRect.x,
                                        model.points[i].y - root.selectionRect.y);
                        }

                        ctx.stroke();

                        // Task 7: Draw selection highlight
                        if (model.selected) {
                            ctx.strokeStyle = "#00AAFF";
                            ctx.lineWidth = 1;
                            ctx.setLineDash([5, 5]);
                            ctx.stroke();
                            ctx.setLineDash([]);
                        }
                    }
                }
            }
        }

        // Box annotation
        Rectangle {
            visible: model.type === 3  // Box
            x: model.boundingBox ? model.boundingBox.x : 0
            y: model.boundingBox ? model.boundingBox.y : 0
            width: model.boundingBox ? model.boundingBox.width : 0
            height: model.boundingBox ? model.boundingBox.height : 0
            color: "transparent"
            border.color: model.color || "red"
            border.width: (model.selected ? 4 : (model.strokeWidth || 2))

            // Task 7: Selection highlight
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "#00AAFF"
                border.width: 2
                visible: model.selected || false
                opacity: 0.5
            }

            // Task 7: Clickable area for box annotations
            MouseArea {
                anchors.fill: parent
                enabled: root.annotationModel !== null
                onClicked: {
                    root.annotationClicked(model.uuid)
                }
            }
        }

        // Circle annotation
        Item {
            visible: model.type === 4  // Circle
            x: model.boundingBox ? model.boundingBox.x : 0
            y: model.boundingBox ? model.boundingBox.y : 0
            width: model.boundingBox ? model.boundingBox.width : 0
            height: model.boundingBox ? model.boundingBox.height : 0

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: model.color || "red"
                border.width: (model.selected ? 4 : (model.strokeWidth || 2))
                radius: width / 2  // Make it circular

                // Task 7: Selection highlight
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "#00AAFF"
                    border.width: 2
                    radius: width / 2
                    visible: model.selected || false
                    opacity: 0.5
                }
            }

            // Task 7: Clickable area for circle annotations
            MouseArea {
                anchors.fill: parent
                enabled: root.annotationModel !== null
                onClicked: {
                    root.annotationClicked(model.uuid)
                }
            }
        }
    }
}
