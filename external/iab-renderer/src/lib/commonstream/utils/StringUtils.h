/*======================================================================*
    Copyright (c) 2015-2023 DTS, Inc. and its affiliates.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *======================================================================*/

#ifndef COMMON_STREAM_SRC_UTILS_STRINGUTILS_H_
#define COMMON_STREAM_SRC_UTILS_STRINGUTILS_H_

#include <iostream>
#include <string>

#include "commonstream/utils/Namespace.h"

namespace CommonStream
{

    namespace StringUtils
    {
        class UTF8
        {
        public:

            static std::string ws2utf8 (const std::wstring &wstr);
            static std::wstring utf82ws (const std::string &str);
            inline static unsigned int utf82uc(const std::string &str, unsigned int stridx, unsigned int &charcnt);

        private:

            static std::string uctail(unsigned int cnt, unsigned int uc);
            static std::string uc2utf8(unsigned int uc);
            static unsigned int nbts(unsigned char s);
            static unsigned int utf82uc(std::string str);
        };

        /**
         *
         * UTF8 Conversion Utitilities
         *
         * When conversion fails, an empty string is returned.
         *
         */

        std::string wtoutf8 (const std::wstring &wstr);
        std::wstring utf8tow(const std::string &str);

    }  // namespace StringUtils

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_UTILS_STRINGUTILS_H_
