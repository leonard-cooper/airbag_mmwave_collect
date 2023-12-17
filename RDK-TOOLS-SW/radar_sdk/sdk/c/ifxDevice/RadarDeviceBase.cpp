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

#include <cmath> // for std::round
#include <cstring> // for std::memset

#include "ifxDevice/internal/RadarDeviceBase.hpp"
#include "ifxDevice/internal/RadarDeviceErrorTranslator.hpp"

// libAvian
#include "ifxAvian_DeviceTraits.hpp"
#include "ifxAvian_Utilities.hpp"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

#define CHECK_AVIAN_ERR_CODE(err) do { if((err) != Avian::Driver::Error::OK) return err; } while(0)
#define CHECK_AVIAN_ERR_CODE_GOTO_FAIL(err) do { if((err) != Avian::Driver::Error::OK) goto fail; } while(0)

// Amplification factor that is applied to the IF signal before sampling; Valid range: [18-60]dB
constexpr uint32_t IFX_IF_GAIN_DB_LOWER_LIMIT = 18;
constexpr uint32_t IFX_IF_GAIN_DB_UPPER_LIMIT = 60;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

namespace
{
    const ifx_Device_Metrics_t BGT24LTR24_Metrics =
    {
        1000000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT60TR12E_Metrics =
    {
        1000000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT60TR13C_Metrics =
    {
        1000000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT60UTR11_Metrics =
    {
        1000000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT60ATR24C_Metrics =
    {
        1000000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT60UTR13D_Metrics =
    {
        1000000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t BGT120UTR13E_Metrics =
    {
        1000000, /* sample_rate_Hz */
        7, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    const ifx_Device_Metrics_t UNSUPPORTED_DEVICE_Metrics =
    {
        1000000, /* sample_rate_Hz */
        1, /* rx_mask */
        1, /* tx_mask */
        31, /* tx_power_level */
        33, /* if_gain_dB */
        0.3, /* range_resolution_m */
        9.6, /* max_range_m */
        2.45, /* max_speed_m_s */
        0.0765, /* speed_resolution_m_s */
        0.1, /* frame_repetition_time_s */
        60500000000.f /* center_frequency_Hz*/
    };

    ifx_Device_Metrics_t get_default_metrics(const Avian::Device_Type device_type)
    {
        switch (device_type)
        {
        case Avian::Device_Type::BGT24LTR24:
            return BGT24LTR24_Metrics;
        case Avian::Device_Type::BGT60TR12E:
            return BGT60TR12E_Metrics;
        case Avian::Device_Type::BGT60TR13C:
            return BGT60TR13C_Metrics;
        case Avian::Device_Type::BGT60UTR11:
            return BGT60UTR11_Metrics;
        case Avian::Device_Type::BGT60ATR24C:
            return BGT60ATR24C_Metrics;
        case Avian::Device_Type::BGT60UTR13D:
            return BGT60UTR13D_Metrics;
        case Avian::Device_Type::BGT120UTR13E:
            return BGT120UTR13E_Metrics;
        default:
            return UNSUPPORTED_DEVICE_Metrics;
        }
    }

    /// Round a double value to an unsigned 64bit value
    uint64_t round_to_uint64(double value)
    {
        return uint64_t(std::round(value));
    }

    //----------------------------------------------------------------------------

    /// Round a double value to an unsigned 32bit value
    uint32_t round_to_uint32(double value)
    {
        return uint32_t(std::round(value));
    }

    /// Compute slice size from number of samples per frame, the frame repetition
    /// time (inverse of frame rate), and the FIFO size
    uint16_t calculate_slice_size(uint32_t num_samples_per_frame, float frame_repetition_time_s, uint32_t fifo_size)
    {
        // Take half the FIFO size as a hard cap to have enough buffer to
        // prevent FIFO overflows in the Avian sensor.
        // The Avian sensor will trigger an interrupt if there are at least
        // max_slice_size samples in the internal FIFO. The board (e.g., the
        // Hatvan board) will then read the FIFO. As reacting to the interrupt
        // takes a bit of time and in the mean time more samples are put in the
        // FIFO, max_slice_size must be smaller than the fifo size.
        const uint32_t max_slice_size = fifo_size / 2;

        // Frame rate in Hz. There are frame_rate of frames per second.
        const float frame_rate = 1 / frame_repetition_time_s;

        // Compute the slice rate from the slice_size. The slice rate is the
        // number of slices that are generated per second for a given slice
        // size.
        auto compute_slice_rate = [num_samples_per_frame, frame_rate](uint32_t slice_size) -> float {
            return (num_samples_per_frame * frame_rate) / slice_size;
        };

        // Compute slice rate from the slice rate. With the returned slice size
        // slice_rate of slices will be generated per second.
        // Note that this value might be larger than max_slice_size or fifo_size.
        auto compute_slice_size = [num_samples_per_frame, frame_rate](float slice_rate) -> uint32_t {
            return uint32_t(num_samples_per_frame * frame_rate / slice_rate);
        };

        // Sending a lot of slices from the board to the host is not efficient.
        // If possible we try to avoid slice rates higher than 20 Hz.
        constexpr float slice_rate_threshold = 20; // 20 slices per second

        if (num_samples_per_frame <= max_slice_size)
        {
            // A complete frame fits into a single slice.

            if (compute_slice_rate(num_samples_per_frame) <= slice_rate_threshold)
            {
                // The complete frame fits into a single slice and the
                // corresponding slice rate is smaller than 20 slices per
                // second.
                // One slice corresponds to exactly one frame.
                return num_samples_per_frame;
            }
            else
            {
                // Even though the full frame fits into a single slice, the
                // resulting slice rate would be too high.
                // To avoid sending many small slices, we send k frames as
                // one slice to achieve a slice rate of about 20Hz.
                auto k = uint32_t(std::ceil(compute_slice_size(slice_rate_threshold) / num_samples_per_frame));

                return std::min(k * num_samples_per_frame, max_slice_size);
            }
        }
        else
        {
            // A full frame does not fit into a slice.

            // We need at least k slices to fit a frame.
            const auto k = uint32_t(std::ceil(float(num_samples_per_frame) / max_slice_size));

            if (compute_slice_rate(num_samples_per_frame / k) <= slice_rate_threshold)
            {
                // If the slice rate is sufficiently low, we go for
                // num_samples_per_frame/k. This has the advantage that k slices
                // are one frame (at least approximately, due to rounding it
                // might not be exactly true). That means advantage that full
                // frames arrive approximately equidistant in time at the host.
                return num_samples_per_frame / k;
            }
            else
            {
                // Slice rate would be too high. We use the maximum possible slice
                // size to reduce the slice rate as much as possible. The main
                // objective here is to use a big slice size to optimize the throughput
                // and make it less likely that we cannot transfer all samples
                // from the sensor to the host in time.
                return max_slice_size;
            }
        }
    }
} // end of anonymous namespace

void RadarDeviceBase::initialize()
{
    Avian::Driver::Error rc;

    /* fetch the timings */
    rc = fetch_timings();
    if (rc != Avian::Driver::Error::OK)
        throw RadarDeviceErrorTranslator::translate_error_code(rc);

    /* read the device information (among others number of RX and TX antennas) */
    rc = read_device_info(&m_device_info);
    if (rc != Avian::Driver::Error::OK)
        throw RadarDeviceErrorTranslator::translate_error_code(rc);

    ifx_Device_Calc_Delays_t delays = {};
    delays.pre_chirp_delay = m_chirp_timing.pre_chirp_delay_100ps * 100e-12;
    delays.post_chirp_delay = m_chirp_timing.post_chirp_delay_100ps * 100e-12;
    delays.pa_delay = m_chirp_timing.pa_delay_100ps * 100e-12;
    delays.adc_delay = m_chirp_timing.adc_delay_100ps * 100e-12;
    delays.wake_up_time = m_startup_timing.wake_up_time_100ps * 100e-12;
    delays.pll_settle_time_coarse = m_startup_timing.pll_settle_time_coarse_100ps * 100e-12;
    delays.pll_settle_time_fine = m_startup_timing.pll_settle_time_fine_100ps * 100e-12;
    delays.chirp_end_delay = m_chirp_end_delay_100ps * 100e-12;

    m_calc = ifx_device_calc_create(&delays);
    if (!m_calc)
        throw ifx_error_get();

    // read configuration from device
    rc = read_config();
    if (rc != Avian::Driver::Error::OK)
        throw RadarDeviceErrorTranslator::translate_error_code(rc);
}

uint16_t RadarDeviceBase::get_slice_size() const
{
    uint16_t slice_size;
    m_driver->get_slice_size(&slice_size);

    return slice_size;
}

const char* RadarDeviceBase::get_shield_uuid() const
{
	return m_shield_uuid.c_str();
}

uint16_t RadarDeviceBase::get_shield_type() const{
    return m_shield_type;
}

//----------------------------------------------------------------------------

const ifx_Device_Info_t* RadarDeviceBase::get_device_info() const
{
    return &m_device_info;
}

//----------------------------------------------------------------------------

const ifx_Firmware_Info_t* RadarDeviceBase::get_firmware_info() const
{
    return &m_firmware_info;
}

//----------------------------------------------------------------------------

uint32_t RadarDeviceBase::export_register_list(bool set_trigger_bit, uint32_t* register_list)
{
    auto registers = m_driver->get_device_configuration().get_configuration_sequence(set_trigger_bit);

    if (register_list != nullptr)
    {
        std::copy(registers.begin(), registers.end(), register_list);
    }
    return (uint32_t)registers.size();
}

//----------------------------------------------------------------------------

ifx_Device_Radar_Type_t RadarDeviceBase::get_device_type() const
{
    return ifx_Device_Radar_Type_t(m_driver->get_device_type());
}

//----------------------------------------------------------------------------

void RadarDeviceBase::configure_adc(const ifx_Device_ADC_Config_t* config)
{
    IFX_ERR_BRK_NULL(config);

    Avian::Adc_Configuration adc_configuration = {};
    //oversampling is turned off by default as it is not officially supported
    adc_configuration.oversampling = Avian::Adc_Oversampling::Off;
    adc_configuration.sample_time = Avian::Adc_Sample_Time(config->sample_time);
    adc_configuration.tracking = Avian::Adc_Tracking(config->tracking);
    adc_configuration.double_msb_time = config->double_msb_time;
    adc_configuration.samplerate_Hz = config->samplerate_Hz;

    auto rc = m_driver->set_adc_configuration(&adc_configuration);
    RadarDeviceErrorTranslator::translate_error_code(rc);
}
//----------------------------------------------------------------------------

void RadarDeviceBase::configure_shapes(ifx_Device_Shape_Set_t* shape_set)
{
    IFX_ERR_BRK_NULL(shape_set);

    Avian::Frame_Definition frame_definition =
    {
        {
            { shape_set->shape[0].num_repetition, static_cast<Avian::Power_Mode>(shape_set->shape[0].following_power_mode), shape_set->shape[0].end_delay_100ps },
            { shape_set->shape[1].num_repetition, static_cast<Avian::Power_Mode>(shape_set->shape[1].following_power_mode), shape_set->shape[1].end_delay_100ps },
            { shape_set->shape[2].num_repetition, static_cast<Avian::Power_Mode>(shape_set->shape[2].following_power_mode), shape_set->shape[2].end_delay_100ps },
            { shape_set->shape[3].num_repetition, static_cast<Avian::Power_Mode>(shape_set->shape[3].following_power_mode), shape_set->shape[3].end_delay_100ps }
        },
        {
            shape_set->num_repetition, static_cast<Avian::Power_Mode>(shape_set->following_power_mode), shape_set->end_delay_100ps
        },
        0
    };

    Avian::Driver::Error rc;

    rc = m_driver->set_frame_definition(&frame_definition);
    CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

    for (uint8_t i = 0; i < 4; i++)
    {
        if (shape_set->shape[i].num_repetition == 0)
            break;

        rc = configure_shape(i, &shape_set->shape[i]);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
    }

    return;

fail:
    ifx_error_set(RadarDeviceErrorTranslator::translate_error_code(rc));
}


//----------------------------------------------------------------------------

void RadarDeviceBase::get_config(ifx_Device_Config_t* config)
{
    IFX_ERR_BRK_NULL(config);

    auto rc = read_config();
    if (rc != Avian::Driver::Error::OK)
    {
        ifx_error_set(RadarDeviceErrorTranslator::translate_error_code(rc));
        return;
    }

    *config = m_config;
}

//----------------------------------------------------------------------------

void RadarDeviceBase::get_default_metrics(ifx_Device_Metrics_t* metrics)
{
    IFX_ERR_BRK_NULL(metrics);

    *metrics = ::get_default_metrics(m_driver->get_device_type());
}

//----------------------------------------------------------------------------

void RadarDeviceBase::set_config(const ifx_Device_Config_t* config)
{
    IFX_ERR_BRK_NULL(config);

    if (m_acquisition_state == Acquisition_State_t::Error)
    {
        ifx_error_set(IFX_ERROR_COMMUNICATION_ERROR);
        return;
    }

    Avian::Driver::Error rc;

    /*
    * As a first step of initialization the device configuration provided by the user is copied into
    * the handle. Some of the parameters are needed during fetching of time domain data.
    */
    m_config = *config;
    m_acquisition_state = Acquisition_State_t::Stopped;

    { /* initialization and non-shape related settings */
        /*
        * After connection it is unclear if the device is in easy mode or in normal mode. To be on the
        * safe side, switch to normal mode. This has the nice side effect that this causes a device
        * reset and stops operation. The according firmware function is known to not fail, but of
        * course communication errors could happen, so the error code must not be omitted.
        */
        rc = m_driver->enable_easy_mode(false);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

        rc = m_driver->set_adc_samplerate(config->sample_rate_Hz);
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

        Avian::Baseband_Configuration baseband_config = {};
        ifx_Error_t err = initialize_baseband_config(&baseband_config);
        if (err != IFX_OK)
        {
            ifx_error_set(err);
            return;
        }

        for (uint8_t shape_num = 0; shape_num < 4; shape_num++)
        {
            rc = select_shape(shape_num, false);
            CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);

            rc = m_driver->set_baseband_configuration(&baseband_config);
            CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
        }
    }

    {
        /* read the device FIFO size in samples
         * The units used by Device_Traits correspond to pairs of samples,
         * therefore the number of samples is obtained by multiplying by two.
         */
        auto device_type = m_driver->get_device_type();
        const auto& device_traits = Avian::Device_Traits::get(device_type);
        uint32_t adc_fifo_size = static_cast<uint32_t>(device_traits.fifo_size) * 2;

        ifx_Device_Shape_Set_t shape_set = {};
        ifx_Error_t error = translate_config_to_shape_set(&shape_set);
        if (error != IFX_OK)
        {
            ifx_error_set(error);
            return;
        }
        IFX_ERR_HANDLE_R(configure_shapes(&shape_set), (void)0);

        uint8_t num_antennas = ifx_devconf_count_rx_antennas(config);
        const uint32_t frame_size = config->num_chirps_per_frame * config->num_samples_per_chirp * num_antennas;

        rc = m_driver->set_slice_size(calculate_slice_size(frame_size, config->frame_repetition_time_s, adc_fifo_size));
        CHECK_AVIAN_ERR_CODE_GOTO_FAIL(rc);
    }

    {
        ifx_Error_t ret = send_to_device();
        if (ret != IFX_OK)
            ifx_error_set(ret);
    }

    return;

fail:
    ifx_error_set(RadarDeviceErrorTranslator::translate_error_code(rc));
}

//----------------------------------------------------------------------------

ifx_Frame_R_t* RadarDeviceBase::create_frame_from_device_handle()
{
    uint8_t num_antennas = ifx_devconf_count_rx_antennas(&m_config);
    return ifx_frame_create_r(num_antennas, m_config.num_chirps_per_frame, m_config.num_samples_per_chirp);
}


ifx_Device_Calc_t* RadarDeviceBase::get_device_calc() const
{
    return m_calc;
};

ifx_Float_t RadarDeviceBase::get_device_calc_chirp_time(const ifx_Device_Config_t* config) const
{
    const auto sampling_time = static_cast<ifx_Float_t>(config->num_samples_per_chirp) / config->sample_rate_Hz;
    return static_cast<ifx_Float_t>(ifx_device_calc_chirp_time(m_calc, sampling_time));
};

//----------------------------------------------------------------------------

ifx_Error_t RadarDeviceBase::translate_config_to_shape_set(ifx_Device_Shape_Set_t* shape_set) const
{
    IFX_ERR_BRV_NULL(shape_set, IFX_ERROR_ARGUMENT_NULL);

    std::memset(shape_set, 0, sizeof(ifx_Device_Shape_Set_t));

    enum ifx_Device_MIMO_Mode mimo_mode = m_config.mimo_mode;

    const double frame_time = m_config.frame_repetition_time_s;

    /* sampling_time = num_samples_per_chirp/adc_samplerate_Hz */
    const double sampling_time = ifx_device_calc_sampling_time(m_calc, m_config.num_samples_per_chirp, m_config.sample_rate_Hz);

    /* chirp_to_chirp_time (also known as pulse_repetition_time) */
    const double chirp_repetition_time = ifx_devconf_get_chirp_repetition_time(&m_config);

    /*
     * With the parameters read from the device and the parameters specified by the caller,
     * calculate the time taken for a complete single chirp. To achieve the specified chirp-to-chirp
     * time, a post delay ("shape end delay" = T_SED) must be applied. Of course a negative delay is
     * not possible, and the chirp-to-chirp time cannot be smaller than the single chirp time.
     */
    ifx_device_power_mode_t shape_end_power_mode, frame_end_power_mode;

    const double shape_end_delay = ifx_device_calc_shape_end_delay(m_calc, mimo_mode == IFX_MIMO_TDM, sampling_time, chirp_repetition_time, &shape_end_power_mode);
    const double frame_end_delay = ifx_device_calc_frame_end_delay(m_calc, sampling_time, chirp_repetition_time, m_config.num_chirps_per_frame, frame_time, &frame_end_power_mode);

    /* check that shape_end_delay and frame_end_delay are valid */
    if (shape_end_delay < 0)
        return IFX_ERROR_CHIRP_RATE_OUT_OF_RANGE;
    if(frame_end_delay < 0)
        return IFX_ERROR_FRAME_RATE_OUT_OF_RANGE;

    const uint64_t T_SED = round_to_uint64(shape_end_delay / 100e-12);
    const uint64_t T_FED = round_to_uint64(frame_end_delay / 100e-12);

    // take care of the Frame_Definition_t via the shape
    shape_set->shape[0].num_repetition = 1; // one repetition
    shape_set->shape[0].following_power_mode = static_cast<uint8_t>(shape_end_power_mode);
    shape_set->shape[0].end_delay_100ps = T_SED;

    /* tx_mode for shape 0 */
    shape_set->shape[0].config.up.antenna_setup.tx_mode = static_cast<uint8_t>(m_config.tx_mask == 2 ? Avian::Tx_Mode::Tx2_Only : Avian::Tx_Mode::Tx1_Only);

    if (m_config.mimo_mode == IFX_MIMO_TDM)
    {
        shape_set->shape[0].following_power_mode = static_cast<uint8_t>(Avian::Power_Mode::Stay_Active);
        shape_set->shape[0].end_delay_100ps = 0;

        shape_set->shape[1].num_repetition = 1;
        shape_set->shape[1].following_power_mode = static_cast<uint8_t>(shape_end_power_mode);
        shape_set->shape[1].end_delay_100ps = T_SED;

        shape_set->shape[0].config.up.antenna_setup.tx_mode = static_cast<uint8_t>(Avian::Tx_Mode::Tx1_Only);
    }

    // this corresponds to the fmcw config for shape 0
    shape_set->shape[0].config.direction = DIR_UPCHIRP_ONLY;
    shape_set->shape[0].config.lower_frequency_kHz = round_to_uint32((ifx_Float_t)m_config.lower_frequency_Hz / 1000);
    shape_set->shape[0].config.upper_frequency_kHz = round_to_uint32((ifx_Float_t)m_config.upper_frequency_Hz / 1000);
    shape_set->shape[0].config.tx_power = static_cast<uint8_t>(m_config.tx_power_level);

    /* frame format for shape 0 */
    shape_set->shape[0].config.up.chirp_setting.num_samples = m_config.num_samples_per_chirp;
    shape_set->shape[0].config.up.chirp_setting.end_delay_100ps = m_chirp_end_delay_100ps;
    shape_set->shape[0].config.up.antenna_setup.rx_mask = static_cast<uint8_t>(m_config.rx_mask);

    if (m_config.mimo_mode == IFX_MIMO_TDM)
    {
        // this corresponds to the fmcw config for shape 1
        shape_set->shape[1].config.direction = DIR_UPCHIRP_ONLY;
        shape_set->shape[1].config.lower_frequency_kHz = (uint32_t)(m_config.lower_frequency_Hz / 1000);
        shape_set->shape[1].config.upper_frequency_kHz = (uint32_t)(m_config.upper_frequency_Hz / 1000);
        /* tx_mode for shape 1 */
        shape_set->shape[1].config.tx_power = static_cast<uint8_t>(m_config.tx_power_level);
        // and here is the frame format for shape 1
        shape_set->shape[1].config.up.chirp_setting.num_samples = m_config.num_samples_per_chirp;
        shape_set->shape[1].config.up.chirp_setting.end_delay_100ps = m_chirp_end_delay_100ps;
        shape_set->shape[1].config.up.antenna_setup.rx_mask = static_cast<uint8_t>(m_config.rx_mask);
        /* and tx_mode for shape[1] */
        shape_set->shape[1].config.up.antenna_setup.tx_mode = static_cast<uint8_t>(Avian::Tx_Mode::Tx2_Only);
    }

    // and at last the parameters for the entire shape group
    shape_set->num_repetition = static_cast<uint16_t>(m_config.num_chirps_per_frame);
    shape_set->following_power_mode = static_cast<uint8_t>(frame_end_power_mode);
    shape_set->end_delay_100ps = T_FED;

    return IFX_OK;
}

//----------------------------------------------------------------------------

ifx_Error_t RadarDeviceBase::initialize_baseband_config(Avian::Baseband_Configuration* baseband_config) const
{
    constexpr uint32_t reset_timer_period_100ps = 15875;

    std::memset(baseband_config, 0, sizeof(Avian::Baseband_Configuration));

    /*
    * Find the closest match to the specified IF gain. HP gain can be 18dB or 30dB, VGA gain
    * can be 0dB to 30dB in steps of 5dB.
    * Assume both, 18dB HP gain and 30dB HP gain, and calculate the vga gain setting. (Note
    * the +2, this is for rounding). Then calculate the gain of both settings and compare with
    * the specified setting. Pick the closer setting and configure device accordingly.
    */
    if ((m_config.if_gain_dB < IFX_IF_GAIN_DB_LOWER_LIMIT) || (m_config.if_gain_dB > IFX_IF_GAIN_DB_UPPER_LIMIT))
        return IFX_ERROR_IF_GAIN_OUT_OF_RANGE;

    int vga_gain_for_hp18 = ((m_config.if_gain_dB - 18) + 2) / 5;
    vga_gain_for_hp18 = (vga_gain_for_hp18 >= 0) ? vga_gain_for_hp18 : 0;
    vga_gain_for_hp18 = (vga_gain_for_hp18 <= 6) ? vga_gain_for_hp18 : 6;
    int gain_error_for_hp18 = (18 + vga_gain_for_hp18 * 5) - m_config.if_gain_dB;
    gain_error_for_hp18 = (gain_error_for_hp18 >= 0) ? gain_error_for_hp18 : -gain_error_for_hp18;

    int vga_gain_for_hp30 = ((m_config.if_gain_dB - 30) + 2) / 5;
    vga_gain_for_hp30 = (vga_gain_for_hp30 >= 0) ? vga_gain_for_hp30 : 0;
    vga_gain_for_hp30 = (vga_gain_for_hp30 <= 6) ? vga_gain_for_hp30 : 6;
    int gain_error_for_hp30 = (30 + vga_gain_for_hp30 * 5) - m_config.if_gain_dB;
    gain_error_for_hp30 = (gain_error_for_hp30 >= 0) ? gain_error_for_hp30 : -gain_error_for_hp30;

    Avian::Hp_Gain hp_gain;
    Avian::Vga_Gain vga_gain;

    if (gain_error_for_hp18 <= gain_error_for_hp30)
    {
        hp_gain = Avian::Hp_Gain::_18dB;
        vga_gain = static_cast<Avian::Vga_Gain>(vga_gain_for_hp18);
    }
    else
    {
        hp_gain = Avian::Hp_Gain::_30dB;
        vga_gain = static_cast<Avian::Vga_Gain>(vga_gain_for_hp30);
    }

    baseband_config->hp_gain_1 = hp_gain;
    baseband_config->hp_cutoff_1 = Avian::Hp_Cutoff::_80kHz;
    baseband_config->vga_gain_1 = vga_gain;
    baseband_config->hp_gain_2 = hp_gain;
    baseband_config->hp_cutoff_2 = Avian::Hp_Cutoff::_80kHz;
    baseband_config->vga_gain_2 = vga_gain;
    baseband_config->hp_gain_3 = hp_gain;
    baseband_config->hp_cutoff_3 = Avian::Hp_Cutoff::_80kHz;
    baseband_config->vga_gain_3 = vga_gain;
    baseband_config->hp_gain_4 = hp_gain;
    baseband_config->hp_cutoff_4 = Avian::Hp_Cutoff::_80kHz;
    baseband_config->vga_gain_4 = vga_gain;
    baseband_config->reset_period_100ps = reset_timer_period_100ps;

    return IFX_OK;
}


//----------------------------------------------------------------------------

bool RadarDeviceBase::stop_and_reset()
{
    bool res = false;

    if (m_driver)
    {
        const auto rc = m_driver->stop_and_reset_sequence();
        res = (RadarDeviceErrorTranslator::translate_error_code(rc) == ifx_Error_t::IFX_OK);

        m_acquisition_state = res ? Acquisition_State_t::Stopped : Acquisition_State_t::Error;
    }

    return res;
}
