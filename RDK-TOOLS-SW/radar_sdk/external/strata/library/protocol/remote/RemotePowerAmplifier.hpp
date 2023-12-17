/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteRegisters.hpp"
#include <components/interfaces/IPowerAmplifier.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


class RemotePowerAmplifier :
    public IPowerAmplifier
{
public:
    RemotePowerAmplifier(IVendorCommands *commands, uint8_t id);

    //IPowerAmplifier
    IRegisters<uint16_t> *getIRegisters() override;

private:
    RemoteRegisters<uint16_t> m_registers;
};
