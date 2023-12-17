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

#include "ifxDevice/internal/DeviceControlAvian.hpp"
#include "ifxDevice/internal/DeviceCalc.h"


#define CHECK_AVIAN_ERR_CODE(err) do { if((err) != Avian::Driver::Error::OK) return err; } while(0)
#define CHECK_AVIAN_ERR_CODE_GOTO_FAIL(err) do { if((err) != Avian::Driver::Error::OK) goto fail; } while(0)


//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::read_device_info(ifx_Device_Info_t* device_info)
{
    Avian::Device_Info avian_device_info = {};
    auto rc = m_driver->get_device_info(&avian_device_info);
    CHECK_AVIAN_ERR_CODE(rc);

    device_info->description = avian_device_info.description;
    device_info->min_rf_frequency_Hz = 1000 * static_cast<uint64_t>(avian_device_info.min_rf_frequency_kHz);
    device_info->max_rf_frequency_Hz = 1000 * static_cast<uint64_t>(avian_device_info.max_rf_frequency_kHz);
    device_info->num_tx_antennas = avian_device_info.num_tx_antennas;
    device_info->num_rx_antennas = avian_device_info.num_rx_antennas;
    device_info->max_tx_power = avian_device_info.max_tx_power;
    device_info->num_temp_sensors = avian_device_info.num_temp_sensors;
    device_info->interleaved_rx = avian_device_info.interleaved_rx;

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::read_chirp_timing()
{
    auto rc = m_driver->get_chirp_timing(&m_chirp_timing);
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::read_startup_timing()
{
    auto rc = m_driver->get_startup_timing(&m_startup_timing);
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::read_chirp_end_delay()
{
    auto rc = m_driver->get_chirp_end_delay(&m_chirp_end_delay_100ps);
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::read_config()
{
    Avian::Driver::Error rc;

    // select shape 0 (up chirp)
    rc = m_driver->select_shape_to_configure(0, false);
    CHECK_AVIAN_ERR_CODE(rc);

    {
        Avian::Adc_Configuration adc_configuration = {};
        rc = m_driver->get_adc_configuration(&adc_configuration);
        CHECK_AVIAN_ERR_CODE(rc);

        m_config.sample_rate_Hz = adc_configuration.samplerate_Hz;
    }

    {
        Avian::Frame_Format frame_format = {};

        rc = m_driver->get_frame_format(&frame_format);
        CHECK_AVIAN_ERR_CODE(rc);

        m_config.num_samples_per_chirp = frame_format.num_samples_per_chirp;
        m_config.rx_mask = frame_format.rx_mask;
    }

    {
        Avian::Frame_Definition frame_definition = {};
        rc = m_driver->get_frame_definition(&frame_definition);
        CHECK_AVIAN_ERR_CODE(rc);

        m_config.num_chirps_per_frame = frame_definition.shape_set.num_repetitions;
    }

    {
        Avian::Tx_Mode tx_mode;
        rc = m_driver->get_tx_mode(&tx_mode);
        CHECK_AVIAN_ERR_CODE(rc);

        switch (tx_mode)
        {
        case Avian::Tx_Mode::Tx1_Only:
            m_config.tx_mask = 1;
            break;
        case Avian::Tx_Mode::Tx2_Only:
            m_config.tx_mask = 2;
            break;
        case Avian::Tx_Mode::Alternating:
            m_config.mimo_mode = IFX_MIMO_TDM;
            m_config.tx_mask = 3;
            break;
        case Avian::Tx_Mode::Off:
        default:
            m_config.tx_mask = 0;
            break;
        }
    }

    {
        Avian::Fmcw_Configuration fmcw_configuration = {};
        rc = m_driver->get_fmcw_configuration(&fmcw_configuration);
        CHECK_AVIAN_ERR_CODE(rc);

        m_config.upper_frequency_Hz = 1000 * static_cast<uint64_t>(fmcw_configuration.upper_frequency_kHz);
        m_config.lower_frequency_Hz = 1000 * static_cast<uint64_t>(fmcw_configuration.lower_frequency_kHz);
        m_config.tx_power_level = fmcw_configuration.tx_power;
    }

    {
        Avian::Baseband_Configuration baseband_configuration = {};
        rc = m_driver->get_baseband_configuration(&baseband_configuration);
        CHECK_AVIAN_ERR_CODE(rc);

        Avian::Hp_Gain hp_gain = baseband_configuration.hp_gain_1;
        uint32_t hpg = hp_gain == Avian::Hp_Gain::_30dB ? 30 : 18;
        Avian::Vga_Gain vga_gain = baseband_configuration.vga_gain_1;
        m_config.if_gain_dB = 5 * static_cast<uint32_t>(vga_gain) + hpg;
    }

    rc = read_chirp_timing();
    CHECK_AVIAN_ERR_CODE(rc);

    rc = read_startup_timing();
    CHECK_AVIAN_ERR_CODE(rc);

    rc = read_chirp_end_delay();
    CHECK_AVIAN_ERR_CODE(rc);

    {
        Avian::Frame_Definition frame_definition = {};
        rc = m_driver->get_frame_definition(&frame_definition);
        CHECK_AVIAN_ERR_CODE(rc);

        // bool indicating if MIMO is activated
        enum ifx_Device_MIMO_Mode mimo_mode = m_config.mimo_mode;

        const Avian::Shape_Group* last_shape = (mimo_mode == IFX_MIMO_TDM)
            ? &frame_definition.shapes[1]  // mimo mode
            : &frame_definition.shapes[0]; // no mimo mode

        // read shape end delay and frame end delay
        double frame_end_delay = frame_definition.shape_set.post_delay_100ps * 100e-12;
        double shape_end_delay = last_shape->post_delay_100ps * 100e-12;

        // read shape end power mode and frame end power mode
        ifx_device_power_mode_t frame_end_power_mode = static_cast<ifx_device_power_mode_t>(frame_definition.shape_set.following_power_mode);
        ifx_device_power_mode_t shape_end_power_mode = static_cast<ifx_device_power_mode_t>(last_shape->following_power_mode);

        // compute frame time (aka frame period, the inverse of the frame rate)
        double sampling_time = ifx_device_calc_sampling_time(m_calc, m_config.num_samples_per_chirp, m_config.sample_rate_Hz);
        double pulse_repetition_time = ifx_device_calc_pulse_repetition_time_from_shape_end_delay(m_calc, mimo_mode == IFX_MIMO_TDM, shape_end_delay, sampling_time, shape_end_power_mode);
        double frame_repetition_time = ifx_device_calc_frame_repetition_time_from_frame_end_delay(m_calc, frame_end_delay, sampling_time, pulse_repetition_time, m_config.num_chirps_per_frame, frame_end_power_mode);

        // convert frame_time from seconds to microseconds
        m_config.frame_repetition_time_s = static_cast<ifx_Float_t>(frame_repetition_time);
        m_config.chirp_repetition_time_s = static_cast<ifx_Float_t>(pulse_repetition_time);
    }

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::set_frame_format(uint32_t num_samples_per_chirp, uint32_t num_chirps_per_frame, uint8_t rx_mask)
{
    /* set frame format, to be in a defined state */
    /* ------------------------------------------ */
    Avian::Frame_Format frame_format = {};
    frame_format.num_samples_per_chirp = num_samples_per_chirp;
    frame_format.num_chirps_per_frame = num_chirps_per_frame;
    frame_format.rx_mask = rx_mask;

    auto rc = m_driver->set_frame_format(&frame_format);
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::configure_chirp(uint8_t shape_num, uint8_t downChirp, ifx_Device_Chirp_Config_t* chirp)
{
    Avian::Driver::Error rc;

    constexpr uint32_t num_chirps_per_frame = 1;

    rc = select_shape(shape_num, downChirp);
    CHECK_AVIAN_ERR_CODE(rc);

    rc = set_frame_format(chirp->chirp_setting.num_samples, num_chirps_per_frame, chirp->antenna_setup.rx_mask);
    CHECK_AVIAN_ERR_CODE(rc);

    rc = m_driver->set_chirp_end_delay(chirp->chirp_setting.end_delay_100ps);
    CHECK_AVIAN_ERR_CODE(rc);

    rc = m_driver->set_tx_mode(static_cast<Avian::Tx_Mode>(chirp->antenna_setup.tx_mode));
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::configure_shape(uint8_t shape_num, ifx_Device_Shape_t* shape)
{
    Avian::Driver::Error rc;

    bool up = shape->config.direction != DIR_DOWNCHIRP_ONLY;
    bool down = shape->config.direction != DIR_UPCHIRP_ONLY;

    rc = set_shape_fmcw_config(shape_num, down, shape->config.direction, shape->config.lower_frequency_kHz, shape->config.upper_frequency_kHz, shape->config.tx_power);
    CHECK_AVIAN_ERR_CODE(rc);

    if (up)
    {
        rc = configure_chirp(shape_num, 0, &shape->config.up);
        CHECK_AVIAN_ERR_CODE(rc);
    }
    if (down)
    {
        rc = configure_chirp(shape_num, 1, &shape->config.down);
        CHECK_AVIAN_ERR_CODE(rc);
    }

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::select_shape(uint8_t shape_num, bool down_chirp)
{
    return m_driver->select_shape_to_configure(shape_num, down_chirp);
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::set_shape_fmcw_config(uint8_t shape_num, uint8_t down_chirp, ifx_chirp_direction_t direction, uint32_t lower_frequency_kHz, uint32_t upper_frequency_kHz, uint8_t tx_power)
{
    Avian::Driver::Error rc;

    rc = select_shape(shape_num, down_chirp);
    CHECK_AVIAN_ERR_CODE(rc);

    Avian::Fmcw_Configuration fmcw_configuration = {};
    fmcw_configuration.lower_frequency_kHz = lower_frequency_kHz;
    fmcw_configuration.upper_frequency_kHz = upper_frequency_kHz;
    fmcw_configuration.shape_type = static_cast<Avian::Shape_Type>(direction);
    fmcw_configuration.tx_power = tx_power;

    rc = m_driver->set_fmcw_configuration(&fmcw_configuration);
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}

//----------------------------------------------------------------------------

Avian::Driver::Error DeviceControlAvian::fetch_timings()
{
    Avian::Driver::Error rc;

    rc = read_chirp_timing();
    CHECK_AVIAN_ERR_CODE(rc);

    rc = read_startup_timing();
    CHECK_AVIAN_ERR_CODE(rc);

    rc = read_chirp_end_delay();
    CHECK_AVIAN_ERR_CODE(rc);

    return Avian::Driver::Error::OK;
}
