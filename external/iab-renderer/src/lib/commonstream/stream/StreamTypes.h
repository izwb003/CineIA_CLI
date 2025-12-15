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

#ifndef COMMON_STREAM_SRC_STREAM_STREAMTYPES_H_
#define COMMON_STREAM_SRC_STREAM_STREAMTYPES_H_

#include <map>
#include <exception>

#include "commonstream/utils/Namespace.h"
#include "StreamBasicTypes.h"

#ifdef USE_MDA_NULLPTR
/* support lack of nullptr on older C++ compilers such as g++-4.4.7 */
#include "nullptrOldCompilers.h"
#endif

namespace CommonStream
{

    /******************************************************************************
     *
     * VarUInt32
     *
     *****************************************************************************/

    template<int I>
    struct VarUInt32
    {

        uint32_t	value_;

        operator uint32_t()
        {
            return value_;
        }

        uint32_t operator=(uint32_t iVal)
        {
            value_ = iVal;
            return iVal;
        }

    };

    /******************************************************************************
     *
     * PackedInteger
     *
     *****************************************************************************/

    struct PackedUInt32
    {

        uint32_t	value_;

        uint32_t operator=(uint32_t iVal)
        {
            value_ = iVal;
            return iVal;
        }

        operator uint32_t()
        {
            return value_;
        }

    };

    /******************************************************************************
     *
     * PackedUInt64
     *
     *****************************************************************************/

    struct PackedUInt64
    {

        uint64_t	value_;

        uint64_t operator=(uint64_t iVal)
        {
            value_ = iVal;
            return iVal;
        }

        operator uint64_t()
        {
            return value_;
        }

    };

    /******************************************************************************
     *
     * PackedLength io
     *
     *****************************************************************************/

    struct PackedLength
    {
        uint32_t	length_;
    };

    template<class T>
    class OptionalParameter
    {

    public:

        OptionalParameter() : present_(false)
        {
        }

        void setValue(const T &iVal)
        {
            value_ = iVal;
            present_ = true;
        }

        T& getValue()
        {
            return value_;
        }

        const T& getValue() const
        {
            return value_;
        }

        bool getPresent() const
        {
            return present_;
        }

        void setPresent(bool iPresent = true)
        {
            present_ = iPresent;
        }

        operator uint32_t()
        {
            return (uint32_t) value_;
        }

        uint32_t operator=(uint32_t iVal)
        {

	// The assignment of value_ = iVal causes
	// a warning for conversion due to VarUInt32.
	//
	// There is no simple way to properly implement casting to
	// remove the warning.
            value_ = iVal;
            
            setPresent();
            return iVal;
        }

        T operator=(T iVal)
        {
            value_ = iVal;
            setPresent();
            return iVal;
        }
    private:
        bool	present_;
        T		value_;
    };

    template<uint32_t N>
    class Plex
    {

    public:

        Plex(uint32_t n)
        {
            value_ = n;
        }

        Plex()
        {
            value_ = 0;
        }

        operator uint32_t() const
        {
            return value_;
        }

        uint32_t operator=(uint32_t iVal)
        {
            value_ = iVal;
            return iVal;
        }

    private:
        uint32_t value_;
    };

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_STREAM_STREAMTYPES_H_
