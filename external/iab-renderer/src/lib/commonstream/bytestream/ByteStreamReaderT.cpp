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

#include "commonstream/bytestream/ByteStreamReaderT.h"

#include <assert.h>
#include <iostream>
#include <locale>
#include <cstring>

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

#include "commonstream/rawstream/RawIStream.h"

namespace CommonStream
{

    template<>
    ByteStreamReaderT<std::ifstream>::ByteStreamReaderT(std::ifstream &iStream)
    {
        stream_ = &iStream;
        byteCount_ = 0;

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    ByteStreamReaderT<std::ifstream>::~ByteStreamReaderT()
    {
    }

    template<>
    ByteStreamReaderT<std::istream>::ByteStreamReaderT(std::istream &iStream)
    {
        stream_ = &iStream;
        byteCount_ = 0;

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    ByteStreamReaderT<std::istream>::~ByteStreamReaderT()
    {
    }

    template<>
    ReturnCode ByteStreamReaderT<RawIStream>::Init(const char *iBuffer, ByteCount_t iBufferLength)
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
        bufferLengthInBytes_ = static_cast<ByteCount_t>(iBufferLength * CHAR_BIT);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return error_ = CMNSTRM_OK;
    }

    template<>
    ReturnCode ByteStreamReaderT<RawIStream>::Init(const unsigned char *iBuffer, ByteCount_t iBufferLength)
    {
        return Init(reinterpret_cast<const char *>(iBuffer), static_cast<uint32_t>(iBufferLength));
    }

    template<>
    ReturnCode ByteStreamReaderT<RawIStream>::Init(const ByteStreamReaderT<RawIStream> &bs2)
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
        ByteStreamStateT<RawIStream> st = bs2.getState();
        setState(st);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return CMNSTRM_OK;
    }

    template<>
    ByteStreamReaderT<RawIStream> &ByteStreamReaderT<RawIStream>::operator=(const ByteStreamReaderT<RawIStream> &bs2)
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
            ByteStreamStateT<RawIStream> state = bs2.getState();
            setState(state);
#endif // #ifndef __clang_analyzer__
        }
        return *this;
    }

    template<>
    ByteStreamReaderT<RawIStream>::~ByteStreamReaderT()
    {
        delete stream_;
    }

    template<>
    char *ByteStreamReaderT<RawIStream>::buffer() const
    {
        if ( stream_ )
        {
            return stream_->buffer();
        }
        return 0;
    }

}  // namespace CommonStream
