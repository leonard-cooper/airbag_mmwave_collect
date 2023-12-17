/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRadarAtr22.hpp"

#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/protocol/protocol_definitions.h>


RemoteRadarAtr22::RemoteRadarAtr22(IVendorCommands *commands, uint8_t id) :
    RemoteRadar(commands, id),
    m_vendorCommands {commands, CMD_COMPONENT, getType(), getImplementation(), id, SUBIF_DEFAULT},
    m_registers(1, commands, getType(), getImplementation(), id),
    m_commands(commands, id)
{
}

IRegisters<uint16_t, uint16_t> *RemoteRadarAtr22::getIRegisters()
{
    return &m_registers;
}

ICommandsAtr22 *RemoteRadarAtr22::getICommandsAtr22()
{
    return &m_commands;
}
