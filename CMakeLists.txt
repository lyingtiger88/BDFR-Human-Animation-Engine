cmake_minimum_required(VERSION 3.24)
project(BDFR_Human_Animation_Engine VERSION 0.1.0 LANGUAGES CXX)

option(BDFR_BUILD_TESTS "Build engine tests" ON)
option(BDFR_BUILD_STUDIO "Build Qt 6 desktop Studio" OFF)
option(BDFR_ENABLE_OPENCV "Enable OpenCV capture adapters" OFF)
option(BDFR_MIGRATE_BUILD_FACE_MODULE "Build the migrated facial-animation module from its preserved source snapshot" ON)
option(BDFR_MIGRATE_BUILD_CAPTURE_MODULE "Build the migrated performance-capture module from its preserved source snapshot" ON)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "BDFR Human Animation Engine requires a 64-bit target. Configure an x64/arm64 toolchain.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(bdfr_engine_core
    engine/core/Platform.cpp
    engine/core/Validation.cpp
)
target_include_directories(bdfr_engine_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/engine/include>
)
target_compile_definitions(bdfr_engine_core PUBLIC BDFR_64BIT=1)

if(MSVC)
    target_compile_options(bdfr_engine_core PRIVATE /W4 /permissive- /Zc:__cplusplus)
else()
    target_compile_options(bdfr_engine_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

if(BDFR_BUILD_TESTS)
    enable_testing()
    add_executable(bdfr_platform_tests tests/test_platform.cpp)
    target_link_libraries(bdfr_platform_tests PRIVATE bdfr_engine_core)
    add_test(NAME bdfr_platform_tests COMMAND bdfr_platform_tests)
endif()

if(BDFR_ENABLE_OPENCV)
    find_package(OpenCV REQUIRED COMPONENTS core imgproc videoio calib3d)
    target_link_libraries(bdfr_engine_core PUBLIC ${OpenCV_LIBS})
    target_compile_definitions(bdfr_engine_core PUBLIC BDFR_HAS_OPENCV=1)
endif()

if(BDFR_BUILD_STUDIO)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
    add_subdirectory(apps/BDFRStudio)
endif()

# Migration bridge: compile the two preserved implementations as independently
# testable modules under the single 64-bit engine superbuild. Their source
# snapshots remain immutable until their APIs are promoted into engine/*.
if(BDFR_MIGRATE_BUILD_FACE_MODULE)
    set(BDFR_BUILD_STUDIO OFF CACHE BOOL "Keep legacy facial Studio disabled in engine superbuild" FORCE)
    add_subdirectory(migration/source-snapshots/BDFR_FacialAnimation)
endif()

if(BDFR_MIGRATE_BUILD_CAPTURE_MODULE)
    set(BDFRPC_BUILD_STUDIO OFF CACHE BOOL "Keep legacy capture Studio disabled in engine superbuild" FORCE)
    add_subdirectory(migration/source-snapshots/BDFR_PerformanceCapture)
endif()

# Canonical integration facade: one public engine frame model over both migrated modules.
if(BDFR_MIGRATE_BUILD_FACE_MODULE AND BDFR_MIGRATE_BUILD_CAPTURE_MODULE)
    add_library(bdfr_human_runtime
        engine/integration/EngineBridge.cpp
    )
    target_include_directories(bdfr_human_runtime PUBLIC
        engine/include
        migration/source-snapshots/BDFR_FacialAnimation/include
        migration/source-snapshots/BDFR_PerformanceCapture/include
    )
    target_link_libraries(bdfr_human_runtime PUBLIC
        bdfr_engine_core
        bdfr_core
        bdfrpc_core
    )

    if(BDFR_BUILD_TESTS)
        add_executable(bdfr_engine_bridge_tests tests/test_engine_bridge.cpp)
        target_link_libraries(bdfr_engine_bridge_tests PRIVATE bdfr_human_runtime)
        add_test(NAME bdfr_engine_bridge_tests COMMAND bdfr_engine_bridge_tests)
    endif()
endif()
