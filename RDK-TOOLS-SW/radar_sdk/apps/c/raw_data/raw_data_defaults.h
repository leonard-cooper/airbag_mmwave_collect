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
 * @file raw_data_defaults.h
 *
 * @brief This file defines the default configuration values for raw data app.
 *
 */

#ifndef RAW_DATA_DEFAULTS_H
#define RAW_DATA_DEFAULTS_H

 // Device configuration

#define IFX_FRAME_RATE_HZ               (30)         /**< This is the frame acquisition rate for raw data.
                                                         Valid range is [0.016 - 100]Hz.*/

#define IFX_TX_POWER_LEVEL              (31)        /**< TX Power level to be used on the radar transceiver.
                                                         Valid range is [1 - 31].*/

#define IFX_RX_MASK             (1)         /**< Receive antenna mask defines which antennas to activate. 
                                                         Multiple antennas can be activated by masking.*/
#define IFX_TX_MASK             (1)         /**< Transmitting antenna mask defines which antennas to activate. */


#define IFX_IF_GAIN_DB                  (33)        /**< This is the amplification factor that is
                                                         applied to the IF signal before sampling.
                                                         Valid range is [18 - 60]dB.*/

#define IFX_SAMPLE_RATE_HZ           (1000000U)  /**< Sample rate of the ADC.
                                                      Valid range is [100k - 2M]Hz.*/

#define IFX_NUM_SAMPLES_PER_CHIRP       (512)       /**< Number of samples per chirp.*/

#define IFX_NUM_CHIRPS_PER_FRAME        (32)        /**< Number of chirps in a frame.*/

#define IFX_LOWER_FREQUENCY_HZ         ((uint64_t)60500000000)  /**< Lower frequency of the bandwidth the chirps start at.*/

#define IFX_UPPER_FREQUENCY_HZ         ((uint64_t)61500000000)  /**< Higher frequency of the bandwidth the chirps end with.*/

#define IFX_CHIRP_REPETITION_TIME_S ((ifx_Float_t)538.61e-6)   /**< The time between consecutive chirps in 100 picoseconds steps.*/

#define IFX_FRAME_REPETITION_TIME_S             ((ifx_Float_t)1 / IFX_FRAME_RATE_HZ)  /**< Frame-rate in Hz to period in microseconds calculation */

#endif /* RAW_DATA_DEFAULTS_H */