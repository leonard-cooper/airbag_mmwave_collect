/**
 * \file ifxAvian_Types.hpp
 */
/* ===========================================================================
** Copyright (C) 2016 - 2021 Infineon Technologies AG
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

#ifndef IFX_AVIAN_TYPES_H
#define IFX_AVIAN_TYPES_H

// ---------------------------------------------------------------------------- namespaces
namespace Infineon
{
    namespace Avian
    {

// ---------------------------------------------------------------------------- Device_Type
//! This enumeration type lists all Avian device types supported by libAvian.
enum class Device_Type
{
    BGT60TR13C,
    BGT60ATR24C,
    BGT60UTR13D,
    BGT60TR12E,
    BGT60UTR11,
    BGT120UTR13E,
    BGT24LTR24,
    Unknown
};
    
// ---------------------------------------------------------------------------- Shape_Type
//! This enumeration type lists the FMCW shape types.
enum class Shape_Type
{
    Saw_Up,    //!< A sawtooth shape with a single up chirp.
    Saw_Down,  //!< A sawtooth shape with a single down chirp.
    Tri_Up,    //!< A triangle shape with an up chirp followed by a down chirp.
    Tri_Down   //!< A triangle shape with a down chirp followed by an up chirp.
};

// ---------------------------------------------------------------------------- Tx_Mode
//! This enumeration type lists all available TX antenna configurations.
enum class Tx_Mode
{
    Tx1_Only,     //!< Use TX 1 for all chirps.
    Tx2_Only,     //!< Use TX 2 for all chirps.
    Alternating,  /*!< Both TX antennas are used alternatingly. The first
                       chirp of a frame and all odd chirps use TX 1, while all
                       even chirps use TX 2. */
    Off           //!< No RF is transmitted during chirps.
};

// ---------------------------------------------------------------------------- Hp_Gain
/*!
 * This enumeration type lists the available gain settings of the Avian
 * device's integrated baseband high pass filters.
 */
enum class Hp_Gain
{
    _18dB,  //!< The gain of the high pass filter is +18dB.
    _30dB   //!< The gain of the high pass filter is +30dB.
};

// ---------------------------------------------------------------------------- Hp_Cutoff
/*!
 * This enumeration type lists the available cutoff frequency settings of the
 * Avian device's integrated baseband high pass filters.
 */
enum class Hp_Cutoff
{
    _20kHz,   //!< The cutoff frequency of the high pass filter is 20kHz.
    _45kHz,   //!< The cutoff frequency of the high pass filter is 45kHz.
    _70kHz,   //!< The cutoff frequency of the high pass filter is 70kHz.
    _80kHz,   //!< The cutoff frequency of the high pass filter is 80kHz.
    _160kHz,  //!< The cutoff frequency of the high pass filter is 160kHz.
};

// ---------------------------------------------------------------------------- Vga_Gain
/*!
 * This enumeration type lists the available gain settings of the Avian
 * device's integrated baseband amplifier (VGA).
 */
enum class Vga_Gain
{
    _0dB,   //!< The gain of the VGA is 0dB.
    _5dB,   //!< The gain of the VGA is +5dB.
    _10dB,  //!< The gain of the VGA is +10dB.
    _15dB,  //!< The gain of the VGA is +15dB.
    _20dB,  //!< The gain of the VGA is +20dB.
    _25dB,  //!< The gain of the VGA is +25dB.
    _30dB   //!< The gain of the VGA is +30dB.
};

// ---------------------------------------------------------------------------- Aaf_Cutoff
/*!
 * This enumeration type lists the available cutoff frequency settings of the
 * Avian device's integrated baseband anti alias filter.
 */
enum class Aaf_Cutoff
{
    _600kHz,  //!< The cutoff frequency of the anti alias filter is 600kHz.
    _1MHz     //!< The cutoff frequency of the anti alias filter is 1MMHz.
};

// ---------------------------------------------------------------------------- Adc_Sample_Time
/*!
 * This enumeration type lists the available ADC sample time settings.
 *
 * The sample time is the time that the sample-and-hold-circuitry of the Avian
 * device's Analog-Digital-Converter (ADC) takes to sample the voltage at it's
 * input.
 *
 * \note The specified timings refer to an external reference clock frequency
 *       of 80MHz. If the clock frequency differs from that, the sample time
 *       periods are scaled accordingly.
 */
