/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemotePinsAvian.hpp"

#include <platform/exception/EProtocol.hpp>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/ipins.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>


RemotePinsAvian::RemotePinsAvian(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_AVIAN, id, SUBIF_PINS}
{
}

void RemotePinsAvian::setResetPin(bool state)
{
    const uint8_t bState = state ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_PINS_SET_RESET_PIN, sizeof(bState), &bState);
}

bool RemotePinsAvian::getIrqPin()
{
    uint8_t state;
    m_vendorCommands.vendorRead(FN_PINS_GET_IRQ, sizeof(state), &state);
    return (state != 0);
}

void RemotePinsAvian::reset()
{
    m_vendorCommands.vendorWrite(FN_PINS_RESET, 0, nullptr);
}
