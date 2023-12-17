/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RadarRxsFirmwareCalls.hpp"

#include <components/exception/ERadar.hpp>


uint16_t RadarRxsFirmwareCalls::executeFirmwareFunction(uint16_t callcode, const uint16_t params[], uint16_t paramCount, uint16_t retVals[], uint16_t maxRetCount, uint16_t *retCount)
{
    startFirmwareFunction(callcode, params, paramCount);

    uint16_t status = 0;
    uint16_t count;
    try
    {
        checkFirmwareFunctionStatus(status, count);
    }
    catch (...)
    {
        return status;
    }

    if (retCount != nullptr)
    {
        *retCount = count;
    }
    else if (count != maxRetCount)
    {
        throw ERadar("Number of return values does not match", count);
    }

    if (count)
    {
        if (count > maxRetCount)
        {
            throw ERadar("Too many return values", count);
        }
        getFirmwareFunctionResult(retVals, count);
    }

    return status;
}

uint16_t RadarRxsFirmwareCalls::executeFirmwareFunction(uint16_t callcode, const std::vector<uint16_t> &params, std::vector<uint16_t> *retVals)
{
    startFirmwareFunction(callcode, params.data(), static_cast<uint16_t>(params.size()));

    uint16_t status = 0;
    uint16_t count;
    try
    {
        checkFirmwareFunctionStatus(status, count);
    }
    catch (...)
    {
        return status;
    }

    if (count)
    {
        if (retVals == nullptr)
        {
            getFirmwareFunctionResult(nullptr, count);
        }
        else
        {
            retVals->resize(count);
            getFirmwareFunctionResult(retVals->data(), count);
        }
    }

    return status;
}
