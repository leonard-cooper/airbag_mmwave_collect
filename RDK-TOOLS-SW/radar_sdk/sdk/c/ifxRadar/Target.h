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
 * @file Target.h
 *
 * @brief Definitions used by Tracker algorithm.
 */

#ifndef IFX_RADAR_TARGET_H
#define IFX_RADAR_TARGET_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stddef.h>

#include "ifxBase/Types.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/** @addtogroup gr_cat_Radar
  * @{
  */

/** @defgroup gr_target Target
  * @brief Definitions used by Tracker algorithm.
  * @{
  */

#define IFX_OFFSET_OF(type, member)          offsetof(type, member)
#define IFX_TARGET_COL_OF(member)            (IFX_OFFSET_OF(ifx_Target_t, member) / sizeof(ifx_Float_t))

#define IFX_TARGET_RANGE_COL_OFFSET          IFX_TARGET_COL_OF(range)  
#define IFX_TARGET_RANGE(m, r)               IFX_MAT_AT(m, r, 0)
#define IFX_TARGET_SPEED_COL_OFFSET          IFX_TARGET_COL_OF(speed)  
#define IFX_TARGET_SPEED(m, r)               IFX_MAT_AT(m, r, IFX_TARGET_SPEED_COL_OFFSET)
#define IFX_TARGET_AZIMUTH_ANGLE_COL_OFFSET  IFX_TARGET_COL_OF(azimuth_angle)  
#define IFX_TARGET_AZIMUTH_ANGLE(m, r)       IFX_MAT_AT(m, r, IFX_TARGET_AZIMUTH_ANGLE_COL_OFFSET)
#define IFX_TARGET_COORDS_COL_OFFSET         IFX_TARGET_COL_OF(coords) 
#define IFX_TARGET_COORDS_LEN                3
#define IFX_TARGET_COORD(m, r, c)            IFX_MAT_AT(m, r, (c) + IFX_TARGET_COORDS_COL_OFFSET)
#define IFX_TARGET_MATRIX_CREATE(r)          ifx_mat_create_r(r, 6);

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * @brief Defines the structure for Target module.
 */
typedef struct
{
    ifx_Float_t range;                          /**< Range of the target.*/
    ifx_Float_t speed;                          /**< Speed of the target.*/
    ifx_Float_t azimuth_angle;                  /**< Azimuth angle of the target.*/
    ifx_Float_t coords[IFX_TARGET_COORDS_LEN];  /**< Cartesian coordinates of the target detections.*/
} ifx_Target_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/**
  * @}
  */
 
/**
  * @}
  */ 


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_TARGET_H */
