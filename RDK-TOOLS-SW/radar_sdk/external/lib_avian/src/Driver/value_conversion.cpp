/**
    @file value_conversion.cpp

    This file is part of the BGT60TRxx driver.

    This file contains functions for all kinds of value conversions needed
    by BGT60TRxx driver.
*/
/* ===========================================================================
** Copyright (C) 2017 - 2020 Infineon Technologies AG
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
#include <cmath>

// ---------------------------------------------------------------------------- namespaces
namespace Infineon
{
    namespace Avian
    {

// ---------------------------------------------------------------------------- divide64
uint64_t Driver::divide64(uint64_t* dividend, uint64_t divisor)
{
    uint64_t quotient = 0;
    uint64_t bitmask = 1;

    /*
     * The division is done bit by bit. To find the highest bit position of
     * the quotient, the divisor is shifted until it's larger than the
     * dividend. To avoid an overflow, the shifting also stops when bit 63 of
     * the divisor is set.
     */
    while ((divisor < *dividend) && ((divisor & 0x8000000000000000ull) == 0))
    {
        bitmask <<= 1;
        divisor *= 2;
    }

    /*
     * Now that the divisor is scaled accordingly, the division is done bit by
     * bit. This works like we all have learned it at school but with binary
     * digits rather than decimal digits. There should be no need to describe
     * this loop in detail.
     */
    while ((bitmask != 0) && (*dividend != 0))
    {
        if (*dividend >= divisor)
        {
            quotient += bitmask;
            *dividend -= divisor;
        }
        bitmask >>= 1;
        divisor /= 2;
    }
    return quotient;
}

// ---------------------------------------------------------------------------- convert_freq_to_reg
int32_t Driver::convert_freq_to_reg(uint32_t frequency_kHz)
{
    /*
     * Formula to calculate register value from frequency:
     * X =  (frequency_kHz * 1000 / (REFERENCE_OSC_FREQ_Hz * PLL_PRE_DIV) -
     *       (PLL_DIVSET + 2) * 4 + 8) * 2^20
     */
    double frequency = double(frequency_kHz) * 1000.0;
    double reference = double(m_reference_clock_freq_Hz) *
                       m_device_traits.pll_pre_divider;
    int32_t offset = int32_t(m_pll_div_set) * 4 + 16;

    return int32_t(round((frequency / reference - offset) * double(1l<<20)));
}

// ---------------------------------------------------------------------------- convert_reg_to_freq
uint32_t Driver::convert_reg_to_freq(int32_t register_value)
{
    /*
     * Formula to calculate frequency from register value according to data
     * sheet:
     * frequency_Hz = REFERENCE_OSC_FREQ_Hz * PLL_PRE_DIV *
     *                (PLL_DIVSET * 4 + 16 + X * 2^-20)
     *
     * The range is returned in kHz, so the result of the formula above must
     * be divided by 1000.
     */
    double reference = double(m_reference_clock_freq_Hz) *
                       m_device_traits.pll_pre_divider;
    int32_t offset = int32_t(m_pll_div_set) * 4 + 16;

    double frequency = (double(register_value) / double(1l<<20) + offset) *
                       reference;

    return uint32_t(round(frequency * 0.001));
}

// ---------------------------------------------------------------------------- count_antennas
uint8_t Driver::count_antennas(uint8_t rx_mask)
{
    uint8_t num_antennas = 0;

    while (rx_mask != 0)
    {
        if (rx_mask & 0x01)
        {
            ++num_antennas;
        }
        rx_mask >>= 1;
    }

    return num_antennas;
}

// ---------------------------------------------------------------------------- convert_cycles_to_time
uint64_t Driver::convert_cycles_to_time(uint64_t num_cycles,
                                        uint32_t reference_clock_freq_Hz)
{
    /*
     * Mathematically the conversion is just a division of num_cycles by the
     * reference clock frequency. The result must be multiplied by 10^10 to
     * scale it to 100ps steps.
     *
     * Due to integer arithmetic in such a case the scaling is usually be done
     * before the division. That order does not work in this because
     * the multiplication might overflow even with 64bit arithmetic. The
     * maximum number of cycles to be expected is 255 * 2^30 * 8 + 33
     * (= maximum shape end delay), which requires 41 bit. The constant 10^10
     * requires 34bit. Nevertheless the reference clock frequency can be
     * represented with 27 bit (which is sufficient for a frequency up to
     * 134.2MHz) and so the final result requires 48 bit and fits well into a
     * 64 bit variable.
     * Addendum: Actually the result can be a 49 bit, because the clock
     * frequency is a 27 bit number, but it's not 2^27!
     *
     * To do the calculation at full precision the constant 10^10 is split into
     * 2^10 * 5^10.
     */

    /* Step 1: Multiplication by 2^10 (41bit + 10bit = 51bit) */
    uint64_t dividend = num_cycles << 10;

    /* Step 2: Division by reference clock frequency (51bit - 27bit = 24bit) */
    uint64_t quotient = divide64(&dividend, reference_clock_freq_Hz);

    /* Step 3: Multiplication by 5^10 (24bit + 24bit = 48bit) */
    dividend *= 9765625;
    quotient *= 9765625;

    /* Step 4: Division of remainder */
    quotient += divide64(&dividend, reference_clock_freq_Hz);

    /* Step 5: Round to nearest */
    if ((dividend << 1) >= reference_clock_freq_Hz)
        ++quotient;

    return quotient;
}

