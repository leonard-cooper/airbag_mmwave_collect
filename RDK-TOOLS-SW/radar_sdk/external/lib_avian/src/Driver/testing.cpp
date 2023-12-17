/**
    @file testing.cpp

    This file is part of the BGT60TRxx driver.

    This file contains functions to configure the testing functionality of the
    BGT60TRxx chip.
*/
/* ===========================================================================
** Copyright (C) 2017 - 2021 Infineon Technologies AG
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


// ---------------------------------------------------------------------------- includes
#include "ifxAvian_Driver.hpp"
#include "ifxAvian_DeviceTraits.hpp"
#include "_configuration.h"
#include "registers_BGT60TRxxC.h"
#include "registers_BGT60TRxxD.h"
#include "registers_BGT60TRxxE.h"

// ---------------------------------------------------------------------------- namespaces
namespace Infineon
{
    namespace Avian
    {

#ifdef IFXRADAR_BGT60TRXX_LAB_FEATURES_ENABLED
extern "C"
{
// ---------------------------------------------------------------------------- radar_hal_start_register_toggling
/**
 * \brief This function starts permanent SPI register programming.
 *
 * When this function is called the interface starts to send a sequence of
 * two SPI words permanently to the connected BGT60TRxx chip. The first word
 * of this sequence is given by parameter \par register_value. The second
 * word of the sequence is the result of \par register_value XOR
 * \par toggle_mask.
 *
 * The toggle interface keeps on sending the pattern to the chip until
 * \ref radar_hal_stop_register_toggling is called. Further more the toggle
 * sequence is paused whenever any other SPI transaction is to be done. This
 * means \ref radar_hal_read_spi_burst and \ref radar_hal_transfer_spi_words
 * must stop sending the toggle sequence, perform their work, and finally
 * restart sending the toggle sequence, if toggling was enabled before.
 *
 * \note This function is meant for a very special testing use case only.
 *       That's why the usual object oriented paradigm is not followed and a
 *       pointer to the instance is not in the argument list.
 *
 * \param[in] register_value  The first value of the SPI toggle sequence.
 * \param[in] toggle_mask     A mask that is XORed to the register value to
 *                            get the second word of the SPI toggle sequence.
 */
void radar_hal_start_register_toggling(uint32_t register_value,
                                       uint32_t toggle_mask);

// ---------------------------------------------------------------------------- radar_hal_stop_register_toggling
/**
 * \brief This function stops permanent SPI register programming.
 *
 * When this function is called the interface stops sending the SPI toggle
 * sequence, that has been started through
 * \ref radar_hal_start_register_toggling. If this function is called, while
 * SPI toggle sequence sending is not active, nothing happens.
 *
 * \return If the interface was not sending the SPI toggle sequence when this
 *         function was called, 0 is returned. A non-zero value is returned
 *         when the interface was sending the SPI toggle sequence.
 */
uint8_t radar_hal_stop_register_toggling(void);
}
#endif

// ---------------------------------------------------------------------------- cw_mode_go_active
Driver::Error Driver::cw_mode_go_active()
{
    HW::Spi_Command_t spi_trigger_words[3];
    uint32_t i;

    /* Now step through the FSM until active mode is reached */
    /* ----------------------------------------------------- */
    spi_trigger_words[0] = m_current_configuration[BGT60TRxxC_REG_MAIN] |
                           BGT60TRxxC_SET(MAIN, FRAME_START, 1) |
                           BGT60TRxxC_SET(MAIN, CW_MODE, 1);
    spi_trigger_words[1] = m_device_traits.has_reordered_register_layout
                           ? BGT60TRxxE_REGISTER_READ_CMD(STAT0)
                           : BGT60TRxxC_REGISTER_READ_CMD(STAT0);
    spi_trigger_words[2] = 0;

    for (i = 0; i < 10000; ++i)
    {
        /* send a trigger */
        m_port.send_commands(spi_trigger_words, 1);

        /* read the current power mode */
        m_port.send_commands(&spi_trigger_words[1], 1,
                             &spi_trigger_words[2]);

        if (BGT60TRxxC_EXTRACT(STAT0, PM, spi_trigger_words[2]) == 1)
        {
            break;
        }
    }

    /* if active wasn't reached, something went wrong */
    if (i >= 10000)
    {
        return Error::BUSY;
    }

    return Error::OK;
}

