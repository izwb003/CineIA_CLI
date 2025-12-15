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

#include "commonstream/bitstream/BitStreamReaderT.h"

#include <assert.h>
#include <iostream>
#include <locale>
#include <cstring>
#include <algorithm>
#include <limits>

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

#include "commonstream/rawstream/RawIStream.h"

namespace CommonStream
{

    template<>
    BitStreamReaderT<std::ifstream>::BitStreamReaderT(std::ifstream &iStream)
    {
        stream_ = &iStream;

        bitBufferFill_ = 0;
        bitBuffer_ = 0;
        bitCount_ = 0;
        bufferLengthInBits_ = std::numeric_limits<BitCount_t>::max();

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    BitStreamReaderT<std::ifstream>::~BitStreamReaderT()
    {
    }

    template<>
    BitStreamReaderT<std::istream>::BitStreamReaderT(std::istream &iStream)
    {
        stream_ = &iStream;

        bitBufferFill_ = 0;
        bitBuffer_ = 0;
        bitCount_ = 0;
        bufferLengthInBits_ = std::numeric_limits<BitCount_t>::max();

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    BitStreamReaderT<std::istream>::~BitStreamReaderT()
    {
    }

    template<>
    ReturnCode BitStreamReaderT<RawIStream>::Init(const char *iBuffer, BitCount_t iBufferLength)
    {
        if ( !iBuffer || !iBufferLength )
        {
            return error_ = CMNSTRM_PARAMS_BAD;
        }
        Init();
        stream_ = new(std::nothrow) RawIStream;
        if ( !stream_ )
        {
            return error_ = CMNSTRM_IO_FAIL;
        }
        ReturnCode rc = stream_->Init(iBuffer, iBufferLength);
        if ( rc )
        {
            return error_ = rc;
        }
        bufferLengthInBits_ = static_cast<BitCount_t>(iBufferLength * CHAR_BIT);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return error_ = CMNSTRM_OK;
    }

    template<>
    ReturnCode BitStreamReaderT<RawIStream>::Init(const unsigned char *iBuffer, BitCount_t iBufferLength)
    {
        return Init(reinterpret_cast<const char *>(iBuffer), static_cast<BitCount_t>(iBufferLength));
    }

    template<>
    ReturnCode BitStreamReaderT<RawIStream>::Init(const BitStreamReaderT<RawIStream> &bs2)
    {
        if ( stream_ )
        {
            delete stream_;
        }
        stream_ = new(std::nothrow) RawIStream;
        if ( !stream_ )
        {
            return error_ = CMNSTRM_IO_FAIL;
        }
        ReturnCode rc = stream_->Init(*bs2.stream_);
        if ( rc )
        {
            delete stream_;
            stream_ = 0;
            return error_ = CMNSTRM_IO_FAIL;
        }
        BitStreamStateT<RawIStream> st = bs2.getState();
        setState(st);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return CMNSTRM_OK;
    }

    template<>
    BitStreamReaderT<RawIStream> &BitStreamReaderT<RawIStream>::operator=(const BitStreamReaderT<RawIStream> &bs2)
    {
        if ( &bs2 != this )
        {
#ifndef __clang_analyzer__
            if ( !bs2.stream_ )
            {
                // this should probably throw an exception
                delete stream_;
            }
            else
            {
                if ( stream_ )
                {
                    *stream_ = *bs2.stream_;
                }
                else
                {
                    stream_ = new RawIStream(*bs2.stream_);
                }
            }
            BitStreamStateT<RawIStream> state = bs2.getState();
            setState(state);
#endif // #ifndef __clang_analyzer__
        }
        return *this;
    }

    template<>
    BitStreamReaderT<RawIStream>::~BitStreamReaderT()
    {
        delete stream_;
    }

    template<>
    char *BitStreamReaderT<RawIStream>::buffer() const
    {
        if ( stream_ )
        {
            return stream_->buffer();
        }
        return 0;
    }


}  // namespace CommonStream
