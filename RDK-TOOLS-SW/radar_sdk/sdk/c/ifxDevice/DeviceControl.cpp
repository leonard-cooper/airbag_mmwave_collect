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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <mutex>
#include <memory>
#include <string>
#include <vector>

#include "ifxBase/Defines.h"
#include "ifxBase/Error.h"
#include "ifxBase/Exception.hpp"
#include "ifxBase/internal/Util.h"
#include "ifxBase/List.h"
#include "ifxBase/Log.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Uuid.h"

#include "ifxDevice/internal/DeviceCalc.h"
#include "ifxDevice/internal/RadarDevice.hpp"
#include "ifxDevice/internal/DummyRadarDevice.hpp"
#include "ifxDevice/DeviceControl.h"
#include "ifxDevice/DeviceConfig.h"
#include "ifxDevice/Frame.h"
#include "ifxDevice/Shapes.h"

// libAvian
#include <ifxAvian_Driver.hpp>

// Strata
#include <common/Logger.hpp>
#include <platform/BoardManager.hpp>
#include <platform/serial/BoardSerial.hpp>
#include <platform/exception/EConnection.hpp>
#include <platform/exception/EAlreadyOpened.hpp>
#include <platform/exception/EProtocol.hpp>

// Strata port adapter
#include "StrataAvianPortAdapter.hpp"

// DeviceControlHelper functions
#include "DeviceControlHelper.hpp"

using namespace Infineon;

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

// Log level for Strata.
#ifdef NDEBUG
    // Release build: set log level to ERROR only
    #define STRATA_LOG_LEVEL (LOG_ERROR)
#else
    // Debug build: Set log level to error, warn, debug
    #define STRATA_LOG_LEVEL (LOG_ERROR | LOG_WARN | LOG_DEBUG)
#endif

constexpr bool use_serial = true;
constexpr bool use_ethernet = true;

// Amplification factor that is applied to the IF signal before sampling; Valid range: [18-60]dB
constexpr uint32_t IFX_IF_GAIN_DB_LOWER_LIMIT = 18;
constexpr uint32_t IFX_IF_GAIN_DB_UPPER_LIMIT = 60;

/* board_manager may be used by multiple threads concurrently. Each access to
 * board_manager must be protected using the mutex mutex_board_manager.
 */
static std::mutex mutex_board_manager;

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/**
 * @brief Defines the structure for device register list access object.
 *        When created, retrieves the register list and stores it in integer and string
 *        format.
 */
typedef struct {
    uint32_t num_registers;
    uint32_t* register_list;
} ifx_Device_Register_List_t;

class BoardSelector : public IEnumerationSelector
{
private:
    const uint8_t* m_uuid = nullptr;

public:
    BoardSelector(const uint8_t* uuid) : m_uuid(uuid) {}
    BoardSelector() : m_uuid(nullptr) {}

    virtual bool select(BoardDescriptor* descriptor) override
    {
        // check that this is a supported board
        const uint16_t pid = descriptor->getPid();
        if (ifx_Radar_Device_s::is_unknown_boardtype_from_pid(pid))
            return false;

        // m_uuid is not given (nullptr), we don't have to read and check the uuid
        if (!m_uuid)
            return true;

        auto instance = descriptor->createBoardInstance();
        ifx_Radar_Device_s device(std::move(instance));
        auto eeprom = device.get_eeprom_header();
        if (!eeprom)
        {
            return false; // cannot read NVM or content is invalid
        }
        // return true if both device uuid and wanted uuid match
        return (std::memcmp(eeprom->uuid, m_uuid, 16) == 0);
    }
};

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * @brief Exports device register list to device control.
 *
 * @param [in]  handle              A handle to the radar device object.
 * @param [in]  set_trigger_bit     boolean for wrapped libAvian function.
 * @return      pointer to initialized ifx_Device_Register_List_t handle. The caller is responsible to run \ref ifx_device_register_list_destroy within the current scope.
 *
 */
static ifx_Device_Register_List_t* register_list_create(ifx_Device_Handle_t handle, bool set_trigger_bit);

