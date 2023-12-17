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
 * @file Frame.h
 *
 * \brief \copybrief gr_frame
 *
 * For details refer to \ref gr_frame
 */

#ifndef IFX_RADAR_FRAME_H
#define IFX_RADAR_FRAME_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdbool.h>

#include "ifxBase/Types.h"
#include "ifxBase/Matrix.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * @brief Defines the structure for a complete real frame of time domain data.
 *
 * When time domain data is acquired by a radar sensor device, it is copied into an instance of
 * this structure. The structure contains one matrix for each virtual active Rx antenna.
 * 
 * The matrices *rx_data* have dimension num_chirps_per_frame x num_samples_per_chirp. Rows correspond
 * to chirps, columns correspond to samples.
 *
 * <b>Virtual Rx antenna ordering</b>
 *
 * The virtual Rx antennas are ordered by [Tx][Rx] according to the number Tx and Rx antennas which are active.
 *
 * For example, assuming all 4 Rx antennas and MIMO (2 Tx antennas) are activated:
 *
 *    Virtual antenna index 0: Tx1, Rx1
 *    Virtual antenna index 1: Tx1, Rx2
 *    Virtual antenna index 2: Tx1, Rx3
 *    Virtual antenna index 3: Tx1, Rx4
 *    Virtual antenna index 4: Tx2, Rx1
 *    Virtual antenna index 5: Tx2, Rx2
 *    Virtual antenna index 6: Tx2, Rx3
 *    Virtual antenna index 7: Tx2, Rx4
 * 
 */
typedef struct
{
    uint8_t          num_rx;   /**< The number of Rx matrices in this instance. This corresponds to
                                    the number of enabled Rx antennas if MIMO is disabled. In case
                                    of time-domain multiplexed MIMO (TDM MIMO), num_rx equals to
                                    the number of activated Rx antennas multiplied by 2. */
    ifx_Matrix_R_t** rx_data;  /**< This is an array of real data matrices. It contains num_rx elements.
                                    Each matrix has the dimension num_chirps_per_frame x num_samples_per_chirp. */
} ifx_Frame_R_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_Radar_Device
  * @{
  */

/** @defgroup gr_frame Frame
  * @brief API for Measurement Frame handling
  *
  * Supports operations such as creation and destruction of frames.
  *
  * @{
  */

/**
 * @brief Creates a frame structure for time domain real data acquisition
 *
 * This function initializes a data structure (of type \ref ifx_Frame_R_t) that can hold a time domain
 * data frame according to the dimensions provided as parameters. These parameters can be obtained for
 * instance by parsing through a pre-recorded file. If a device is connected then the
 * \ref ifx_device_create_frame_from_device_handle should be used instead of this function, as that function
 * reads the dimensions from configured the device handle.
 *
 * @param [in]     num_antennas                  Number of Rx antennas available in the device
 * @param [in]     num_chirps_per_frame          Number of chirps configured in a frame
 * @param [in]     num_samples_per_chirp         Number of samples per chirp
 *
 * @return Pointer to allocated and initialized frame structure \ref ifx_Frame_R_t or NULL if allocation failed.
 */
IFX_DLL_PUBLIC
ifx_Frame_R_t* ifx_frame_create_r(uint8_t num_antennas,
                                  uint32_t num_chirps_per_frame,
                                  uint32_t num_samples_per_chirp);

/**
 * @brief Frees the memory allocated for the real frame structure.
 *
 * This function frees the memory associated with the provided frame structure and un-initializes
 * it. This function must be called when the frame is not used any longer to avoid memory leaks. *frame*
 * must not be used any more after this function was called.
 *
 * This function frees the memory for the frame handle and the internal array of matrices.
 *
 * @param [in]     frame     The real frame structure to be deallocated.
 */
IFX_DLL_PUBLIC
void ifx_frame_destroy_r(ifx_Frame_R_t* frame);

/**
 * @brief Gets real matrix from antenna.
 *
 * @param [in]     frame     Pointer to time domain data real frame structure.
 * @param [in]     antenna   Rx antenna.
 * @return Pointer to real matrix containing time domain data corresponding to the specified Rx antenna.
 */
IFX_DLL_PUBLIC
ifx_Matrix_R_t* ifx_frame_get_mat_from_antenna_r(ifx_Frame_R_t* frame, 
                                                 uint8_t antenna);

/**
  * @}
  */

/**
  * @}
  */
 
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_FRAME_H */
