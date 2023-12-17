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

#include "ifxBase/Exception.hpp"

#include "ifxDevice/internal/RadarDevice.hpp"
#include "ifxDevice/internal/RadarDeviceErrorTranslator.hpp"
#include "ifxDevice/internal/RadarDeviceUtils.hpp"

#include <cstring>
#include "ifxBase/Uuid.h"
#include "ifxBase/internal/Util.h"
#include <platform/exception/EConnection.hpp>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

namespace
{
    template<typename T>
    bool check_if_equal_and_set(std::atomic<T>& value,
                                T value_to_check, const T& new_value)
    {
        return value.compare_exchange_strong(value_to_check, new_value);
    }
}

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * @brief Compute frame size from number of chirps, number of samples per chirp, and number of virtual antennas
 */
static uint32_t compute_frame_size(uint32_t num_chirps_per_frame, uint32_t num_samples_per_chirp, uint32_t num_antennas);

//----------------------------------------------------------------------------

static std::vector<uint16_t> packRaw12(const uint8_t* input, size_t num_samples)
{
    std::vector<uint16_t> output;
    output.reserve(num_samples);

    for (size_t i = 0; i < num_samples / 2; i++)
    {
        output.push_back((input[3 * i + 0] << 4) | (input[3 * i + 1] >> 4));
        output.push_back(((input[3 * i + 1] & 0x0f) << 8) | input[3 * i + 2]);
    }

    return output;
}


//----------------------------------------------------------------------------

/// Change byte order of uint16_t
static uint16_t byte_swap16(uint16_t x)
{
    return ((x >> 8) & 0xFF) | ((x << 8) & 0xFF00);
}

//----------------------------------------------------------------------------

/**
 * @brief Check if host is little endian
 *
 * @retval true    if host is little endian
 * @retval false   if host is big endian
 */
static bool is_little_endian(void)
{
    /*
     * Little endian           Big endian
     * +----+----+----+----+   +----+----+----+----+
     * |0x01|0x00|0x00|0x00|   |0x00|0x00|0x00|0x01|
     * +----+----+----+----+   +----+----+----+----+
     *  ^^^^                    ^^^^
     *   &x                      &x
     */
    const uint32_t x = 1;
    const char is_le = ((char*)&x)[0];
    return is_le;
}

//----------------------------------------------------------------------------

static uint32_t compute_frame_size(uint32_t num_chirps_per_frame, uint32_t num_samples_per_chirp, uint32_t num_antennas)
{
    return num_chirps_per_frame * num_samples_per_chirp * num_antennas;
}

//----------------------------------------------------------------------------

bool ifx_Radar_Device_s::is_unknown_boardtype_from_pid(const uint16_t pid)
{
    return (RadarDeviceUtils::get_boardtype_from_pid(pid) == BoardType::Unknown);
}

//----------------------------------------------------------------------------

ifx_Radar_Device_s::ifx_Radar_Device_s(std::unique_ptr<BoardInstance> board)
    : m_board(std::move(board))
{
    // detect board type
    BoardType boardtype = RadarDeviceUtils::get_boardtype_from_pid(m_board->getPid());
    if(boardtype == BoardType::Unknown || m_board->getVid() != vid_infineon)
    {
        // this is an internal error because it must not be possible that this
        // constructor is called for a board which is neither Aurix nor MCU7.
        throw rdk::exception::internal();
    }

    /* read firmware information only after having checked the firmware version */
    {
        const auto version = m_board->getIBridge()->getIBridgeControl()->getVersionInfo();

        if (! RadarDeviceUtils::is_firmware_version_supported(boardtype, version[0], version[1], version[2]))
        {
            throw rdk::exception::firmware_version_not_supported();
        }

        // The memory for description must not be freed.
        m_firmware_info.description = m_board->getName();
        m_firmware_info.version_major = version[0];
        m_firmware_info.version_minor = version[1];
        m_firmware_info.version_build = version[2];

        m_firmware_info.extendedVersion = m_board->getIBridge()->getIBridgeControl()->getExtendedVersionString().c_str();
    }

    m_avian_port = std::make_unique<Communication::StrataAvianPortAdapter>(m_board.get());

    m_driver = Avian::Driver::create_driver(*m_avian_port);
    if (!m_driver)
    {
        throw rdk::exception::device_not_supported();
    }

    initialize();

    m_eeprom_header_valid = read_eeprom_header();
    if(m_eeprom_header_valid)
    {
        // convert uuid from EEPROM
        char str[64];
        ifx_uuid_to_string(m_eeprom_header.uuid, str);
        m_shield_uuid = std::string(str);

        // the shield type
        {
            m_shield_type = m_eeprom_header.board_type;
            // also update the device info with the just read board type
            m_device_info.shield_type = ifx_Device_Shield_Type_t(m_shield_type);
        }
    }

    //m_driver->stop_and_reset_sequence();
    stop_and_reset();
}

