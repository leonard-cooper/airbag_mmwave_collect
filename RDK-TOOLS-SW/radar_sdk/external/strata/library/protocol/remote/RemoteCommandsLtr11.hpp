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
#include <components/interfaces/ICommandsLtr11.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemoteCommandsLtr11 :
    public ICommandsLtr11
{
public:
    RemoteCommandsLtr11(IVendorCommands *commands, uint8_t id);

    void executeWrite(const Write &command, uint16_t count, uint16_t values[]) override;
    void executeRead(const Read &command, uint16_t count, uint16_t values[]) override;

private:
    RemoteVendorCommands m_vendorCommands;
};
