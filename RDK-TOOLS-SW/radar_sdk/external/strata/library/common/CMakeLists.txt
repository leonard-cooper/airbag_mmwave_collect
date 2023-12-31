
set(COMMON_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp11/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp11/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc6.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc16.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc32.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian/Big.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian/General.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian/Little.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian/LittleEndianReader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EConfig.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EException.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/EGenericException.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/ENarrowCast.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/ENotImplemented.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception/ETimeout.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BinUtils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ConsoleRedirect.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/EndianConversion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/HandleManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/NarrowCast.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ProductVersion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Raw12.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Serialization.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ScopeExit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Time.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Timing.hpp"
    )

set(COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc6.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc16.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/crc/Crc32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/endian/LittleEndianReader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ProductVersion.cpp"
    )

add_library(common OBJECT ${COMMON_HEADERS} ${COMMON_SOURCES})


set(STRATA_LIBRARY_OBJECTS
    ${STRATA_LIBRARY_OBJECTS}
    $<TARGET_OBJECTS:common>
    CACHE INTERNAL ""
    )
set(STRATA_LIBRARY_DEPENDENCIES
    ${STRATA_LIBRARY_DEPENDENCIES}
    CACHE INTERNAL ""
    )
