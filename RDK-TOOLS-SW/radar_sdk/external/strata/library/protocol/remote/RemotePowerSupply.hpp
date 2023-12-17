/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteRegisters.hpp"
#include <components/interfaces/IPowerSupply.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemotePowerSupply :
    public IPowerSupply
{
public:
    RemotePowerSupply(IVendorCommands *commands, uint8_t id);

    //IPowerSupply
    IRegisters<uint8_t> *getIRegisters() override;

private:
    RemoteRegisters<uint8_t> m_registers;
};
