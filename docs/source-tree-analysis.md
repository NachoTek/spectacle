# Source Tree Analysis

```text
spectacle/
  src/                       # Core application code
    Main.cpp                 # Application entry point
    SpectacleCore.*          # Capture/recording core logic
    ExportManager.*          # Export/share/save workflows
    OcrManager.*             # OCR integration
    Platforms/               # Platform-specific capture backends
      ImagePlatformKWin.*    # KWin DBus capture backend
      ImagePlatformXcb.*     # X11/XCB capture backend
      VideoPlatformWayland.* # Wayland screencasting backend
    Gui/                     # UI layer (Qt/QML + widgets)
      SpectacleWindow.*      # Main window / QQuickView host
      CaptureWindow.*        # Capture overlay window
      ViewerWindow.*         # Image preview/annotation
      SettingsDialog/        # Preferences UI + settings pages
      *.qml                  # QML UI components
  dbus/                      # DBus interface and service files
  desktop/                   # Desktop entry + KDE integration
  kconf_update/              # KDE config migration scripts
  icons/                     # Application icons
  doc/                       # User documentation assets
  tests/                     # Test sources
  po/                        # Localization files
  CMakeLists.txt             # Top-level build configuration
```