file(GLOB sdk_device_src *.cpp *.hpp *.c *.h internal/*.hpp internal/*.cpp internal/*.h)

add_library(sdk_device_obj OBJECT ${sdk_device_src})
target_compile_definitions(sdk_device_obj PRIVATE radar_sdk_EXPORTS=1)
target_link_libraries(sdk_device_obj PUBLIC sdk_base_obj)
target_link_libraries(sdk_device_obj PRIVATE strata_static lib_avian_static)
#target_include_directories(sdk_device_obj PUBLIC ifxDevice) # XXX

add_library(sdk_device SHARED ../dummy.c)
target_link_libraries(sdk_device PUBLIC sdk_device_obj sdk_base_obj)

add_library(sdk_device_static STATIC ../dummy.c)
target_link_libraries(sdk_device_static PUBLIC sdk_device_obj sdk_base_obj)
