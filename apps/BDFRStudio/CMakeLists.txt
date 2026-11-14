find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(BDFRStudio
    main.cpp
    StudioMainWindow.h
    StudioMainWindow.cpp
    CalibrationWizardDialog.h
    CalibrationWizardDialog.cpp
)

target_link_libraries(BDFRStudio PRIVATE
    bdfr_engine_core
    bdfrpc_core
    bdfrpc_opencv
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_definitions(BDFRStudio PRIVATE
    BDFR_CAMERA_CALIBRATE_EXE="$<TARGET_FILE:bdfrpc_calibrate_camera>"
    BDFR_RIG_CALIBRATE_EXE="$<TARGET_FILE:bdfrpc_calibrate_rig>"
)

add_dependencies(BDFRStudio
    bdfrpc_calibrate_camera
    bdfrpc_calibrate_rig
)

set_target_properties(BDFRStudio PROPERTIES
    WIN32_EXECUTABLE ON
    OUTPUT_NAME "BDFR Studio"
)
