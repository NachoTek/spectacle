include_directories(${PROJECT_SOURCE_DIR}/src)

SET(FILENAME_TEST_SRCS
    FilenameTest.cpp
    ../src/ShortcutActions.cpp
    ../src/ExportManager.cpp
    ../src/Platforms/ImagePlatform.cpp
    ../src/Platforms/VideoPlatform.cpp
)

ecm_qt_declare_logging_category(FILENAME_TEST_SRCS
    HEADER spectacle_debug.h
    IDENTIFIER SPECTACLE_LOG
    CATEGORY_NAME spectacle
    DESCRIPTION "spectacle (general)"
    EXPORT SPECTACLE
)

kconfig_add_kcfg_files(FILENAME_TEST_SRCS GENERATE_MOC ${PROJECT_SOURCE_DIR}/src/Gui/SettingsDialog/settings.kcfgc)

ecm_add_test(
    ${FILENAME_TEST_SRCS}
    TEST_NAME "filename_test"
    LINK_LIBRARIES  Qt::Test
        Qt::PrintSupport Qt::Qml KF6::I18n KF6::ConfigCore KF6::GlobalAccel KF6::KIOCore KF6::WindowSystem KF6::XmlGui KF6::GuiAddons KF6::PrisonScanner
)

# Helper Process Tests (Story 1.1 - Windows Port)
SET(HELPER_PROCESS_TEST_SRCS
    Integration/HelperProcessTest.cpp
    ../src/Platforms/Windows/HelperProcess.cpp
    ../src/Platforms/Windows/IPCProtocol.cpp
)

ecm_add_test(
    ${HELPER_PROCESS_TEST_SRCS}
    TEST_NAME "helper_process_test"
    LINK_LIBRARIES  Qt::Test
)

# WGC Capture Benchmark Tests (Story 1.1 - Performance Validation)
SET(WGC_CAPTURE_BENCHMARK_SRCS
    Integration/WGCCaptureBenchmark.cpp
    ../src/Platforms/Windows/WGCCapture.cpp
    ../src/Platforms/Windows/WGCCaptureMock.cpp
)

ecm_add_test(
    ${WGC_CAPTURE_BENCHMARK_SRCS}
    TEST_NAME "wgc_capture_benchmark"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
)

# Selection Overlay Tests (Story 1.1 - UI Components)
SET(SELECTION_OVERLAY_TEST_SRCS
    Unit/SelectionGeometryTest.cpp
    ../src/Gui/Overlay/SelectionGeometry.cpp
)

ecm_add_test(
    ${SELECTION_OVERLAY_TEST_SRCS}
    TEST_NAME "selection_geometry_test"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
        Qt::Qml
        Qt::QuickWidgets
)

# Named Pipe Server Tests (Story 1.1 - IPC Communication)
SET(NAMED_PIPE_SERVER_TEST_SRCS
    Integration/NamedPipeServerTest.cpp
    ../src/Platforms/Windows/NamedPipeServer.cpp
    ../src/Platforms/Windows/IPCProtocol.cpp
)

ecm_add_test(
    ${NAMED_PIPE_SERVER_TEST_SRCS}
    TEST_NAME "named_pipe_server_test"
    LINK_LIBRARIES  Qt::Test
)

# Window Detector Tests (Story 1.2 - Selection Targeting)
SET(WINDOW_DETECTOR_TEST_SRCS
    Integration/WindowDetectorTest.cpp
    ../src/Platforms/Windows/WindowDetector.cpp
)

ecm_add_test(
    ${WINDOW_DETECTOR_TEST_SRCS}
    TEST_NAME "window_detector_test"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
)

# Selection Snapper Tests (Story 1.2 - Click-to-Select)
SET(SELECTION_SNAPPER_TEST_SRCS
    Unit/SelectionSnapperTest.cpp
    ../src/Gui/Overlay/SelectionSnapper.cpp
)

ecm_add_test(
    ${SELECTION_SNAPPER_TEST_SRCS}
    TEST_NAME "selection_snapper_test"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
)

# Target Highlighter Tests (Story 1.2 - Visual Component)
SET(TARGET_HIGHLIGHTER_TEST_SRCS
    Integration/TargetHighlighterTest.cpp
)

ecm_add_test(
    ${TARGET_HIGHLIGHTER_TEST_SRCS}
    TEST_NAME "target_highlighter_test"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
        Qt::Qml
        Qt::QuickWidgets
)

# Window Event Monitor Tests (Story 1.8 - Window Movement Detection)
SET(WINDOW_EVENT_MONITOR_TEST_SRCS
    Integration/WindowEventMonitorTest.cpp
    ../src/Platforms/Windows/WindowEventMonitor.cpp
)

ecm_add_test(
    ${WINDOW_EVENT_MONITOR_TEST_SRCS}
    TEST_NAME "window_event_monitor_test"
    LINK_LIBRARIES  Qt::Test
)

# Annotation Data Model Tests (Story 1.3 - Pre-Capture Annotation Tools)
SET(ANNOTATION_TEST_SRCS
    Unit/AnnotationTest.cpp
    ../src/Gui/Annotation/Annotation.cpp
    ../src/Gui/Annotation/AnnotationListModel.cpp
    ../src/Gui/Annotation/AnnotationRenderer.cpp
)

ecm_add_test(
    ${ANNOTATION_TEST_SRCS}
    TEST_NAME "annotation_test"
    LINK_LIBRARIES  Qt::Test
        Qt::Gui
)


