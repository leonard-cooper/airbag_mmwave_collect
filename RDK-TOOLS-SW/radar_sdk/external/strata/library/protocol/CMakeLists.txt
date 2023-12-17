
set(PROTOCOL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsAtr22.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsAvian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteModuleRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsAvian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsRxs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteProcessingRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRegisters.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePowerSupply.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarAvian.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarAtr22.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarRxs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePowerAmplifier.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteVendorCommands.hpp"
    )

set(PROTOCOL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsAtr22.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteCommandsLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteModuleRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePinsRxs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteProcessingRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRegisters.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePowerSupply.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarAtr22.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteRadarRxs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemotePowerAmplifier.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/remote/RemoteVendorCommands.cpp"
    )

add_library(protocol OBJECT ${PROTOCOL_HEADERS} ${PROTOCOL_SOURCES})


set(STRATA_LIBRARY_OBJECTS
    ${STRATA_LIBRARY_OBJECTS}
    $<TARGET_OBJECTS:protocol>
    CACHE INTERNAL ""
    )
set(STRATA_LIBRARY_DEPENDENCIES
    ${STRATA_LIBRARY_DEPENDENCIES}
    CACHE INTERNAL ""
    )
