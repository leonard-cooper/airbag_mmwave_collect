file(GLOB app_constant_wave_src *.c *.h)
add_executable(app_constant_wave ${app_constant_wave_src})
target_link_libraries(app_constant_wave sdk_device)