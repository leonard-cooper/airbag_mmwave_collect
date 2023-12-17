/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteVendorCommands.hpp"
#include <components/interfaces/IPinsRxs.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemotePinsRxs :
    public IPinsRxs
{
public:
    RemotePinsRxs(IVendorCommands *commands, uint8_t id);

    void setResetPin(bool state) override;
    void reset() override;
    bool getOkPin() override;
    void configureDmuxPin(uint8_t index, uint8_t flags) override;
    bool getDmuxPin(uint8_t index) override;
    void setDmuxPin(uint8_t index, bool state) override;

private:
    RemoteVendorCommands m_vendorCommands;
};
