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

#include "gtest/gtest.h"
#include "DLC/DLCAudioData.h"
#include <vector>

namespace
{

    TEST(PCMResidualSubBlock, CodeType)
    {
        dlc::PCMResidualsSubBlock pcmr(200);

        ASSERT_EQ(pcmr.getCodeType(), dlc::eCodeType_PCM_CODE_TYPE);
    }

    TEST(PCMResidualSubBlock, ResidualsCount)
    {
        dlc::PCMResidualsSubBlock pcmr(200);

        ASSERT_EQ(pcmr.getSize(), 200);

        ASSERT_THROW(dlc::PCMResidualsSubBlock a(0), std::out_of_range);
    }

    TEST(PCMResidualSubBlock, BitDepth)
    {
        dlc::PCMResidualsSubBlock pcmr(200);

        /* default value */

        ASSERT_EQ(pcmr.getBitDepth(), 0);

        /* getter/setter */

        pcmr.setBitDepth(2);

        ASSERT_EQ(pcmr.getBitDepth(), 2);

        ASSERT_THROW(pcmr.setBitDepth(64), std::out_of_range);
    }

}
