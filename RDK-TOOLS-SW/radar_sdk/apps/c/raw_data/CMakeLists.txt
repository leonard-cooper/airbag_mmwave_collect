file(GLOB app_raw_data_src *.c *.h)
add_executable(app_raw_data ${app_raw_data_src})
target_link_libraries(app_raw_data sdk_device)