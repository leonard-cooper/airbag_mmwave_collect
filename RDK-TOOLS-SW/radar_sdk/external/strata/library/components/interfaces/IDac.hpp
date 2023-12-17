/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <cstdint>

class IDac
{
public:
    virtual ~IDac() = default;

    void setGain(uint8_t gain1, uint8_t gain2);
    void enableInternalReference(bool enableInternalReference);
    void powerUpDown(uint8_t ID, bool powerUp);
    void setValue(uint8_t ID, uint16_t value);
};
