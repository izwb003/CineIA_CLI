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

#ifndef COMMON_STREAM_SRC_UTILS_STREAMBASICTYPES_H_
#define COMMON_STREAM_SRC_UTILS_STREAMBASICTYPES_H_

#if defined(_MSC_VER) && _MSC_VER <= 1500

// Visual Studio 2008 and below doesn't support C99 headers
// 8 bit unsigned integer
typedef unsigned char uint8_t;
// 8 bit signed integer
typedef signed char int8_t;
// 16 bit unsigned integer
typedef unsigned short uint16_t; // NOLINT
// 16 bit signed integer
typedef short int16_t; // NOLINT
// 32 bit unsigned integer
typedef unsigned int uint32_t;
// 32 bit signed integer
typedef int int32_t;
// 64 bit unsigned integer
typedef unsigned long long uint64_t; // NOLINT
// 64 bit signed integer
typedef long long int64_t; // NOLINT

#define UINT8_MAX   0xff
#define UINT16_MAX  0xffff
#define UINT32_MAX  0xffffffff
#define UINT64_MAX  0xffffffffffffffff

#define INT8_MAX    0x7f
#define INT16_MAX   0x7fff
#define INT32_MAX   0x7fffffff
#define INT64_MAX   0x7fffffffffffffff

#define INT8_MIN    (-0x7f - 1)
#define INT16_MIN   (-0x7fff - 1)
#define INT32_MIN   (-0x7fffffff - 1)
#define INT64_MIN   (-0x7fffffffffffffff - 1)

#else
#include <stdint.h>
#endif

#include <stddef.h> // contains additional types like size_t etc. & macro NULL

#ifdef USE_MDA_NULLPTR
/* support lack of nullptr on older C++ compilers such as g++-4.4.7 */
#include "nullptrOldCompilers.h"
#endif

#endif  // COMMON_STREAM_SRC_UTILS_STREAMBASICTYPES_H_
