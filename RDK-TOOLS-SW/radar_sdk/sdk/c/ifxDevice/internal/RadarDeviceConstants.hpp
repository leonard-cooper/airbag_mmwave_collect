/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @internal
 * @file RadarDeviceConstants.hpp
 *
 * @brief Defines the Radar Device constants.
*/

#include <cstdint>
#include <array>

#ifndef IFX_RADAR_INTERNAL_RADAR_DEVICE_CONSTANTS_HPP
#define IFX_RADAR_INTERNAL_RADAR_DEVICE_CONSTANTS_HPP

// vendor and product ids of our boards
constexpr uint16_t vid_infineon = 0x058b;
constexpr uint16_t pid_RadarBaseboardMCU7 = 0x0251;  // PID of RadarBaseboardMCU7 (as returned by Strata)
constexpr uint16_t pid_RadarBaseboardAurix = 0x0252; // PID of RadarBaseboardAurix (as returned by Strata)
constexpr uint16_t pid_V9 = 0x0253; // PID of V9 board (as returned by Strata)

// Minimum firmware versions required for Aurix and MCU7 boards
constexpr std::array<uint16_t, 3> min_firmware_version_aurix = { 2, 4, 0 };
constexpr std::array<uint16_t, 3> min_firmware_version_mcu7 = { 2, 4, 0 };
constexpr std::array<uint16_t, 3> min_firmware_version_v9 = { 2, 4, 0 };

#endif
