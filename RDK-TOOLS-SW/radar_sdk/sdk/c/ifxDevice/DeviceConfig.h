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
 * @file DeviceConfig.h
 *
 * \brief \copybrief gr_deviceconfig
 *
 * For details refer to \ref gr_deviceconfig
 */

#ifndef IFX_RADAR_DEVICE_CONFIG_H
#define IFX_RADAR_DEVICE_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdint.h>

#include "ifxBase/Types.h"
#include "ifxDevice/Shield.h"


/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/


#define IFX_DEVICE_SAMPLE_RATE_HZ_LOWER ((uint32_t)80000)
#define IFX_DEVICE_SAMPLE_RATE_HZ_UPPER ((uint32_t)2800000)

#define IF_GAIN_DB_LOWER ((uint32_t)18)
#define IF_GAIN_DB_UPPER ((uint32_t)60)

#define TX_POWER_LEVEL_LOWER ((uint32_t)0)
#define TX_POWER_LEVEL_UPPER ((uint32_t)31)


/*
==============================================================================
   3. TYPES
==============================================================================
*/

enum ifx_Device_MIMO_Mode
{
    IFX_MIMO_OFF = 0, /**< MIMO is deactivated */
    IFX_MIMO_TDM = 1  /**< time-domain multiplexing MIMO */
};

/**
 * This enumeration type lists the available ADC tracking modes.
 *
 * The Avian device's internal Analog-Digital-Converter (ADC) has 11 bit
 * resolution and a 12 bit result register. A single conversion always produces
 * result values with unset LSB. The resolution can be increased by performing
 * additional tracking conversion. The result will be the average of all
 * conversions.
 */
enum ifx_Device_ADC_Tracking
{
    IFX_ADC_NO_SUBCONVERSIONS = 0,  //!< Each sampled voltage value is converted once.
    IFX_ADC_1_SUBCONVERSIONS  = 1,  //!< Each sampled voltage value is converted two times.
    IFX_ADC_3_SUBCONVERSIONS  = 2,  //!< Each sampled voltage value is converted four times.
    IFX_ADC_7_SUBCONVERSIONS  = 3   //!< Each sampled voltage value is converted eight times.
};

/**
 * This enumeration type lists the available ADC sample time settings.
 *
 * The sample time is the time that the sample-and-hold-circuitry of the Avian
 * device's Analog-Digital-Converter (ADC) takes to sample the voltage at it's
 * input.
 *
 * \note The specified timings refer to an external reference clock frequency
 *       of 80MHz. If the clock frequency differs from that, the sample time
 *       periods are scaled accordingly.
 */
enum ifx_Device_ADC_SampleTime
{
    IFX_ADC_SAMPLETIME_50NS  = 0,   //!< The voltage is sampled for 50ns.
    IFX_ADC_SAMPLETIME_100NS = 1,   //!< The voltage is sampled for 100ns.
    IFX_ADC_SAMPLETIME_200NS = 2,   //!< The voltage is sampled for 200ns.
    IFX_ADC_SAMPLETIME_400NS = 3    //!< The voltage is sampled for 400ns.
};

/**
 * This enumeration type lists the available ADC oversampling modes.
 *
 * The Avian device's internal Analog-Digital-Converter (ADC) is capable to
 * repeat the full sample-hold-convert cycle multiple times and return the
 * average of all cycles.
 */
enum ifx_Device_ADC_Oversampling
{
    IFX_ADC_OVERSAMPLING_OFF = 0,   //!< No oversampling
    IFX_ADC_OVERSAMPLING_2x  = 1,   //!< Oversampling factor 2
    IFX_ADC_OVERSAMPLING_4x  = 2,   //!< Oversampling factor 4
    IFX_ADC_OVERSAMPLING_8x  = 3    //!< Oversampling factor 8
};

/**
* This type enumerates the available modes of the Avian device's test
* signal generator.
*/
enum ifx_Device_Test_Signal_Generator_Mode
{
    IFX_TEST_SIGNAL_MODE_OFF,               /*!< The generator is not used.*/
    IFX_TEST_SIGNAL_MODE_BASEBAND_TEST,     /*!< The test signal is routed to the input of the
                                                 baseband filters instead of the RX mixer output signals. */
    IFX_TEST_SIGNAL_MODE_TOGGLE_TX_ENABLE,  /*!< TX1 is toggled on and off controlled by the
                                                 generated test signal. */
    IFX_TEST_SIGNAL_MODE_TOGGLE_DAC_VALUE,  /*!< The power DAC value is toggled between minimum
                                                 value and the value set trough
                                                 \ref ifx_device_cw_set_tx_dac_value controlled by the
                                                 generated test signal. */
    IFX_TEST_SIGNAL_MODE_TOGGLE_RX_SELF_TEST /*!< The test signal is routed directly into the RF
                                                 mixer of RX antenna 1. */
};

