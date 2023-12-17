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
#include <components/interfaces/ICommandsAvian.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemoteCommandsAvian :
    public ICommandsAvian
{
public:
    RemoteCommandsAvian(IVendorCommands *commands, uint8_t id);

    void execute(const Command commands[], uint32_t count, uint32_t results[] = nullptr) override;
    void setBits(const Command commandMask) override;

private:
    RemoteVendorCommands m_vendorCommands;
};
