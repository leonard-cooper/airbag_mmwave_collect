/****************************************************************************\
* Copyright (C) 2021 Infineon Technologies
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
\****************************************************************************/
#ifndef DATA_AVIAN_SETTINGS
#define DATA_AVIAN_SETTINGS 1


#define DATA_AVIAN_SETTINGS_FLAGS_AGGREGATION (1u << 2)  ///< enable frame slice aggreggation


#define DATA_AVIAN_SETTINGS_SIZE (4)

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>

typedef uint8_t DataAvianSettings_t[DATA_AVIAN_SETTINGS_SIZE];
#endif


static inline void DataAvianSettings_initialize(uint8_t (*settings)[DATA_AVIAN_SETTINGS_SIZE], uint8_t flags, uint8_t address, uint16_t size)
{
    (*settings)[0] = flags;
    (*settings)[1] = address;
    (*settings)[2] = size & 0xFF;
    (*settings)[3] = size >> 8;
}


#ifdef __cplusplus

struct DataAvianSettings_t
{
    DataAvianSettings_t(uint8_t flags, uint8_t address, uint16_t size)
    {
        initialize(flags, address, size);
    }

    inline void initialize(uint8_t flags, uint8_t address, uint16_t size)
    {
        DataAvianSettings_initialize(&settings, flags, address, size);
    }

    uint8_t settings[DATA_AVIAN_SETTINGS_SIZE];
};

#endif

#endif /* DATA_AVIAN_SETTINGS H */