// ---------------------------------------------------------------------------- test_antennas
Driver::Error Driver::test_antennas(uint8_t tx_antenna_mask,
                                    uint8_t rx_antenna_mask,
                                    uint32_t frequency_kHz,
                                    uint8_t tx_power)
{
    /* first do a range check of provided parameters */
    /* --------------------------------------------- */
    if (((tx_antenna_mask & ~((1 << m_device_traits.num_tx_antennas) - 1)) != 0) ||
        ((rx_antenna_mask & ~((1 << m_device_traits.num_rx_antennas) - 1)) != 0))
    {
        return Error::ANTENNA_DOES_NOT_EXIST;
    }

    uint32_t min_rf_frequency;
    uint32_t max_rf_frequency;
    get_frequency_range(&min_rf_frequency, &max_rf_frequency);
    if ((frequency_kHz != 0) && ((frequency_kHz < min_rf_frequency) ||
                                 (frequency_kHz > max_rf_frequency)))
    {
        return Error::FREQUENCY_OUT_OF_RANGE;
    }

    if (tx_power > BGT60TRXX_MAX_TX_POWER)
    {
        return Error::POWER_OUT_OF_RANGE;
    }

    if ((tx_antenna_mask != 0) || (rx_antenna_mask != 0))
    {
        /* user requested to enable antenna test mode */
        /* ------------------------------------------ */
        Shape_Settings backup_shape1;
        Channel_Set_Settings backup_channel_set1;
        uint16_t backup_num_reps[3];
        uint16_t backup_num_frames;
        uint16_t backup_num_set_repetitions;
        Power_Mode backup_frame_end_power_mode;
        Mul_Counter backup_frame_end_delay;
        Power_Down_Configuration backup_deep_sleep;
        Power_Down_Configuration backup_idle;
        Mul_Counter backup_wakeup_time;
        Mul_Counter backup_init0_time;
        Mul_Counter backup_inti1_time;

        uint32_t spi_words[10];
        Error error_code;

        /* can't enable antenna testing, while automatic trigger is running */
        if (m_current_mode == MODE_EASY_TRIGGER_RUNNING)
        {
            return Error::BUSY;
        }

        /* remember, that antenna test mode is active */
        m_current_mode = Mode_e(m_current_mode | MODE_ANTENNA_TEST);

        /* in antenna test mode with data capture, setup FIFO */
        if (m_current_mode == MODE_ANTENNA_TEST_CAPTURE)
        {
            /* calculate number of samples per frame */
            m_slice_size = m_shape[0].num_samples_up *
                           count_antennas(rx_antenna_mask);
        }

        /* configure chip for antenna test mode */
        /* ------------------------------------ */

        /* create a backup of the current chip configuration cache */
        backup_num_reps[0] = m_shape[1].num_repetitions;
        backup_num_reps[1] = m_shape[2].num_repetitions;
        backup_num_reps[2] = m_shape[3].num_repetitions;
        backup_shape1 = m_shape[0];
        backup_channel_set1 = m_channel_set[0];
        backup_num_set_repetitions = m_num_set_repetitions;
        backup_frame_end_power_mode = m_frame_end_power_mode;
        backup_frame_end_delay = m_frame_end_delay;
        backup_num_frames = m_num_frames_before_stop;
        backup_deep_sleep = m_deep_sleep_settings;
        backup_idle = m_idle_settings;
        backup_wakeup_time = m_time_wake_up;
        backup_init0_time = m_time_init0;
        backup_inti1_time = m_time_init1;

        /* configure channel set 1 up for antenna test mode */
        m_channel_set[0].rx_mask = rx_antenna_mask;
        m_channel_set[0].num_rx_antennas = count_antennas(rx_antenna_mask);
        m_channel_set[0].tx_power = tx_power;

        m_channel_set[0].tx_mode =
            (tx_antenna_mask & 0x01)
              ? ((tx_antenna_mask & 0x02) ? Tx_Mode::Alternating
                                          : Tx_Mode::Tx1_Only)
              : ((tx_antenna_mask & 0x02) ? Tx_Mode::Tx2_Only
                                          : Tx_Mode::Off);

        /* enable only shape 1 */
        m_shape[0].shape_type = Shape_Type::Saw_Up;
        m_shape[0].num_repetitions = 1;
        m_shape[0].following_power_mode = Power_Mode::Stay_Active;
        m_shape[0].post_delay.counter = 0;
        m_shape[0].post_delay.shift_factor = 0;
        m_shape[1].num_repetitions = 0;
        m_shape[2].num_repetitions = 0;
        m_shape[3].num_repetitions = 0;

        m_frame_end_power_mode = Power_Mode::Stay_Active;
        m_frame_end_delay.counter = 0;
        m_frame_end_delay.shift_factor = 0;
        m_num_frames_before_stop = 0;

        /* configure Deep Sleep mode to default settings */
        m_deep_sleep_settings.enable_pll = 0;
        m_deep_sleep_settings.enable_vco = 0;
        m_deep_sleep_settings.enable_fdiv = 0;
        m_deep_sleep_settings.enable_baseband = 0;
        m_deep_sleep_settings.enable_rf = 0;
        m_deep_sleep_settings.enable_madc = 0;
        m_deep_sleep_settings.enable_madc_bandgap = 0;
        m_deep_sleep_settings.enable_sadc = 0;
        m_deep_sleep_settings.enable_sadc_bandgap = 0;

        /* configure Idle mode to default settings */
        m_idle_settings.enable_pll = 0;
        m_idle_settings.enable_vco = 0;
        m_idle_settings.enable_fdiv = 0;
        m_idle_settings.enable_baseband = 0;
        m_idle_settings.enable_rf = 0;
        m_idle_settings.enable_madc = 0;
        m_idle_settings.enable_madc_bandgap = 1;
        m_idle_settings.enable_sadc = 0;
        m_idle_settings.enable_sadc_bandgap = 0;

        /* configure startup timing */
        m_time_wake_up = convert_to_multiplied_counter(10000*8,   /* 1 ms */
                                                       BGT60TRXX_MAX_WAKEUP_COUNTER_SHIFT,
                                                       BGT60TRXX_MAX_WAKEUP_COUNTER);
        m_time_init0 = convert_to_multiplied_counter(600*8,      /* 60 us */
                                                     BGT60TRXX_MAX_PLL_INIT_COUNTER_SHIFT,
                                                     !m_device_traits.has_extra_startup_delays
                                                     ? BGT60TRXXC_MAX_PLL_INIT0_COUNTER
                                                     : BGT60TRXXD_MAX_PLL_INIT0_COUNTER);
        m_time_init1 = convert_to_multiplied_counter(100*8,      /* 10 us */
                                                     BGT60TRXX_MAX_PLL_INIT_COUNTER_SHIFT,
                                                     BGT60TRXX_MAX_PLL_INIT1_COUNTER);

        /* update SPI registers of BGT60TR24 chip */
        update_spi_register_set();

        /* additional register settings, that are not programmed in
         * non test mode
         */

        /* enable TX power sensors */
        spi_words[0] = m_current_configuration[BGT60TRxxC_REG_CS1_U_0] |
                       BGT60TRxxC_SET(CS1_U_0, PD1_EN, 1) |
                       BGT60TRxxC_SET(CS1_U_0, PD2_EN, 1);

        /* set frequency */
        spi_words[1] = BGT60TRxxC_SET(PLL1_0, FSU,
                                      convert_freq_to_reg(frequency_kHz));

        /* configure endless sequence of one chirp */
        spi_words[2] = BGT60TRxxC_SET(PLL1_1, RSU, 0);
        spi_words[3] = BGT60TRxxC_SET(PLL1_2, RTU, 0);
        spi_words[4] = BGT60TRxxC_SET(PLL1_3, APU,
                                      m_shape[0].num_samples_up);
        spi_words[5] = BGT60TRxxC_SET(PLL1_4, FSD, 0);
        spi_words[6] = BGT60TRxxC_SET(PLL1_5, RSD, 0);
        spi_words[7] = BGT60TRxxC_SET(PLL1_6, RTD, 0);

        /* enable CW mode */
        spi_words[8] = BGT60TRxxC_SET(PDFT0, BYPRMPEN, 1);

        spi_words[9] = m_current_configuration[BGT60TRxxC_REG_MAIN] |
                       BGT60TRxxC_SET(MAIN, CW_MODE, 1);

        for (auto spi_word : spi_words)
            m_current_configuration.set(spi_word);

        /*
         * If the device does not have an SADC, MADC must be enabled to be
         * ready for temperature and power measurement. MADC can't be enabled
         * directly, it must be enabled implicitly by enabling at least one RX
         * channel.
         */
        if (!m_device_traits.has_sadc)
        {
            spi_words[0] = m_current_configuration[BGT60TRxxC_REG_CS1_U_1] |
                           BGT60TRxxC_SET(CS1_U_1, BBCH_SEL, 1);
            m_current_configuration.set(spi_words[0]);
        }

        /* send configuration to chip */
        reset(false);
        get_device_configuration().send_to_device(m_port, false);

        /* Now step through the FSM until active mode is reached */
        /* ----------------------------------------------------- */
        error_code = cw_mode_go_active();
        if (error_code != Error::OK)
        {
            test_antennas(0, 0, 0, 0);
        }
        else
        {
#ifdef IFXRADAR_BGT60TRXX_LAB_FEATURES_ENABLED
            if ((m_device_type == Device_Type::BGT60TR13C) ||
                (m_device_type == Device_Type::BGT60ATR24C))
            {
                Tx_Toggle_Settings settings;
                settings.mode = m_tx_toggle_mode;
                settings.frequency_Hz = ((m_reference_clock_freq_Hz * 2) /
                                         m_test_tone_freq_divider + 1) / 2;
                set_tx_toggle_settings(&settings);
            }
#endif
        }

        /* restore chip configuration cache */
        /* -------------------------------- */
        m_shape[1].num_repetitions = backup_num_reps[0];
        m_shape[2].num_repetitions = backup_num_reps[1];
        m_shape[3].num_repetitions = backup_num_reps[2];
        m_shape[0] = backup_shape1;
        m_channel_set[0] = backup_channel_set1;
        m_num_set_repetitions = backup_num_set_repetitions;
        m_frame_end_power_mode = backup_frame_end_power_mode;
        m_frame_end_delay = backup_frame_end_delay;
        m_num_frames_before_stop = backup_num_frames;
        m_deep_sleep_settings = backup_deep_sleep;
        m_idle_settings = backup_idle;
        m_time_wake_up = backup_wakeup_time;
        m_time_init0 = backup_init0_time;
        m_time_init1 = backup_inti1_time;

        update_spi_register_set();

        return error_code;
    }
    else
    {
        /* user requested to disable antenna test mode */
        /* ------------------------------------------- */
        if (m_current_mode & MODE_ANTENNA_TEST)
        {
#ifdef IFXRADAR_BGT60TRXX_LAB_FEATURES_ENABLED
            if ((m_device_type == Device_Type::BGT60TR13C) ||
                (m_device_type == Device_Type::BGT60ATR24C))
            {
                radar_hal_stop_register_toggling();
            }
#endif

            /* turn off test mode logically */
            m_current_mode = Mode_e(m_current_mode & ~MODE_ANTENNA_TEST);

            /* prevent programming of PDFT register */
            m_current_configuration.remove(BGT60TRxxC_REG_PDFT0);

            /* in easy mode, restore FIFO buffer setup */
            enable_easy_mode(Mode_e(m_current_mode & MODE_EASY));

            /* (reset is already done by radar_enable_easy_mode) */
        }
    }

    return Error::OK;
}

