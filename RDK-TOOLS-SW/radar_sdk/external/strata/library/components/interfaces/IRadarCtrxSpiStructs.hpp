#pragma once

#include <stdint.h>
#include <vector>

namespace CtrxSpiStructs
{
    struct SpiResult
    {
        uint32_t header;             ///< Header word of the SPI frame
        uint8_t spiMsgError;         ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;    ///< Command Error Code byte of the SPI frame
        std::vector<uint32_t> resp;  ///< The response data. These are only the pure response data words, not including the information like errors and command ID or other parts of the entire SPI frame.
        uint32_t crc32;              ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct ExecuteDirectlyFwCmdResult
    {
        uint32_t header;             ///< Header word of the SPI frame
        uint8_t spiMsgError;         ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;    ///< Command Error Code byte of the SPI frame
        uint8_t reserved;            ///< Reserved byte of the SPI frame
        uint8_t commandId;           ///< Command ID byte of the SPI frame
        std::vector<uint32_t> resp;  ///< The response data. These are only the pure response data words, not including the information like errors and command ID or other parts of the entire SPI frame.
        uint32_t crc32;              ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct AbortRampScenarioResult
    {
        uint32_t header;           ///< Header word of the SPI frame
        uint8_t spiMsgError;       ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;  ///< Command Error Code byte of the SPI frame
        uint16_t reserved1;        ///< Reserved two bytes of the SPI frame
        uint32_t reserved2;        ///< Reserved four bytes of the SPI frame
        uint32_t crc32;            ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct ConfigureHandleResult
    {
        uint32_t header;           ///< Header word of the SPI frame
        uint8_t spiMsgError;       ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;  ///< Command Error Code byte of the SPI frame
        uint8_t handle;            ///< Handle of the stored configuration
        uint8_t commandId;         ///< Command ID byte of the SPI frame
        uint32_t reserved;         ///< Reserved four bytes of the SPI frame
        uint32_t crc32;            ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct TriggerResetResult
    {
        uint32_t header;           ///< Header word of the SPI frame
        uint8_t spiMsgError;       ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;  ///< Command Error Code byte of the SPI frame
        uint8_t reserved1;         ///< Reserved byte of the SPI frame
        uint8_t lastReset;         ///< lastReset byte of the SPI frame
        uint32_t reserved2;        ///< Reserved four bytes of the SPI frame
        uint32_t crc32;            ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct ExecuteHandleResult
    {
        uint32_t header;             ///< Header word of the SPI frame
        uint8_t spiMsgError;         ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;    ///< Command Error Code byte of the SPI frame
        uint8_t handle;              ///< Handle of the stored configuration
        uint8_t commandId;           ///< Command ID byte of the SPI frame
        std::vector<uint32_t> resp;  ///< The response data. These are only the pure response data words, not including the information like errors and command ID or other parts of the entire SPI frame.
        uint32_t crc32;              ///< CRC32 (frame CRC) word of the SPI frame
    };

    struct NopResult
    {
        uint32_t header;                ///< Header word of the SPI frame
        uint8_t spiMsgError;            ///< SPI MSG Error byte of the SPI frame
        uint8_t commandErrorCode;       ///< Command Error Code byte of the SPI frame
        std::vector<uint32_t> resp;     ///< The response data. These are only the pure response data words, not including the information like errors and command ID or other parts of the entire SPI frame.
        uint32_t crc32;                 ///< CRC32 (frame CRC) word of the SPI frame
        std::vector<uint32_t> rawData;  ///< The raw data that was received via the NOP command. In case it could not be parsed into the single items the raw data will still be there.
    };
}
