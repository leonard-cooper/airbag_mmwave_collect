/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <components/exception/ENonvolatileMemory.hpp>
#include <components/nonvolatileMemory/NonvolatileMemoryEepromI2c.hpp>


NonvolatileMemoryEepromI2c::NonvolatileMemoryEepromI2c(II2c *access, uint16_t devAddr, const NonvolatileMemoryConfig_t &config) :
    NonvolatileMemory(config, access->getMaxTransfer()),
    m_access {access},
    m_devAddr {devAddr}
{
    if (((config.totalSize - 1) >> 16) & m_devAddr)
    {
        throw ENonvolatileMemory("Device address for I2C EEPROM and total size are conflicting", config.totalSize);
    }

    if (m_maxTransfer < config.pageSize)
    {
        throw ENonvolatileMemory("Access supports writes only smaller than page size", m_maxTransfer);
    }
}

NonvolatileMemoryEepromI2c::~NonvolatileMemoryEepromI2c()
{
}

void NonvolatileMemoryEepromI2c::readMemoryInterface(uint32_t address, uint32_t length, uint8_t buffer[])
{
    const uint8_t addressHigh = static_cast<uint8_t>(address >> 16);
    const uint16_t addressLow = static_cast<uint16_t>(address);

    if (addressHigh & m_devAddr)
    {
        throw ENonvolatileMemory("Device address for I2C EEPROM and read address are conflicting", address);
    }

    m_access->readWith16BitPrefix(m_devAddr | addressHigh, addressLow, length, buffer);
}

void NonvolatileMemoryEepromI2c::writeMemoryInterface(uint32_t address, uint32_t length, const uint8_t buffer[])
{
    const uint8_t addressHigh = static_cast<uint8_t>(address >> 16);
    const uint16_t addressLow = static_cast<uint16_t>(address);

    if (addressHigh & m_devAddr)
    {
        throw ENonvolatileMemory("Device address for I2C EEPROM and write address are conflicting", address);
    }

    m_access->writeWith16BitPrefix(m_devAddr | addressHigh, addressLow, length, buffer);
}

void NonvolatileMemoryEepromI2c::eraseMemoryInterface(uint32_t /*address*/)
{
}
