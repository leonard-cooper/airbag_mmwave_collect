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


class ICommandsLtr11
{
public:
    using WordType = uint16_t;

    constexpr static unsigned int valueWidth    = 16;
    constexpr static unsigned int adressWidth   = 7;
    constexpr static unsigned int addressOffset = 1;

    constexpr static uint16_t burstPrefix = 0xFF00;
    constexpr static uint16_t writeBit    = (1u << 0);

    struct Command
    {
        constexpr operator const WordType &() const
        {
            return m_command;
        }

        inline void increment(uint16_t count)
        {
            m_command += (count << addressOffset);
        }
        Command() = delete;

    protected:
        constexpr Command(WordType command) :
            m_command {command}
        {}

    private:
        WordType m_command;
    };
    static_assert(sizeof(Command) == sizeof(WordType), "Memory layout of Command contains additional stuff");

    struct Read :
        public Command
    {
        constexpr Read(uint8_t address) :
            Command(burstPrefix | (address << addressOffset))
        {}
    };

    struct Write :
        public Command
    {
        constexpr Write(uint8_t address) :
            Command(burstPrefix | (address << addressOffset) | writeBit)
        {}
    };


    virtual void executeWrite(const Write &command, uint16_t count, uint16_t values[]) = 0;
    virtual void executeRead(const Read &command, uint16_t count, uint16_t values[])   = 0;

    template <std::size_t N>
    inline void executeWrite(const Write &command, const uint16_t (&values)[N])
    {
        executeWrite(command, N, values);
    }

    inline void executeWrite(const Write &command, uint16_t value)
    {
        executeWrite(command, 1, &value);
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
