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

#ifndef COMMON_STREAM_SRC_RAWSTREAM_RAWISTREAM_H_
#define COMMON_STREAM_SRC_RAWSTREAM_RAWISTREAM_H_

#include "commonstream/utils/Namespace.h"
#include "RawStream.h"

namespace CommonStream
{
    class RawIStream : public RawStream
    {
        using RawStream::eofbit_;
        using RawStream::index_;
        using RawStream::buffer_;
        using RawStream::buffer_size_;
        using RawStream::gcount_;
        using RawStream::state_; // eof | fail | bad

    public:
    
        RawIStream();
        RawIStream(const char *buf, BitCount_t bufsize); ///< constructor with outside storage
        RawIStream(BitCount_t buf_size); ///< construct with own storage
        RawIStream(const RawIStream &s2); ///< copy constructor
        RawIStream &operator=(const RawIStream &s2); ///< assignment

        ReturnCode read(void *buf, BitCount_t nbytes);
        ReturnCode get(char &c);
        BitCount_t tellg() const;
        ReturnCode seekg(std::streampos pos);
        ReturnCode seekg(std::streamoff offs, std::ios_base::seekdir way);
    };

}   // namespace CommonStream

#endif // COMMON_STREAM_SRC_RAWSTREAM_RAWISTREAM_H_

