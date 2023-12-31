add_compile_definitions(STRATA_USE_CONAN_UNIVERSAL)
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    execute_process(COMMAND conan install "${CMAKE_SOURCE_DIR}")
endif()
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
include_directories(${CONAN_INCLUDE_DIRS_UNIVERSAL})
MESSAGE (STATUS  "Conan package ${CONAN_PACKAGE_NAME}, version ${CONAN_PACKAGE_VERSION}, build type ${CMAKE_BUILD_TYPE}")