// ---------------------------------------------------------------------------- Hp_Gain
/**
 * This enumeration type lists the available gain settings of the Avian
 * device's integrated baseband high pass filters.
 */
enum ifx_Device_Baseband_Hp_Gain
{
    IFX_HP_GAIN_18dB,  //!< The gain of the high pass filter is +18dB.
    IFX_HP_GAIN_30dB   //!< The gain of the high pass filter is +30dB.
};

// ---------------------------------------------------------------------------- Hp_Cutoff
/**
 * This enumeration type lists the available cutoff frequency settings of the
 * Avian device's integrated baseband high pass filters.
 */
enum ifx_Device_Baseband_Hp_Cutoff
{
    IFX_HP_CUTOFF_20kHz,   //!< The cutoff frequency of the high pass filter is 20kHz.
    IFX_HP_CUTOFF_45kHz,   //!< The cutoff frequency of the high pass filter is 45kHz.
    IFX_HP_CUTOFF_70kHz,   //!< The cutoff frequency of the high pass filter is 70kHz.
    IFX_HP_CUTOFF_80kHz,   //!< The cutoff frequency of the high pass filter is 80kHz.
    IFX_HP_CUTOFF_160kHz,  //!< The cutoff frequency of the high pass filter is 160kHz.
};

// ---------------------------------------------------------------------------- Vga_Gain
/*!
 * This enumeration type lists the available gain settings of the Avian
 * device's integrated baseband amplifier (VGA).
 */
enum ifx_Device_Baseband_Vga_Gain
{
    IFX_VGA_GAIN_0dB,   //!< The gain of the VGA is 0dB.
    IFX_VGA_GAIN_5dB,   //!< The gain of the VGA is +5dB.
    IFX_VGA_GAIN_10dB,  //!< The gain of the VGA is +10dB.
    IFX_VGA_GAIN_15dB,  //!< The gain of the VGA is +15dB.
    IFX_VGA_GAIN_20dB,  //!< The gain of the VGA is +20dB.
    IFX_VGA_GAIN_25dB,  //!< The gain of the VGA is +25dB.
    IFX_VGA_GAIN_30dB   //!< The gain of the VGA is +30dB.
};

// ---------------------------------------------------------------------------- Aaf_Cutoff
/*!
 * This enumeration type lists the available cutoff frequency settings of the
 * Avian device's integrated baseband anti alias filter.
 */
enum ifx_Device_Baseband_Aaf_Cutoff
{
    IFX_AAF_CUTOFF_600kHz,  //!< The cutoff frequency of the anti alias filter is 600kHz.
    IFX_AAF_CUTOFF_1MHz     //!< The cutoff frequency of the anti alias filter is 1MMHz.
};

typedef struct
{
    struct { uint32_t min; uint32_t max; } sample_rate_Hz;
    struct { uint32_t min; uint32_t max; } rx_mask;
    struct { uint32_t min; uint32_t max; } tx_mask;
    struct { uint32_t min; uint32_t max; } tx_power_level;
    struct { uint32_t min; uint32_t max; } if_gain_dB;
    struct { ifx_Float_t min; ifx_Float_t max; } max_range_m;
    struct { ifx_Float_t min; ifx_Float_t max; } range_resolution_m;
    struct { ifx_Float_t min; ifx_Float_t max; } speed_resolution_m_s;
    struct { ifx_Float_t min; ifx_Float_t max; } max_speed_m_s;
    struct { ifx_Float_t min; ifx_Float_t max; } frame_repetition_time_s;
} ifx_Device_Metrics_Limits_t;

typedef struct
{
    const char* description;               /**< A pointer to a null terminated
                                                string holding a human
                                                readable description of the
                                                device. */
    uint64_t         min_rf_frequency_Hz;  /**< The minimum RF frequency the
                                                the sensor device can emit. */
    uint64_t         max_rf_frequency_Hz;  /**< The maximum RF frequency the
                                                sensor device can emit. */
    uint8_t          num_tx_antennas;      /**< The number of RF antennas used
                                                for transmission. */
    uint8_t          num_rx_antennas;      /**< The number of RF antennas used
                                                for reception. */
    uint8_t          max_tx_power;         /**< The amount of RF transmission
                                                power can be controlled in the
                                                range of 0 ... max_tx_power.
                                                */
    uint8_t          num_temp_sensors;     /**< The number of available
                                                temperature sensors. */
    uint8_t          interleaved_rx;       /**< If this is 0, the radar data
                                                of multiple RX antennas is
                                                stored in consecutive data
                                                blocks, where each block holds
                                                data of one antenna. If this
                                                is non-zero, the radar data of
                                                multiple RX antennas is stored
                                                in one data block, where for
                                                each point in time the samples
                                                from all RX antennas are
                                                stored consecutively before
                                                the data of the next point in
                                                time follows. */
    ifx_Device_Shield_Type_t shield_type;  /**< The shield type. */
} ifx_Device_Info_t;

