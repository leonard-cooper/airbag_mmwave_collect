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


class ICommandsAvian
{
public:
    using WordType = uint32_t;

    constexpr static unsigned int valueWidth    = 24;
    constexpr static unsigned int addressOffset = valueWidth + 1;
    constexpr static WordType writeBit          = (1u << valueWidth);
    constexpr static WordType valueMask         = ((1u << valueWidth) - 1);

    struct Command
    {
        constexpr operator const WordType &() const
        {
            return m_command;
        }

        Command() = delete;

    protected:
        constexpr Command(WordType command) :
            m_command {command}
        {}

    private:
        const WordType m_command;
    };
    static_assert(sizeof(Command) == sizeof(WordType), "Memory layout of Command contains additional stuff");


    struct Read :
        public Command
    {
        Read() = delete;

        constexpr Read(uint8_t address) :
            Command(address << addressOffset)
        {}
    };

    struct Write :
        public Command
    {
        constexpr Write(uint8_t address, uint32_t value) :
            Command((address << addressOffset) | writeBit | (value & valueMask))
        {}
    };


    virtual void execute(const Command commands[], uint32_t count, uint32_t results[] = nullptr) = 0;
    virtual void setBits(const Command commandMask)                                              = 0;

    template <std::size_t N>
    inline void execute(const Command (&commands)[N], uint32_t (&results)[N])
    {
        execute(commands, N, results);
    }
    template <std::size_t N>
    inline void execute(const Command (&commands)[N])
    {
        execute(commands, N);
    }
    inline void execute(const Write &command)
    {
        execute(&command, 1);
    }
    inline uint32_t execute(const Read &command)
    {
        uint32_t result;
        execute(&command, 1, &result);
        return result;
    }
};
