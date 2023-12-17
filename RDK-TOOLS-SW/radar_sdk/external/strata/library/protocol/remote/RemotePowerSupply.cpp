/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemotePowerSupply.hpp"


RemotePowerSupply::RemotePowerSupply(IVendorCommands *commands, uint8_t id) :
    m_registers(1, commands, id, getType(), COMPONENT_IMPL_DEFAULT)
{
}

IRegisters<uint8_t> *RemotePowerSupply::getIRegisters()
{
    return &m_registers;
}
