file(GLOB bgt60-configurator-cli_src *.cpp *.hpp)
add_executable(bgt60-configurator-cli ${bgt60-configurator-cli_src})
target_link_libraries(bgt60-configurator-cli app_common sdk_device_static)