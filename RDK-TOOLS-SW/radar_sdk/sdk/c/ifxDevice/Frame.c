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

#include <stdlib.h>
#include <string.h>

#include "ifxBase/Mem.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Error.h"

#include "ifxDevice/Frame.h"


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

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_Frame_R_t* ifx_frame_create_r(uint8_t num_antennas,
                                  uint32_t num_chirps_per_frame,
                                  uint32_t num_samples_per_chirp)
{
    ifx_Frame_R_t* f = NULL;

    IFX_ERR_BRN_ARGUMENT(num_antennas == 0);
    IFX_ERR_BRN_ARGUMENT(num_chirps_per_frame == 0);
    IFX_ERR_BRN_ARGUMENT(num_samples_per_chirp == 0);
    f = ifx_mem_calloc(1, sizeof(ifx_Frame_R_t));
    IFX_ERR_BRN_NULL(f);

    f->rx_data = ifx_mem_calloc(num_antennas, sizeof(ifx_Matrix_R_t));

    if (f->rx_data == NULL)
    {
        ifx_frame_destroy_r(f);
        ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
        return NULL;
    }


    f->num_rx = num_antennas;

    for (int i = 0; i < num_antennas; i++)
    {
        IFX_ERR_HANDLE_N(f->rx_data[i] =
                           ifx_mat_create_r(num_chirps_per_frame,
                                            num_samples_per_chirp),
                           ifx_frame_destroy_r(f));
    }

    return f;
}

//----------------------------------------------------------------------------

void ifx_frame_destroy_r(ifx_Frame_R_t* frame)
{
    if (frame == NULL)
    {
        return;
    }

    if(frame->rx_data != NULL)
    {
        for (uint8_t i = 0; i < frame->num_rx; i++)
        {
            ifx_mat_destroy_r(frame->rx_data[i]);
        }

        ifx_mem_free(frame->rx_data);
    }

    frame->rx_data = NULL;
    frame->num_rx = 0;
    ifx_mem_free(frame);
}

//----------------------------------------------------------------------------

ifx_Matrix_R_t* ifx_frame_get_mat_from_antenna_r(ifx_Frame_R_t* frame,
                                                 uint8_t antenna)
{
   IFX_ERR_BRN_NULL(frame);
   IFX_ERR_BRN_ARGUMENT(antenna >= frame->num_rx);
   return frame->rx_data[antenna];
}
