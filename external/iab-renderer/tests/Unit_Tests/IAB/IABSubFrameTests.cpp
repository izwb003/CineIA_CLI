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
#include "common/IABElements.h"
#include <vector>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{

    // Test IAB preamble setter and getter APIs
    TEST(PreambleTests, TestSettersGetters)
    {
        IABPreamble aPreamble;
        uint8_t *payLoad = new uint8_t[5];
        uint8_t *pPayload = payLoad;
        
        // Initialise contents
        for (uint8_t i = 0; i < 5; i++)
        {
            *pPayload++ = i;
        }

        IABSubframeLengthType length;
        IABSubframeLengthType subframeLength = 0;

        // Test default subframe length
        aPreamble.GetSubframeLength(subframeLength);
        EXPECT_EQ(subframeLength, 0);

        // Test default
        aPreamble.GetPreamblePayload(pPayload, length);
        EXPECT_TRUE(pPayload == NULL);
        EXPECT_EQ(length, 0);

        // Test set subframe length
        EXPECT_EQ(aPreamble.SetSubframeLength(0xFFFFFFFF), kIABNoError);        // set to unsigned 32-bit maximum value
        aPreamble.GetSubframeLength(subframeLength);
        EXPECT_EQ(subframeLength, 0xFFFFFFFF);
        
        // Test set payload
        EXPECT_EQ(aPreamble.SetPreamblePayload(payLoad, 5), kIABNoError);
        aPreamble.GetPreamblePayload(pPayload, length);
        ASSERT_TRUE(pPayload != NULL);
        EXPECT_EQ(length, 5);
        
        for (uint8_t i = 0; i < 5; i++)
        {
            EXPECT_EQ(*pPayload++, i);
        }

    }
    
    // Test IA subframe setter and getter APIs
    TEST(IASubframeTests, TestSettersGetters)
    {
        IABIASubFrame iaSubFrame;
        IABSubframeLengthType subframeLength = 0;
        
        // Test default subframe length
        iaSubFrame.GetSubframeLength(subframeLength);
        EXPECT_EQ(subframeLength, 0);
        
        // Test set subframe length
        EXPECT_EQ(iaSubFrame.SetSubframeLength(0xFFFFFFFF), kIABNoError);        // set to unsigned 32-bit maximum value
        iaSubFrame.GetSubframeLength(subframeLength);
        EXPECT_EQ(subframeLength, 0xFFFFFFFF);
        
    }
}
