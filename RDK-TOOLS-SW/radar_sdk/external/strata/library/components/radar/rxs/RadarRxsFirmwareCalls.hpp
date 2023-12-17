/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IRadarRxs.hpp>


class RadarRxsFirmwareCalls :
    public IRadarRxs
{
public:
    virtual ~RadarRxsFirmwareCalls() = default;

    uint16_t executeFirmwareFunction(uint16_t callcode, const uint16_t params[] = nullptr, uint16_t paramCount = 0, uint16_t retVals[] = nullptr, uint16_t maxRetCount = 0, uint16_t *retCount = nullptr) override;
    uint16_t executeFirmwareFunction(uint16_t callcode, const std::vector<uint16_t> &params, std::vector<uint16_t> *retVals = nullptr) override;
};
