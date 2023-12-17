include(TargetPlatform)

# detect target platform
detect_target_platform(TARGET_PLATFORM)

if(TARGET_PLATFORM MATCHES "unknown")
    message("-- unknown platform - segmentation will not be available")
else()
    link_directories("${RDK_LIB_DIR}/${TARGET_PLATFORM}")

    file(GLOB app_segmentation_src *.c *.h)
    add_executable(app_segmentation ${app_segmentation_src})
    target_link_libraries(app_segmentation radar_sdk app_common)

    if(MINGW OR MSYS OR WIN32)
        if(MINGW OR MSYS)
            # for msys give the exact path to the library to avoid linking again
            # radar_sdk.lib
            target_link_libraries(app_segmentation "${CMAKE_SOURCE_DIR}/libs/${TARGET_PLATFORM}/sdk_radar_segmentation.dll")
        else()
            target_link_libraries(app_segmentation "${CMAKE_SOURCE_DIR}/libs/${TARGET_PLATFORM}/sdk_radar_segmentation.lib")
        endif()

        add_custom_command(
            TARGET app_segmentation
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/libs/${TARGET_PLATFORM}/sdk_radar_segmentation.dll"
            "$<TARGET_FILE_DIR:app_segmentation>"
        )
    else()
        link_directories("${RDK_LIB_DIR}/${TARGET_PLATFORM}")
        target_link_libraries(app_segmentation "${CMAKE_SOURCE_DIR}/libs/${TARGET_PLATFORM}/libsdk_radar_segmentation.so")

        add_custom_command(
            TARGET app_segmentation
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/libs/${TARGET_PLATFORM}/libsdk_radar_segmentation.so"
            "$<TARGET_FILE_DIR:app_segmentation>"
        )
    endif()
endif()