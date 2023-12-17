option(STRATA_PACKAGES_AUTO_DETECT "find and build additional packages automatically" ON)
option(STRATA_SHARED_LIBRARY "build shared library" ON)
option(STRATA_BUILD_WRAPPERS "try to build wrappers for Matlab, Python, etc." ON)
option(STRATA_BUILD_SAMPLES "build sample applications" OFF)
option(STRATA_BUILD_TOOLS "build tool applications" OFF)
option(STRATA_BUILD_TESTS "build integration and unit tests" OFF)

# if STRATA_MULTIPLE_PYTHON_WRAPPER_VERSIONS is enabled pybind11MultiVersion and the conan python package will be used for the build
option(STRATA_MULTIPLE_PYTHON_WRAPPER_VERSIONS "build the python wrapper for multiple python versions at once" OFF)
option(STRATA_USE_CONAN_UNIVERSAL "use the universal library from Conan" OFF)

if (NOT STRATA_USE_CONAN_UNIVERSAL)
    set(QT_CREATOR_SKIP_CONAN_SETUP True)
endif()


#set(STRATA_PACKAGES "") # STRATA_PACKAGES_AUTO_DETECT has to be disabled, since it overwrites this list


cmake_minimum_required(VERSION 3.5.1)
project(Strata)

set(STRATA_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/Strata.cmake")
include(${STRATA_CMAKE})
include(../local_settings.cmake OPTIONAL)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(STRATA_USE_CONAN_UNIVERSAL)
    strata_include(Conan)
endif()

strata_include(Format)
addFormatTargets(library samples tests tools wrappers)

add_subdirectory_optional(contrib)

add_definitions(-DSTRATA_EXPORT) # this will tell the windows compiler to generate the DLL export file for the linker

add_subdirectory(library)

if(STRATA_BUILD_WRAPPERS)
    include(wrappers/matlab.cmake)
    include(wrappers/python.cmake)

    add_subdirectory(wrappers)
endif()
if(STRATA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(STRATA_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
if(STRATA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

add_subdirectory_optional(packages)
