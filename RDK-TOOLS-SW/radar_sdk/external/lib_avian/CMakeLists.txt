cmake_minimum_required(VERSION 3.10)

project(lib_avian)

###############################################################################
# Source Files
###############################################################################
# All source and header files are collected.
file(GLOB_RECURSE source_files
     *.hpp
     *.cpp
     *.h)

# The file system structure of source files is preserved in generated IDE
# project.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             PREFIX "Source Files"
             FILES ${source_files})

###############################################################################
# Target #1: Static Library
###############################################################################
# The static library is built from all source files. The file
# VersionHistory.txt is added to make it appear in the IDE project, it does not
# contribute to the build.
add_library(lib_avian_static STATIC
            ${source_files}
            VersionHistory.txt)

# The directory containing API headers is announced for targets that link
# against this library. The directory containing source files is only used
# when building lib_avian, but not announced to depending projects.
target_include_directories(lib_avian_static
                           PUBLIC include
                           PRIVATE src)

# The name of the artifact is kept compact. It's necessary to call it "lib",
# because the file extension will indicate that the target is a library.
set_target_properties(lib_avian_static PROPERTIES
                      OUTPUT_NAME avian)
