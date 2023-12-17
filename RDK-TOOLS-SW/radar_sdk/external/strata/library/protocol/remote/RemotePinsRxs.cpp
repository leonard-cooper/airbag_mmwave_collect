/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemotePinsRxs.hpp"

#include <platform/exception/EProtocol.hpp>
#include <thread>
#include <universal/components/implementations/radar.h>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/ipins.h>
#include <universal/components/types.h>
#include <universal/protocol/protocol_definitions.h>

RemotePinsRxs::RemotePinsRxs(IVendorCommands *commands, uint8_t id) :
    m_vendorCommands {commands, CMD_COMPONENT, COMPONENT_TYPE_RADAR, COMPONENT_IMPL_RADAR_RXS, id, SUBIF_PINS}
{
}

void RemotePinsRxs::setResetPin(bool state)
{
    const uint8_t bState = state ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_PINS_SET_RESET_PIN, sizeof(bState), &bState);
}

void RemotePinsRxs::reset()
{
    //Special treatment for RXS where the AURIX clock changes during reset of the RXS
    //The reset command will not get a reply as the communication breaks during reset.
    //Then we test for a re-established connection by reading a pin
    uint8_t retryCnt          = 30;
    constexpr uint8_t delayMs = 100;
    try
    {
        m_vendorCommands.vendorWrite(FN_PINS_RESET, 0, nullptr);
    }
    catch (EProtocol &)
    {
        //Nothing to do, just catch
    }

    while (retryCnt--)
    {
        try
        {
            getOkPin();
            return;
        }
        catch (EProtocol &)
        {
            //Wait before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }

    //Generate an error if we didn't get the connection back
    throw EProtocol("Connection lost upon hard reset");
}

bool RemotePinsRxs::getOkPin()
{
    uint8_t state;
    m_vendorCommands.vendorRead(FN_PINS_GET_OK, sizeof(state), &state);
    return state;
}

void RemotePinsRxs::configureDmuxPin(uint8_t index, uint8_t flags)
{
    const uint8_t payload[] = {index, flags};
    m_vendorCommands.vendorWrite(FN_PINS_CONFIGURE_DMUX, sizeof(payload), payload);
}

bool RemotePinsRxs::getDmuxPin(uint8_t index)
{
    uint8_t state;
    m_vendorCommands.vendorTransferChecked(FN_PINS_GET_DMUX, sizeof(index), &index, sizeof(state), &state);
    return (state != 0);
}

void RemotePinsRxs::setDmuxPin(uint8_t index, bool state)
{
    const uint8_t payload[] = {index, static_cast<uint8_t>(state ? 1 : 0)};
    m_vendorCommands.vendorWrite(FN_PINS_SET_DMUX, sizeof(payload), payload);
}
