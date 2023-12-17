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
 * @file Shield.h
 *
 * \brief \copybrief gr_shield
 *
 * For details refer to \ref gr_shield
 */

#ifndef IFX_RADAR_SHIELD_H
#define IFX_RADAR_SHIELD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** @defgroup gr_shield Shield
  * @brief Defines the Enumeration for the presence of the available Shield.
  *
  * @{
  */
typedef enum
{
    IFX_SHIELD_MISSING = 0x0000,             /**< Board not connected */
    IFX_SHIELD_UNKNOWN = 0x0001,             /**< Unknown shield */
    IFX_SHIELD_RBB_MCU7 = 0x0100,            /**< Radar Baseboard MCU 7 */
    IFX_SHIELD_BGT60TR13AIP = 0x0200,        /**< Shield ID for BGT60TR13AIP */
    IFX_SHIELD_BGT60ATR24AIP = 0x0201,       /**< Shield ID for BGT60ATR24AIP */
    IFX_SHIELD_BGT60UTR11 = 0x0202,          /**< Shield ID for BGT60UTR11 */
    IFX_SHIELD_BGT60UTR13D = 0x0203,         /**< Shield ID for BGT60UTR13D */
    IFX_SHIELD_BGT60LTR11 = 0x0300,          /**< Shield ID for BGT60LTR11 */
    IFX_SHIELD_BGT60LTR11_MONOSTAT = 0x0301, /**< Shield for BGT60LTR11 */
    IFX_SHIELD_BGT60LTR11_B11 = 0x302,       /**< Shield for BGT60LTR11_B11 */
    IFX_SHIELD_BGT24ATR22_ES = 0x400,        /**< Shield for BGT24ATR22 ENGINEERING SAMPLE */
    IFX_SHIELD_BGT24ATR22_PROD = 0x401,      /**< Shield for BGT24ATR22 PRODUCTION SAMPLE */
    IFX_SHIELD_ANY = 0xFFFF                  /**< Any shield */
} ifx_Device_Shield_Type_t;

/**
  * @}
  */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_SHIELD_H */
