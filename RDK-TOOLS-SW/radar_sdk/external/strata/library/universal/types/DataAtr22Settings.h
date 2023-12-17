/****************************************************************************\
* Copyright (C) 2021 Infineon Technologies
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
\****************************************************************************/
#ifndef DATA_ATR22_SETTINGS
#define DATA_ATR22_SETTINGS 1


#define DATA_ATR22_SETTINGS_SIZE (4)

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>

typedef uint8_t DataAtr22Settings_t[DATA_ATR22_SETTINGS_SIZE];
#endif


static inline void DataAtr22Settings_initialize(uint8_t (*settings)[DATA_ATR22_SETTINGS_SIZE], uint8_t flags, uint8_t address, uint16_t size)
{
    (*settings)[0] = flags;
    (*settings)[1] = address;
    (*settings)[2] = size & 0xFF;
    (*settings)[3] = size >> 8;
}


#ifdef __cplusplus

struct DataAtr22Settings_t
{
    DataAtr22Settings_t(uint8_t flags, uint8_t address, uint16_t size)
    {
        initialize(flags, address, size);
    }

    inline void initialize(uint8_t flags, uint8_t address, uint16_t size)
    {
        DataAtr22Settings_initialize(&settings, flags, address, size);
    }

    uint8_t settings[DATA_ATR22_SETTINGS_SIZE];
};

#endif

#endif /* DATA_ATR22_SETTINGS H */
