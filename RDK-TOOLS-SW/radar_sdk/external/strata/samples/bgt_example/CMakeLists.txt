cmake_minimum_required(VERSION 3.5.1)

project(bgt_example)


set(STRATA_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../bin")
set(BINARY_BUILD OFF CACHE BOOL "" FORCE)
set(STATIC_BUILD ON CACHE BOOL "" FORCE)


if(BINARY_BUILD)
    # the following is duplicated from cmake/Platform.cmake to avoid this dependency

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

    # end of duplicated code

    set(STRATA_LIB_DIR "${STRATA_BIN_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS}")


    if(EXISTS ${STRATA_LIB_DIR})
        # link with the already built binary strata lib
        include_directories(
            "${STRATA_BIN_DIR}/includes"
        )

        link_directories("${STRATA_LIB_DIR}")

        set(DEP_FILE "${STRATA_LIB_DIR}/deps.txt")
        if(EXISTS ${DEP_FILE})
            file(READ "${STRATA_LIB_DIR}/deps.txt" STRATA_LIBRARY_DEPENDENCIES)
        else()
            message(WARNING "Dependency information not found, link libraries have to be specified manually or linking might fail.")
        endif()
    else()
        message(STATUS "Target ${CMAKE_PROJECT_NAME}: Binary library directory not found, trying to link to default build...")
    endif()
endif()


add_executable(bgt_example
    bgt_example.cpp
)

if(STATIC_BUILD)
    target_link_libraries(bgt_example strata_static)
    if(STRATA_LIBRARY_DEPENDENCIES)
        target_link_libraries(bgt_example ${STRATA_LIBRARY_DEPENDENCIES})
    endif()
else()
    target_link_libraries(bgt_example strata_shared)
endif()
