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

#include "commonstream/bitstream/BitStreamWriterT.h"

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
    BitStreamWriterT<std::ofstream>::BitStreamWriterT(std::ofstream &iStream)
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
    BitStreamWriterT<std::ofstream>::~BitStreamWriterT()
    {
    }

    template<>
    BitStreamWriterT<std::ostream>::BitStreamWriterT(std::ostream &iStream)
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
    BitStreamWriterT<std::ostream>::~BitStreamWriterT()
    {
    }

    template<>
    ReturnCode  BitStreamWriterT<RawOStream>::Init(char *iBuffer, BitCount_t iBufferLength)
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

        bufferLengthInBits_ = static_cast<BitCount_t>(iBufferLength*CHAR_BIT);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return error_ = CMNSTRM_OK;
    }

    template<>
    ReturnCode BitStreamWriterT<RawOStream>::Init(const BitStreamWriterT<RawOStream> &bs2)
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
        
        BitStreamStateT<RawOStream> st = bs2.getState();
        setState(st);

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return CMNSTRM_OK;
    }

    template<>
    BitStreamWriterT<RawOStream> &BitStreamWriterT<RawOStream>::operator=(const BitStreamWriterT<RawOStream> &bs2)
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
                    // reuse if possible
                    *stream_ = *bs2.stream_;
                }
                else
                {
                    stream_ = new RawOStream(*bs2.stream_);
                }
            }
            
            BitStreamStateT<RawOStream> state = bs2.getState();
            setState(state);
#endif // #ifndef __clang_analyzer__
        }
        return *this;
    }

    template<>
    BitStreamWriterT<RawOStream>::~BitStreamWriterT()
    {
        delete stream_;
    }

    template<>
    char *BitStreamWriterT<RawOStream>::buffer() const
    {
        if ( stream_ )
        {
            return stream_->buffer();
        }
        return 0;
    }

    template<>
    void BitStreamWriterT<RawOStream>::dump_buffer_ascii(FILE *fp)
    {
        if ( !stream_ )
        {
            return;
        }
        
        char *buf = stream_->buffer();
        BitCount_t len = (bitCount()+7)/8;
        for ( BitCount_t i=0; i < len; i++ )
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
