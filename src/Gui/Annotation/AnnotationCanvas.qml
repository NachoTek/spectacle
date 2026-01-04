/*
 *  Annotation Canvas
 *  Renders all annotations on the overlay
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

import QtQuick 2.15

Item {
    id: root

    // List of annotations to render
    property var annotationModel: null

    // Selection bounds for clipping
    property rect selectionRect: Qt.rect(0, 0, 0, 0)

    // Render all annotations
    Repeater {
        model: root.annotationModel

        // Free draw annotation
        Item {
            visible: model.type === 1  // FreeDraw
            x: root.selectionRect.x
            y: root.selectionRect.y

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
            border.width: model.strokeWidth || 2
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
                border.width: model.strokeWidth || 2
                radius: width / 2  // Make it circular
            }
        }
    }
}
