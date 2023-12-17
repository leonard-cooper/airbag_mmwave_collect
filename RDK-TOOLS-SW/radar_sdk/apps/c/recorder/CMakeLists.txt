file(GLOB app_recorder_src *.c *.h)
add_executable(app_recorder ${app_recorder_src})
target_link_libraries(app_recorder radar_sdk app_common)