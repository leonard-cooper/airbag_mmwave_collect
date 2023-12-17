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
 * @file segmentation_defaults.h
 *
 * @brief This file defines the default configuration values for segmentation app.
 *
 * For more detailed information about the parameters please refer to
 * the section on \ref sssct_app_segmentation_cnfg_param_summary in the documentation.
 *
 */

#ifndef SEGMENTATION_DEFAULTS_H
#define SEGMENTATION_DEFAULTS_H

// Device configuration

#define IFX_RANGE_RESOLUTION_M          (0.150f)    /**< Range resolution translates into bandwidth(BW)=c/(2*Rres).
                                                         This bandwidth impacts the range captured by one range FFT bin.
                                                         Valid range is [0.025 - 1.00] meter.*/

#define IFX_MAX_RANGE_M                 (9.59f)     /**< Maximum range recommended to be at least 8 times the range resolution.
                                                         Valid range is [0.20 - 20] meter.*/

#define IFX_SPEED_RESOLUTION_M_S        (0.08f)     /**< This paramter impacts the precision in speed.
                                                         Valid range is [0.025 - 0.833]m/s.*/

#define IFX_MAX_SPEED_M_S               (2.45f)     /**< This impacts chirp to chirp time parameter of chip.
                                                         Valid range is [0.25 - 25]m/s.*/

#define IFX_FRAME_RATE_HZ               (10)        /**< This is the frame acquisition rate for raw data.
                                                         Valid range is [0.016 - 100]Hz.*/

#define IFX_BGT_TX_POWER                (31U)       /**< TX Power level to be used on the radar transceiver.
                                                         Valid range is [1 - 31].*/

#define IFX_RX_MASK                     (5)         /**< Receive antenna mask defines which antennas to activate. 
                                                         Multiple antennas can be activated by masking.*/

#define IFX_TX_MASK                    (1)         /**< tx_mode decides which tx antenna to use in case multiple
                                                         tx antennas are supported by the device  e.g. 1 => Tx1; 2 => Tx2 */

#define IFX_IF_GAIN_DB                  (33)        /**< This is the amplification factor that is applied to the IF signal before sampling.
                                                         Valid range is [18 - 60]dB.*/

#define IFX_ADC_SAMPLERATE_HZ           (1000000U)  /**< Samplerate of the ADC. 
                                                         Valid range is [100k - 2M]Hz.*/

// Segmentation algorithm configuration

#define IFX_SEGMENT_MAX_RANGE_M         (7)         /**< Targets beyond this distance are not considered, even though the radar device can detect
                                                         them. This value cannot be greater than maximum range configured on device.
                                                         Valid range is [0.20 - 7.0] meter.
                                                         Note: this value cannot be greater than IFX_MAX_RANGE_M.*/

#define IFX_MAX_FOV_DEGREES             (120)       /**< Field of view in degrees 
                                                         Valid range is [0 - 120].*/

#define IFX_NUM_SEGMENTS                (3)         /**< Number of segments the field of view will be divided into.
                                                         Valid range is [1 - 12].*/

#define IFX_DETECTION_SENSITIVITY       (2)         /**< Sensitivity of detection, 
                                                         1 = high, 2 = medium, 3 = low.*/

#define IFX_STATIC_TARGET_CANCELATION   (2)         /**< Static target cancelation,
                                                         1 = high, 2 = medium, 3 = low.*/

#define IFX_HYSTERESIS                  (1)         /**< Enable or disable hysteresis for segment detection, 
                                                         0 = off, 1 = on.*/

#define IFX_MAX_NUM_TRACKS              (5)         /**< The maximum number of targets tracked.*/

#endif /* SEGMENTATION_DEFAULTS_H */