/**
 * @brief destroys ifx_Device_Register_List_t object.
 *
 * @param [in]  register_list_handle       handle to ifx_Device_Register_List_t object
 *
 */
static void register_list_destroy(ifx_Device_Register_List_t* register_list);


/**
 * @brief Translate register list in uint32_t array format to encapsulated string
 */
static char* register_list_to_string(uint32_t* register_list, uint32_t num_reg);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static ifx_Device_Register_List_t* register_list_create(ifx_Device_Handle_t handle, bool set_trigger_bit)
{
    IFX_ERR_BRV_NULL(handle, nullptr);

    ifx_Device_Register_List_t* device_register_list_handle = (ifx_Device_Register_List_t*)ifx_mem_alloc(sizeof(ifx_Device_Register_List_t));
    IFX_ERR_BRV_MEMALLOC(device_register_list_handle, nullptr);

    uint32_t num_reg = 0;

    try
    {
        num_reg = handle->export_register_list(set_trigger_bit, nullptr);
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }

    // allocate memory for register_list based on number of registers
    device_register_list_handle->register_list = (uint32_t*)ifx_mem_calloc(num_reg, sizeof(uint32_t));
    IFX_ERR_BRF_MEMALLOC(device_register_list_handle->register_list);

    //populate device_register_list_handle->register_list
    try
    {
        device_register_list_handle->num_registers = handle->export_register_list(set_trigger_bit, device_register_list_handle->register_list);
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    return device_register_list_handle;

fail:
    ifx_mem_free(device_register_list_handle);
    return nullptr;
}

//----------------------------------------------------------------------------

static void register_list_destroy(ifx_Device_Register_List_t* device_register_list_handle)
{
    if (!device_register_list_handle)
    {
        return;
    }
    ifx_mem_free(device_register_list_handle->register_list);
    ifx_mem_free(device_register_list_handle);
}

//----------------------------------------------------------------------------

static char* register_list_to_string(uint32_t* register_list, uint32_t num_reg)
{
    IFX_ERR_BRV_NULL(register_list, nullptr);

    std::stringstream string_cpp;

    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_REGS (" << num_reg << ")" << std::endl;

    string_cpp << std::endl;

    string_cpp << "#if defined(XENSIV_BGT60TRXX_CONF_IMPL)" << std::endl;

    string_cpp << "uint32_t register_list[] = { " << std::endl;
    for (uint32_t i = 0; i < (num_reg - 1); i++)
    {
        string_cpp << "    0x" << std::hex << register_list[i] << "UL, " << std::endl;
    }
    string_cpp << "    0x" << std::hex << register_list[num_reg - 1] << "UL" << std::endl << "};" << std::endl;

    string_cpp << "#endif" << std::endl;

    // size()+1 to include the string delimiter
    char* outstring_c = (char*)ifx_mem_calloc(string_cpp.str().size() + 1, sizeof(char));
    IFX_ERR_BRV_MEMALLOC(outstring_c, nullptr);

    std::memcpy(outstring_c, string_cpp.str().data(), string_cpp.str().size()+1);

    return(outstring_c);
}

//----------------------------------------------------------------------------

static ifx_Device_Handle_t open_dummy_device(ifx_Device_Radar_Type_t device_type)
{
  ifx_Device_Handle_t device = nullptr;

  try
  {
      device = new ifx_Dummy_Radar_Device_s(device_type);
  }
  catch (const rdk::exception::exception& e)
  {
      ifx_Error_t error_code = e.error_code();
      IFX_LOG_WARNING("Cannot open radar device: %s", ifx_error_to_string(error_code));
      ifx_error_set(error_code);
      delete device;
  }
  catch (const std::bad_alloc&)
  {
      IFX_LOG_WARNING("Cannot open radar device: Memory allocation failed");
      ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
      delete device;
  }

  return device;
}

//----------------------------------------------------------------------------

static ifx_Device_Handle_t open_device(BoardSelector* selector)
{
    // Set Strata log level. This is done before the first call to a Strata function
    LOG_LEVELS(STRATA_LOG_LEVEL);

    std::unique_lock<std::mutex> lock(mutex_board_manager);

    BoardManager board_manager(use_serial, use_ethernet);
    board_manager.setEnumerationSelector(selector);

    const uint16_t num_boards = board_manager.enumerate();
    if (num_boards == 0)
    {
        IFX_LOG_WARNING("Cannot find a radar device");
        ifx_error_set(IFX_ERROR_NO_DEVICE);
        return nullptr;
    }

    try
    {
        auto board = board_manager.createBoardInstance(uint8_t(0));
        return new ifx_Radar_Device_s(std::move(board));
    }
    catch (const rdk::exception::exception& e)
    {
        ifx_Error_t error_code = e.error_code();
        IFX_LOG_WARNING("Cannot open radar device: %s", ifx_error_to_string(error_code));
        ifx_error_set(error_code);
    }
    catch (const std::bad_alloc&)
    {
        IFX_LOG_WARNING("Cannot open radar device: Memory allocation failed");
        ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
    }
    catch (const EProtocol& e)
    {
        // might happen if the device not properly closed by a call to
        // ifx_device_destroy and the user tries to open again the device
        // within a couple of seconds
        IFX_IGNORE(e);
        IFX_LOG_WARNING("Cannot open radar device: %s (Strata EProtocol exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EConnection& e)
    {
        IFX_IGNORE(e);
        IFX_LOG_WARNING("Cannot open radar device: %s (Strata EConnection exception)", e.what());
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
    }
    catch (const EException& e)
    {
        // catch all other errors. On the RadarBaseboardMCU7 this exception
        // is for example thrown if the shield is not properly plugged in.
        IFX_IGNORE(e);
        IFX_LOG_WARNING("Cannot open radar device: %s (Strata EException exception)", e.what());
        ifx_error_set(IFX_ERROR);
    }

    return nullptr;
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_List_t* ifx_device_get_list_by_shield_type(ifx_Device_Shield_Type_t board_type)
{
    ifx_List_t* list = ifx_list_create(ifx_mem_free);
    if (list == nullptr)
        return nullptr;

    // Set Strata log level. This is done before the first call to a Strata function
    LOG_LEVELS(STRATA_LOG_LEVEL);

    BoardManager board_manager(use_serial, use_ethernet);
    board_manager.enumerate();

    for (const auto& board : board_manager.getEnumeratedList())
    {
        try
        {
            auto descriptor = board.get();
            auto instance = descriptor->createBoardInstance();
            ifx_Radar_Device_s device(std::move(instance));

            auto entry = static_cast<ifx_Device_List_Entry_t *>(ifx_mem_alloc(sizeof(ifx_Device_List_Entry_t)));
            if (entry == nullptr)
            {
                ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
                continue;
            }

            std::memset(entry, 0, sizeof(ifx_Device_List_Entry_t));

            auto eeprom = device.get_eeprom_header();
            if (!eeprom)
            {
                IFX_LOG_WARNING("Cannot read EEPROM on device");
                continue;
            }

            ifx_uuid_to_string(eeprom->uuid, entry->shield_uuid);

            entry->board_type = ifx_Device_Shield_Type_t(eeprom->board_type);

            if ((board_type == IFX_SHIELD_ANY) || (board_type == entry->board_type))
            {
                // if ret is false, the memory allocation failed and we have to
                // free the memory for entry ourselves. ifx_list_push_back will
                // set an error using ifx_error_set
                bool ret = ifx_list_push_back(list, entry);
                if (!ret)
                    ifx_mem_free(entry);
            }
            else
            {
                ifx_mem_free(entry);
            }
        }
        catch (const EConnection&)
        {
            continue;
        }
        catch(const EException&)
        {
            continue;
        }
    }

    return list;
}

//----------------------------------------------------------------------------

ifx_List_t* ifx_device_get_list()
{
    return ifx_device_get_list_by_shield_type(IFX_SHIELD_ANY);
}

//----------------------------------------------------------------------------

ifx_Device_Handle_t ifx_device_create_by_port(const char* port)
{
    // if port is NULL call ifx_device_create. This ensures the previous behavior.
    if (port == nullptr)
        return ifx_device_create();

    std::unique_lock<std::mutex> lock(mutex_board_manager);

    std::unique_ptr<BoardInstance> board;
    try
    {
        board = BoardSerial::createBoardInstance(port);
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_NO_DEVICE);
        return nullptr;
    }
    catch (const EException&)
    {
        ifx_error_set(IFX_ERROR_NO_DEVICE);
        return nullptr;
    }

    try
    {
        return new ifx_Radar_Device_s(std::move(board));
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_NO_DEVICE);
    }
    catch (const EException&)
    {
        ifx_error_set(IFX_ERROR_NO_DEVICE);
    }
    catch (const rdk::exception::exception& e)
    {
        ifx_error_set(e.error_code());
    }
    catch (const std::bad_alloc& err)
    {
        (void)err;
        ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
    }
    catch (...)
    {
        ifx_error_set(IFX_ERROR);
    }

    return nullptr;
}

//----------------------------------------------------------------------------

ifx_Device_Handle_t ifx_device_create_dummy(ifx_Device_Radar_Type_t device_type)
{
    return open_dummy_device(device_type);
}

ifx_Device_Handle_t ifx_device_create(void)
{
    BoardSelector selector;
    return open_device(&selector);
}

//----------------------------------------------------------------------------

ifx_Device_Handle_t ifx_device_create_by_uuid(const char* uuid)
{
    IFX_ERR_BRV_NULL(uuid, nullptr);

    uint8_t uuid_bytes[16];
    if (!ifx_uuid_from_string(uuid, uuid_bytes))
    {
        ifx_error_set(IFX_ERROR_ARGUMENT_INVALID);
        return nullptr;
    }
    BoardSelector selector(uuid_bytes);
    return open_device(&selector);
}

//----------------------------------------------------------------------------

void ifx_device_translate_metrics_to_config(const ifx_Device_Handle_t handle, const ifx_Device_Metrics_t* metrics, ifx_Device_Config_t* config)
{
    IFX_ERR_BRK_NULL(metrics);
    IFX_ERR_BRK_NULL(config);

    double lower_bound, upper_bound;

    /* input parameters */
    const uint32_t adc_sample_rate = metrics->sample_rate_Hz;
    const double range_resolution = metrics->range_resolution_m;
    const double max_range = metrics->max_range_m;
    const double speed_resolution = metrics->speed_resolution_m_s;
    const double max_speed = metrics->max_speed_m_s;
    const double frame_repetition_time = metrics->frame_repetition_time_s;

    // make sure that max_speed >= speed_resolution
    IFX_ERR_BRK_BIGGER_INCL(speed_resolution, max_speed);

    /* if necessary we create out own instance of device calc */
    ifx_Device_Calc_t* calc_internal = nullptr;
    const ifx_Device_Calc_t* calc = nullptr;
    if (handle)
        calc = handle->get_device_calc();
    else
    {
        calc_internal = ifx_device_calc_create(nullptr);
        IFX_ERR_BRK_MEMALLOC(calc_internal);

        ifx_device_calc_set_power_of_twos(calc_internal, true);
        ifx_device_calc_set_speed_measurement(calc_internal, true);
        calc = calc_internal;
    }

    /* compute parameters for ifx_Device_Config_t structure */
    const uint32_t num_samples_per_chirp = ifx_device_calc_num_samples_per_chirp(calc, max_range, range_resolution);
    const double sampling_time = ifx_device_calc_sampling_time(calc, num_samples_per_chirp, adc_sample_rate);
    const uint32_t number_of_chirps_per_frame = ifx_device_calc_num_chirps_per_frame(calc, max_speed, speed_resolution);

    const double sampling_bandwidth = ifx_device_calc_sampling_bandwidth(calc, range_resolution);
    const double chirp_time = ifx_device_calc_chirp_time(calc, sampling_time);
    const double total_bandwidth = ifx_device_calc_total_bandwidth(calc, sampling_time, chirp_time, sampling_bandwidth);

    const double center_frequency = (metrics->center_frequency_Hz > 0)
        ? metrics->center_frequency_Hz
        : ifx_device_calc_center_frequency(calc, total_bandwidth);

    double min_frequency, max_frequency;
    ifx_device_calc_chirp_frequency_bounds(calc, total_bandwidth, center_frequency, &min_frequency, &max_frequency);

    const double wavelength = ifx_device_calc_wavelength(calc, center_frequency);
    const double chirp_repetition_time = ifx_device_calc_chirp_repetition_time(calc, max_speed, wavelength);

    /* bound checking*/
    ifx_device_calc_range_resolution_bounds(calc, &lower_bound, &upper_bound);
    if (range_resolution < lower_bound || range_resolution > upper_bound)
        goto arguments_out_of_bounds;

    ifx_device_calc_max_range_bounds(calc, range_resolution, &lower_bound, &upper_bound);
    if (max_range < lower_bound || max_range > upper_bound)
        goto arguments_out_of_bounds;

    uint32_t lower_bound_uint32, upper_bound_uint32;
    ifx_device_calc_sample_rate_bounds(calc, &lower_bound_uint32, &upper_bound_uint32);
    if (adc_sample_rate < lower_bound_uint32 || adc_sample_rate > upper_bound_uint32)
        goto arguments_out_of_bounds;

    ifx_device_calc_max_speed_bounds(calc, wavelength, sampling_time, &lower_bound, &upper_bound);
    if (max_speed < lower_bound || max_speed > upper_bound)
        goto arguments_out_of_bounds;

    // destroy calc object if we have created it
    if(calc_internal)
        ifx_device_calc_destroy(calc_internal);

    config->rx_mask = metrics->rx_mask;
    config->tx_mask = metrics->tx_mask;
    config->tx_power_level = metrics->tx_power_level;
    config->if_gain_dB = metrics->if_gain_dB;
    config->mimo_mode = IFX_MIMO_OFF;

    /* write parameters to output structure */
    config->num_samples_per_chirp = num_samples_per_chirp;
    config->num_chirps_per_frame = number_of_chirps_per_frame;
    config->sample_rate_Hz = adc_sample_rate;
    config->frame_repetition_time_s = ifx_Float_t(frame_repetition_time);
    config->lower_frequency_Hz = uint64_t(std::round(min_frequency));
    config->upper_frequency_Hz = uint64_t(std::round(max_frequency));
    config->chirp_repetition_time_s = ifx_Float_t(chirp_repetition_time);

    // done: success
    return;

arguments_out_of_bounds:
    // destroy calc object if we have created it
    if (calc_internal)
        ifx_device_calc_destroy(calc_internal);

    ifx_error_set(IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);
}

//----------------------------------------------------------------------------

char* ifx_device_configuration_string(ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);

    ifx_Device_Config_t config;
    handle->get_config(&config);

    std::stringstream string_cpp;

    const uint32_t num_samples_per_fifo_word = 2;
    const uint32_t num_rx_antennas = ifx_util_popcount(config.rx_mask);
    const uint32_t num_tx_antennas = ifx_util_popcount(config.tx_mask);
    const uint32_t frame_size = config.num_samples_per_chirp * config.num_chirps_per_frame * num_rx_antennas;

    string_cpp << "#define XENSIV_BGT60TRXX_CONF_LOWER_FREQ_HZ (" << config.lower_frequency_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_UPPER_FREQ_HZ (" << config.upper_frequency_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP (" << config.num_samples_per_chirp << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_CHIRPS_PER_FRAME (" << config.num_chirps_per_frame << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_RX_ANTENNAS (" << num_rx_antennas << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_NUM_TX_ANTENNAS (" << num_tx_antennas << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_SAMPLE_RATE (" << config.sample_rate_Hz << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_CHIRP_REPETION_TIME_S (" << config.chirp_repetition_time_s << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_FRAME_REPETION_TIME_S (" << config.frame_repetition_time_s << ")" << std::endl;
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_FRAME_XFER_SIZE_WORDS (" << (frame_size / num_samples_per_fifo_word) << ")" << std::endl;    
    string_cpp << "#define XENSIV_BGT60TRXX_CONF_BUFFER_SIZE_BYTES ((XENSIV_BGT60TRXX_CONF_FRAME_XFER_SIZE_WORDS * XENSIV_BGT60TRXX_FIFO_WORD_SIZE_BYTES) + XENSIV_BGT60TRXX_SPI_BURST_HEADER_SIZE_BYTES)" << std::endl;    

    // size()+1 to include the string delimiter
    char* outstring_c = (char*)ifx_mem_calloc(string_cpp.str().size() + 1, sizeof(char));
    IFX_ERR_BRV_MEMALLOC(outstring_c, nullptr);

    std::memcpy(outstring_c, string_cpp.str().data(), string_cpp.str().size()+1);

    return(outstring_c);
}

//----------------------------------------------------------------------------

char* ifx_device_register_list_string(ifx_Device_Handle_t handle, bool set_trigger_bit)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    ifx_Device_Register_List_t* device_register_list = register_list_create(handle, set_trigger_bit);
    IFX_ERR_BRV_NULL(device_register_list, nullptr);

    char* register_list_string = register_list_to_string(device_register_list->register_list, device_register_list->num_registers); // allocates memory
    IFX_ERR_BRV_NULL(register_list_string, nullptr);
    register_list_destroy(device_register_list);

    return register_list_string;
}

//----------------------------------------------------------------------------

bool ifx_device_metrics_get_limits(const ifx_Device_Handle_t handle, const ifx_Device_Metrics_t* metrics, ifx_Device_Metrics_Limits_t* limits)
{
    double lower_bound, upper_bound;

    IFX_ERR_BRV_NULL(handle, false);
    IFX_ERR_BRV_NULL(metrics, false);
    IFX_ERR_BRV_NULL(limits, false);

    /* initialize limits with zeros. In case we forget to set a field, min and
     * max will be 0 which makes potential bugs easier to find. */
    std::memset(limits, 0, sizeof(ifx_Device_Metrics_Limits_t));

    const ifx_Device_Calc_t* calc = handle->get_device_calc();
    const ifx_Device_Info_t* device_info = handle->get_device_info();

    /* rx mask is within [1, 2**num_rx_antennas-1] */
    limits->rx_mask.min = 1;
    limits->rx_mask.max = (1 << device_info->num_rx_antennas) - 1;

    /* tx mask is within [1, 2**num_tx_antennas-1] */
    limits->tx_mask.min = 0;
    limits->tx_mask.max = (1 << device_info->num_tx_antennas) - 1;

    /* tx_power_level is within [1, device_info->max_tx_power] */
    limits->tx_power_level.min = 1;
    limits->tx_power_level.max = device_info->max_tx_power;

    limits->if_gain_dB.min = IFX_IF_GAIN_DB_LOWER_LIMIT;
    limits->if_gain_dB.max = IFX_IF_GAIN_DB_UPPER_LIMIT;

    /* get bounds for ADC sample rate. If sample rate in metrics structure is
     * outside the allowed range, set it to the closest allowed value.
     */
    uint32_t sample_rate = metrics->sample_rate_Hz;
    {
        uint32_t sample_rate_min, sample_rate_max;
        ifx_device_calc_sample_rate_bounds(calc, &sample_rate_min, &sample_rate_max);
        limits->sample_rate_Hz.min = sample_rate_min;
        limits->sample_rate_Hz.max = sample_rate_max;

        if (sample_rate < sample_rate_min)
            sample_rate = sample_rate_min;
        else if (sample_rate > sample_rate_max)
            sample_rate = sample_rate_max;
    }

    /* To avoid floating point problems due to rounding, we output a bit
     * more conservative limits and make the lower bounds a bit higher
     * and the upper bounds a bit lower than actually allowed. */
    const ifx_Float_t epsp1 = (ifx_Float_t)1.0001;

    // lambda clamp
    auto clamp = [](ifx_Float_t value, ifx_Float_t min, ifx_Float_t max) -> ifx_Float_t
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    };

    /* Compute bounds for range_resolution, max_range, max_speed, speed_resolution.
     *
     * 1. range_resolution: The bounds only on the maximum bandwidth of the
     *    device.
     * 2. max_range: Depends on range_resolution.
     * 3. max_speed: Depends on max_range and range_resolution.
     * 4. speed_resolution: Depends on max_speed (and implicitly on
     *    range_resolution and max_range).
     * 5. frame_repetition_time: Depends on range_resolution, max_range,
     *    max_speed, speed_resolution.
     */

    ifx_device_calc_range_resolution_bounds(calc, &lower_bound, &upper_bound);
    limits->range_resolution_m.min = (ifx_Float_t)lower_bound*epsp1;
    limits->range_resolution_m.max = (ifx_Float_t)upper_bound/epsp1;
    const double range_resolution = clamp(metrics->range_resolution_m, limits->range_resolution_m.min, limits->range_resolution_m.max);

    ifx_device_calc_max_range_bounds(calc, range_resolution, &lower_bound, &upper_bound);
    limits->max_range_m.min = (ifx_Float_t)lower_bound*epsp1;
    limits->max_range_m.max = (ifx_Float_t)upper_bound/epsp1;
    const double max_range = clamp(metrics->max_range_m, limits->max_range_m.min, limits->max_range_m.max);

    const uint32_t num_samples_per_chirp = ifx_device_calc_num_samples_per_chirp(calc, max_range, range_resolution);
    const double sampling_time = ifx_device_calc_sampling_time(calc, num_samples_per_chirp, sample_rate);

    const double samplingBandwidth = ifx_device_calc_sampling_bandwidth(calc, range_resolution);

    const double chirp_time = ifx_device_calc_chirp_time(calc, sampling_time);
    const double totalBandwidth = ifx_device_calc_total_bandwidth(calc, sampling_time, chirp_time, samplingBandwidth);
    const double center_frequency = ifx_device_calc_center_frequency(calc, totalBandwidth);
    const double wavelength = ifx_device_calc_wavelength(calc, center_frequency);

    ifx_device_calc_max_speed_bounds(calc, wavelength, sampling_time, &lower_bound, &upper_bound);
    limits->max_speed_m_s.min = (ifx_Float_t)lower_bound*epsp1;
    limits->max_speed_m_s.max = (ifx_Float_t)upper_bound/epsp1;
    const double max_speed = clamp(metrics->max_speed_m_s, limits->max_speed_m_s.min, limits->max_speed_m_s.max);

    ifx_device_calc_speed_resolution_bounds(calc, max_speed, &lower_bound, &upper_bound);
    limits->speed_resolution_m_s.min = (ifx_Float_t)lower_bound*epsp1;
    limits->speed_resolution_m_s.max = (ifx_Float_t)upper_bound/epsp1;
    const double speed_resolution = clamp(metrics->speed_resolution_m_s, limits->speed_resolution_m_s.min, limits->speed_resolution_m_s.max);

    const double chirp_repetition_time = ifx_device_calc_chirp_repetition_time(calc, max_speed, wavelength);
    uint32_t num_chirps_per_frame = ifx_device_calc_num_chirps_per_frame(calc, max_speed, speed_resolution);
    ifx_device_calc_frame_repetition_time_bounds(calc, sampling_time, chirp_repetition_time, num_chirps_per_frame, &lower_bound, &upper_bound);
    limits->frame_repetition_time_s.min = (ifx_Float_t)lower_bound*epsp1;
    limits->frame_repetition_time_s.max = (ifx_Float_t)upper_bound/epsp1;

    /* check if input metrics are valid (within bounds of limits) */
    if (metrics->sample_rate_Hz < limits->sample_rate_Hz.min || metrics->sample_rate_Hz > limits->sample_rate_Hz.max)
        return false;
    if (metrics->rx_mask < limits->rx_mask.min || metrics->rx_mask > limits->rx_mask.max)
        return false;
    if (metrics->tx_mask < limits->tx_mask.min || metrics->tx_mask > limits->tx_mask.max)
        return false;
    if (metrics->tx_power_level < limits->tx_power_level.min || metrics->tx_power_level > limits->tx_power_level.max)
        return false;
    if (metrics->if_gain_dB < limits->if_gain_dB.min || metrics->if_gain_dB > limits->if_gain_dB.max)
        return false;

    if (metrics->range_resolution_m < limits->range_resolution_m.min || metrics->range_resolution_m > limits->range_resolution_m.max)
        return false;
    if (metrics->max_range_m < limits->max_range_m.min || metrics->max_range_m > limits->max_range_m.max)
        return false;
    if (metrics->max_speed_m_s < limits->max_speed_m_s.min || metrics->max_speed_m_s > limits->max_speed_m_s.max)
        return false;
    if (metrics->speed_resolution_m_s < limits->speed_resolution_m_s.min || metrics->speed_resolution_m_s > limits->speed_resolution_m_s.max)
        return false;
    if (metrics->frame_repetition_time_s < limits->frame_repetition_time_s.min || metrics->frame_repetition_time_s > limits->frame_repetition_time_s.max)
        return false;

    return true;
}

//----------------------------------------------------------------------------

void ifx_device_config_get_limits(const ifx_Device_Handle_t handle, ifx_Device_Config_t* config_lower, ifx_Device_Config_t* config_upper)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config_lower);
    IFX_ERR_BRK_NULL(config_upper);

    auto device_info = handle->get_device_info();

	config_lower->sample_rate_Hz = 100000; // 100kHz, 80kHz could be also, because of default ADC configs (38.6MHz clock sets 80kHz) 
    config_lower->tx_mask = 0; // no tx antenna 
	config_lower->tx_power_level = TX_POWER_LEVEL_LOWER;
	config_lower->if_gain_dB = IF_GAIN_DB_LOWER;
	config_lower->lower_frequency_Hz = 57'000'000u; // 57GHz
	config_lower->upper_frequency_Hz = 57'000'000u; // 57GHz
	config_lower->num_samples_per_chirp = 2;  //Strata has an issue to not pack odd 12-bit samples to even number of bytes, CW supports the same lower limit
	config_lower->num_chirps_per_frame = 1;
	config_lower->chirp_repetition_time_s = 0.00001f; // 10µs
	config_lower->frame_repetition_time_s = 0.02f; // frame rate of 50Hz
	config_lower->mimo_mode = IFX_MIMO_OFF;

	config_upper->sample_rate_Hz = 2500000; // 2.5MHz
	config_upper->rx_mask = (1 << device_info->num_rx_antennas) - 1; // all rx antennas activated
    config_upper->tx_mask = 1 << (device_info->num_tx_antennas - 1); // all tx antennas activated
	config_upper->tx_power_level = TX_POWER_LEVEL_UPPER;
	config_upper->if_gain_dB = IF_GAIN_DB_UPPER;
	config_upper->lower_frequency_Hz = 64'500'000'000u; // 64.5GHz
	config_upper->upper_frequency_Hz = 64'500'000'000u; // 64.5GHz
	config_upper->num_samples_per_chirp = 1024;
	config_upper->num_chirps_per_frame = 512;
	config_upper->chirp_repetition_time_s = 0.01f; // 10ms
	config_upper->frame_repetition_time_s = 10; // frame rate of 0.1Hz
	config_upper->mimo_mode = device_info->num_tx_antennas > 1 ? IFX_MIMO_TDM : IFX_MIMO_OFF; // IFX_MIMO_TDM if device has more than one RX antenna
}


//----------------------------------------------------------------------------

ifx_Device_Radar_Type_t ifx_device_get_device_type(const ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, IFX_DEVICE_UNKNOWN);
    return handle->get_device_type();
}

//----------------------------------------------------------------------------

Infineon::Avian::Driver* ifx_device_get_avian_driver(const ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_avian_driver();
}

//----------------------------------------------------------------------------
BoardInstance* ifx_device_get_strata_avian_board(const ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_strata_avian_board();
}

//----------------------------------------------------------------------------

Infineon::Communication::StrataAvianPortAdapter* ifx_device_get_strata_avian_port(const ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_strata_avian_port();
}

Infineon::Avian::Constant_Wave_Controller* ifx_device_get_constant_wave_controller(const ifx_Device_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_constant_wave_controller();
}

Infineon::Avian::Constant_Wave_Controller* ifx_Radar_Device_s::get_constant_wave_controller()
{
    if (!m_cw_controller)
    {
        auto avian_port = get_strata_avian_port();
        m_cw_controller = std::make_unique<Avian::Constant_Wave_Controller>(*avian_port);
    }

    return m_cw_controller.get();
}
