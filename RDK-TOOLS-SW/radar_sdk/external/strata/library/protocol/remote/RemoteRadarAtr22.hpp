/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteCommandsAtr22.hpp"
#include "RemoteRadar.hpp"
#include "RemoteRegisters.hpp"
#include <components/interfaces/IRadarAtr22.hpp>


class RemoteRadarAtr22 :
    public IRadarAtr22,
    public RemoteRadar
{
public:
    RemoteRadarAtr22(IVendorCommands *commands, uint8_t id);

    IRegisters<uint16_t, uint16_t> *getIRegisters() override;
    ICommandsAtr22 *getICommandsAtr22() override;

private:
    RemoteVendorCommands m_vendorCommands;

    RemoteRegisters<uint16_t, uint16_t> m_registers;
    RemoteCommandsAtr22 m_commands;
};
