/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <Definitions.hpp>
#include <components/interfaces/IComponent.hpp>
#include <components/interfaces/IPinsCtrx.hpp>
#include <components/interfaces/IRadarCtrxSpiCommands.hpp>
#include <components/interfaces/IRadarCtrxSpiProtocol.hpp>
#include <components/interfaces/IRegisters.hpp>
#include <platform/interfaces/access/IMemory.hpp>
#include <universal/components/implementations/radar.h>
#include <vector>

/**
 * @brief IRadarCtrx class provides functionality for the CTRX component
 * @note loadPasskey should be called before interacting with memory and registers (e.g. loadPasskeys("my_passkeys.csv") )
 */
class IRadarCtrx : public IComponent
{
public:
    /**
    * @brief CTRX chip type including the design step
    */
    enum class ChipType : uint8_t
    {
        Altair_A = 0,  //!< A Ctrx Altair, design step A, Chip
        Altair_B = 1,  //!< A Ctrx Altair, design step B, Chip
        Altair_C = 2   //!< A Ctrx Altair, design step C, Chip
    };

    static constexpr uint8_t getType()
    {
        return COMPONENT_TYPE_RADAR;
    }
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_RADAR_CTRX;
    }
    STRATA_API virtual ~IRadarCtrx() = default;

    /**
     * @brief Retrieve the CTRX pins interface to access specific GPIO pins
     * @return The pins interface
     */
    virtual IPinsCtrx *getIPinsCtrx() = 0;

    /**
     * @brief Retrieve the CTRX SPI protocol interface
     * @return The SPI protocol interface
     */
    virtual ICtrxSpiProtocol *getICtrxSpiProtocol() = 0;

    /**
     * @brief Retrieve a memory interface for reading and writing to the memory
     * @param region The name of the region to access via this interface
     * @return The memory access interface
     */
    virtual IMemory<uint32_t> *getIMemory(const char *region) = 0;

    /**
     * @brief Load a file with passkeys for memory region access
     * @param path The path to the file containing the passkeys
     * @note Passkey file is subject to below format:
     *           Mode;Type;Region;Offset;Length;Passkey
     *           read;Memory;IRAM;0x100;10;0x12345678
     *           write;Memory;IRAM;;;0x87654321
     * @note Offset and length fields are optional, but if one of them is present the other should be also present
     * @note Empty offset and length fields means passkey is valid for the whole region
     */
    virtual void loadPasskeys(const char *path) = 0;

    ///
    /// @brief Set the maximum Spi Payload size
    ///
    /// @param radarChipType CTRX chip type used to determine the maximum payload size
    ///
    virtual void setMaxSpiPayload(ChipType radarChipType) = 0;

    /** ---------- SPI Commands  ---------- **/

    /**
     * @brief Retrieve the interface for executing SPI commands
     * @return The SPI commands interface
     */
    virtual ISpiCommandsCtrx *getISpiCommands() = 0;

    /** ---------- SPI FW Commands  ---------- **/

    /**
     * @brief Load Firmware program by writing instructions and/or data at one or more addresses in instruction and data RAM.
     * @note If the firmware program is larger than the allowed SPI payload size, it will be automatically split into several requests.
     * @note Throws an exception upon error
     * @param startAddress The address in memory where writing shall start
     * @param fwProgram The actual data to write
     * @param length The length of the data to write (number of 32bit words)
     */
    virtual void Download_FW_Functions(uint32_t startAddress, const uint32_t fwProgram[], uint32_t length) = 0;

    /**
     * @brief Read a set of registers. The passkey will be added automatically if configured.
     * @param[in] addresses Array of addresses to be read
     * @param[out] registerArray An array of address/value pairs
     * @param[in] count The number of values to read
     */
    virtual void Read_Register(const uint32_t addresses[], IRegisters<uint32_t>::BatchType registerArray[], uint32_t count) = 0;

    /**
     * @brief Write a set of registers. The passkey will be added automatically if configured.
     * @param[in] registerArray An array of address/value pairs to write
     * @param[in] count The number of values to write
     */
    virtual void Write_Register(const IRegisters<uint32_t>::BatchType registerArray[], uint32_t count) = 0;

    /**
     * @brief Writes multiple address/value pairs given as a 2D array. The passkey will be added automatically if configured.
     * @param[in] registerArray An array of address/value pairs to write
     * @param[in] count The number of values to write
     */
    STRATA_API inline void Write_Register(const uint32_t registerArray[][2], uint32_t count)
    {
        static_assert(sizeof(IRegisters<uint32_t>::BatchType) == sizeof(*registerArray), "BatchType cannot be reinterpreted as an array of 2!");
        Write_Register(reinterpret_cast<const IRegisters<uint32_t>::BatchType *>(registerArray), count);
    }
};