typedef struct
{
    const char* description; /**< A pointer to a zero-terminated string
                                  containing a firmware description. */
    uint16_t version_major; /**< The firmware version major number. */
    uint16_t version_minor; /**< The firmware version minor number. */
    uint16_t version_build; /**< The firmware version build number. */

    const char* extendedVersion; /**< Extended firmware version with additional version information */
} ifx_Firmware_Info_t;

/**
* @brief Defines the structure for the metrics of the feature space used for presence detection.
*
* The presence detection algorithm analyzes the distance (range) and velocity (speed) of targets
* in the field of view, so the time domain input data must be transformed to range/speed feature
* space. Resolution and maximum values in this feature space depend on the time domain acquisition
* parameters and the presence sensing algorithm calculates them from the device configuration. To
* go the other way round and specify the metrics the function
* \ref ifx_device_translate_metrics_to_config can be used to derive those acquisition parameters
* from desired feature space metrics. This structure is used to specify the desired metrics to
* that function.
*/
typedef struct
{
    uint32_t sample_rate_Hz; /**< Sampling rate of the ADC used to acquire the samples
                                  during a chirp. The duration of a single chirp depends
                                  on the number of samples and the sampling rate. */

    uint32_t rx_mask;        /**< Bitmask where each bit represents one RX antenna of
                                  the radar device. If a bit is set the according RX
                                  antenna is enabled during the chirps and the signal
                                  received through that antenna is captured. The least
                                  significant bit corresponds to antenna 1. */
    uint32_t tx_mask;        /**< Bitmask where each bit represents one TX antenna. */
    uint32_t tx_power_level; /**< This value controls the power of the transmitted RX
                                  signal. This is an abstract value between 0 and 31
                                  without any physical meaning. */
    uint32_t if_gain_dB;     /**< Amplification factor that is applied to the IF signal
                                  coming from the RF mixer before it is fed into the ADC. */

    ifx_Float_t range_resolution_m;   /**< The range resolution is the distance between two consecutive
                                           bins of the range transform. Note that even though zero
                                           padding before the range transform seems to increase this
                                           resolution, the true resolution does not change but depends
                                           only from the acquisition parameters. Zero padding is just
                                           interpolation! */
    ifx_Float_t max_range_m;          /**< The bins of the range transform represent the range
                                           between 0m and this value. (If the time domain input data it
                                           is the range-max_range_m ... max_range_m.) */
    ifx_Float_t max_speed_m_s;        /**< The bins of the Doppler transform represent the speed values
                                           between -max_speed_m_s and max_speed_m_s. */
    ifx_Float_t speed_resolution_m_s; /**< The speed resolution is the distance between two consecutive
                                           bins of the Doppler transform. Note that even though zero
                                           padding before the speed transform seems to increase this
                                           resolution, the true resolution does not change but depends
                                           only from the acquisition parameters. Zero padding is just
                                           interpolation! */

    ifx_Float_t frame_repetition_time_s; /**< The desired frame repetition time in seconds (also known
                                              as frame time or frame period). The frame repetition time
                                              is the inverse of the frame rate */

    ifx_Float_t center_frequency_Hz; /**< Center frequency of the FMCW chirp. If the value is set to 0
                                          the center frequency will be determined from the device */
} ifx_Device_Metrics_t;

/**
 * @brief Defines the structure for acquisition of time domain data related settings.
 *
 * When a connection to sensor device is established, the device is configured according to the
 * parameters of this struct.
 */