// ---------------------------------------------------------------------------- convert_time_to_cycles
uint64_t Driver::convert_time_to_cycles(uint64_t period_100ps,
                                        uint32_t reference_clock_freq_Hz)
{
    /*
     * Mathematically the conversion is just a multiplication of period_100ps
     * by the reference clock frequency. The result must be divided by 10^10 to
     * compensate scaling to 100ps steps.
     *
     * The maximum period to be expected is a 49 bit value representing the
     * maximum shape end delay of 255 * 2^30 * 8 + 33 clock cycles, which is
     * between 7 and 8 hours. The clock_frequency is a 27bit number, so the
     * product does not fit into a 64bit variable.
     *
     * To do the calculation at full precision the division by th 34 bit
     * constant 10^10 is done first. The result is a 15bit quotient and a
     * 34 bit remainder. Both can be multiplied by the 27 bit clock frequency
     * without overflow. Finally the multiplied remainder is divided again to
     * get the final result.
     */
    const uint64_t divisor = 10000000000;
    uint64_t quotient = divide64(&period_100ps, divisor);

    period_100ps *= reference_clock_freq_Hz;
    quotient *= reference_clock_freq_Hz;

    quotient += divide64(&period_100ps, divisor);

    /* Finally the result is rounded to the nearest value. */
    if ((period_100ps * 2) >= 10000000000)
        ++quotient;

    return quotient;
}

// ---------------------------------------------------------------------------- convert_to_multiplied_counter
Driver::Mul_Counter Driver::convert_to_multiplied_counter(uint64_t clock_cycles,
                                                          uint8_t max_shift,
                                                          uint8_t max_count)
{
    uint8_t shift;
    uint64_t current_error = clock_cycles;
    Mul_Counter result;

    /*
     * If a multiplied timer is used (counter > 0) the minimum period is
     * 11 cycles. If the counter is not used (counter = 0) it's only 1 cycle.
     * Check for the special case counter = 0 and bypass the iteration below.
     */
    if (clock_cycles < 7)
    {
        result.counter = 0;
        result.shift_factor = 0;
        return result;
    }

    /* initialize with value, that will indicate an overflow */
    result.counter = 255;
    result.shift_factor = max_shift + 1;

    for (shift = 0; shift <= max_shift; ++shift)
    {
        Mul_Counter new_result;
        uint64_t new_error;
        uint64_t counter;

        /*
         * The relation between clock_cycles (x), counter (c) and
         * shift_factor (s) is x = c * 2^(s+3) + s + 3
         * Try with the current shift factor and round to nearest counter
         * value.
         */
        counter = (((clock_cycles - shift - 3) >> (shift + 2)) + 1) / 2;

        /* if counter does not fit into 8 bit, ship this iteration */
        if (counter > max_count)
        {
            continue;
        }

        /* calculate the new rounding error */
        new_result.counter = (uint8_t)counter;
        new_result.shift_factor = shift;

        new_error = convert_from_multiplied_counter(new_result);
        new_error = (new_error > clock_cycles) ? (new_error - clock_cycles)
                                               : (clock_cycles - new_error);

        /* if error becomes bigger again, the final result has been found */
        if (new_error > current_error)
        {
            return result;
        }
        result = new_result;
        current_error = new_error;
    }
    return result;
}

// ---------------------------------------------------------------------------- convert_from_multiplied_counter
uint64_t Driver::convert_from_multiplied_counter(Mul_Counter multi_counter)
{
    /*
     * If the counter value is 0 FSM skips the timer logic. This results in
     * only 1 cycle.
     */
    if (multi_counter.counter == 0)
        return 1;

    uint64_t clock_cycles;
    clock_cycles   = multi_counter.counter;
    clock_cycles <<= multi_counter.shift_factor;
    clock_cycles  *= 8;
    clock_cycles  += multi_counter.shift_factor;

    /* add additional offset cycles */
    clock_cycles += 3;

    return clock_cycles;
}

/* ------------------------------------------------------------------------ */
    } // namespace Avian
} // namespace Infineon

/* --- End of File -------------------------------------------------------- */
