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
// RendererConfiguration_Test.cpp
// Unit Test
//

#include "gtest/gtest.h"
#include <math.h>
#include <stdint.h>

#include "renderutils/RendererConfigurationFile.h"
#include "renderutils/RenderSpeaker.h"
#include "coreutils/CoreDefines.h"
#include "testConfig.h"

using namespace RenderUtils;

namespace
{

    class RendererConfiguration_Test : public testing::Test
    {
    protected:


        // **********************************************
        // RendererConfiguration API tests
        // **********************************************

        void TestAPIs()
        {
            RenderUtils::IRendererConfiguration *rendererConfig_ = NULL;
            rendererConfig_ = RenderUtils::IRendererConfigurationFile::FromBuffer((char*) IABConfigWith91OHSoundfield.c_str());
            
            ASSERT_TRUE(NULL != rendererConfig_);
            
            // The following speakers in test configuration are non-VBAP speakers (i.e. not in a in VBAP RenderPatch), expect false
            
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("LFE"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("LSS"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("RSS"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("LRS"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("RRS"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("TSL"));
            EXPECT_EQ(false, rendererConfig_->IsVBAPSpeaker("TSR"));
            
            // The following speakers in test configuration are VBAP speakers (i.e. in a in VBAP RenderPatch), expect true
            
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("L"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("C"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("R"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LRS1"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LRS2"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LSS1"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LSS2"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LSS3"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("LSS4"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("RSS1"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("RSS2"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("RSS3"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("RSS4"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSL1"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSL2"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSL3"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSL4"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSR1"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSR2"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSR3"));
            EXPECT_EQ(true, rendererConfig_->IsVBAPSpeaker("TSR4"));
            
            // TODO Add test for other RendererConfigurationFile class APIs
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
            }

        }

    private:
        

    };


    TEST_F(RendererConfiguration_Test, Test_APIs)
    {
        TestAPIs();
    }


}