enum class Adc_Sample_Time
{
    _50ns,   //!< The voltage is sampled for 50ns.
    _100ns,  //!< The voltage is sampled for 100ns.
    _200ns,  //!< The voltage is sampled for 200ns.
    _400ns   //!< The voltage is sampled for 400ns.
};

// ---------------------------------------------------------------------------- Adc_Tracking
/*!
 * This enumeration type lists the available ADC tracking modes.
 *
 * The Avian device's internal Analog-Digital-Converter (ADC) has 11 bit 
 * resolution and a 12 bit result register. A single conversion always produces
 * result values with unset LSB. The resolution can be increased by performing
 * additional tracking conversion. The result will be the average of all
 * conversions.
 */
enum class Adc_Tracking
{
    None,              //!< Each sampled voltage value is converted once.
    _1_Subconversion,  //!< Each sampled voltage value is converted two times.
    _3_Subconversions, //!< Each sampled voltage value is converted four times.
    _7_Subconversions  //!< Each sampled voltage value is converted eight times.
};

// ---------------------------------------------------------------------------- Adc_Oversampling
/*!
 * This enumeration type lists the available ADC oversampling modes.
 *
 * The Avian device's internal Analog-Digital-Converter (ADC) is capable to
 * repeat the full sample-hold-convert cycle multiple times and return the
 * average of all cycles.
 */
enum class Adc_Oversampling
{
    Off,  //!< No oversampling */
    _2x,  //!< Oversampling factor 2
    _4x,  //!< Oversampling factor 4
    _8x   //!< Oversampling factor 8
};

// ---------------------------------------------------------------------------- Power_Mode
/*!
 * This enumeration type lists the power modes that can be configured at the
 * end of a shape or frame.
 */
enum class Power_Mode
{
    Stay_Active,         //!< No power saving.
    Idle,                //!< Go to Idle Mode after last chirp.
    Deep_Sleep,          //!< Go to Deep Sleep Mode after last chirp.
    Deep_Sleep_Continue  /*!< Go to Deep Sleep Mode after last chirp, but keep
                              system clock enabled. */
};

// ---------------------------------------------------------------------------- Fifo_Power_Mode
/*!
 * This enumeration lists the power saving modes of an Avian device's built in
 * FIFO memory.
 */
enum class Fifo_Power_Mode
{
    Always_On,               /*!< FIFO is always enabled. */
    Deep_Sleep_Off,          /*!< FIFO is disabled when empty in Deep Sleep
                                  mode. */
    Deep_Sleep_And_Idle_Off  /*!< FIFO is disabled when empty in Deep Sleep
                                  and Idle mode. */
};

// ---------------------------------------------------------------------------- Pad_Driver_Mode
//! This enumeration lists the driver modes of the Avian device's pads.
enum class Pad_Driver_Mode
{
    Normal,
    Strong
};

// ---------------------------------------------------------------------------- Reference_Clock_Frequency
/*!
 * This enumeration lists the reference clock frequencies support by Avian
 * devices.
 */
enum class Reference_Clock_Frequency
{
    _80MHz,    //!< 80MHz
    _76_8MHz,  //!< 76.8MHz
    _40MHz,    //!< 40MHz
    _38_4MHz   //!< 38.4MHz
};

// ---------------------------------------------------------------------------- Reference_Clock_Frequency
/*!
 * This enumeration lists the duty cycle correction options of the reference
 * clock doubler.
 * 
 * Avian devices have two clock frequency doublers (if at all), one for the
 * system clock one for the PLL clock. Duty cycle correction is always applied
 * to the output of both doublers. For doubler input signals duty cycle
 * correction is optional.
 */
enum class Duty_Cycle_Correction_Mode
{
    Only_Out,  //!< Only output signals are corrected.
    In_Out,    //!< Input signals to both doublers are corrected.
    SysIn_Out  /*!< Only input to system clock doubler is corrected. The input
                    signal to the PLL clock doubler is not corrected. */
};

/* ------------------------------------------------------------------------ */
    } // namespace Avian
} // namespace Infineon

#endif /* IFX_AVIAN_TYPES_H */

/* --- End of File -------------------------------------------------------- */

