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

#ifndef COMMON_STREAM_SRC_BITSTREAM_BITSTREAMSTATET_H_
#define COMMON_STREAM_SRC_BITSTREAM_BITSTREAMSTATET_H_

#include <climits>
#include <fstream>

#include "commonstream/utils/Namespace.h"
#include "commonstream/stream/StreamDefines.h"

namespace CommonStream
{

    template<typename StreamType>
    class BitStreamStateT
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
        
        int32_t bitBufferFill() const
        {
            return bitBufferFill_;
        }

        int32_t bitBufferFill(int32_t bbf)
        {
            return bitBufferFill_ = bbf;
        }

        uint8_t bitBuffer() const
        {
            return bitBuffer_;
        }

        uint8_t bitBuffer(uint8_t bb)
        {
            return bitBuffer_ = bb;
        }

        BitCount_t  bitCount() const
        {
            return bitCount_;
        }

        BitCount_t  bitCount(BitCount_t bc)
        {
            return bitCount_ = bc;
        }

        ReturnCode error() const
        {
            return error_;
        }

        ReturnCode error(ReturnCode er)
        {
            return error_ = er;
        }

        BitCount_t buffer_size()
        {
            return bufferLengthInBits_/CHAR_BIT;
        }

        BitCount_t bufferLengthInBits() const
        {
            return bufferLengthInBits_;
        }

        BitCount_t bufferLengthInBits(BitCount_t bl)
        {
            return (bufferLengthInBits_ = bl);
        }

        BitCount_t bitsLeft()
        {
            return bufferLengthInBits_ - bitCount_;
        }

    protected:
        std::ios_base::iostate      streamState_;
        std::streampos    streamPosition_;

        int32_t                     bitBufferFill_;
        uint8_t                     bitBuffer_;
        BitCount_t                  bitCount_;
        BitCount_t                  bufferLengthInBits_;
        ReturnCode                  error_;
    };

} // namespace CommonStream

#endif // COMMON_STREAM_SRC_BITSTREAM_BITSTREAMSTATET_H_
