#cmake_minimum_required(VERSION 3.12)

include(CheckLibraryExists)
include(Macros)

option(SDK_ENABLE_DOUBLE "double-precision" OFF)
option(SDK_ENABLE_LOGS   "enable logging with debug log level " OFF)
option(SDK_ENABLE_IPO    "enable interprocedural optimization if available" ON)

if(SDK_ENABLE_LOGS)
    add_definitions(-DIFX_LOG_SEVERITY_DEBUG=1)
endif()

if(SDK_ENABLE_DOUBLE)
    add_definitions(-DIFX_USE_DOUBLE=1)
endif()

if(IPO AND NOT MINGW)
    # check if compiler supports interprocedural optimization
    include(CheckIPOSupported)
    check_ipo_supported(RESULT sdk_use_ipo OUTPUT ipo_error)
else()
    # no IPO on MinGW
    set(sdk_use_ipo False)
endif()

# Check if it is necessary to link against libm
check_library_exists(m sqrt "" HAS_LIBM)

subdirlist(directories ${CMAKE_CURRENT_LIST_DIR})
foreach(directory ${directories})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
		add_subdirectory(${directory})
	endif()
endforeach()