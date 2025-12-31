# Comprehensive Analysis (root)

## Entry Points

- cmake\tesseract_test.cpp
- kconf_update\spectacle-24.02.0-change_placeholder_format.cpp
- kconf_update\spectacle-24.02.0-keep_old_filename_templates.cpp
- kconf_update\spectacle-24.02.0-keep_old_save_location.cpp
- kconf_update\spectacle-24.02.0-rename_settings.cpp
- kconf_update\spectacle-24.02.0-video_format.cpp
- src\Main.cpp

## Configuration

- No .env/config/*.config files detected; configuration appears to be handled via KDE KConfig and CMake

## Settings / KConfig

- kconf_update\ConfigUtils.h
- kconf_update\spectacle-24.02.0-change_placeholder_format.cpp
- kconf_update\spectacle-24.02.0-keep_old_filename_templates.cpp
- kconf_update\spectacle-24.02.0-keep_old_save_location.cpp
- kconf_update\spectacle-24.02.0-rename_settings.cpp
- kconf_update\spectacle-24.02.0-video_format.cpp
- src\Gui\OptionsMenu.cpp
- src\Gui\SettingsDialog\OcrLanguageSelector.h
- src\Gui\SettingsDialog\SettingsDialog.cpp
- src\Gui\SettingsDialog\SettingsDialog.h
- src\Gui\SettingsDialog\SettingsUtils.h

## Shared Utilities

- kconf_update\ConfigUtils.h
- src\DebugUtils.h
- src\Gui\QmlUtils.qml
- src\Gui\SettingsDialog\SaveOptionsUtils.h
- src\Gui\SettingsDialog\SettingsUtils.h
- src\Gui\WidgetWindowUtils.h

## Async / Event-driven Patterns

- DBus: dbus\CMakeLists.txt
- DBus: desktop\CMakeLists.txt
- DBus: desktop\org.kde.spectacle.desktop.cmake
- DBus: src\CMakeLists.txt
- DBus: src\Gui\AnimatedLoader.qml
- DBus: src\Gui\AnnotationEditor.qml
- DBus: src\Gui\AnnotationOptionsToolBarContents.qml
- DBus: src\Gui\AnnotationsToolBarContents.qml
- DBus: src\Gui\ButtonGrid.qml
- DBus: src\Gui\CaptureModeButtonsColumn.qml
- DBus: src\Gui\CaptureOptions.qml
- DBus: src\Gui\CaptureOverlay.qml
- DBus: src\Gui\CaptureSettingsColumn.qml
- DBus: src\Gui\CopyImageAction.qml
- DBus: src\Gui\DashedOutline.qml
- DBus: src\Gui\DelaySpinBox.qml
- DBus: src\Gui\DialogPage.qml
- DBus: src\Gui\EditAction.qml
- DBus: src\Gui\ExportMenuButton.qml
- DBus: src\Gui\FloatingBackground.qml
- DBus: src\Gui\FloatingToolBar.qml
- DBus: src\Gui\HelpMenuButton.qml
- DBus: src\Gui\InlineMessageList.qml
- DBus: src\Gui\Magnifier.qml
- DBus: src\Gui\NewScreenshotToolButton.qml
- DBus: src\Gui\OcrAction.qml
- DBus: src\Gui\OptionsMenuButton.qml
- DBus: src\Gui\Outline.qml
- DBus: src\Gui\QmlUtils.qml
- DBus: src\Gui\RecordAction.qml
- DBus: src\Gui\RecordOptions.qml
- DBus: src\Gui\RecordingModeButtonsColumn.qml
- DBus: src\Gui\RecordingModeMenuButton.qml
- DBus: src\Gui\RecordingSettingsColumn.qml
- DBus: src\Gui\RecordingView.qml
- DBus: src\Gui\SaveAction.qml
- DBus: src\Gui\SceenshotModeMenu.qml
- DBus: src\Gui\ScreenshotModeMenuButton.qml
- DBus: src\Gui\ScreenshotView.qml
- DBus: src\Gui\ShortcutsTextBox.qml
- DBus: src\Gui\SizeLabel.qml
- DBus: src\Gui\TtToolButton.qml
- DBus: src\Gui\UndoRedoGroup.qml
- DBus: src\Gui\ViewerPage.qml
- DBus: src\Gui\ViewerWindow.cpp
- DBus: src\Main.cpp
- DBus: src\PlasmaVersion.cpp
- DBus: src\Platforms\ImagePlatformKWin.cpp
- DBus: src\Platforms\ImagePlatformKWin.h
- DBus: src\Platforms\ImagePlatformXcb.cpp
- DBus: src\Platforms\VideoPlatformWayland.cpp
- DBus: src\Platforms\VideoPlatformWayland.h
- DBus: src\RecordingModeModel.cpp
- DBus: src\ScreenShotEffect.cpp
- DBus: src\ShortcutActions.cpp
- DBus: src\SpectacleCore.cpp
- DBus: src\SpectacleDBusAdapter.cpp
- DBus: src\SpectacleDBusAdapter.h
- Signals/slots/events: kconf_update\spectacle-24.02.0-rename_settings.cpp
- Signals/slots/events: src\CaptureModeModel.h
- Signals/slots/events: src\ExportManager.h
- Signals/slots/events: src\Gui\AnnotationOptionsToolBarContents.qml
- Signals/slots/events: src\Gui\CaptureWindow.h
- Signals/slots/events: src\Gui\ExportMenu.cpp
- Signals/slots/events: src\Gui\ExportMenu.h
- Signals/slots/events: src\Gui\HelpMenu.h
- Signals/slots/events: src\Gui\InlineMessageModel.h
- Signals/slots/events: src\Gui\OptionsMenu.cpp
- Signals/slots/events: src\Gui\OptionsMenu.h
- Signals/slots/events: src\Gui\Selection.cpp
- Signals/slots/events: src\Gui\Selection.h
- Signals/slots/events: src\Gui\SelectionEditor.cpp
- Signals/slots/events: src\Gui\SelectionEditor.h
- Signals/slots/events: src\Gui\SettingsDialog\GeneralOptionsPage.h
- Signals/slots/events: src\Gui\SettingsDialog\ImageSaveOptions.ui
- Signals/slots/events: src\Gui\SettingsDialog\OcrLanguageSelector.cpp
- Signals/slots/events: src\Gui\SettingsDialog\OcrLanguageSelector.h
- Signals/slots/events: src\Gui\SettingsDialog\ShortcutsOptionsPage.h
- Signals/slots/events: src\Gui\SettingsDialog\VideoFormatComboBox.h
- Signals/slots/events: src\Gui\SmartSpinBox.h
- Signals/slots/events: src\Gui\SpectacleMenu.h
- Signals/slots/events: src\Gui\SpectacleWindow.h
- Signals/slots/events: src\Gui\ViewerWindow.cpp
- Signals/slots/events: src\Gui\ViewerWindow.h
- Signals/slots/events: src\OcrManager.h
- Signals/slots/events: src\Platforms\ImagePlatform.h
- Signals/slots/events: src\Platforms\ImagePlatformKWin.cpp
- Signals/slots/events: src\Platforms\ImagePlatformKWin.h
- Signals/slots/events: src\Platforms\ImagePlatformXcb.h
- Signals/slots/events: src\Platforms\PlatformNull.h
- Signals/slots/events: src\Platforms\VideoPlatform.cpp
- Signals/slots/events: src\Platforms\VideoPlatform.h
- Signals/slots/events: src\Platforms\VideoPlatformWayland.cpp
- Signals/slots/events: src\Platforms\VideoPlatformWayland.h
- Signals/slots/events: src\Platforms\screencasting.h
- Signals/slots/events: src\RecordingModeModel.cpp
- Signals/slots/events: src\RecordingModeModel.h
- Signals/slots/events: src\SpectacleCore.cpp
- Signals/slots/events: src\SpectacleCore.h
- Signals/slots/events: src\SpectacleDBusAdapter.h
- Signals/slots/events: src\VideoFormatModel.h
- Signals/slots/events: tests\FilenameTest.cpp

## CI/CD

- .gitlab-ci.yml

## Localization

- .po files: 54
- Docbook files: 22
- Base locale path: po/