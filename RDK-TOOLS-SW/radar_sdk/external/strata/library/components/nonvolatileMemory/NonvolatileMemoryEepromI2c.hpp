/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/nonvolatileMemory/NonvolatileMemory.hpp>
#include <platform/interfaces/access/II2c.hpp>

class NonvolatileMemoryEepromI2c :
    public NonvolatileMemory
{
public:
    NonvolatileMemoryEepromI2c(II2c *access, uint16_t devAddr, const NonvolatileMemoryConfig_t &config);
    virtual ~NonvolatileMemoryEepromI2c();

protected:
    void readMemoryInterface(uint32_t address, uint32_t length, uint8_t buffer[]) override;
    void writeMemoryInterface(uint32_t address, uint32_t length, const uint8_t buffer[]) override;
    void eraseMemoryInterface(uint32_t address) override;

private:
    II2c *m_access;
    const uint16_t m_devAddr;
};
