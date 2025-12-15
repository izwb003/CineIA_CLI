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

#ifndef COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMSTATET_H
#define COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMSTATET_H

#include <climits>
#include <fstream>

#include "commonstream/utils/Namespace.h"
#include "commonstream/stream/StreamDefines.h"

namespace CommonStream
{

    template<typename StreamType>
    class ByteStreamStateT
    {
    public:

        std::ios_base::iostate streamState() const
        {
            return streamState_;
        }

        std::ios_base::iostate streamState(std::ios_base::iostate st)
        {
            return streamState_ = st;
        }

        std::streampos streamPosition() const
        {
            return streamPosition_;
        }

        std::streampos streamPosition(std::streampos sp)
        {
            return streamPosition_ = sp;
        }

        ByteCount_t byteCount() const
        {
            return byteCount_;
        }

        ByteCount_t byteCount(ByteCount_t bc)
        {
            return byteCount_ = bc;
        }

        ReturnCode error() const
        {
            return error_;
        }

        ReturnCode error(ReturnCode er)
        {
            return error_ = er;
        }

        ByteCount_t buffer_size()
        {
            return bufferLengthInBytes_ / CHAR_BIT;
        }

        ByteCount_t bufferLengthInBytes() const
        {
            return bufferLengthInBytes_;
        }

        ByteCount_t bufferLengthInBytes(ByteCount_t bl)
        {
            return (bufferLengthInBytes_ = bl);
        }

        ByteCount_t bytesLeft()
        {
            return bufferLengthInBytes_ - byteCount_;
        }

    protected:
        std::ios_base::iostate      streamState_;
        std::streampos              streamPosition_;

        ByteCount_t                 byteCount_;
        ByteCount_t                 bufferLengthInBytes_;
        ReturnCode                  error_;
    };

} // namespace CommonStream

#endif // COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMSTATET_H
