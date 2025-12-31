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
