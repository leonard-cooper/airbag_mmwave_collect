/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteCommandsAtr22.hpp"

#include <algorithm>
#include <common/Serialization.hpp>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/icommands.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemoteCommandsAtr22::RemoteCommandsAtr22(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_ATR22, id, SUBIF_COMMANDS}
{
}

void RemoteCommandsAtr22::executeWrite(const Write commands[], uint16_t count)
{
    constexpr uint16_t elemSize    = sizeof(commands[0]);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const decltype(count) wCount   = std::min(count, maxCount);

    while (count > 0)
    {
        const uint16_t wLengthSend = wCount * elemSize;
        m_vendorCommands.vendorWrite(FN_COMMANDS_EXECUTE, wLengthSend, reinterpret_cast<const uint8_t *>(commands));

        commands += wCount;
        count -= wCount;
    }
}

void RemoteCommandsAtr22::executeRead(const Read &command, uint16_t count, uint16_t values[])
{
    constexpr uint16_t elemSize    = sizeof(values[0]);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;
    const decltype(count) wCount   = std::min(count, maxCount);

    Read cmd = command;  // create a local copy of the command to be able to increment it for multi-part transfers

    while (count > 0)
    {
        const uint16_t wLengthReceive = wCount * elemSize;
        const uint16_t wLengthSend    = sizeof(cmd) + sizeof(wCount);
        uint8_t payload[wLengthSend];
        uint8_t *it = payload;
        it          = hostToSerial(it, cmd);
        it          = hostToSerial(it, wCount);

        m_vendorCommands.vendorTransferChecked(FN_COMMANDS_EXECUTE, wLengthSend, payload, wLengthReceive, reinterpret_cast<uint8_t *>(values));

        cmd.increment(wCount);
        values += wCount;
        count -= wCount;
    }
}
