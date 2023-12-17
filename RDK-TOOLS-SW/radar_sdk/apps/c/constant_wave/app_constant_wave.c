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
 * @file app_constant_wave.c
 *
 * @brief Constant wave signal capturing application.
 * 
 */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ifxBase/Base.h"
#include "ifxDevice/Device.h"
#include "ifxDevice/DeviceConfig.h"
#include "constant_wave_defaults.h"
#include "ifxDevice/ConstantWaveControl.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

#define NUM_FETCHED_FRAMES  5  /**< Number of frames to fetch */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/*
==============================================================================
   4. DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

void process_frame(ifx_Frame_R_t* frame, uint32_t frame_count);

void process_antenna_data(ifx_Matrix_R_t* antenna_data);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

/**
 * This function will separate different antenna signals
 * and prints all samples per RX antenna to console.
 */
void process_frame(ifx_Frame_R_t* frame, uint32_t frame_count)
{
    ifx_Vector_R_t samples = { 0 };

    printf("\n========== Frame: %d ===========\n", frame_count);

    for(uint32_t ant = 0; ant < frame->num_rx; ant++)
    {        
        // Fetch a samples from the antenna data cube, where chirps = 1
        ifx_mat_get_rowview_r(frame->rx_data[ant], 0, &samples);

        printf("\n========== Rx Antenna: %d ===========\n", ant);
        for (uint32_t i = 0; i < samples.len; i++)
        {
            printf("%.4f ", IFX_VEC_AT(&samples, i));
        }
        printf("\n");
    }
}

/*
==============================================================================
   7. MAIN METHOD
==============================================================================
 */
