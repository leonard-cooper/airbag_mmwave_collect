/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteCommandsAvian.hpp"

#include <algorithm>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/icommands.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemoteCommandsAvian::RemoteCommandsAvian(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_AVIAN, id, SUBIF_COMMANDS}
{
}

void RemoteCommandsAvian::execute(const Command commands[], uint32_t count, uint32_t results[])
{
    constexpr uint16_t elemSize    = sizeof(commands[0]);
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t wLength       = wCount * elemSize;

        if (results)
        {
            m_vendorCommands.vendorTransferChecked(FN_COMMANDS_EXECUTE, wLength, reinterpret_cast<const uint8_t *>(commands), wLength, reinterpret_cast<uint8_t *>(results));
            results += wCount;
        }
        else
        {
            m_vendorCommands.vendorWrite(FN_COMMANDS_EXECUTE, wLength, reinterpret_cast<const uint8_t *>(commands));
        }

        commands += wCount;
        count -= wCount;
    }
}

void RemoteCommandsAvian::setBits(const ICommandsAvian::Command commandMask)
{
    m_vendorCommands.vendorWrite(FN_COMMANDS_SET_BITS, sizeof(commandMask), reinterpret_cast<const uint8_t *>(&commandMask));
}
