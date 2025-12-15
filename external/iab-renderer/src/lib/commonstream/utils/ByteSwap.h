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

#ifndef COMMON_STREAM_SRC_UTILS_BYTESWAP_H_
#define COMMON_STREAM_SRC_UTILS_BYTESWAP_H_

#include "commonstream/utils/Namespace.h"

namespace CommonStream
{

    namespace BYTESWAP
    {
        inline void swapByteOrder(uint16_t& us)
        {
            us = (us >> 8) |
                 (us << 8);
        }

        inline void swapByteOrder(uint32_t& ui)
        {
            ui = (ui >> 24) |
                 ((ui<<8) & 0x00FF0000) |
                 ((ui>>8) & 0x0000FF00) |
                 (ui << 24);
        }

        inline void swapByteOrder(uint64_t& ull)
        {
            ull = (ull >> 56) |
                  ((ull<<40) & 0x00FF000000000000) |
                  ((ull<<24) & 0x0000FF0000000000) |
                  ((ull<<8) & 0x000000FF00000000) |
                  ((ull>>8) & 0x00000000FF000000) |
                  ((ull>>24) & 0x0000000000FF0000) |
                  ((ull>>40) & 0x000000000000FF00) |
                  (ull << 56);
        }

    }  // namespace BYTESWAP

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_UTILS_BYTESWAP_H_