//----------------------------------------------------------------------------

ifx_Radar_Device_s::~ifx_Radar_Device_s()
{
    stop_acquisition();
};

//----------------------------------------------------------------------------

bool ifx_Radar_Device_s::start_acquisition()
{
    const auto aquisition_stopped_previously = ::check_if_equal_and_set(m_acquisition_state, Acquisition_State_t::Stopped, Acquisition_State_t::Starting);

    if(! aquisition_stopped_previously)
    {
        return false;
    }

    try
    {
        // The driver knows the current slice size
        uint16_t slice_size;
        m_driver->get_slice_size(&slice_size);

        // clear the buffer
        m_fifo.clear();

        auto data_ready_callback = [this, slice_size](Avian::HW::Spi_Response_t /*status_word*/) {
            if (m_acquisition_state != Acquisition_State_t::Started)
                return;

            // handle packed raw data
            const uint8_t* data = reinterpret_cast<uint8_t*>(m_avian_buffer.data());
            std::vector<uint16_t> v = packRaw12(data, slice_size);
            this->m_fifo.push(v);
        };

        auto error_callback = [this](Communication::StrataAvianPortAdapter* /*port_adapter*/, uint32_t error_code) {
            if (error_code == Communication::ERR_FIFO_OVERFLOW)
                m_acquisition_state = Acquisition_State_t::FifoOverflow;
            else
                m_acquisition_state = Acquisition_State_t::Error;
        };

        // The data buffer is partitioned according to the new block size.
        m_avian_buffer.resize(static_cast<size_t>(slice_size) * 2);

        auto avian_port = get_strata_avian_port();

        // Finally buffering is enabled by passing the first data block to the Avian port.
        avian_port->set_buffer(m_avian_buffer.data());

        // Register error callback which is called on FIFO overflows or communication errors
        avian_port->register_error_callback(error_callback);

        // The reader is setup with the new slice size
        avian_port->start_reader(m_driver->get_burst_prefix(), slice_size, data_ready_callback);

        // Data reading is active now, but the Avian device must be triggered, too.
        m_driver->get_device_configuration().send_to_device(*avian_port, true);
        m_driver->notify_trigger();

        m_acquisition_state = Acquisition_State_t::Started;

        return true;
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        m_acquisition_state = Acquisition_State_t::Error;
    }
    catch (const EException&)
    {
        ifx_error_set(IFX_ERROR);
        m_acquisition_state = Acquisition_State_t::Error;
    }

    return false;
}

//----------------------------------------------------------------------------

bool ifx_Radar_Device_s::stop_acquisition()
{
    bool stopped = false;

    if(m_acquisition_state != Acquisition_State_t::Started)
    {
        return stopped;
    }

    const auto aquisition_started_previously = ::check_if_equal_and_set(m_acquisition_state, Acquisition_State_t::Started, Acquisition_State_t::Stopping);

    if(! aquisition_started_previously)
    {
        return stopped;
    }

    try
    {
        auto avian_port = get_strata_avian_port();
        avian_port->stop_reader();
        stopped = stop_and_reset();
    }
    catch (const EConnection&)
    {
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        m_acquisition_state = Acquisition_State_t::Error;
        stopped = false;
    }
    catch (const EException&)
    {
        ifx_error_set(IFX_ERROR);
        m_acquisition_state = Acquisition_State_t::Error;
        stopped = false;
    }

    return stopped;
}

//----------------------------------------------------------------------------

ifx_Error_t ifx_Radar_Device_s::send_to_device()
{
    Avian::HW::RegisterSet register_set = m_driver->get_device_configuration();

    try
    {
        register_set.send_to_device(*m_avian_port, false);

        return IFX_OK;
    }
    catch (...)
    {
        return IFX_ERROR_COMMUNICATION_ERROR;
    }
}

//----------------------------------------------------------------------------

