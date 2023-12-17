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
 * @file app_recorder.c
 *
 * @brief Recorder application main program source file.
 *
 */


/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/


#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "ifxRadar/SDK.h"
#include "recorder_defaults.h"
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
    int dummy;
} recorder_t;

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/



//----------------------------------------------------------------------------
ifx_Error_t recorder_init(recorder_t* recorder_context)
{
    return ifx_error_get();
}

ifx_Error_t recorder_config(recorder_t* segmentation_context, ifx_Device_Handle_t device, ifx_json_t* json, ifx_Device_Config_t* dev_config)
{
    return ifx_error_get();
}

ifx_Error_t recorder_cleanup(recorder_t* segmentation_context)
{
    return ifx_error_get();
}

ifx_Error_t recorder_process(recorder_t* segmentation_context, ifx_Frame_R_t* frame)
{
    return ifx_error_get();
}
/*
==============================================================================
   7. MAIN METHOD
==============================================================================
 */

int main(int argc, char* argv[])
{
    app_t s_recorder;
    recorder_t recorder_context;
    int exitcode;

    // function description
    static const char* app_description = "Radar Recorder";
    static const char* app_epilog = NULL;

    s_recorder.app_description = app_description;
    s_recorder.app_epilog = app_epilog;

    s_recorder.app_init = (void *)recorder_init;
    s_recorder.app_config = (void*)recorder_config;
    s_recorder.app_process = (void*)recorder_process;
    s_recorder.app_cleanup = (void*)recorder_cleanup;

    s_recorder.default_metrics = NULL;

    exitcode = app_start(argc,argv,&s_recorder, &recorder_context);

    return exitcode;
}
