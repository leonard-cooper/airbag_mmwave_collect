/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Dac7562.hpp"
#include <universal/spi_definitions.h>


Dac7562::Dac7562(ISpi *accessSpi, uint8_t devId) :
    m_accessSpi {accessSpi},
    m_devId {devId}
{
    m_accessSpi->configure(m_devId, SPI_MODE_1, 24, 5000000);
}

void Dac7562::setGain(uint8_t gain1, uint8_t gain2)
{
    // gain can only be 1 or 2
    uint8_t gain;
    if (gain1 == 2 && gain2 == 2)
    {
        gain = 0x00;
    }
    else if (gain1 == 1 && gain2 == 2)
    {
        gain = 0x01;
    }
    else if (gain1 == 2 && gain2 == 1)
    {
        gain = 0x02;
    }
    else if (gain1 == 1 && gain2 == 1)
    {
        gain = 0x03;
    }
    else
    {
        gain = 0x00;
    }

    // Write to DAC-A input register and update DAC-A
    // 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    //  X  X  0  0  0  0  1  0  X  X  X  X  X  X  X  X  X  X  X  X  X  X  G  G
    const uint32_t cmd = (0x2 << 16) | gain;
    m_accessSpi->write(m_devId, 1, &cmd);
}

void Dac7562::enableInternalReference(bool enableInternalReference)
{
    // enableInternalReference == 1 -> enable -> gain will be set to 2 automatically
    // enableInternalReference == 0 -> disable -> gain will be set to 1 automatically
    const uint8_t enabled = (enableInternalReference) ? 0x01 : 0x00;

    // Write to DAC-A input register and update DAC-A
    // 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    //  X  X  1  1  1  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  X  E
    const uint32_t cmd = (0x7 << 19) | enabled;
    m_accessSpi->write(m_devId, 1, &cmd);
}

void Dac7562::powerUpDown(uint8_t ID, bool powerUp)
{
    // ID: 0 = DAC-A, 1 = DAC-B
    uint8_t powerCfg;
    if (powerUp)
    {
        powerCfg = 0x01 + ID;
    }
    else
    // Power down (Hi-Z)
    {
        powerCfg = 0x31 + ID;
    }

    // Power on / off a specific DAC
    // 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    //  X  X  1  0  0  X  X  X  X  X  X  X  X  X  X  X  X  X  P  P  X  X  P  P
    const uint32_t cmd = (0x4 << 19) | powerCfg;
    m_accessSpi->write(m_devId, 1, &cmd);
}

void Dac7562::setValue(uint8_t ID, uint16_t value)
{
    // uValue: only 12 bits are valid
    // Analog Value = DIN / 2^12 * VRF * Gain; VRF = 2.5V or 2V
    // ID: 0 = DAC-A, 1 = DAC-B

    // Write to DAC-A input register and update DAC-A
    // 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    //  X  X  0  1  1  0  0 ID  D  D  D  D  D  D  D  D  D  D  D  D  X  X  X  X
    const uint32_t cmd = static_cast<uint32_t>(0x3 << 19) | static_cast<uint32_t>((ID & 0x01) << 16) | static_cast<uint32_t>((value & 0x0FFF) << 4);
    m_accessSpi->write(m_devId, 1, &cmd);
}