bool ifx_Radar_Device_s::read_eeprom_header()
{
    constexpr uint16_t address_ee24cw128x = 0x50;  // Address of EEPROM on RF Shield
    constexpr uint8_t magic[8] = { 'I', 'f', 'x', 'B', 'o', 'a', 'r', 'd' };

    auto i2c = m_board->getIBridge()->getIBridgeControl()->getII2c();
    const auto busId = m_avian_port->getComponentId(); // this corresponds by definition of the board FW

    std::memset(&m_eeprom_header, 0, sizeof(magic));
    uint8_t* data = reinterpret_cast<uint8_t*>(&m_eeprom_header);

    try
    {
        i2c->readWith16BitPrefix(I2C_ADDR(busId, address_ee24cw128x), 0, sizeof(eeprom_header_t), data);
    }
    catch (...)
    {
        return false;
    }


    // check if magic string is correct
    if (std::memcmp(m_eeprom_header.magic, magic, sizeof(magic)) != 0)
        return false;

    // swap endianess if we are not on a little endian machine
    if (!is_little_endian())
    {
        m_eeprom_header.board_type = byte_swap16(m_eeprom_header.board_type);
        m_eeprom_header.nvm_type = byte_swap16(m_eeprom_header.nvm_type);
        m_eeprom_header.reserved = byte_swap16(m_eeprom_header.reserved);
    }

    return true;
}

//----------------------------------------------------------------------------

void ifx_Radar_Device_s::get_temperature(ifx_Float_t* temperature_celsius)
{
    IFX_ERR_BRK_NULL(temperature_celsius);

    // Reading temperature from a BGT60UTR11 is not supported:
    // With BGT60UTR11 you can measure the temperature when the chip is off,
    // in CW mode, or when the chip is operating with a triangular shape.
    // During saw tooth shapes temperature measurement is not supported.
    // To avoid any problems we set the error IFX_ERROR_NOT_SUPPORTED
    // when reading the temperature with a BGT60UTR11.
    if (m_driver->get_device_type() == Avian::Device_Type::BGT60UTR11)
    {
        ifx_error_set(IFX_ERROR_NOT_SUPPORTED);
        return;
    }

    // Do not read the temperature from the radar sensor too often as it
    // decreases performance (negative impact on data rate), might cause
    // problems as fetching temperature takes some time, and it is not
    // required (the temperature hardly changes within 100ms).
    //
    // If the cached temperature value is not yet expired, we simply use the
    // cached value instead of reading a new value from the radar sensor.
    auto now = std::chrono::steady_clock::now();
    if (now < m_temperature_expiration_time)
    {
        *temperature_celsius = m_temperature_value;
        return;
    }

    // temperature in units of 0.001 degree Celsius
    int32_t temp = 0;
    Avian::Driver::Error rc = m_driver->get_temperature(&temp);
    if (rc == Avian::Driver::Error::OK)
    {
        *temperature_celsius = temp * ifx_Float_t(0.001);

        // Save the current temperature value and get a new temperature value
        // from the radar sensor earliest in 100ms again.
        m_temperature_value = *temperature_celsius;
        m_temperature_expiration_time = now + std::chrono::milliseconds(100);
    }
    else
        ifx_error_set(RadarDeviceErrorTranslator::translate_error_code(rc));
}

//----------------------------------------------------------------------------

BoardType ifx_Radar_Device_s::get_board_type_from_pid() const{
    return RadarDeviceUtils::get_boardtype_from_pid(m_board->getPid());
}

