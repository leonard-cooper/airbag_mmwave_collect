#pragma once

struct DataAvianSettings
{
    DataAvianSettings(uint8_t flags, uint8_t address, uint16_t size)
    {
        settings[0] = flags;
        settings[1] = address;
        settings[2] = size & 0xFF;
        settings[3] = size >> 8;
    }

    uint8_t settings[4];
};
