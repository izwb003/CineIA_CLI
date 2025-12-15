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

#include <cmath>

#include "gtest/gtest.h"
#include "renderer/VBAPRenderer/VBAPRendererDataStructures.h"

using namespace IABVBAP;

class IABVBAPLFETest : public testing::Test
{
protected:
    
    virtual void SetUp()
    {
    }
    
    virtual void TearDown()
    {
    }
    
    void TestSetterGetterAPIs()
    {
        // Arbitrary speaker and chnannel count, e.g. 5.1 config with a virtual top speaker
        uint32_t speakerCount = 7;
        uint32_t channelCount = 6;
        IABVBAP::vbapRendererLFEChannel  lfeChannel = IABVBAP::vbapRendererLFEChannel(speakerCount, channelCount);
        
        // Check default
        
        EXPECT_FLOAT_EQ(1.0f, lfeChannel.GetGain());
        EXPECT_EQ(0, lfeChannel.GetId());
        EXPECT_EQ(speakerCount, lfeChannel.speakerGains_.size());
        EXPECT_EQ(channelCount, lfeChannel.channelGains_.size());
        
        std::vector<float>::iterator iter;
        float gain = 0.1f;
        
        for (iter = lfeChannel.speakerGains_.begin(); iter!= lfeChannel.speakerGains_.end(); iter++)
        {
            ASSERT_FLOAT_EQ(0.0f, *iter);
            
            // write new test value
            *iter = gain;
            gain += 0.1f;
        }
        
        // Check value written in previous test
        gain = 0.1f;
        
        for (iter = lfeChannel.speakerGains_.begin(); iter!= lfeChannel.speakerGains_.end(); iter++)
        {
            ASSERT_FLOAT_EQ(gain, *iter);
            gain += 0.1f;
        }

        gain = 0.1f;
        
        for (iter = lfeChannel.channelGains_.begin(); iter!= lfeChannel.channelGains_.end(); iter++)
        {
            ASSERT_FLOAT_EQ(0.0f, *iter);
            *iter = gain;
            gain += 0.1f;
        }

        gain = 0.1f;
        
        for (iter = lfeChannel.channelGains_.begin(); iter!= lfeChannel.channelGains_.end(); iter++)
        {
            ASSERT_FLOAT_EQ(gain, *iter);
            gain += 0.1f;
        }
        
        // Test ID Set and Get. Vaid gain range [0.0f, 1.0f]

        // Set to an arbitrary value
        EXPECT_EQ(kVBAPNoError, lfeChannel.SetId(100));
        EXPECT_EQ(100, lfeChannel.GetId());
        
        // Test Gain Set and Get

        // Set to an arbitrary value
        EXPECT_EQ(kVBAPNoError, lfeChannel.SetGain(0.5f));
        EXPECT_FLOAT_EQ(0.5f, lfeChannel.GetGain());
        
        // Set to maximum gain of 1.0f
        EXPECT_EQ(kVBAPNoError, lfeChannel.SetGain(1.0f));
        EXPECT_FLOAT_EQ(1.0f, lfeChannel.GetGain());

        // Set to invalid value
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, lfeChannel.SetGain(-0.1f));
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, lfeChannel.SetGain(1.1f));
        
        
    }
    
    
private:
    
};

TEST_F(IABVBAPLFETest, TestAPIs)
{
    TestSetterGetterAPIs();
}


