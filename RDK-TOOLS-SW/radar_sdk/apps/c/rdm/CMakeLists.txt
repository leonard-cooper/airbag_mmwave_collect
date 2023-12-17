file(GLOB app_rdm_src *.c *.h)
add_executable(app_rdm ${app_rdm_src})
target_link_libraries(app_rdm radar_sdk app_common)