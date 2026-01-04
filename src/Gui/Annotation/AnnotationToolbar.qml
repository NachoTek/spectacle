/*
 *  Annotation Toolbar
 *  Color and stroke size picker
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Row {
    id: root

    signal colorPicked(var color)
    signal strokeSizePicked(int size)

    spacing: 5

    // Color picker buttons
    Row {
        spacing: 3

        Repeater {
            model: [
                { color: "#FF0000", name: "Red" },
                { color: "#0000FF", name: "Blue" },
                { color: "#00FF00", name: "Green" },
                { color: "#FFFF00", name: "Yellow" },
                { color: "#000000", name: "Black" },
                { color: "#FFFFFF", name: "White" }
            ]

            Rectangle {
                width: 24
                height: 24
                color: modelData.color
                border.color: "white"
                border.width: 2
                radius: 4

                // Highlight selected color
                property bool isSelected: false

                border.color: isSelected ? "white" : "#808080"

                Controls.MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.colorPicked(modelData.color)
                        // Update selection state
                        // (would need parent reference in real implementation)
                    }
                }
            }
        }
    }

    // Stroke size buttons
    Row {
        spacing: 3

        Repeater {
            model: [1, 2, 4, 8]

            Rectangle {
                width: 32
                height: 24
                color: "#404040"
                border.color: "white"
                border.width: 2
                radius: 4

                Text {
                    anchors.centerIn: parent
                    text: modelData + "px"
                    color: "white"
                    font.pixelSize: 10
                }

                Controls.MouseArea {
                    anchors.fill: parent
                    onClicked: root.strokeSizePicked(modelData)
                }
            }
        }
    }
}
