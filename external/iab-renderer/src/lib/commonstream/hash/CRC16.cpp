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

#include "commonstream/hash/CRC16.h"
#include <string.h>
#include <cassert>

/*
	Implements CCIT CRC 16
*/

namespace CommonStream
{

    CRC16Generator::CRC16Generator()
    {
        state_ = eCRCState_Stopped;
    }

    CRC16Generator::~CRC16Generator()
    {
    }

    ECRCType CRC16Generator::getCRCType(void)
    {
        return eCRCType_CRC16;
    }

    CRCGenerator::ECRCState CRC16Generator::getState(void)
    {
        return state_;
    }

    bool CRC16Generator::useCRC(void)
    {
        bool useCRC = (state_ == eCRCState_Started) || (state_ == eCRCState_Paused);

        return useCRC;
    }

    bool CRC16Generator::start()
    {
        if ((state_ == eCRCState_Stopped)
                || (state_ == eCRCState_Started)
                || (state_ == eCRCState_Paused))
        {
            state_ = eCRCState_Started;
            register_ = 0xFFFF;

            return true;
        }
        else
        {
            return false;
        }
    }

    bool CRC16Generator::accumulate(uint32_t iBits, uint8_t iBitCount)
    {
        if (state_ == eCRCState_Stopped)
        {
            return false;
        }
        else if (state_ == eCRCState_Paused)
        {
            return true;
        }

        bool high_bit = false;

        iBits = iBits << (32 - iBitCount);

        for(int i = iBitCount - 1; i >= 0; i--)
        {

            high_bit = ((register_ >> 15) != 0);

            register_ = (register_ << 1) | ((iBits >> 31) & 1);

            iBits = iBits << 1;

            if (high_bit)
            {
                register_ = register_ ^ 0x1021;
            }
        }

        return true;
    }

    bool CRC16Generator::accumulate(uint64_t iBits, uint8_t iBitCount)
    {
        if (state_ == eCRCState_Stopped)
        {
            return false;
        }
        else if (state_ == eCRCState_Paused)
        {
            return true;
        }

        bool high_bit = false;

        iBits = iBits << (64 - iBitCount);

        for(int i = iBitCount - 1; i >= 0; i--)
        {

            high_bit = ((register_ >> 15) != 0);

            register_ = (register_ << 1) | ((iBits >> 63) & 1);

            iBits = iBits << 1;

            if (high_bit)
            {
                register_ = register_ ^ 0x1021;
            }
        }

        return true;
    }

    bool CRC16Generator::stop()
    {
        uint32_t bits = 0;
        accumulate(bits, 16);

        if ((state_ == eCRCState_Stopped)
                || (state_ == eCRCState_Started)
                || (state_ == eCRCState_Paused))
        {
            state_ = eCRCState_Stopped;
        }
        else
        {
            return false;
        }

        return true;
    }

    bool CRC16Generator::pause(void)
    {
        if ((state_ == eCRCState_Paused) || (state_ == eCRCState_Started))
        {
            state_ = eCRCState_Paused;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool CRC16Generator::resume(void)
    {
        if (state_ == eCRCState_Paused)
        {
            state_ = eCRCState_Started;
        }
        else
        {
            return false;
        }

        return true;
    }

    uint32_t CRC16Generator::getCRCRegisterSize(void)
    {
        return sizeof(register_);
    }

    bool CRC16Generator::getCRCRegister(uint8_t *oCRC)
    {
        memcpy(oCRC, &register_, this->getCRCRegisterSize());

        return true;
    }

}  // namespace CommonStream
