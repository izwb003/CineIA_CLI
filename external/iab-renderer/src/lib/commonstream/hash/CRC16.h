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

/**
 * Class to compute CRC from a bitstream block
 *
 * @file
 */
 
#ifndef COMMON_STREAM_SRC_HASH_CRC16_H_
#define COMMON_STREAM_SRC_HASH_CRC16_H_

#include "commonstream/utils/Namespace.h"
#include "commonstream/hash/CRCGenerator.h"

namespace CommonStream
{

    class CRC16Generator : public CRCGenerator
    {
    public:

        CRC16Generator();
        ~CRC16Generator();

        virtual ECRCType getCRCType(void);

        virtual ECRCState getState(void);

        virtual bool useCRC(void);

        virtual bool start(void);
        virtual bool stop(void);

        virtual bool pause(void);
        virtual bool resume(void);

        virtual bool accumulate(uint32_t iBits, uint8_t iBitCount);
        virtual bool accumulate(uint64_t iBits, uint8_t iBitCount);

        virtual uint32_t getCRCRegisterSize(void);
        virtual bool getCRCRegister(uint8_t *oCRC);

    private:

        uint16_t    register_;
        ECRCState   state_;
    };

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_HASH_CRC16_H_
