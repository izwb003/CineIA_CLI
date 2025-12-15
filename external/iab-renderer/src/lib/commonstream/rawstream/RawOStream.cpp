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

#include "commonstream/rawstream/RawOStream.h"

namespace CommonStream
{

    RawOStream::RawOStream():RawStream()
    {
    }

    RawOStream::RawOStream(char *buf, BitCount_t bufsize)
    :
    RawStream(buf, bufsize)
    {
    }

    RawOStream::RawOStream(BitCount_t bufsize)
    :
    RawStream(bufsize)
    {
    }

    RawOStream::RawOStream(const RawOStream &s2)
    :
    RawStream(s2)
    {
    }

    RawOStream &RawOStream::operator=(const RawOStream &s2)
    {
        RawStream::operator=(s2);
        return *this;
    }

    ReturnCode RawOStream::write(const void *buf, uint32_t nbytes)
    {
        ReturnCode rc = CMNSTRM_OK;
        if ( index_ + nbytes > buffer_size_ )
        {
            BitCount_t copySize = buffer_size_ - index_;

            // We are overflowing the ability of memcpy
            //
            nbytes = static_cast<uint32_t>(copySize);

            // Should this be failbit_ instead of eofbit_?
            //
            state_ |= eofbit_;
            rc = CMNSTRM_IO_EOF;
        }
        memcpy(buffer_+index_, buf, nbytes);
        gcount_ = nbytes;
        index_ += nbytes;
        return rc;
    }

    BitCount_t RawOStream::tellp() const
    {
        return RawStream::tell();
    }

    ReturnCode RawOStream::seekp(std::streampos pos)
    {
        BitCount_t bpos = static_cast<BitCount_t>(pos);
        return RawStream::seek(bpos);
    }

    ReturnCode RawOStream::seekp(std::streamoff offs, std::ios_base::seekdir way)
    {
        SBitCount_t boffs = static_cast<SBitCount_t>(offs);
        return RawStream::seek(boffs, way);
    }

} // namespace CommonStream
