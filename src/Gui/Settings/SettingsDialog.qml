/*
 *  Settings Dialog
 *  Story 1.5 - Saving & Defaults
 *  Task 2: Create settings UI (6 subtasks)
 *  Windows 11 Fluent Design
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    title: qsTr("Spectacle Settings")
    width: 600
    height: 500
    minimumWidth: 500
    minimumHeight: 400
    modality: Qt.ApplicationModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

    // Windows 11 Fluent colors
    readonly property color backgroundColor: "#202020"
    readonly property color cardColor: "#2C2C2C"
    readonly property color borderColor: "#3A3A3A"
    readonly property color accentColor: "#60CDFF"
    readonly property color textColor: "#FFFFFF"
    readonly property color secondaryTextColor: "#A0A0A0"

    color: backgroundColor

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            spacing: 20
            anchors.fill: parent
            anchors.margins: 40

            // Task 2.1-2.6: Settings groups with Windows 11 Fluent styling

            // Task 2.2: Autosave toggle
            GroupBox {
                title: qsTr("Automatic Saving")
                Layout.fillWidth: true

                background: Rectangle {
                    color: cardColor
                    border.color: borderColor
                    border.width: 1
                    radius: 8
                }

                label: Label {
                    color: textColor
                    font.pixelSize: 14
                    font.weight: Font.Medium
                }

                ColumnLayout {
                    spacing: 12
                    anchors.fill: parent

                    RowLayout {
                        spacing: 12

                        Label {
                            text: qsTr("Automatically save captures")
                            color: textColor
                            font.pixelSize: 12
                            Layout.fillWidth: true
                        }

                        Switch {
                            id: autosaveSwitch
                            checked: settingsManager ? settingsManager.autosave : true
                            onToggled: {
                                if (settingsManager) {
                                    settingsManager.setAutosave(checked)
                                }
                            }

                            // Custom Fluent-style switch
                            indicator: Rectangle {
                                implicitWidth: 44
                                implicitHeight: 24
                                x: autosaveSwitch.leftPadding
                                y: parent.height / 2 - height / 2
                                radius: height / 2
                                color: autosaveSwitch.checked ? accentColor : borderColor
                                border.color: "transparent"

                                Behavior on color {
                                    ColorAnimation { duration: 150 }
                                }

                                Rectangle {
                                    x: autosaveSwitch.checked ? parent.width - width : 0
                                    width: 20
                                    height: 20
                                    y: parent.height / 2 - height / 2
                                    radius: width / 2
                                    color: "white"

                                    Behavior on x {
                                        NumberAnimation {
                                            duration: 150
                                            easing.type: Easing.InOutCubic
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Label {
                        text: qsTr("When enabled, captures are automatically saved to your default location")
                        color: secondaryTextColor
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                }
            }

            // Task 2.3: Save location picker
            GroupBox {
                title: qsTr("Save Location")
                Layout.fillWidth: true

                background: Rectangle {
                    color: cardColor
                    border.color: borderColor
                    border.width: 1
                    radius: 8
                }

                label: Label {
                    color: textColor
                    font.pixelSize: 14
                    font.weight: Font.Medium
                }

                ColumnLayout {
                    spacing: 12
                    anchors.fill: parent

                    RowLayout {
                        spacing: 12

                        TextField {
                            id: locationField
                            text: settingsManager ? settingsManager.defaultSaveLocation : ""
                            readOnly: true
                            color: textColor
                            font.pixelSize: 12
                            Layout.fillWidth: true

                            background: Rectangle {
                                color: "#1A1A1A"
                                border.color: locationField.activeFocus ? accentColor : borderColor
                                border.width: locationField.activeFocus ? 2 : 1
                                radius: 4
                            }
                        }

                        Button {
                            text: qsTr("Browse...")
                            onClicked: folderDialog.open()

                            background: Rectangle {
                                color: parent.hovered ? "#3A3A3A" : "#2A2A2A"
                                border.color: borderColor
                                border.width: 1
                                radius: 4

                                Behavior on color {
                                    ColorAnimation { duration: 100 }
                                }
                            }

                            contentItem: Text {
                                text: parent.text
                                color: textColor
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    CheckBox {
                        id: rememberLocationCheckbox
                        text: qsTr("Remember last save location")
                        checked: settingsManager ? settingsManager.rememberLastLocation : true
                        onToggled: {
                            if (settingsManager) {
                                settingsManager.setRememberLastLocation(checked)
                            }
                        }

                        contentItem: Text {
                            text: rememberLocationCheckbox.text
                            color: textColor
                            font.pixelSize: 12
                            leftPadding: rememberLocationCheckbox.indicator.width + rememberLocationCheckbox.spacing
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // Task 2.4: Default format dropdown
            GroupBox {
                title: qsTr("Default Format")
                Layout.fillWidth: true

                background: Rectangle {
                    color: cardColor
                    border.color: borderColor
                    border.width: 1
                    radius: 8
                }

                label: Label {
                    color: textColor
                    font.pixelSize: 14
                    font.weight: Font.Medium
                }

                RowLayout {
                    spacing: 12
                    anchors.fill: parent

                    Label {
                        text: qsTr("Image format:")
                        color: textColor
                        font.pixelSize: 12
                    }

                    ComboBox {
                        id: formatCombo
                        model: ["PNG", "JPEG", "BMP", "TIFF"]
                        currentIndex: {
                            if (settingsManager) {
                                const formats = ["png", "jpeg", "bmp", "tiff"]
                                const currentFormat = settingsManager.defaultFormat.toLowerCase()
                                for (let i = 0; i < formats.length; i++) {
                                    if (formats[i] === currentFormat) {
                                        return i
                                    }
                                }
                            }
                            return 0  // Default to PNG
                        }

                        onActivated: {
                            if (settingsManager) {
                                const formats = ["png", "jpeg", "bmp", "tiff"]
                                settingsManager.setDefaultFormat(formats[currentIndex])
                            }
                        }

                        background: Rectangle {
                            color: "#1A1A1A"
                            border.color: formatCombo.popup.visible ? accentColor : borderColor
                            border.width: formatCombo.popup.visible ? 2 : 1
                            radius: 4

                            Behavior on border.color {
                                ColorAnimation { duration: 100 }
                            }
                        }

                        contentItem: Text {
                            text: formatCombo.displayText
                            color: textColor
                            font.pixelSize: 12
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 12
                            rightPadding: formatCombo.indicator.width + formatCombo.spacing
                        }
                    }
                }
            }

            // Task 2.5: Action buttons
            RowLayout {
                spacing: 12
                Layout.alignment: Qt.AlignRight

                Button {
                    text: qsTr("Reset to Defaults")
                    onClicked: {
                        if (settingsManager) {
                            settingsManager.resetToDefaults()
                            // Update UI elements
                            autosaveSwitch.checked = settingsManager.autosave
                            locationField.text = settingsManager.defaultSaveLocation
                            rememberLocationCheckbox.checked = settingsManager.rememberLastLocation
                            // Reset format combo to PNG (index 0)
                            formatCombo.currentIndex = 0
                        }
                    }

                    background: Rectangle {
                        color: parent.hovered ? "#3A3A3A" : "#2A2A2A"
                        border.color: borderColor
                        border.width: 1
                        radius: 4

                        Behavior on color {
                            ColorAnimation { duration: 100 }
                        }
                    }

                    contentItem: Text {
                        text: parent.text
                        color: secondaryTextColor
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    text: qsTr("Apply")
                    highlighted: true
                    onClicked: {
                        // Save all settings
                        if (settingsManager) {
                            settingsManager.saveSettings()
                        }
                        root.close()
                    }

                    background: Rectangle {
                        color: parent.hovered ? "#0078D4" : "#0066BB"
                        border.color: "transparent"
                        border.width: 0
                        radius: 4

                        Behavior on color {
                            ColorAnimation { duration: 100 }
                        }
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    // Task 2.3: Folder dialog for browsing save location
    FolderDialog {
        id: folderDialog
        currentFolder: settingsManager ? settingsManager.defaultSaveLocation : ""
        onAccepted: {
            if (settingsManager) {
                settingsManager.setDefaultSaveLocation(selectedFolder)
            }
        }
    }
}
