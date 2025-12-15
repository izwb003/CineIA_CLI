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

#include <cstring>

#include "commonstream/rawstream/RawIStream.h"

namespace CommonStream
{

    RawIStream::RawIStream():RawStream()
    {
    }

    RawIStream::RawIStream(const char *buf, BitCount_t bufsize)
    :
    RawStream(buf, bufsize)
    {
    }

    RawIStream::RawIStream(BitCount_t bufsize)
    :
    RawStream(bufsize)
    {
    }

    RawIStream::RawIStream(const RawIStream &s2)
    :
    RawStream(s2)
    {
    }

    RawIStream &RawIStream::operator=(const RawIStream &s2)
    {
        RawStream::operator=(s2);
        return *this;
    }

    ReturnCode RawIStream::read(void *buf, BitCount_t nbytes)
    {
        ReturnCode rc = CMNSTRM_OK;
        if ( index_ + nbytes > buffer_size_ )
        {
            nbytes = buffer_size_ - index_;
            state_ |= eofbit_;
            rc = CMNSTRM_IO_EOF;
        }
        memcpy(buf, buffer_+index_, static_cast<uint32_t>(nbytes));
        gcount_ = nbytes;
        index_ += nbytes;
        return rc;
    }

    ReturnCode RawIStream::get(char &c)
    {
        if ( index_ < buffer_size_ )
        {
            c = buffer_[index_];
            index_++;
            gcount_ = 1;
            return CMNSTRM_OK;
        }
        else
        {
            state_ |= eofbit_;
            return CMNSTRM_IO_EOF;
        }
    }

    BitCount_t RawIStream::tellg() const
    {
        return RawStream::tell();
    }

    ReturnCode RawIStream::seekg(std::streampos pos)
    {
        BitCount_t bpos = static_cast<BitCount_t>(pos);
        return RawStream::seek(bpos);
    }

    ReturnCode RawIStream::seekg(std::streamoff offs, std::ios_base::seekdir way)
    {
        SBitCount_t boffs = static_cast<SBitCount_t>(offs);
        return RawStream::seek(boffs, way);
    }

} // namespace CommonStream
