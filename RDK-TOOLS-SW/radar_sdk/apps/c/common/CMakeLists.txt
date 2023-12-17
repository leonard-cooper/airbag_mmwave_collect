file(GLOB app_common_src *.c *.h *.cpp)
add_library(app_common ${app_common_src})
target_include_directories(app_common PUBLIC "." "../../../3rd_party/libs/")
target_link_libraries(app_common radar_sdk argparse)