// ---------------------------------------------------------------------------- is_in_antenna_test_mode
bool Driver::is_in_antenna_test_mode() const
{
    return (m_current_mode & MODE_ANTENNA_TEST) != 0;
}

// ---------------------------------------------------------------------------- set_baseband_test_configuration
Driver::Error Driver::set_baseband_test_configuration(const Baseband_Test_Configuration* configuration)
{
    /* in antenna test mode, settings can't be changed */
    if (m_current_mode == MODE_ANTENNA_TEST_CAPTURE)
    {
        return Error::BUSY;
    }

    /* BGT60TR24 has 4 RX antennas, don't try to activate more than this */
    if ((configuration->rx_mask & ~((1 << m_device_traits.num_rx_antennas) - 1)) != 0)
    {
        return Error::ANTENNA_DOES_NOT_EXIST;
    }

    if ((m_tx_toggle_mode != RADAR_BGT60TRXX_TX_TOGGLE_OFF) &&
        (configuration->rx_mask != 0))
    {
        return Error::TEST_CONFLICT_BB_TX;
    }

    if (configuration->frequency_Hz != 0)
    {
        if (configuration->frequency_Hz > 2000000)
        {
            return Error::FREQUENCY_OUT_OF_RANGE;
        }

        /* round frequency */
        m_test_tone_freq_divider = ((m_reference_clock_freq_Hz * 2) /
                                        configuration->frequency_Hz + 1) / 2;
        if (m_test_tone_freq_divider > BGT60TRxxC_RFT0_RFTSIGCLK_DIV_msk)
            m_test_tone_freq_divider = BGT60TRxxC_RFT0_RFTSIGCLK_DIV_msk;
    }

    m_base_band_test_rx_mask = configuration->rx_mask;

    /* update SPI registers of BGT60TR24 chip */
    /* --------------------------------------- */
    update_spi_register_set();

    return Error::OK;
}

