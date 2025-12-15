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

//
// RenderSpeaker_Test.cpp
// Unit Test
//

#include "gtest/gtest.h"
#include <math.h>
#include <stdint.h>

#include "renderutils/RenderSpeaker.h"
#include "coreutils/CoreDefines.h"

using namespace RenderUtils;

namespace
{

    class RenderSpeaker_Test : public testing::Test
    {
    protected:

        // Set up test variables and configuration
        void SetUp()
        {
        }

        // **********************************************
        // RendereSpeaker get API tests
        // **********************************************

        void TestGetAPIs()
        {

            CoreUtils::Vector3 pos;

            // *******************************
            // Test get APIs
            // *******************************

            // Speaker with channel index = -1, output index = 1. Arbitray speaker azimuth and elevation -30 and 45
            RenderSpeaker testSpeaker1 = RenderSpeaker("TestSpeaker1", -1, 1, -30.0f, 45.0f, "TestSpeaker1_URI");

            // Check getChannel API
            EXPECT_EQ(-1, testSpeaker1.getChannel());

            // Check getRendererOutputIndex API
            EXPECT_EQ(1, testSpeaker1.getRendererOutputIndex());

            // check getAzim API
            EXPECT_FLOAT_EQ(-30.0f, testSpeaker1.getAzimuth());

            // check getElevation API
            EXPECT_FLOAT_EQ(45.0f, testSpeaker1.getElevation());

            // Check getName API
            EXPECT_STREQ("TestSpeaker1", testSpeaker1.getName().c_str());

            // Check getURI API
            EXPECT_STREQ("TestSpeaker1_URI", testSpeaker1.getURI().c_str());

            // Check getPosition API

            // Calculate expected x,y,z coordinates (as in RenderSpeaker)
            float theta = -30.0f * CoreUtils::kPI / 180.0f;
            float phi = 45.0f * CoreUtils::kPI / 180.0f;

            float x = sin(theta) * cos(phi);
            float y = cos(theta) * cos(phi);
            float z = sin(phi);

            pos = testSpeaker1.getPosition();
            EXPECT_FLOAT_EQ(x, pos.getX());
            EXPECT_FLOAT_EQ(y, pos.getY());
            EXPECT_FLOAT_EQ(z, pos.getZ());

        }


        void TestDownMixAPIs()
        {
            // *******************************
            // Test default downmix using testSpeaker1
            // This speaker has channel index > -1, so should contain a single downmix : ch = 0, coefficient = 1
            // *******************************

            RenderSpeaker testSpeaker1 = RenderSpeaker("TestSpeaker1", 10, 5, 0.0f, 0.0f, "TestSpeaker1_URI");
            EXPECT_EQ(10, testSpeaker1.getChannel());

            // Check default downmix, with channel index > -1, should be true
            EXPECT_TRUE(testSpeaker1.hasDownmix());

            std::vector<DownmixValue> downMix;
            downMix = testSpeaker1.getNormalizedDownmixValues();
            EXPECT_EQ(1, downMix.size());
            EXPECT_EQ(10, downMix[0].ch_);
            EXPECT_FLOAT_EQ(1.0f, downMix[0].coefficient_);

            // *******************************
            // Test addDownMix API using testSpeaker2
            // This speaker has channel index = -1, so speaker no downmix to start with
            // *******************************

            RenderSpeaker testSpeaker2 = RenderSpeaker("TestSpeaker2", -1, -1, -30.0f, 45.0f, "TestSpeaker2_URI");

            // Check default downmix, with channel index = -1, should be none
            EXPECT_FALSE(testSpeaker2.hasDownmix());

            // Set down mix values
            DownmixValue downMix1 = DownmixValue(1, 0.3f);
            DownmixValue downMix2 = DownmixValue(2, 0.7f);
            testSpeaker2.addDownMixValue(downMix1);
            testSpeaker2.addDownMixValue(downMix2);
            testSpeaker2.normalizeDownMixValues();

            downMix.clear();
            downMix = testSpeaker2.getNormalizedDownmixValues();
            EXPECT_EQ(2, downMix.size());
            EXPECT_EQ(1, downMix[0].ch_);
            // The values should be L2 normalized
            EXPECT_FLOAT_EQ(1., downMix[0].coefficient_ * downMix[0].coefficient_ + downMix[1].coefficient_ * downMix[1].coefficient_);
            EXPECT_FLOAT_EQ(0.3939193f, downMix[0].coefficient_);
            EXPECT_EQ(2, downMix[1].ch_);
            EXPECT_FLOAT_EQ(0.9191449f, downMix[1].coefficient_);

            // Test getMixmaps API
            const std::vector<DownmixValue>& downMixList = testSpeaker2.getMixmaps();

            EXPECT_EQ(2, downMixList.size());
            EXPECT_EQ(1, downMixList[0].ch_);
            EXPECT_FLOAT_EQ(0.3f, downMixList[0].coefficient_);
            EXPECT_EQ(2, downMixList[1].ch_);
            EXPECT_FLOAT_EQ(0.7f, downMixList[1].coefficient_);

        }


    private:


    };


    TEST_F(RenderSpeaker_Test, Test_GetAPIs)
    {
        TestGetAPIs();
    }

    TEST_F(RenderSpeaker_Test, Test_DownMix_APIs)
    {
        TestDownMixAPIs();
    }

}
