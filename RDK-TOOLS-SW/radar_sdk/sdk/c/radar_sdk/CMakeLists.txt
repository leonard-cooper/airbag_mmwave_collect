# shared library of radar_sdk
add_library(radar_sdk SHARED ../dummy.c)
if(WIN32 OR MINGW OR MSYS)
    # on Windows the library should not have the prefix lib
    set_target_properties(radar_sdk PROPERTIES PREFIX "")
endif()
target_link_libraries(radar_sdk PUBLIC sdk_base_obj sdk_algo_obj sdk_radar_obj sdk_device_obj)

add_library(radar_sdk_static STATIC ../dummy.c)
target_compile_definitions(radar_sdk_static PUBLIC RADAR_SDK_BUILD_STATIC=1)
target_link_libraries(radar_sdk_static PUBLIC sdk_base_obj sdk_algo_obj sdk_radar_obj sdk_device_obj)

if(sdk_use_ipo)
	set_property(TARGET radar_sdk PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET radar_sdk_static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

