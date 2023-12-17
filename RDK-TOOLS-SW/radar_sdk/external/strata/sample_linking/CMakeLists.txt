cmake_minimum_required(VERSION 3.5.1)

project(sample_linking)

set(STRATA_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

set(STATIC_BUILD ON CACHE BOOL "" FORCE)

# some of the code is duplicated from cmake/Platform.cmake because it should be available
# even without having access to the whole strata code
set(STRATA_TARGET_PLATFORM ${CMAKE_SYSTEM_NAME})
if(STRATA_TARGET_PLATFORM MATCHES "Darwin")
    set(STRATA_TARGET_PLATFORM Macos)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set(STRATA_TARGET_ARCHITECTURE x86)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm)|(ARM)")
    set(STRATA_TARGET_ARCHITECTURE arm)
else()
    set(STRATA_TARGET_ARCHITECTURE UNKNOWN)
endif()

math(EXPR STRATA_BITNESS "${CMAKE_SIZEOF_VOID_P}*8")

set(STRATA_LIB_DIR "${STRATA_BIN_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS}")

###

# try to link to the already built strata lib
if(EXISTS ${STRATA_LIB_DIR})
    include_directories("${STRATA_BIN_DIR}/includes")
    link_directories("${STRATA_LIB_DIR}")

    set(DEP_FILE "${STRATA_LIB_DIR}/deps.txt")
    if(EXISTS "${DEP_FILE}")
        file(READ "${DEP_FILE}" STRATA_LIBRARY_DEPENDENCIES)
    else()
        message(WARNING "Dependency information not found, link libraries have to be specified manually or linking might fail.")
    endif()
endif()

add_executable(sample_linking
    sample_linking.cpp
)

if (STATIC_BUILD)
    target_link_libraries(sample_linking strata_static$<$<CONFIG:Debug>:-d>)
    if(STRATA_LIBRARY_DEPENDENCIES)
        target_link_libraries(sample_linking ${STRATA_LIBRARY_DEPENDENCIES})
    endif()
else()
    target_link_libraries(sample_linking strata_shared$<$<CONFIG:Debug>:-d>)
endif()
