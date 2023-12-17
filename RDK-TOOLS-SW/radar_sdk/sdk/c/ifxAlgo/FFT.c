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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <kiss_fft.h>

#include "ifxAlgo/FFT.h"

#include "ifxBase/Complex.h"
#include "ifxBase/internal/Macros.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Vector.h"
#include "ifxBase/Error.h"
#include "ifxBase/Math.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

#define FFT_MAX_SIZE    (65536U)

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/**
 * @brief Defines the structure for FFT module.
 *        Use type ifx_FFT_t for this struct.
 */
struct ifx_FFT_s
{
    uint32_t            fft_size;             /**< FFT Size, must be power of 2 and not greater than \see FFT_MAX_SIZE.*/
    ifx_FFT_Type_t      fft_type;             /**< FFT type defined by \see ifx_FFT_Type_t.*/
    kiss_fft_cpx*       zero_pad_fft_input_c; /**< Container to store complex zero padded FFT input
                                                   in case fft_type is \see IFX_FFT_TYPE_C2C. Otherwise ignored.*/
    kiss_fft_cpx*       fft_output_c;         /**< Container to store complex input FFT with half output use case.*/
    kiss_fft_cfg        fft_plan;             /**< FFT plan.*/
};


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
static void fill_negative_half(ifx_FFT_Handle_t handle,
                    uint32_t output_size);

static void copy_to_ifx_output(ifx_FFT_Handle_t handle,
                    ifx_Vector_C_t* output);

static void zero_internal_input_till_end(ifx_FFT_Handle_t handle, uint32_t start);

static void copy_to_internal_input_rc(ifx_FFT_Handle_t handle,
                    const ifx_Vector_R_t* input);

static void copy_to_internal_input_c(ifx_FFT_Handle_t handle,
                    const ifx_Vector_C_t* input);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

void ifx_fft_shift_r(const ifx_Vector_R_t* input,
    ifx_Vector_R_t* output)
{
    IFX_ERR_BRK_NULL(input);
    IFX_ERR_BRK_NULL(output);

    ifx_vec_copyshift_r(input, vLen(input) / 2, output);
}

//----------------------------------------------------------------------------

void ifx_fft_shift_c(const ifx_Vector_C_t* input,
    ifx_Vector_C_t* output)
{
    IFX_ERR_BRK_NULL(input);
    IFX_ERR_BRK_NULL(output);

    ifx_vec_copyshift_c(input, vLen(input) / 2, output);
}

//----------------------------------------------------------------------------

static void fill_negative_half(ifx_FFT_Handle_t handle,
                    uint32_t output_size)
{

    kiss_fft_cpx* output_ptr = handle->fft_output_c;
    if (output_size >= handle->fft_size) // Needs to fill negative half
    {
        for (uint32_t i = (handle->fft_size / 2 + 1); i < handle->fft_size; i++)
        {
            int i_pos =  handle->fft_size - i;
            ifx_Complex_t cpx_in = IFX_COMPLEX_DEF(output_ptr[i_pos].r, output_ptr[i_pos].i);
            ifx_Complex_t cpx_out = ifx_complex_conj(cpx_in);
            output_ptr[i].r = cpx_out.data[0];
            output_ptr[i].i = cpx_out.data[1];
        }
    }
}

//----------------------------------------------------------------------------

static void copy_to_ifx_output(ifx_FFT_Handle_t handle,
                    ifx_Vector_C_t* output)
{
    uint32_t min_size= MIN(vLen(output), handle->fft_size);
    kiss_fft_cpx* output_ptr = handle->fft_output_c;
    uint32_t idx = 0;
    
    for(; idx < min_size; ++idx)
    {
        kiss_fft_cpx kcpx = output_ptr[idx];
        ifx_Complex_t cpx = {.data={kcpx.r, kcpx.i}};
        vAt(output, idx) = cpx;
    }
    for(; idx < vLen(output); ++idx)
    {
        vAt(output, idx) = ifx_complex_zero;
    }
}

//----------------------------------------------------------------------------

static void zero_internal_input_till_end(ifx_FFT_Handle_t handle, uint32_t start)
{
   kiss_fft_cpx* input_ptr = handle->zero_pad_fft_input_c;
   for(uint32_t i_input = start; i_input < handle->fft_size; i_input++){
        input_ptr[i_input].r = 0;
        input_ptr[i_input].i = 0;          
    }
}

//----------------------------------------------------------------------------

static void copy_to_internal_input_rc(ifx_FFT_Handle_t handle,
                    const ifx_Vector_R_t* input)
{
    kiss_fft_cpx* input_ptr = handle->zero_pad_fft_input_c;
    
    uint32_t input_cpy_size = MIN(handle->fft_size, vLen(input));
    uint32_t i_input = 0;
    for(; i_input < input_cpy_size; i_input++ ){
        ifx_Float_t r_val = vAt(input, i_input);
        input_ptr[i_input].r  = r_val;
        input_ptr[i_input].i = 0;
    }
    zero_internal_input_till_end(handle, i_input);
}

//----------------------------------------------------------------------------