// ---------------------------------------------------------------------------- get_baseband_test_configuration
Driver::Error Driver::get_baseband_test_configuration(Baseband_Test_Configuration* configuration)
{
    /* return current base band settings from driver object */
    configuration->rx_mask = m_base_band_test_rx_mask;
    configuration->frequency_Hz = ((m_reference_clock_freq_Hz * 2) /
                                   m_test_tone_freq_divider + 1) / 2;

    return Error::OK;
}

// ---------------------------------------------------------------------------- dump_registers
Driver::Error Driver::dump_registers(uint32_t* register_data, uint8_t* num_registers)
{
    HW::Spi_Command_t spi_words[BGT60TRxxE_NUM_REGISTERS];
    uint8_t i;

    /* setup SPI words for read out */
    uint8_t num_read_regs = m_device_traits.num_registers;
    for (i = 0; i < num_read_regs; ++i)
    {
        /*
         *  setup read register command:
         *  - address in upper bits 25...31,
         *  - R/W in bit 24 (0 = read)
         *  - bits 0...23 are don't care, send 0
         */
        spi_words[i] = i << 25;
    }

    /* send words to chip and read registers back */
    m_port.send_commands(spi_words, num_read_regs, spi_words);

    /* copy data back to buffer */
    *num_registers = (*num_registers > num_read_regs) ?
                     num_read_regs : *num_registers;

    for (i = 0; i < *num_registers; ++i)
    {
        register_data[i] = spi_words[i] & 0x00FFFFFF;
        register_data[i] |=  i << 24;
    }

    return Error::OK;
}

