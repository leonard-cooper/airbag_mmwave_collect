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
 * @file DeviceControl.h
 *
 * \brief \copybrief gr_devicecontrol
 *
 * For details refer to \ref gr_devicecontrol
 */

#ifndef IFX_RADAR_DEVICE_CONTROL_H
#define IFX_RADAR_DEVICE_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdbool.h>

#include "ifxBase/Types.h"
#include "ifxBase/Error.h"
#include "ifxBase/List.h"

#include "ifxDevice/DeviceConfig.h"
#include "ifxDevice/Frame.h"
#include "ifxDevice/Shield.h"


/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/
typedef enum
{
    IFX_DEVICE_BGT60TR13C,
    IFX_DEVICE_BGT60ATR24C,
    IFX_DEVICE_BGT60UTR13D,
    IFX_DEVICE_BGT60TR12E,
    IFX_DEVICE_BGT60UTR11,
    IFX_DEVICE_BGT120UTR13E,
    IFX_DEVICE_BGT24LTR24,
    IFX_DEVICE_UNKNOWN
} ifx_Device_Radar_Type_t;

typedef struct {
    ifx_Device_Shield_Type_t board_type;
    char shield_uuid[64];
} ifx_Device_List_Entry_t;

typedef struct RadarDeviceBase* ifx_Device_Handle_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Radar_Device
  * @{
  */

/** @defgroup gr_devicecontrol Device Control
  * @brief API for Radar device control operations
  * @{
  */

/**
 * \brief This function returns a list of available devices.
 *
 * The function returns a list of available device radar devices. Each
 * list element is of type ifx_Device_List_entry_t.
 *
 * The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example how to use the function:
 * @code
 *      ifx_List_t* device_list = ifx_device_get_list();
 *      for (size_t i = 0; i < ifx_list_size(device_list); i++)
 *      {
 *          ifx_Device_List_Entry_t* entry = ifx_list_get(device_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(device_list);
 * @endcode
 *
 * \return The function returns a list of all found devices.
 *
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_device_get_list();

/**
 * \brief This function returns a list of available specified devices.
 *
 * The function is similar to \ref ifx_device_get_list. It returns a list of available
 * radar devices with a specified Shield type mounted. Each
 * list element is of type ifx_Device_List_entry_t. If the specified device is not
 * found, an empty list is returned.
 *
 * The list must be freed after use using \ref ifx_list_destroy.
 *
 * Here is an example how to use the function:
 * @code
 *      ifx_List_t* device_list = ifx_device_get_list_by_shieldtype(IFX_DEVICE_BGT60TR13AIP_SHIELD);
 *      for (size_t i = 0; i < ifx_list_size(device_list); i++)
 *      {
 *          ifx_Device_List_Entry_t* entry = ifx_list_get(device_list, i);
 *          // ...
 *      }
 *      ifx_list_destroy(device_list);
 * @endcode
 *
 * \return The function returns a list of all found devices.
 *
 */
IFX_DLL_PUBLIC
ifx_List_t* ifx_device_get_list_by_shield_type(ifx_Device_Shield_Type_t board_type);

/**
 * @brief Creates a dummy device handle.
 *
 * This function create a dummy device.
 *
 * @param [in]  device_type    specialization of the dummy device.
 *
 * @return Handle to the newly created dummy instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Device_Handle_t ifx_device_create_dummy(ifx_Device_Radar_Type_t device_type);

/**
 * @brief Creates a device handle.
 *
 * This function searches for a BGT60TRxx sensor device connected to the host machine and
 * connects to the first found sensor device.
 *
 * @return Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Device_Handle_t ifx_device_create(void);

/**
 * @brief Creates a device handle.
 *
 * This function opens the sensor device connected to the port given by port.
 *
 * @param [in]  port    name of port to open
 * @return Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Device_Handle_t ifx_device_create_by_port(const char* port);

/**
 * @brief Creates a device handle.
 **
 * This function searches for a BGT60TRxx sensor device connected to the host machine and
 * connects to the sensor device with a matching UUID.
 *
 * For converting the UUID from byte array to/from string see
 * \see ifx_uuid_from_string and \see ifx_uuid_to_string.
 *
 * @param [in]     uuid       uuid as string
 *
 * @return Handle to the newly created instance or NULL in case of failure.
 *
 */
IFX_DLL_PUBLIC
ifx_Device_Handle_t ifx_device_create_by_uuid(const char* uuid);

/**
* @brief Derives a device configuration from specified feature space metrics.
*
* This functions calculates FMCW frequency range, number of samples per chirp, number of chirps
* per frame and chirp-to-chirp time needed to achieve the specified feature space metrics. Number
* of samples per chirp and number of chirps per frame are rounded up to the next power of two,
* because this is a usual constraint for range and Doppler transform. The resulting maximum range
* and maximum speed may therefore be larger than specified.
*
* The calculated values are written to the members of the *device_config* struct, which must be
* provided by the caller.
*
* If handle is a valid pointer the delays for the connected radar chip are used. If handle is NULL,
* default values for the delays are used.
*
* @param [in]     handle     The device handle.
* @param [in]     metrics    The desired feature space metrics to be converted.
* @param [out]    config     The struct where the parameters calculated from the metrics are
*                            written to.
*/
IFX_DLL_PUBLIC
void ifx_device_translate_metrics_to_config(const ifx_Device_Handle_t handle,
                                            const ifx_Device_Metrics_t* metrics,
                                            ifx_Device_Config_t* config);

/**
 * @brief Get the limits for the Metrics parameters
 *
 * Retrieve the limits (lower and upper bounds) for the parameters in
 * ifx_Device_Metrics_t.
 *
 * The outputted limits depend on the input metrics. Different inputs for
 * metrics will lead to different bounds in limits.
 *
 * The limits depend on:
 *   - sample_rate_Hz depends on the connected the device device.
 *   - rx_mask depends on the connected radar device.
 *   - tx_mask depends on the connected radar device.
 *   - tx_power_level on the connected radar device.
 *   - if_gain_dB on the connected radar device.
 *   - range_resolution_m depends on the maximum bandwidth of the connected radar device.
 *   - max_range_m depends on range_resolution_m.
 *   - max_speed_m_s depends on max_range_m and range_resolution_m.
 *   - speed_resolution_m_s depends on max_speed_m_s (and implicitly on range_resolution_m and max_range_m).
 *   - frame_repetition_time_s depends on range_resolution_m, max_range_m, max_speed_m_s, speed_resolution_m_s.
 *
 * If all values in metrics are within the bounds computed in limits, the function returns true.
 * That means the function returns true if
 * \code
 *      limits->sample_rate_Hz.min <= metrics->sample_rate_Hz <= limits->sample_rate_Hz.max
 * \endcode
 * and similar for all other other struct members of metrics. Otherwise the function returns false.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @param [in]     metrics   Pointer to metrics structure.
 * @param [out]    limits    Pointer to limits structure.
 * @retval         true      if all members of metrics are within the corresponding min and max values of limits
 * @retval         false     otherwise
 */
IFX_DLL_PUBLIC
bool ifx_device_metrics_get_limits(const ifx_Device_Handle_t handle, const ifx_Device_Metrics_t* metrics, ifx_Device_Metrics_Limits_t* limits);

/**
 * @brief returns configuration parameters as a set of macros.
 *
 * The returned string is of the format \c '\#define BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP (128) ...' 
 * There is memory allocated by the function for this string. The caller is responsible to deallocate it using \ref ifx_mem_free
 *
 * @param [in]  handle              A handle to the radar device object.
 * @return      pointer to allocated string containing configuration parameters macros. this needs to be deallocated using \ref ifx_mem_free
 *
 */
IFX_DLL_PUBLIC
char* ifx_device_configuration_string(ifx_Device_Handle_t handle);

/**
 * @brief returns exported register list as hexadecimal string format.
 *
 * The returned string is of the format 'char register_list[] = { 0x3140210, 0x9e967fd, 0xb0805b4, ... 0x11e8271 };'
 * There is memory allocated by the function for this string. The caller is responsible to deallocate it using \ref ifx_mem_free
 *
 * @param [in]  handle              A handle to the radar device object.
 * @param [in]  set_trigger_bit     boolean for wrapped libAvian function.
 * @return      pointer to allocated string containing register values in hexadecimal string format. this needs to be deallocated using \ref ifx_mem_free
 *
 */
IFX_DLL_PUBLIC
char* ifx_device_register_list_string(ifx_Device_Handle_t handle, bool set_trigger_bit);

 /**
 * @brief Configures radar sensor device and starts acquisition of time domain data.
 * The board is configured according to the parameters provided through *config*
 * and acquisition of time domain data is started.
 *
 * If the function fails ifx_error_get() function will return one of the following error codes:
 *         - \ref IFX_ERROR_NO_DEVICE
 *         - \ref IFX_ERROR_DEVICE_BUSY
 *         - \ref IFX_ERROR_COMMUNICATION_ERROR
 *         - \ref IFX_ERROR_NUM_SAMPLES_OUT_OF_RANGE
 *         - \ref IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED
 *         - \ref IFX_ERROR_TX_ANTENNA_MODE_NOT_ALLOWED
 *         - \ref IFX_ERROR_IF_GAIN_OUT_OF_RANGE
 *         - \ref IFX_ERROR_SAMPLERATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_RF_OUT_OF_RANGE
 *         - \ref IFX_ERROR_TX_POWER_OUT_OF_RANGE
 *         - \ref IFX_ERROR_CHIRP_RATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_FRAME_RATE_OUT_OF_RANGE
 *         - \ref IFX_ERROR_NUM_CHIRPS_NOT_ALLOWED
 *         - \ref IFX_ERROR_FRAME_SIZE_NOT_SUPPORTED
 *
 * @param [in]     handle    A handle to the radar device object
 * @param [in]     config    This struct contains the parameters for data acquisition.
 *                           The device in configured according to these parameters.
 */
IFX_DLL_PUBLIC
void ifx_device_set_config(ifx_Device_Handle_t handle, const ifx_Device_Config_t* config);

/**
* @brief Reads the current configurations of the radar sensor device.
* If this function succeeds and time domain data is available, IFX_OK is returned and the
* configuration is copied to *config*. If readout fails IFX_ERROR_COMMUNICATION_ERROR will be
* returned.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    config     The current configuration of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_device_get_config(ifx_Device_Handle_t handle, ifx_Device_Config_t* config);

/**
* @brief Returns the default configuration of the radar sensor device (according to the device type).
* If this function succeeds, the configuration is copied to the *config* output paramter otherwise the
* IFX_ERROR will be set.
* returned.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    config     The default configuration of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_device_get_config_defaults(ifx_Device_Handle_t handle, ifx_Device_Config_t* config);

/**
* @brief Returns the upper and lower limits configuration of the radar sensor device (according to the device type).
* If this function succeeds, the configuration is copied to the *config_lower* and *config_upper* output paramters otherwise the
* IFX_ERROR will be set.
* returned.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    config_lower     The configuration of the lower limits for the device.
* @param [out]    config_upper     The configuration of the upper limits for the device.
*
*/

IFX_DLL_PUBLIC
void ifx_device_config_get_limits(const ifx_Device_Handle_t handle, ifx_Device_Config_t* config_lower, ifx_Device_Config_t* config_upper);
/**
* @brief Returns the default metrics of the radar sensor device (according to the device type).
* If this function succeeds, the metrics is copied to the *metrics* output paramter otherwise the
* IFX_ERROR will be set.
* returned.
*
* @param [in]     handle     A handle to the radar device object.
* @param [out]    metrics    The default metrics of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_device_get_metrics_defaults(ifx_Device_Handle_t handle, ifx_Device_Metrics_t* metrics);

/**
 * @brief Get device type of connected sensor
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return                  Device type.
 *
 */
IFX_DLL_PUBLIC
ifx_Device_Radar_Type_t ifx_device_get_device_type(const ifx_Device_Handle_t handle);

/**
* @brief Reads the temperature of the radar sensor device.
*
* On success, the current temperature of the radar sensor is written to the variable
* temperature_celsius. The temperature is in units of degrees Celsius.
* The 'temperature' field of the device handle is updated with the millidegree Celsius
* value.
*
* If an error occurs, temperature_celsius is not accessed and the error code can
* be retrieved using the function \ref ifx_error_get
*
* It is currently not possible to read the temperature from a UTR11. For a
* UTR11 this function will return IFX_ERROR_NOT_SUPPORTED.
*
* @param [in]     handle                  A handle to the radar device object.
* @param [out]    temperature_celsius     The current configuration of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_device_get_temperature(ifx_Device_Handle_t handle, ifx_Float_t* temperature_celsius);

/**
 * @brief Configures radar sensor device ADC settings.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @param [in]     config    This struct contains the ADC parameters to apply.
 *                           The ADC in configured according to these parameters.
 *
 */
IFX_DLL_PUBLIC
void ifx_device_configure_adc(ifx_Device_Handle_t handle,
                              const ifx_Device_ADC_Config_t* config);

/**
 * @brief Closes the connection to the radar sensor device.
 *
 * This function stops acquisition of time domain data, closes the connection to the device and
 * destroys the handle. The handle becomes invalid and must not be used any more after this
 * function was called.
 *
 * Please note that this function is not thread-safe and must not accessed
 * at the same time by multiple threads.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 */
IFX_DLL_PUBLIC
void ifx_device_destroy(ifx_Device_Handle_t handle);

/**
 * @brief Starts the acquisition of time domain data.
 *
 * This function starts the acquisition of time domain data.
 * If the data acquisition has already been started or the data acquisition cannot be started the function returns false.
 * The time domain data can be retrieved by calling the function \ref ifx_device_get_next_frame.
 *
 * This function is thread-safe.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @retval         true      if time domain data acquisition has started
 * @retval         false     otherwise
 */
IFX_DLL_PUBLIC
bool ifx_device_start_acquisition(ifx_Device_Handle_t handle);

/**
 * @brief Stops the acquisition of time domain data.
 *
 * This function stops the acquisition of time domain data.
 * If data acquisition has started through the \ref ifx_device_get_next_frame or the \ref ifx_device_start_acquisition
 * functions then the data acquisition stops; otherwise if the data acquisition is already stopped the function has no effect.
 *
 * This function is thread-safe.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @retval         true      if time domain data acquisition has stopped
 * @retval         false     otherwise
 */
IFX_DLL_PUBLIC
bool ifx_device_stop_acquisition(ifx_Device_Handle_t handle);

/**
 * @brief Creates a frame structure for time domain data acquisition using information
 * from a \see ifx_Device_Handle_t.
 *
 * This function checks the current configuration of the specified sensor device and initializes a
 * data structure (of type \ref ifx_Frame_R_t) that can hold a time domain data frame according
 * acquired through that device.
 * The device must be initialized before calling this function.
 *
 * If the configuration of the device is changed (e.g. by calling \ref ifx_device_set_config), the
 * frame handle is no longer valid. In that case you have to destroy the old frame using
 * \ref ifx_frame_destroy_r and create a new one with this function.
 *
 * Use \ref ifx_frame_destroy_r to destroy the frame handle and free the allocated memory.
 *
 * It is important to note that samples per chirp and chirps per frame has an impact on range spectrum and range doppler map
 * module. So, if these parameters are changed in new frame then user has to destroy and create new handles for
 * range spectrum and range doppler map modules.
 * 
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return Pointer to allocated frame \ref ifx_Frame_R_t or NULL if allocation failed.
 *
 */
IFX_DLL_PUBLIC
ifx_Frame_R_t* ifx_device_create_frame_from_device_handle(ifx_Device_Handle_t handle);

/**
 * @brief Retrieves the next frame of time domain data from a radar device.
 *
 * This function retrieves the next complete frame of time domain data from the
 * connected device. The samples from all chirps and all enabled RX antennas
 * (virtual RX antennas in case of MIMO) will be copied to the provided *frame*
 * data structure of type \ref ifx_Frame_R_t (link provides a description of
 * antenna order). It is assumed that *frame* was allocated beforehand with
 * \ref ifx_frame_create_r or \ref ifx_device_create_frame_from_device_handle.
 *
 * The function blocks until a full frame has been received or an error
 * occurred. Possible errors are:
 *  - \ref IFX_ERROR_COMMUNICATION_ERROR - communication error between board and
 *    host. This error typically occurs if the board was unplugged while fetching
 *    data.
 *  - \ref IFX_ERROR_FIFO_OVERFLOW - the radar board could not fetch the data
 *    fast enough from the radar sensor. The FIFO of the radar sensor overflowed
 *    and the state machine of the radar sensor was stopped. This typically
 *    occurs with radar configurations that cause high data rates, e.g. high
 *    frame rate, many activated receiving antennas, many samples per frame...
 *  - \ref IFX_ERROR_TIMEOUT - no full frame was available within 10 seconds.
 *  - \ref IFX_ERROR - an unknown error occured.
 *
 * This function is equivalent to a call to \ref
 * ifx_device_get_next_frame_timeout with a timeout of 10 seconds. For high
 * values of the frame repetition time (10 seconds or larger, corresponding to
 * a frame rate of 0.1Hz or lower) the function might return \ref
 * IFX_ERROR_TIMEOUT. In that case, please use \ref
 * ifx_device_get_next_frame_timeout instead.
 *
 * This function automatically starts the time domain data acquisition if not
 * previously specified. It is not required to call \ref
 * ifx_device_start_acquisition manually.
 *
 * The radar sensor sends the time-domain data in slices and not in full frames.
 * This means that after fetching a frame it does not necessarily take
 * frame_repetition_time_s seconds until this function returns the next frame.
 * For example, if both the frame and the frame repetition time are small (not
 * too many samples in a frame, high frame rate), one slice may contain several
 * frames. In this case two calls to \ref ifx_device_get_next_frame will return
 * a frame immediately.
 *
 * See also \ref ifx_device_start_acquisition and \ref
 * ifx_device_get_next_frame_timeout.
 *
 * @param [in]     handle    A handle to the radar device object.
 * @param [out]    frame     The frame structure where the time domain data should be copied to.
 *
 * @return \ref IFX_OK                           if the function suceeded.
 * @return \ref IFX_ERROR_COMMUNICATION_ERROR    if a communication error occured.
 * @return \ref IFX_ERROR_TIMEOUT                if a timeout occured.
 * @return \ref IFX_ERROR_FIFO_OVERFLOW          if a FIFO overflow occured.
 * @return \ref IFX_ERROR                        if an unknown error occured.
 */
IFX_DLL_PUBLIC
ifx_Error_t ifx_device_get_next_frame(ifx_Device_Handle_t handle,
                                      ifx_Frame_R_t* frame);

/**
 * @brief Retrieves the next frame of time domain data from a radar device (non-blocking).
 *
 * This function retrieves the next frame of time domain data from the
 * connected device. The function will either write the current frame into
 * *frame* as soon as a complete frame is available or return \ref
 * IFX_ERROR_TIMEOUT if no frame was avaiable within *timeout_ms* of
 * miliseconds. See \ref ifx_device_get_next_frame for more information.
 *
 * Here is a typical usage of this function:
 * @code
 *      const uint16_t timeout_100ms = 100;
 *      while(1)
 *      {
 *          ifx_Error_t ret = ifx_device_get_next_frame_timeout(device_handle, frame, timeout_100ms);
 *          if(ret == IFX_ERROR_TIMEOUT)
 *              continue; // no data available, do something else
 *          else if(ret != IFX_OK)
 *              // error handling
 *              break;
 *
 *          // process or copy data
 *
 *      }
 * @endcode
 *
 * See also \ref ifx_device_get_next_frame for a blocking version of this
 * function.
 *
 * @param [in]      handle              A handle to the radar device object.
 * @param [out]     frame               The frame structure where the time domain data should be copied to.
 * @param [in]      timeout_ms          Time in miliseconds after which 
 *
 * @return \ref IFX_OK                           if the function suceeded.
 * @return \ref IFX_ERROR_COMMUNICATION_ERROR    if a communication error occured.
 * @return \ref IFX_ERROR_TIMEOUT                if a timeout occured.
 * @return \ref IFX_ERROR_FIFO_OVERFLOW          if a FIFO overflow occured.
 * @return \ref IFX_ERROR                        if an unknown error occured.
 */
IFX_DLL_PUBLIC
ifx_Error_t ifx_device_get_next_frame_timeout(ifx_Device_Handle_t handle, ifx_Frame_R_t* frame, uint16_t timeout_ms);

/**
 * @brief Retrieves the number of RX antennas available on the connected radar device.
 *
 * This function returns the number of receiving antennas that are available on the
 * radar device specified by the given device handle.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return Number of available receiving antennas. If the input parameter is null
 *         the returned value would be 0
 */
IFX_DLL_PUBLIC
uint8_t ifx_device_get_num_rx_antennas(ifx_Device_Handle_t handle);

/**
 * @brief Retrieves the number of TX antennas available on the connected radar device.
 *
 * This function returns the number of transmitting antennas that are available on the
 * radar device specified by the given device handle.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return Number of available transmitting antennas. If the input parameter is null
 *         the returned value would be 0
 */
IFX_DLL_PUBLIC
uint8_t ifx_device_get_num_tx_antennas(ifx_Device_Handle_t handle);

/**
 * @brief Retrieves the unique id of the radar device (RF shield).
 *
 * This function returns the unique id of the connected RF shield.
 *
 * @param [in]     handle    A handle to the radar device object.
 *
 * @return True on success.
 * @return False otherwise.
 */
IFX_DLL_PUBLIC
const char* ifx_device_get_shield_uuid(ifx_Device_Handle_t handle);

/**
 * @brief Get information about the connected device.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return      pointer to \ref ifx_Device_Info_t structure
 *
 */
IFX_DLL_PUBLIC
const ifx_Device_Info_t* ifx_device_get_device_information(const ifx_Device_Handle_t handle);

/**
 * @brief Get information about the firmware version.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @return      pointer to \ref ifx_Firmware_Info_t structure
 *
 */
IFX_DLL_PUBLIC
const ifx_Firmware_Info_t* ifx_device_get_firmware_information(const ifx_Device_Handle_t handle);

/**
 * @brief Computes the chirp time.
 *
 * The chirp time is the total chirp time consisting of the sampling time and
 * the sampling delay.
 *
 * @param [in]  handle      A handle to the radar device object.
 * @param [in]  config      The current configuration of the device.
 * @return      chirp time
 *
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_device_get_chirp_time(const ifx_Device_Handle_t handle, const ifx_Device_Config_t* config);

/**
 * @brief Retrieves the current RF transmission power from the sensor device.
 *
 * The power is returned for the specified antenna in units of dBm.
 *
 * tx_antenna must be smaller than the number of total TX antennas, see also
 * \ref ifx_device_get_num_tx_antennas.
 *
 * On errors NAN is returned and the error code can be retrieved using \ref ifx_error_get.
 *
 * @param [in]     handle       A handle to the radar device object.
 * @param [in]     tx_antenna   TX antenna.
 * @return TX power for specified antenna in units of dBm.
 */
IFX_DLL_PUBLIC
ifx_Float_t ifx_device_get_tx_power(ifx_Device_Handle_t handle, uint8_t tx_antenna);

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_DEVICE_CONTROL_H */
