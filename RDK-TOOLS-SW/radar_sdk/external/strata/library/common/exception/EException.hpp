/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <stdexcept>
#include <string>


/// @brief Parent class for all exceptions.
class EException :
    public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return m_what.c_str();
    }

    virtual int code() const noexcept
    {
        return m_code;
    }

protected:
    EException() = default;

    mutable std::string m_what;
    mutable int m_code;
};
