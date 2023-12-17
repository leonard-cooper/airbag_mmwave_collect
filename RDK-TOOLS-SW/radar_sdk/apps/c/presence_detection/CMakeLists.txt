file(GLOB app_presence_detection_src *.c *.h)
add_executable(app_presence_detection ${app_presence_detection_src})
target_link_libraries(app_presence_detection radar_sdk app_common)