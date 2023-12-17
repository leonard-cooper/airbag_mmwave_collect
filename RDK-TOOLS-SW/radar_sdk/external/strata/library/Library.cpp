/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Library.hpp"


void Strata::getVersion(uint16_t &major, uint16_t &minor, uint16_t &patch, uint16_t &build)
{
    major = STRATA_VERSION_MAJOR;
    minor = STRATA_VERSION_MINOR;
    patch = STRATA_VERSION_PATCH;
    build = STRATA_VERSION_BUILD;
}