const eeprom_header_t *ifx_Radar_Device_s::get_eeprom_header() const
{
    if (m_eeprom_header_valid)
    {
        return &m_eeprom_header;
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------

ifx_Error_t ifx_Radar_Device_s::get_next_frame(ifx_Frame_R_t* frame, uint16_t timeout_ms)
{
    if (frame == nullptr)
        return IFX_ERROR_ARGUMENT_NULL;
    if (frame->num_rx != ifx_devconf_count_rx_antennas(&m_config))
        return IFX_ERROR_ARGUMENT_INVALID;


    Acquisition_State_t state = m_acquisition_state.load();
    switch (state)
    {
    // If an error occurred (typically a communication error), try to stop and
    // start data acqusition.
    case Acquisition_State_t::Error:
        return IFX_ERROR_COMMUNICATION_ERROR;

    // On FIFO overflow stop and start data acqusition.
    case Acquisition_State_t::FifoOverflow:
        stop_acquisition();
        start_acquisition();
        return IFX_ERROR_FIFO_OVERFLOW;

    case Acquisition_State_t::Stopped:
        start_acquisition();
        break;

    // avoid warning from gcc
    default:
        break;
    }

    const uint32_t num_chirps_per_frame = IFX_MAT_ROWS(frame->rx_data[0]);
    const uint32_t num_samples_per_chirp = IFX_MAT_COLS(frame->rx_data[0]);
    const size_t frame_size = compute_frame_size(num_chirps_per_frame, num_samples_per_chirp, frame->num_rx);

    std::vector<uint16_t> raw_data;

    /* Check every 100ms (timeout_pop_ms) if a FIFO overflow or another
     * error occurred. If an error occured return an error code and
     * return from this function.
     */
    do
    {
        constexpr uint16_t timeout_pop_ms = 100;
        const uint16_t cur_timeout = std::min(timeout_pop_ms, timeout_ms);
        bool success = m_fifo.pop(raw_data, frame_size, cur_timeout);
        timeout_ms -= cur_timeout;

        switch (m_acquisition_state.load())
        {
        case Acquisition_State_t::FifoOverflow:
            return IFX_ERROR_FIFO_OVERFLOW;
        case Acquisition_State_t::Error:
            return IFX_ERROR_COMMUNICATION_ERROR;

        // avoid warning from gcc
        default:
            break;
        }

        if (success)
            break;
    } while (timeout_ms > 0);

    // if we still have no data return timeout
    if (raw_data.empty())
        return IFX_ERROR_TIMEOUT;

    // number of virtual RX antennas
    const uint8_t num_antennas = frame->num_rx;

    // number of physical RX antennas used
    const size_t num_rx = ifx_util_popcount(m_config.rx_mask);

    // number of physical TX antennas used (2 if MIMO, otherwise 1)
    const size_t num_tx = m_config.mimo_mode == IFX_MIMO_TDM ? 2 : 1;

    // maximum ADC value: 2**12-1
    constexpr ifx_Float_t adc_max = 4095;

    // Copy the data into the frame structure
    for (size_t tx = 0; tx < num_tx; tx++)
    {
        for (size_t rx = 0; rx < num_rx; rx++)
        {
            const size_t antenna = tx * num_rx + rx;
            ifx_Matrix_R_t* m = frame->rx_data[antenna];
            const size_t tx_offset = tx * num_rx * num_samples_per_chirp;

            for (size_t chirp = 0; chirp < num_chirps_per_frame; chirp++)
            {
                const size_t chirp_offset = chirp * num_antennas * num_samples_per_chirp;

                for (size_t sample = 0; sample < num_samples_per_chirp; sample++)
                {
                    // sample0_RX1 sample0_RX2, sample1_RX1, sample2_RX2, ...
                    //const size_t index = sample + chirp * num_samples_per_chirp + antenna * num_chirps_per_frame * num_samples_per_chirp;
                    //const size_t index = antenna + sample * num_antennas + chirp * num_antennas * num_samples_per_chirp;
                    const size_t sample_offset = sample * num_rx;
                    const size_t index = rx + sample_offset + chirp_offset + tx_offset;
                    IFX_MAT_AT(m, chirp, sample) = raw_data.at(index) / adc_max;
                }
            }
        }
    }

    return IFX_OK;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_Radar_Device_s::get_tx_power(uint8_t tx_antenna)
{
    if (m_config.mimo_mode == IFX_MIMO_TDM)
    {
        /* In case of MIMO the two antennas are not active at the same time. In
         * shape 0 TX1 is activated and in shape 1 TX2 is activated. In order to
         * avoid measuring the TX power of a disabled TX antenna, we first have
         * to select the correct shape.
         */
        uint8_t shape;
        bool down_chirp;
        m_driver->get_selected_shape(&shape, &down_chirp);
        if (tx_antenna == 0)
            m_driver->select_shape_to_configure(0, down_chirp);
        else
            m_driver->select_shape_to_configure(1, down_chirp);
    }

    int32_t tx_power_001dBm = 0;
    auto rc = m_driver->get_tx_power(tx_antenna, &tx_power_001dBm);

    if (rc != Avian::Driver::Error::OK)
    {
        ifx_error_set(RadarDeviceErrorTranslator::translate_error_code(rc));
        return NAN;
    }

    return tx_power_001dBm * static_cast<ifx_Float_t>(0.001);
}
