/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemotePowerAmplifier.hpp"


RemotePowerAmplifier::RemotePowerAmplifier(IVendorCommands *commands, uint8_t id) :
    m_registers(2, commands, id, getType(), COMPONENT_IMPL_DEFAULT)
{
}

IRegisters<uint16_t> *RemotePowerAmplifier::getIRegisters()
{
    return &m_registers;
}
