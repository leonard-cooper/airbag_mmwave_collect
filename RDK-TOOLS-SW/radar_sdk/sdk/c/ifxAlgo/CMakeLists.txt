file(GLOB sdk_algo_src *.c *.h *.cpp *.hpp)

add_library(sdk_algo_obj OBJECT ${sdk_algo_src})
target_compile_definitions(sdk_algo_obj PRIVATE radar_sdk_EXPORTS=1)

if(HAS_LIBM)
    target_link_libraries(sdk_algo_obj PUBLIC m)
endif()

if(sdk_use_ipo)
    set_property(TARGET sdk_algo_obj PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_link_libraries(sdk_algo_obj PUBLIC sdk_base_obj kissfft)

target_include_directories(sdk_algo_obj PRIVATE ${FFT_INCLUDE_DIRS})
target_compile_definitions(sdk_algo_obj PRIVATE ${FFT_LIB_DEFINE})
# ---------------------------------------------------------------------------