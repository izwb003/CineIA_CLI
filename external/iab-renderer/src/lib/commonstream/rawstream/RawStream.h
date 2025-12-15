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

#ifndef COMMON_STREAM_SRC_RAWSTREAM_RAWSTREAM_H_
#define COMMON_STREAM_SRC_RAWSTREAM_RAWSTREAM_H_

#include <fstream>

#include "commonstream/utils/Namespace.h"
#include "commonstream/stream/StreamDefines.h"

namespace CommonStream
{

    class RawStream
    {
    public:
        
        RawStream();
        RawStream(char *buf, BitCount_t buf_size); ///< construct with outside storage
        RawStream(const char *buf, BitCount_t buf_size); ///< construct with outside storage
        RawStream(BitCount_t buf_size); ///< construct with own storage
        RawStream(const RawStream &stream2); ///< copy constructor
        RawStream &operator=(const RawStream &stream2);
        ~RawStream();
        ReturnCode Init();
        ReturnCode Init(char *buf, BitCount_t buf_size); ///< init with outside storage
        ReturnCode Init(const char *buf, BitCount_t buf_size); ///< init with outside storage
        ReturnCode Init(BitCount_t buf_size); ///< init with own storage
        ReturnCode Init(const RawStream &stream2); ///< Init with copy and own new storage
        ReturnCode InitReuse(const RawStream &stream2); ///< Init with copy, reusing storage if possible

        void Dealloc();
        // getters
        char *buffer() const
        {
            return buffer_;
        }
        BitCount_t buffer_size() const
        {
            return buffer_size_;
        }

        std::ios_base::iostate rdstate() const;
        std::ios_base::iostate setstate(std::ios_base::iostate st);
        void clear();
        bool good() const;
        bool fail() const;
        bool eof() const;
        bool bad() const;
        void reset();
        BitCount_t gcount() const;
        BitCount_t tell() const;
        ReturnCode seek(BitCount_t pos);
        ReturnCode seek(SBitCount_t offs, std::ios_base::seekdir way);

    protected:
    
        static const std::ios_base::iostate eofbit_ = static_cast<std::ios_base::iostate>(4);
        static const std::ios_base::iostate failbit_ = static_cast<std::ios_base::iostate>(2);
        static const std::ios_base::iostate badbit_ = static_cast<std::ios_base::iostate>(1);
        static const std::ios_base::iostate goodstate_ = static_cast<std::ios_base::iostate>(0);

        BitCount_t index_;
        char *buffer_;
        BitCount_t buffer_size_;
        BitCount_t gcount_;
        std::ios_base::iostate state_; // eof | fail | bad
        bool own_storage_;
    };
} // namespace CommonStream

#endif // COMMON_STREAM_SRC_RAWSTREAM_RAWSTREAM_H_
