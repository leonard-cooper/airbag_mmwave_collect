
strata_include(Compiler)

set(STRATA_LIBRARY_OBJECTS $<TARGET_OBJECTS:strata_library> CACHE INTERNAL "")
set(STRATA_LIBRARY_DEPENDENCIES "" CACHE INTERNAL "")
set(STRATA_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/impl/${STRATA_TARGET_PLATFORM}"
    CACHE INTERNAL ""
    )

include_directories(${STRATA_INCLUDE_DIRS})


set(STRATA_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Definitions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Library.hpp"
    )

set(STRATA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Library.cpp"
    )

add_library(strata_library OBJECT ${STRATA_HEADERS} ${STRATA_SOURCES})
target_compile_definitions(strata_library PUBLIC ${STRATA_VERSION_DEFINES})

add_library(strata_boards OBJECT BoardListProtocolImpl.cpp)


add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(modules)
add_subdirectory(platform)
add_subdirectory(protocol)

if (NOT STRATA_USE_CONAN_UNIVERSAL)
    add_subdirectory(universal)
endif()


add_library(strata_static STATIC Dummy.cpp $<TARGET_OBJECTS:strata_boards> ${STRATA_LIBRARY_OBJECTS})
target_link_libraries(strata_static PRIVATE ${STRATA_LIBRARY_DEPENDENCIES})
target_include_directories(strata_static INTERFACE ${STRATA_INCLUDE_DIRS})
target_compile_definitions(strata_static INTERFACE ${STRATA_VERSION_DEFINES})

if(STRATA_SHARED_LIBRARY)
    add_library(strata_shared SHARED Dummy.cpp $<TARGET_OBJECTS:strata_boards> ${STRATA_LIBRARY_OBJECTS})
    target_link_libraries(strata_shared PRIVATE ${STRATA_LIBRARY_DEPENDENCIES})
    target_include_directories(strata_static INTERFACE ${STRATA_INCLUDE_DIRS})
    target_compile_definitions(strata_shared INTERFACE ${STRATA_VERSION_DEFINES})
    target_compile_definitions(strata_shared INTERFACE "STRATA_IMPORT")
endif()


#file(GLOB_RECURSE PUBLIC_LIBRARY_HEADERS */interfaces/*.hpp)
set(PUBLIC_LIBRARY_INTERFACE_HEADERS
    "Definitions.hpp"
    "Library.hpp"
    "common/cpp11/memory.hpp"
    "common/cpp11/iterator.hpp"
    "components/interfaces/IComponent.hpp"
    "components/interfaces/IDac.hpp"
    "components/interfaces/IPinsRxs.hpp"
    "components/interfaces/IPowerSupply.hpp"
    "components/interfaces/IProcessingRadar.hpp"
    "components/interfaces/IRadar.hpp"
    "components/interfaces/IRadarRxs.hpp"
    "components/interfaces/IRegisters.hpp"
    "modules/interfaces/IModule.hpp"
    "modules/interfaces/IModuleRadar.hpp"
    "platform/interfaces/IBoard.hpp"
    "platform/interfaces/IBridge.hpp"
    "platform/interfaces/IBridgeControl.hpp"
    "platform/interfaces/IBridgeData.hpp"
    "platform/interfaces/IEnumerator.hpp"
    "platform/interfaces/IFrame.hpp"
    "platform/interfaces/IFrameListener.hpp"
    "platform/interfaces/IVendorCommands.hpp"
    "platform/interfaces/access/IData.hpp"
    "platform/interfaces/access/IGpio.hpp"
    "platform/interfaces/access/IMemory.hpp"
    "platform/interfaces/access/II2c.hpp"
    "platform/interfaces/access/ISpi.hpp"
    "platform/interfaces/access/IFlash.hpp"
#    "platform/interfaces/link/ISerialPort.hpp"
#    "platform/interfaces/link/ISocket.hpp"
#    "platform/interfaces/link/ISocketUdp.hpp"
#    "platform/interfaces/link/ISocketTcp.hpp"
#    "platform/interfaces/link/IMulticoreDebugger.hpp"
#    "platform/interfaces/link/IUvcExtension.hpp"
    "platform/BoardDescriptor.hpp"
    "platform/BoardInstance.hpp"
    "platform/BoardManager.hpp"
    "universal/data_definitions.h"
    "universal/error_definitions.h"
    "universal/gpio_definitions.h"
    "universal/i2c_definitions.h"
    "universal/spi_definitions.h"
    "universal/components/subinterfaces.h"
    "universal/components/types.h"
    "universal/components/implementations/radar.h"
    "universal/components/implementations/radar/iradarrxs.h"
    "universal/components/types/iradar.h"
    "universal/components/types/iprocessingradar.h"
    "universal/modules/types.h"
    "universal/modules/types/imoduleradar.h"
    "universal/types/IfxRfe_Types.h"
    "universal/types/IfxRsp_Types.h"
    "universal/types/Types_IRadarRxs.h"
    )


# clean include files from output directory on install
install(CODE "file(REMOVE_RECURSE ${STRATA_BINARY_DIR}/includes)")

install(DIRECTORY DESTINATION ${STRATA_BINARY_DIR})

install(CODE "file(WRITE \"${STRATA_BINARY_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS}/deps.txt\" \"${STRATA_LIBRARY_DEPENDENCIES}\")")

install(TARGETS strata_static DESTINATION ${STRATA_BINARY_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS})

if(STRATA_SHARED_LIBRARY)
    install(TARGETS strata_shared DESTINATION ${STRATA_BINARY_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS})
endif()

foreach(file ${PUBLIC_LIBRARY_INTERFACE_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file} DESTINATION ${STRATA_BINARY_DIR}/includes/${dir})
endforeach()

