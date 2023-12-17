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

#include <string.h> // memset

#include "ifxRadar/SDK.h"
#include "segmentation_defaults.h"

#include "app_common.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/


/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/
typedef struct {
    ifx_Segmentation_Handle_t segmentation_handle;
    ifx_Vector_R_t* segments;
    ifx_Matrix_R_t* tracks;
    ifx_Cube_R_t* matrix_data;
    ifx_Segmentation_Config_t seg_config;
} segmentation_t;

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/
static void use_default_metrics(ifx_Device_Metrics_t* metrics)
{
    metrics->sample_rate_Hz = IFX_ADC_SAMPLERATE_HZ,
    metrics->tx_power_level = IFX_BGT_TX_POWER,
    metrics->rx_mask = IFX_RX_MASK,
    metrics->tx_mask = IFX_TX_MASK,
    metrics->if_gain_dB = IFX_IF_GAIN_DB,
    metrics->range_resolution_m = IFX_RANGE_RESOLUTION_M,
    metrics->max_range_m = IFX_MAX_RANGE_M,
    metrics->speed_resolution_m_s = IFX_SPEED_RESOLUTION_M_S,
    metrics->max_speed_m_s = IFX_MAX_SPEED_M_S,
    metrics->frame_repetition_time_s = ((ifx_Float_t)1)/IFX_FRAME_RATE_HZ,
    metrics->center_frequency_Hz = 0; /* read center frequency from device */
}

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static void process_segmentation_result(ifx_Vector_R_t* segments, ifx_Matrix_R_t* tracks)
{
    IFX_ERR_BRK_NULL(segments);
    IFX_ERR_BRK_NULL(tracks);

    app_print(" ,\"segments\":[");
    for (uint32_t i = 0; i < IFX_VEC_LEN(segments); i++)
    {
        app_print("%d", (int)IFX_VEC_AT(segments, i));
        if (i < (IFX_VEC_LEN(segments) - 1))
            app_print(", ");
    }
    app_print("], \"tracks\":[");
    for (uint32_t i = 0; i < IFX_MAT_ROWS(tracks); i++)
    {
        if(IFX_MAT_AT(tracks, i, 0) == 0)
            break;

        if(i > 0)
            app_print(",");

        app_print("[%d,%5.2f,%5.2f,%5.2f]", (int)IFX_MAT_AT(tracks, i, 0),
            IFX_MAT_AT(tracks, i, 1),
            IFX_MAT_AT(tracks, i, 2),
            IFX_MAT_AT(tracks, i, 3));
    }
    app_print("]");
}

static uint32_t determine_number_of_rx_antennas_needed(uint32_t rx_mask)
{
    uint32_t count = 0;
    uint32_t bitno = 0;

    for (; rx_mask; rx_mask >>= 1, ++bitno)
    {
        count = (rx_mask & 0x01) * (bitno + 1);
    }

    return count;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
ifx_Error_t segmentation_init(segmentation_t *segmentation_context)
{
    memset(segmentation_context, 0, sizeof(segmentation_t));
    return IFX_OK;
}

ifx_Error_t segmentation_config(segmentation_t* segmentation_context, ifx_Device_Handle_t device, ifx_json_t* json, ifx_Device_Config_t* dev_config)
{
    if((device != NULL) && ifx_device_get_num_rx_antennas(device) < determine_number_of_rx_antennas_needed(dev_config->rx_mask))
    {
        return IFX_ERROR_RX_ANTENNA_COMBINATION_NOT_ALLOWED;
    }

    if (ifx_json_has_segmentation(json))
    {
        // get configuration from json
        bool ret = ifx_json_get_segmentation(json, dev_config, &(segmentation_context->seg_config));
        if (!ret)
        {
            fprintf(stderr, "Invalid segmentation configuration: %s", ifx_json_get_error(json));
            return IFX_ERROR_APP;
        }
    }
    else
    {
        ifx_Device_Radar_Type_t device_type = ifx_device_get_device_type(device);
        // use default segmentation configuration
        ifx_segmentation_config_get_defaults(device_type, dev_config, &segmentation_context->seg_config);

        // add it to the json structure
        ifx_json_set_segmentation(json, &segmentation_context->seg_config);
    }

    segmentation_context->matrix_data = ifx_cube_create_r(segmentation_context->seg_config.num_chirps_per_frame, segmentation_context->seg_config.num_samples_per_chirp, 2);
    segmentation_context->tracks = ifx_mat_create_r(segmentation_context->seg_config.max_num_tracks, 4);
    segmentation_context->segments = ifx_vec_create_r(IFX_VEC_LEN(segmentation_context->seg_config.segments_degrees));

    segmentation_context->segmentation_handle = ifx_segmentation_create(&(segmentation_context->seg_config));

    return ifx_error_get();
}


ifx_Error_t segmentation_cleanup(segmentation_t* segmentation_context)
{
    ifx_vec_destroy_r(segmentation_context->seg_config.segments_degrees);

    ifx_cube_destroy_r(segmentation_context->matrix_data);
    ifx_vec_destroy_r(segmentation_context->segments);
    ifx_mat_destroy_r(segmentation_context->tracks);
    ifx_segmentation_destroy(segmentation_context->segmentation_handle);

    return ifx_error_get();
}

ifx_Error_t segmentation_process(segmentation_t* segmentation_context, ifx_Frame_R_t* frame)
{
    ifx_Matrix_R_t slice;
    ifx_cube_get_slice_r(segmentation_context->matrix_data, 0, &slice);
    ifx_mat_copy_r(frame->rx_data[frame->num_rx - 1], &slice);

    ifx_cube_get_slice_r(segmentation_context->matrix_data, 1, &slice);
    ifx_mat_copy_r(frame->rx_data[0], &slice);

    ifx_segmentation_run(segmentation_context->segmentation_handle,
                         segmentation_context->matrix_data,
                         segmentation_context->segments,
                         segmentation_context->tracks);

    process_segmentation_result(segmentation_context->segments, segmentation_context->tracks);
    return(ifx_error_get());
}
/*
==============================================================================
   7. MAIN METHOD
==============================================================================
 */

int main(int argc, char* argv[])
{
    ifx_Device_Metrics_t metrics = { 0 };
    use_default_metrics(&metrics);

    app_t s_segmentation;
    segmentation_t segmentation_context;
    int exitcode;
    // function Description
    static const char* app_description = "Segmentation detection";
    static const char* app_epilog = "\n"
        "Output\n"
        "    The output format is given in JSON format. elapsed_time and frame_number\n"
        "    denote the time passed since starting the application and the current frame\n"
        "    number, respectively. The array segments list if an object was detected in\n"
        "    the respective segment (1) or not (0). The array tracks contains a list of\n"
        "    all current tracks. Within a track, the first number denotes the track number\n"
        "    (starting from 1). The following three numbers are the distance of the object\n"
        "    to the sensor in meters, the angle in degrees, and the speed in meters per\n"
        "    second.";

    s_segmentation.app_description = app_description;
    s_segmentation.app_epilog = app_epilog;

    s_segmentation.app_init = (void*)segmentation_init;
    s_segmentation.app_config = (void*)segmentation_config;
    s_segmentation.app_process = (void*)segmentation_process;
    s_segmentation.app_cleanup = (void*)segmentation_cleanup;

    s_segmentation.default_metrics = &metrics;

    exitcode = app_start(argc, argv, &s_segmentation, &segmentation_context);

    return exitcode;
}