static void copy_to_internal_input_c(ifx_FFT_Handle_t handle,
                    const ifx_Vector_C_t* input)
{
    kiss_fft_cpx* input_ptr = handle->zero_pad_fft_input_c; 

    uint32_t input_cpy_size = MIN(handle->fft_size, vLen(input));
    uint32_t i_input = 0;
    for(; i_input < input_cpy_size; i_input++ ){
        ifx_Complex_t cpx = vAt(input, i_input);
        input_ptr[i_input].r = cpx.data[0];
        input_ptr[i_input].i = cpx.data[1];
    }
    zero_internal_input_till_end(handle, i_input);
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_FFT_Handle_t ifx_fft_create(const ifx_FFT_Type_t fft_type,
                                const uint32_t fft_size)
{
    IFX_ERR_BRN_ARGUMENT((fft_type != IFX_FFT_TYPE_R2C) && (fft_type != IFX_FFT_TYPE_C2C));

    int fft_size_error = !ifx_math_ispower_of_2(fft_size);
    IFX_ERR_BRN_ARGUMENT(fft_size_error || (fft_size > FFT_MAX_SIZE));

    //------------------------- handle creation ------------------------------

    ifx_FFT_Handle_t h = ifx_mem_calloc(1, sizeof(struct ifx_FFT_s));
    IFX_ERR_BRN_MEMALLOC(h);

    h->fft_size = fft_size;
    h->fft_type = fft_type;

    //--------------------------- fftw plan creation -------------------------

    IFX_ERR_HANDLE_N(h->fft_output_c =  ifx_mem_alloc(fft_size*sizeof(kiss_fft_cpx)),
                     ifx_fft_destroy(h));

    const int is_inversed_fft = 0;    
    IFX_ERR_HANDLE_N(h->fft_plan = kiss_fft_alloc(fft_size, is_inversed_fft, NULL, NULL), 
                        ifx_fft_destroy(h));

    IFX_ERR_HANDLE_N(h->zero_pad_fft_input_c = ifx_mem_alloc(fft_size*sizeof(kiss_fft_cpx)),
                        ifx_fft_destroy(h));


    return h;
}

//----------------------------------------------------------------------------

void ifx_fft_destroy(ifx_FFT_Handle_t handle)
{
    if (handle == NULL)
    {
        return;
    }
    if(handle->fft_plan)
    {
        kiss_fft_free(handle->fft_plan);
    }

    ifx_mem_free(handle->zero_pad_fft_input_c);
    ifx_mem_free(handle->fft_output_c);

    ifx_mem_free(handle);
}

//----------------------------------------------------------------------------

void ifx_fft_run_rc(ifx_FFT_Handle_t handle,
                    const ifx_Vector_R_t* input,
                    ifx_Vector_C_t* output)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(input);
    IFX_ERR_BRK_NULL(output);
    IFX_VEC_BRK_MINSIZE(output, handle->fft_size / 2); // Half spectrum output supported
    IFX_ERR_BRK_COND(handle->fft_type != IFX_FFT_TYPE_R2C, IFX_ERROR_ARGUMENT_INVALID_EXPECTED_REAL);
    
    copy_to_internal_input_rc(handle, input);
    
    kiss_fft_cpx* output_ptr = handle->fft_output_c;
    kiss_fft_cpx* input_ptr = handle->zero_pad_fft_input_c;
    kiss_fft(handle->fft_plan, input_ptr, output_ptr);

    fill_negative_half(handle, vLen(output));
    copy_to_ifx_output(handle, output);
}

//----------------------------------------------------------------------------

void ifx_fft_run_c(ifx_FFT_Handle_t handle,
                   const ifx_Vector_C_t* input,
                   ifx_Vector_C_t* output)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(input);
    IFX_ERR_BRK_NULL(output);
    IFX_VEC_BRK_MINSIZE(output, handle->fft_size / 2); // Half spectrum output supported
    IFX_ERR_BRK_COND(handle->fft_type != IFX_FFT_TYPE_C2C, IFX_ERROR_ARGUMENT_INVALID_EXPECTED_REAL);

    kiss_fft_cpx* output_ptr = handle->fft_output_c;
    if(input->len >= handle->fft_size)
    {
        union {
            ifx_Complex_t* external_input;
            kiss_fft_cpx* internal_input;
        } view_internal_input = {.external_input = input->d};
        kiss_fft_cpx* input_ptr = view_internal_input.internal_input;

        kiss_fft(handle->fft_plan, input_ptr, output_ptr);
        
        union {
            kiss_fft_cpx* internal_output;
            ifx_Complex_t* external_output;
        } view_output = {.internal_output = output_ptr};
        output->d = view_output.external_output;
    }
    else
    {
        copy_to_internal_input_c(handle, input);
        kiss_fft_cpx* input_ptr = handle->zero_pad_fft_input_c;
        
        kiss_fft(handle->fft_plan, input_ptr, output_ptr);

        copy_to_ifx_output(handle, output);
    }
}

//----------------------------------------------------------------------------

uint32_t ifx_fft_get_fft_size(const ifx_FFT_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 0);

    return handle->fft_size;
}

//----------------------------------------------------------------------------

ifx_FFT_Type_t ifx_fft_get_fft_type(const ifx_FFT_Handle_t handle)
{
    IFX_ERR_BRV_NULL(handle, 255U);

    return handle->fft_type;
}