int main(int argc, char** argv)
{
    ifx_Error_t error;
    ifx_Device_Handle_t device_handle = NULL;
    ifx_Device_CW_Control_Handle_t cw_handle = NULL;

    ifx_Device_CW_Config_t cw_config;

    int frame_number = 0;
    
    printf("Radar SDK Version: %s\n", ifx_sdk_get_version_string_full());

    /* Open the device */
    device_handle = ifx_device_create();

    if ((error = ifx_error_get()) != IFX_OK)
    {
        fprintf(stderr, "Failed to open device: %s\n", ifx_error_to_string(error));
        return EXIT_FAILURE;
    }
    /* Create the CW controller */
    cw_handle = ifx_device_cw_create(device_handle);

    if ((error = ifx_error_get()) != IFX_OK)
    {
        fprintf(stderr, "Failed to create constant wave controller: %s\n", ifx_error_to_string(error));
        return EXIT_FAILURE;
    }

    /* Read defaults of CW controller*/
    ifx_device_cw_get_default_config(cw_handle, &cw_config);

    ifx_Device_Baseband_Config_t baseband_configs;
    ifx_Device_ADC_Config_t adc_configs;
    const ifx_Device_ADC_Config_t* curr_adc_config;
    ifx_Device_Test_Signal_Generator_t test_signal_configs;

    /* Baseband configurations */
    baseband_configs = cw_config.baseband_configs;
    baseband_configs.aaf_cutoff = IFX_AAF_CUTOFF_1MHz; // Only for D & E variants
    baseband_configs.vga_gain = IFX_VGA_GAIN_5dB;
    baseband_configs.hp_gain = IFX_HP_GAIN_18dB;
    baseband_configs.hp_cutoff = IFX_HP_CUTOFF_80kHz;  // IFX_HP_CUTOFF_160kHz is for D and E variants
    ifx_device_cw_set_baseband_params(cw_handle, &baseband_configs);

    printf("\n************* Baseband configurations  \n");
    printf(" AAF Cutoff Enum: %d\n", baseband_configs.aaf_cutoff);
    printf(" HP Cutoff Enum: %d\n", baseband_configs.hp_cutoff);
    printf(" HP Gain Enum: %d\n", baseband_configs.hp_gain);
    printf(" VGA Gain Enum: %d\n", baseband_configs.vga_gain);

    /* ADC configurations */
    adc_configs = cw_config.adc_configs;
    adc_configs.samplerate_Hz = IFX_DEVICE_CW_SAMPLING_FREQ_HZ;
    adc_configs.oversampling = IFX_ADC_OVERSAMPLING_2x;
    adc_configs.tracking = IFX_ADC_3_SUBCONVERSIONS;
    ifx_device_cw_set_adc_params(cw_handle, &adc_configs);

    curr_adc_config = ifx_device_cw_get_adc_params(cw_handle);

    ifx_Float_t min_sample_rate, max_sample_rate;
    ifx_device_cw_get_sampling_rate_limits(cw_handle, &adc_configs, &min_sample_rate, &max_sample_rate);

    ifx_Float_t curr_sample_rate = ifx_device_cw_get_sampling_rate(cw_handle);

    printf("\n************* ADC configurations \n");
    printf(" Lower Limit of Sample Rate (Hz): %6.2f\n", min_sample_rate);
    printf(" Upper Limit of Sample Rate (Hz): %6.2f\n", max_sample_rate);
    printf(" Current Sample Rate (Hz): %6.2f\n", curr_sample_rate);
    printf(" Sample Rate (Hz): %d\n", curr_adc_config->samplerate_Hz);
    printf(" Over sampling Enum: %d\n", curr_adc_config->oversampling);
    printf(" Sample Time Enum: %d\n", curr_adc_config->sample_time);
    printf(" Tracking Enum: %d\n", curr_adc_config->tracking);
    

    /* Test signal configurations */
    test_signal_configs = cw_config.test_signal_configs;
    test_signal_configs.frequency_Hz = IFX_DEVICE_CW_TEST_SIGNAL_FREQ_HZ;
    ifx_device_cw_set_test_signal_generator_config(cw_handle, &test_signal_configs);

    printf("\n************* Test Signal configurations \n");
    printf(" Frequency (Hz): %10.1f\n", test_signal_configs.frequency_Hz);
    printf(" Test Mode Enum value: %d\n", test_signal_configs.mode);

    /* Overwrite CW defaults */
    ifx_device_cw_set_num_of_samples_per_antenna(cw_handle, IFX_DEVICE_CW_NUM_SAMPLES);
    ifx_device_cw_set_rf_frequency(cw_handle, (uint64_t)IFX_DEVICE_CW_FREQUENCY);
    ifx_device_cw_set_tx_dac_value(cw_handle, IFX_DEVICE_CW_DAC_VALUE);

    ifx_device_cw_enable_tx_antenna(cw_handle, 0, true); // Enable Tx1 antenna

    for (uint32_t i = 0; i < IFX_DEVICE_CW_RX_ANTENNAS; i++) // Enable Rx antennas
    {
        ifx_device_cw_enable_rx_antenna(cw_handle, i, true);
    }
    
    ifx_Frame_R_t* frame = ifx_frame_create_r(IFX_DEVICE_CW_RX_ANTENNAS, 1, IFX_DEVICE_CW_NUM_SAMPLES);

    if(frame == NULL)
    {
        fprintf(stderr, "Failed to create frame from constant wave handle: %s\n", ifx_error_to_string(ifx_error_get()));
        return EXIT_FAILURE;
    }

    /* Loop over the number of frames to fetch, defined by NUM_FETCHED_FRAMES.
       The number of frames can be altered as desired for the intended target application. */

    ifx_device_cw_start_emission(cw_handle);

    while(frame_number < NUM_FETCHED_FRAMES)
    {
        ifx_Error_t ret = ifx_device_cw_capture_frame(cw_handle, frame);
        if (ret != IFX_OK)
        {
            fprintf(stderr, "%s (%d)\n", ifx_error_to_string(ret), ret);
            break;
        }

        frame_number++;

        process_frame(frame, frame_number);
    }

    ifx_device_cw_destroy(cw_handle);

    /* Close the device after processing all frames. */
    if(device_handle) {
        ifx_device_destroy(device_handle); // CW controller will be destroyed within device handle
        ifx_frame_destroy_r(frame); // explicitly destroy CW frame else memory leak
        device_handle = NULL;
        cw_handle = NULL;
    }

    return EXIT_SUCCESS;
}
