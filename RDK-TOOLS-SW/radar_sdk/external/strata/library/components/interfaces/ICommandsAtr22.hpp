/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <cstddef>
#include <cstdint>


class ICommandsAtr22
{
public:
    using WriteType                             = uint8_t[4];
    using ReadType                              = uint16_t;
    constexpr static uint16_t commandBit        = (1u << 0);
    constexpr static uint16_t readBit           = (1u << 1);
    constexpr static unsigned int addressOffset = 2;

    struct Write
    {
        constexpr operator const WriteType &() const
        {
            return m_command;
        }

        Write() = delete;

        constexpr Write(uint16_t address, uint16_t value) :
            m_command {static_cast<uint8_t>(address >> (8 - addressOffset)), static_cast<uint8_t>((address << addressOffset) | commandBit), static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value & 0xFF)}
        {}

    private:
        const WriteType m_command;
    };
    static_assert(sizeof(Write) == sizeof(WriteType), "Memory layout of Write contains additional stuff");


    struct Read
    {
        constexpr operator const ReadType &() const
        {
            return m_command;
        }

        Read() = delete;

        constexpr Read(uint16_t address) :
            m_command {static_cast<ReadType>((address << addressOffset) | readBit | commandBit)}
        {}

        inline void increment(uint16_t count)
        {
            m_command += (count << addressOffset);
        }

    private:
        ReadType m_command;
    };
    static_assert(sizeof(Read) == sizeof(ReadType), "Memory layout of Read contains additional stuff");


    /**
     * Executes write commands on ATR22 device
     *
     * @param commands sequence of write commands
     * @param count number of write commands in the sequence
     */
    virtual void executeWrite(const Write commands[], uint16_t count) = 0;
    /**
     * Executes a read command on ATR22 device
     *
     * @param command initial read command
     * @param count number of values to be read in sequence
     * @return execution result readout values
     */
    virtual void executeRead(const Read &command, uint16_t count, uint16_t values[]) = 0;

    template <std::size_t N>
    inline void executeWrite(const Write (&commands)[N])
    {
        executeWrite(commands, N);
    }

    inline void executeWrite(const Write &command)
    {
        executeWrite(&command, 1);
    }
    template <std::size_t N>
    inline void executeRead(const Read &command, uint16_t (&values)[N])
    {
        executeRead(command, N, values);
    }

    inline uint16_t executeRead(const Read &command)
    {
        uint16_t value;
        executeRead(command, 1, &value);
        return value;
    }
};