typedef struct
{
    uint32_t sample_rate_Hz; /**< Sampling rate of the ADC used to acquire the samples
                                  during a chirp. The duration of a single chirp depends
                                  on the number of samples and the sampling rate. */

    uint32_t rx_mask;        /**< Bitmask where each bit represents one RX antenna of
                                  the radar device. If a bit is set the according RX
                                  antenna is enabled during the chirps and the signal
                                  received through that antenna is captured. The least
                                  significant bit corresponds to antenna 1. */
    uint32_t tx_mask;        /**< Bitmask where each bit represents one TX antenna. */
    uint32_t tx_power_level; /**< This value controls the power of the transmitted RX
                                  signal. This is an abstract value between 0 and 31
                                  without any physical meaning. */
    uint32_t if_gain_dB;     /**< Amplification factor that is applied to the IF signal
                                  coming from the RF mixer before it is fed into the ADC. */

    uint64_t lower_frequency_Hz; /**< Lower frequency (start frequency) of the FMCW chirp. */
    uint64_t upper_frequency_Hz; /**< Upper frequency (stop frequency) of the FMCW chirp. */

    uint32_t num_samples_per_chirp; /**< This is the number of samples acquired during each
                                         chirp of a frame. The duration of a single
                                         chirp depends on the number of samples and the
                                         sampling rate. */
    uint32_t num_chirps_per_frame;  /**< This is the number of chirps a single data frame
                                         consists of. */

    ifx_Float_t chirp_repetition_time_s; /**< This is the time period that elapses between the
                                              beginnings of two consecutive chirps in a frame.
                                              (Also commonly referred to as pulse repetition time
                                              or chirp-to-chirp-time.) */
    ifx_Float_t frame_repetition_time_s; /**< This is the time period that elapses between the
                                              beginnings of two consecutive frames. The reciprocal
                                              of this parameter is the frame rate. (Also commonly
                                              referred to as frame time or frame period.) */

    enum ifx_Device_MIMO_Mode mimo_mode; /**< Mode of MIMO, see definition of \ref ifx_Device_MIMO_Mode */
} ifx_Device_Config_t;

/**
 * @brief Defines the structure for the ADC configuration.
 */
typedef struct
{
    uint32_t samplerate_Hz;
    enum ifx_Device_ADC_Tracking tracking;
    enum ifx_Device_ADC_SampleTime sample_time;
    uint8_t double_msb_time;
    enum ifx_Device_ADC_Oversampling oversampling;
} ifx_Device_ADC_Config_t;

/**
 * @brief Defines the structure for the Baseband configuration.
 */
typedef struct {
    enum ifx_Device_Baseband_Vga_Gain vga_gain;
    enum ifx_Device_Baseband_Hp_Gain hp_gain;
    enum ifx_Device_Baseband_Hp_Cutoff hp_cutoff;
    enum ifx_Device_Baseband_Aaf_Cutoff aaf_cutoff;
} ifx_Device_Baseband_Config_t;

/**
 * @brief Defines the structure for the Test Signal configuration.
 */
typedef struct {
    enum ifx_Device_Test_Signal_Generator_Mode mode;
    ifx_Float_t frequency_Hz;
} ifx_Device_Test_Signal_Generator_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Radar_Device
  * @{
  */

/** @defgroup gr_deviceconfig Device Configuration
  * @brief API for Radar device configuration
  * @{
  */

/**
* @brief Calculates the center frequency in use from given device configuration
*
* @param [in]     config    Device configuration structure
* @return Center frequency (in Hz) used by device configuration
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_devconf_get_center_frequency(const ifx_Device_Config_t* config);

/**
* @brief Calculates the bandwidth in use from given device configuration
*
* @param [in]     config    Device configuration structure
* @return Bandwidth (in Hz) used by device configuration
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_devconf_get_bandwidth(const ifx_Device_Config_t* config);

/**
* @brief Calculates the Chirp Repetition Time in use from given device configuration
*
* The chirp repetition time is also known as pulse repetition time (PRT).
*
* @param [in]     config    Device configuration structure
* @return Pulse Repetition Time (in seconds) used by device configuration
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_devconf_get_chirp_repetition_time(const ifx_Device_Config_t* config);

/**
* @brief Calculates the Chirp time in use from given device configuration
*
* The chirp time is the time where sampling is performed to get sampler per chirp. Initial chirp delays 
* like PLL setup delay and ADC sampling delay are not used for chirp time calculations.
*
* @param [in]     config    Device configuration structure
* @return Chirp time (in seconds) used by device configuration
*/
IFX_DLL_PUBLIC
ifx_Float_t ifx_devconf_get_chirp_time(const ifx_Device_Config_t* config);

/**
* @brief Counts the number of receive antennas based on the given device configuration
*
* If MIMO is active this function will return virtual receive antenna count. 
* 
* @param [in]     config    Device configuration structure
* @return Number of receive antennas activated by device configuration
*/
IFX_DLL_PUBLIC
uint8_t ifx_devconf_count_rx_antennas(const ifx_Device_Config_t* config);
/**
  * @}
  */
 
/**
  * @}
  */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_DEVICE_CONFIG_H */
