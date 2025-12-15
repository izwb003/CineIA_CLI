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

#ifndef COMMON_STREAM_SRC_HASH_MD5GENERATOR_H_
#define COMMON_STREAM_SRC_HASH_MD5GENERATOR_H_

#include "commonstream/utils/Namespace.h"
#include "commonstream/hash/HashGenerator.h"

namespace CommonStream
{

    class MD5Generator : public HashGenerator
    {
    public:

        MD5Generator();
        virtual ~MD5Generator();

        virtual EHashType getHashType(void);

        virtual EHashState getState(void);

        virtual bool useHash(void);

        virtual bool start(void);

        virtual bool stop(void);

        virtual bool pause(void);

        virtual bool resume(void);

        virtual bool accumulate(uint8_t *iValue, uint32_t iByteCount);

        virtual uint32_t getHashSize(void);

        virtual bool getHash(uint8_t *oHash);

    private:
        
        struct MD5Context {
            uint32_t buf[4];
            uint32_t bits[2];
            unsigned char in[64];
        };

        static void MD5Init(struct MD5Context *context);
        static void MD5Update(struct MD5Context *context, unsigned char const *buf, unsigned len);
        static void MD5Final(unsigned char digest[16], struct MD5Context *context);
        static void MD5Transform(uint32_t buf[4], uint32_t const in[16]);

        MD5Context ctx_;
        unsigned char ss_[16];
        EHashState      state_;
    };

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_HASH_MD5GENERATOR_H_
