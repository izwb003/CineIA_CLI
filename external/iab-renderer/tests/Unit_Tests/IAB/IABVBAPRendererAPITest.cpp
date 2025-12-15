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

#include <fstream>
#include <iterator>
#include <iomanip>
#include <map>
#include <iostream>
#include <cmath>

#include "renderer/VBAPRenderer/VBAPRenderer.h"
#include "renderer/VBAPRenderer/VBAPRendererErrors.h"
#include "testcfg.h"
#include "renderer/VBAPRenderer/VBAPRendererDataStructures.h"

#include "gtest/gtest.h"

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

#define TEST_SAMPLE_SIZE    2000

using namespace IABVBAP;
using namespace RenderUtils;

// ************************************************************************************************************
// Create a test fixture for IAB Renderer API tests

// googleTest test fixture: derive a class from testing::Test.
class IABVABPRendererAPITest : public testing::Test
{
protected:

    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
        rendererConfig_ = NULL;
        channelCount_ = 0;
        speakerCount_ = 0;

        audioSamples_ = NULL;
        audioBuffer_ = NULL;
        audioBufferArray_ = NULL;

        rendererConfig_ = RenderUtils::IRendererConfigurationFile::FromBuffer((char*) c51cfg.c_str());
        ASSERT_TRUE(rendererConfig_ != NULL);
        
        std::vector<RenderUtils::RenderSpeaker>::const_iterator iterA;
        const std::vector<RenderUtils::RenderSpeaker>*	speakerList;
        
        ASSERT_EQ(kNoRendererConfigurationError, rendererConfig_->GetSpeakers(speakerList));
        ASSERT_GT((*speakerList).size(), 0);
        speakerCount_ = static_cast<uint32_t>((*speakerList).size());
        
        ASSERT_EQ(kNoRendererConfigurationError, rendererConfig_->GetChannelCount(channelCount_));
        
        ASSERT_EQ(7, speakerCount_);
        ASSERT_EQ(6, channelCount_);
        
        renderer_ = new IABVBAP::VBAPRenderer();
        ASSERT_TRUE(renderer_ != NULL);
        
        ASSERT_EQ(kVBAPNoError, renderer_->InitWithConfig(rendererConfig_));
    }

    // virtual void TearDown() to do any clean-up
    virtual void TearDown()
    {
        if (rendererConfig_)
        {
            delete rendererConfig_;
        }
        
        if (renderer_)
        {
            delete renderer_;
        }
        
        delete[] audioSamples_;
        delete[] audioBuffer_;
        delete[] audioBufferArray_;
    }

    virtual void TestObjectRenderingAPIs()
    {
        ASSERT_TRUE(renderer_ != NULL);

        // uses all defaults
        IABVBAP::vbapRendererObject *object = new IABVBAP::vbapRendererObject(channelCount_);
        IABVBAP::vbapRendererExtendedSource *extendedSource = new IABVBAP::vbapRendererExtendedSource(speakerCount_, channelCount_);

        // ID
        uint32_t id = 1;

        EXPECT_EQ(kVBAPNoError, object->SetId(id));
        
        EXPECT_EQ(kVBAPNoError,renderer_->RenderExtendedSource(extendedSource));

		// object contains 0 extended source, expect error
        EXPECT_EQ(kVBAPNoExtendedSourceError, renderer_->RenderObject(object));

		// Add the entended source, as the single on-dome source, to object
		object->extendedSources_.push_back(*extendedSource);

		// With 1 extended source added, expect no error
		EXPECT_EQ(kVBAPNoError, renderer_->RenderObject(object));

        delete object;
		delete extendedSource;
    }

    virtual void TestLFERenderingAPIs()
    {
        ASSERT_TRUE(renderer_ != NULL);
        
        // uses all defaults
        IABVBAP::vbapRendererLFEChannel *lfeChannel = new IABVBAP::vbapRendererLFEChannel(speakerCount_, channelCount_);
        
        // ID
        uint32_t id = 2;
        
        EXPECT_EQ(kVBAPNoError, lfeChannel->SetId(id));
        
		EXPECT_EQ(kVBAPNoError, renderer_->RenderLFEChannel(lfeChannel));
        
        delete lfeChannel;
    }

    
private:
    
    // Handles to a list of Renderer entities
    IABVBAP::VBAPRenderer  *renderer_;
    RenderUtils::IRendererConfiguration *rendererConfig_;
    
    uint32_t channelCount_;
    uint32_t speakerCount_;
    
    float *audioSamples_;           // Input samples
    float *audioBuffer_;            // Output samples
    float **audioBufferArray_;		// Channel output pointers
    
};

// Test Object rendering API
TEST_F(IABVABPRendererAPITest, ObjectAPITest)
{
    TestObjectRenderingAPIs();
}

// Test LFE rendering API
TEST_F(IABVABPRendererAPITest, LFEAPITest)
{
    TestLFERenderingAPIs();
}

