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

#include "commonstream/bytestream/ByteStreamWriterT.h"

#include <assert.h>
#include <iostream>
#include <locale>
#include <algorithm>
#include <limits>

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

#include "commonstream/rawstream/RawOStream.h"

namespace CommonStream
{

    template<>
    ByteStreamWriterT<std::ofstream>::ByteStreamWriterT(std::ofstream &iStream)
    {
        stream_ = &iStream;

        byteCount_ = 0;
        bufferLengthInBytes_ = std::numeric_limits<ByteCount_t>::max();

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    ByteStreamWriterT<std::ofstream>::~ByteStreamWriterT()
    {
    }

    template<>
    ByteStreamWriterT<std::ostream>::ByteStreamWriterT(std::ostream &iStream)
    {
        stream_ = &iStream;

        byteCount_ = 0;
        bufferLengthInBytes_ = std::numeric_limits<ByteCount_t>::max();

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
    }

    template<>
    ByteStreamWriterT<std::ostream>::~ByteStreamWriterT()
    {
    }

    template<>
    ReturnCode  ByteStreamWriterT<RawOStream>::Init(char *iBuffer, ByteCount_t iBufferLength)
    {
        if ( !iBuffer || !iBufferLength )
        {
            return error_ = CMNSTRM_PARAMS_BAD;
        }
        Init();

        if ( stream_ )
        {
            delete stream_;
        }

        stream_ = new(std::nothrow) RawOStream;

        if ( !stream_ )
        {
            return error_ = CMNSTRM_IO_FAIL;
        }

        ReturnCode rc = stream_->Init(iBuffer, iBufferLength);

        if ( rc )
        {
            return error_ = rc;
        }

        bufferLengthInBytes_ = static_cast<ByteCount_t>(iBufferLength*CHAR_BIT);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;

        return CMNSTRM_OK;
    }

    template<>
    ReturnCode ByteStreamWriterT<RawOStream>::Init(const ByteStreamWriterT<RawOStream> &bs2)
    {
        if ( stream_ )
        {
            delete stream_;
        }
        stream_ = new(std::nothrow) RawOStream;
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
        ByteStreamStateT<RawOStream> st = bs2.getState();
        setState(st);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return CMNSTRM_OK;
    }

    template<>
    ByteStreamWriterT<RawOStream> &ByteStreamWriterT<RawOStream>::operator=(const ByteStreamWriterT<RawOStream> &bs2)
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
                    // reuse if possible
                {
                    *stream_ = *bs2.stream_;
                }
                else
                {
                    stream_ = new RawOStream(*bs2.stream_);
                }
            }
            ByteStreamStateT<RawOStream> state = bs2.getState();
            setState(state);
#endif // #ifndef __clang_analyzer__
        }
        return *this;
    }

    template<>
    ByteStreamWriterT<RawOStream>::~ByteStreamWriterT()
    {
        delete stream_;
    }

    template<>
    char *ByteStreamWriterT<RawOStream>::buffer() const
    {
        if ( stream_ )
        {
            return stream_->buffer();
        }
        return 0;
    }

    template<>
    void ByteStreamWriterT<RawOStream>::dump_buffer_ascii(FILE *fp)
    {
        if ( !stream_ )
        {
            return;
        }
        char *buf = stream_->buffer();
        ByteCount_t len = (byteCount()+7)/8;
        for ( ByteCount_t i=0; i < len; i++ )
        {
            if ( i%16 == 0 )
            {
                fprintf(fp,"\n");
            }
            unsigned x = static_cast<unsigned>(buf[i]);
            fprintf(fp,"%02x ",x & 0xFF);
        }
        fprintf(fp, "\n");
        fflush(fp);
    }

}  // namespace CommonStream
