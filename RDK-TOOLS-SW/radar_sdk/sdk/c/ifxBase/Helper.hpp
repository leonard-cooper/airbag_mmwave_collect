/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#ifndef IFX_BASE_HELPER_HPP
#define IFX_BASE_HELPER_HPP

#include "Types.h"
#include <vector>
#include <string>

namespace ifx
{
    namespace string
    {

        /**
        * @brief Split string 
        *
        * Return a vector of the words in the string, using delim as the delimiter
        * string.
        *
        * @param [in]  text    text string to split 
        * @param [in]  delim   delimiter string
        * @retval vector of splitted input text
        */

        IFX_DLL_HIDDEN std::vector<std::string> split(const std::string& text, const std::string& delim);

        /**
        * @brief Left strip string 
        *
        * Return a string without white spaces on the left side.
        *
        * @param [in]  text    text string to strip 
        * @retval left stripped input text
        */
       
        IFX_DLL_HIDDEN std::string lstrip(const std::string& text);

        /**
        * @brief Right strip string 
        *
        * Return a string without white spaces on the right side.
        *
        * @param [in]  text    text string to strip 
        * @retval right stripped input text
        */

        IFX_DLL_HIDDEN std::string rstrip(const std::string& text);

        /**
        * @brief Strip string 
        *
        * Return a string without white spaces on the left and right side.
        *
        * @param [in]  text    text string to strip 
        * @retval string without white spaces on begin or end
        */

        IFX_DLL_HIDDEN std::string strip(const std::string& text);

        /**
        * @brief Lowercase string 
        *
        * Return a string all in lowercase.
        *
        * @param [in]  text    text string to convert 
        * @retval string converted to lowercase
        */

        IFX_DLL_HIDDEN std::string lower(const std::string& text);

        /**
        * @brief Uppercase string 
        *
        * Return a string all in uppercase.
        *
        * @param [in]  text    text string to convert 
        * @retval string converted to uppercase
        */

        IFX_DLL_HIDDEN std::string upper(const std::string& text);

    }
}

#endif /* #ifndef IFX_BASE_HELPER_HPP */
