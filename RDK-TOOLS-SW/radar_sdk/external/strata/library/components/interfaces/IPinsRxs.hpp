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
#include <universal/types/Types_IRadarRxs.h>


/**
 * This interface controls the platform GPIO pins
 * that are connected to the RXS.
 * When configuring the DMUX map on the RXS, these
 * functions are called automatically with the
 * correct settings.
 */
class IPinsRxs
{
public:
    virtual ~IPinsRxs() = default;

    virtual void setResetPin(bool state)                        = 0;
    virtual void reset()                                        = 0;
    virtual bool getOkPin()                                     = 0;
    virtual void configureDmuxPin(uint8_t index, uint8_t flags) = 0;
    virtual bool getDmuxPin(uint8_t index)                      = 0;
    virtual void setDmuxPin(uint8_t index, bool state)          = 0;
};
