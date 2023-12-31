/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <Definitions.hpp>
#include <cstdint>
#include <universal/modules/types.h>

class IModule
{
public:
    STRATA_API virtual ~IModule() = default;

    /**
     * @brief startMeasurements
     * @param measurementCycle time in seconds between measurements.
     * If 0 , measurements shall be manually trigger by calling doMeasurement().
     */
    virtual void startMeasurements(double measurementCycle) = 0;
    virtual void stopMeasurements()                         = 0;
    virtual void doMeasurement()                            = 0;
    virtual void reset()                                    = 0;
    virtual void configure()                                = 0;
    //  virtual void run() = 0;
};
