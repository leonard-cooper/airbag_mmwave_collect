function(strata_package_add_library name type)
    add_library(${name} ${type} ${ARGN})
    target_include_directories(${name} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${STRATA_INCLUDE_DIRS}"
        )
    if(NOT ${type} STREQUAL OBJECT)
        target_sources(${name} PRIVATE ${STRATA_LIBRARY_OBJECTS})
        target_link_libraries(${name} ${STRATA_LIBRARY_DEPENDENCIES})
    endif()
endfunction()