// ---------------------------------------------------------------------------- set_tx_toggle_settings
Driver::Error Driver::set_tx_toggle_settings(const Tx_Toggle_Settings* settings)
{
    /* check if parameters are valid */
    if ((settings->mode != RADAR_BGT60TRXX_TX_TOGGLE_OFF) &&
        (settings->mode != RADAR_BGT60TRXX_TX_TOGGLE_TX_EN) &&
        (settings->mode != RADAR_BGT60TRXX_TX_TOGGLE_DAC) &&
        (settings->mode != RADAR_BGT60TRXX_TX_TOGGLE_RX_SELF_TEST))
    {
        return Error::TX_TOGGLE_MODE_NOT_SUPPORTED;
    }

    if (!m_device_traits.supports_tx_toggling)
    {
        if (settings->frequency_Hz > 1000000)
            return Error::FREQUENCY_OUT_OF_RANGE;
    }

    if ((settings->mode != RADAR_BGT60TRXX_TX_TOGGLE_OFF) &&
        (m_base_band_test_rx_mask != 0))
    {
        return Error::TEST_CONFLICT_BB_TX;
    }

    /* remember if toggle mode and rounded frequency */
    m_tx_toggle_mode = settings->mode;

    if (settings->frequency_Hz != 0)
    {
        uint32_t uDivider = ((m_reference_clock_freq_Hz * 2) /
                             settings->frequency_Hz + 1) / 2;
        if (m_device_traits.supports_tx_toggling)
            uDivider = (uDivider < 8191) ? uDivider : 8191;
        m_test_tone_freq_divider = uDivider;
    }

    /* if antenna test mode is enabled, apply change, otherwise stop here */
    if ((m_current_mode & MODE_ANTENNA_TEST) == 0)
    {
        return Error::OK;
    }

    if (m_device_traits.supports_tx_toggling)
    {
        /* Normal SPI register setup for BGT60TR13D takes TX toggle settings
         * into account, so triggering a full register setup will do the job.
         */
        update_spi_register_set();
        get_device_configuration().send_to_device(m_port, false);
    }
    else
    {
        /*
         * Device earlier then BGT60TR13D (digital chip ID < 6) don't support
         * TX register toggling. For those devices the HAL takes the toggling
         * job. Toggling causes a high load on the SPI bus, and HAL was not
         * mode for this. Therefore this feature is only support for special
         * builds. Official firmwares will not support this feature.
         */
#ifdef IFXRADAR_BGT60TRXX_LAB_FEATURES_ENABLED
        /* first stop toggling ... */
        radar_hal_stop_register_toggling();

        /* ... and now restart toggling in desired mode */
        if (m_tx_toggle_mode == RADAR_BGT60TRXX_TX_TOGGLE_TX_EN)
        {
            uint32_t reg = m_current_configuration[BGT60TRxxC_REG_CS1_U_0] |
                           BGT60TRxxC_SET(CS1_U_0, TX1_EN, 0);
            radar_hal_start_register_toggling(reg, BGT60TRxxC_CS1_U_0_TX1_EN_msk);
        }
        else if (m_tx_toggle_mode == RADAR_BGT60TRXX_TX_TOGGLE_DAC)
        {
            uint32_t reg = m_current_configuration[BGT60TRxxC_REG_CS1_U_1] |
                           BGT60TRxxC_SET(CS1_U_1, TX1_DAC, 0);
            uint32_t mask = reg & BGT60TRxxC_CS1_U_1_TX1_DAC_msk;
            radar_hal_start_register_toggling(reg, mask);
        }
        else if (m_tx_toggle_mode == RADAR_BGT60TRXX_TX_TOGGLE_RX_SELF_TEST)
        {
            uint32_t reg = m_current_configuration[BGT60TRxxC_REG_RFT0] |
                           BGT60TRxxC_SET(RFT0, TEST_SIG_RF_EN, 0);
            radar_hal_start_register_toggling(reg, 0x004000);
        }
#else
        return Error::FEATURE_NOT_SUPPORTED;
#endif
    }
    return Error::OK;
}

// ---------------------------------------------------------------------------- get_tx_toggle_settings
Driver::Error Driver::get_tx_toggle_settings(Tx_Toggle_Settings* settings)
{
    settings->mode = m_tx_toggle_mode;
    settings->frequency_Hz = ((m_reference_clock_freq_Hz * 2) /
                              m_test_tone_freq_divider + 1) / 2;
    return Error::OK;
}

/* ------------------------------------------------------------------------ */
    } // namespace Avian
} // namespace Infineon

/* --- End of File -------------------------------------------------------- */
