/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <cstdint>


/**
 * Unpack raw12 data from a uint8_t buffer to a uint16_t buffer.
 * The destination buffer has to be allocated for the unpacked data size of (length/3*2) elements!
 *
 * @param dstBuf receiving the unpacked data
 * @param srcBuf contains the raw12 data with a size of (length) bytes
 * @param length number of bytes to be unpacked
 */
inline void unpackRaw12(uint16_t *dstBuf, uint8_t *srcBuf, uint32_t length)
{
    for (uint_fast32_t i = 0; i < length; i += 3)
    {
        *(dstBuf++) = (srcBuf[i + 0] << 4) + (srcBuf[i + 2] & 0x0F);
        *(dstBuf++) = (srcBuf[i + 1] << 4) + (srcBuf[i + 2] >> 4);
    }
}

/**
 * Unpack raw12 data within a buffer.
 * The buffer has to be allocated for (count) elements!
 *
 * @param dstBuf contains the raw12 data with a size of (count/2*3) bytes
 * @param count number of elements (pixels) to be unpacked
 */
inline void unpackRaw12(uint16_t *dstBuf, uint32_t count)
{
    uint8_t *const srcBuf = reinterpret_cast<uint8_t *>(dstBuf);
    dstBuf += (count - 1);
    for (uint8_t *offset = srcBuf + (count / 2 * 3) - 1; offset > srcBuf;)
    {
        const uint8_t tmp = *(offset--);
        *(dstBuf--)       = (*(offset--) << 4) + (tmp >> 4);
        *(dstBuf--)       = (*(offset--) << 4) + (tmp & 0x0F);
    }
}
