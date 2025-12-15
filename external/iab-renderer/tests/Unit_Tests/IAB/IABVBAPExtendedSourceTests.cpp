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

class IABVBAPExtendedSourceTest : public testing::Test
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
		// Init both speaker count and channel count to 0 for testing extent parameters
		//
        IABVBAP::vbapRendererExtendedSource extendedSource1(0, 0);
        IABVBAP::vbapRendererExtendedSource extendedSource2(0, 0);
        
        // Check defaults
        
        EXPECT_FLOAT_EQ(extendedSource1.extSourceGain_, 1.0f);
        EXPECT_FLOAT_EQ(extendedSource1.aperture_, 0.0f);
        EXPECT_FLOAT_EQ(extendedSource1.divergence_, 0.0f);
        EXPECT_EQ(extendedSource1.touched_, false);
        
        // Test Gain Set, Get API
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetGain(0.5f));
        EXPECT_FLOAT_EQ(0.5f, extendedSource1.GetGain());
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetGain(1.0f));
        EXPECT_FLOAT_EQ(1.0f, extendedSource1.GetGain());
        
        // Invalid tests
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetGain(-0.5f));
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetGain(1.1f));
        
        // Test Aperture Set, Get API
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetAperture(CoreUtils::kPI));
        EXPECT_FLOAT_EQ(CoreUtils::kPI, extendedSource1.GetAperture());
        EXPECT_EQ(kVBAPNoError,extendedSource1.SetAperture(0.0f));
        EXPECT_FLOAT_EQ(0.0f, extendedSource1.GetAperture());

        // Invalid tests
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetAperture(-0.5f));
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetAperture(CoreUtils::kPI + 0.1f));
        
        // Test Divergence Set, Get API
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetDivergence(CoreUtils::kPI/2));
        EXPECT_FLOAT_EQ(CoreUtils::kPI/2, extendedSource1.GetDivergence());
        EXPECT_EQ(kVBAPNoError,extendedSource1.SetDivergence(0.0f));
        EXPECT_FLOAT_EQ(0.0f, extendedSource1.GetDivergence());
        
        // Invalid tests
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetDivergence(-0.5f));
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetDivergence(CoreUtils::kPI/2 + 0.1f));
        
        // Check touched_
        
        // Default setting
        EXPECT_EQ(extendedSource1.touched_, false);
        
        // Set to true
        extendedSource1.touched_ = true;
        EXPECT_EQ(extendedSource1.touched_, true);
        
        // Default local gain vector size
        EXPECT_EQ(0, extendedSource1.renderedSpeakerGains_.size());
        
        extendedSource1.renderedSpeakerGains_.push_back(0.5f);
        extendedSource1.renderedSpeakerGains_.push_back(1.0f);
        EXPECT_EQ(2, extendedSource1.renderedSpeakerGains_.size());
        EXPECT_FLOAT_EQ(0.5f, extendedSource1.renderedSpeakerGains_[0]);
        EXPECT_FLOAT_EQ(1.0f, extendedSource1.renderedSpeakerGains_[1]);
        
        // Test position Get,, Set APIs
        
        /*
         
         x   Range [-1.0, 1.0]
         y   Range [-1.0, 1.0]
         z   Range [0.0, 1.0]
         
         */
        
        CoreUtils::Vector3 pos1(0,1,0);    // x, y, z
        CoreUtils::Vector3 pos2(0,0,0);
        
        // Check default (0, 1, 0)
        pos2 = extendedSource1.GetPosition();
        EXPECT_EQ(pos1, pos2);
        
        // Position vector x,, y, z component at minimum value
        pos1 = CoreUtils::Vector3(-1.0f, -1.0f, 0.0f);
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetPosition(pos1));
        EXPECT_EQ(CoreUtils::Vector3(-1.0f, -1.0f, 0.0f), extendedSource1.GetPosition());

        // Position vector x,, y, z component at maximum value
        pos1 = CoreUtils::Vector3(1.0f, 1.0f, 1.0f);
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetPosition(pos1));
        EXPECT_EQ(CoreUtils::Vector3(1.0f, 1.0f, 1.0f), extendedSource1.GetPosition());
        
        // Position vector x,, y, z component at arbitrary x,y,z values (0.5, 0.6, 0.7)
        pos1 = CoreUtils::Vector3(0.5f, 0.6f, 0.7f);
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetPosition(pos1));
        EXPECT_EQ(CoreUtils::Vector3(0.5f, 0.6f, 0.7f), extendedSource1.GetPosition());
        
        // Invalid tests
        
        // Invalid x values
        pos1 = CoreUtils::Vector3(-1.1f, 0.0f, 0.0f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));
        pos1 = CoreUtils::Vector3(1.1f, 0.0f, 0.0f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));

        // Invalid y values
        pos1 = CoreUtils::Vector3(0.0f, -1.1f, 0.0f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));
        pos1 = CoreUtils::Vector3(0.0f, 1.1f, 0.0f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));
        
        // Invalid y values
        pos1 = CoreUtils::Vector3(0.0f, 0.0f, -0.1f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));
        pos1 = CoreUtils::Vector3(0.0f, 0.0f, 1.1f);
        EXPECT_EQ(kVBAPParameterOutOfBoundsError, extendedSource1.SetPosition(pos1));
        
        // Set extendedSource1 parameters to arbitrary values
        pos1 = CoreUtils::Vector3(0.1f, 0.2f, 0.3f);
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetPosition(pos1));
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetGain(0.5f));
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetAperture(0.5f));
        EXPECT_EQ(kVBAPNoError, extendedSource1.SetDivergence(0.6f));
        
        // Set extendedSource2 parameters to same values as extendedSource1
        pos1 = CoreUtils::Vector3(0.1f, 0.2f, 0.3f);
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetPosition(pos1));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetGain(0.5f));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetAperture(0.5f));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetDivergence(0.6f));
        
        // Test HasSameRenderingParams. This checks two extended sources having same aperture, divergence and position
        EXPECT_EQ(true, extendedSource1.HasSameRenderingParams(&extendedSource2));
        
        // Different position
        pos2 = CoreUtils::Vector3(0.0f, 0.2f, 0.3f);
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetPosition(pos2));
        EXPECT_EQ(false, extendedSource1.HasSameRenderingParams(&extendedSource2));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetPosition(pos1)); // restore position
        
        // Different Aperture
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetAperture(0.4f));
        EXPECT_EQ(false, extendedSource1.HasSameRenderingParams(&extendedSource2));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetAperture(0.6f));
        
        // Different Divergence
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetDivergence(0.4f));
        EXPECT_EQ(false, extendedSource1.HasSameRenderingParams(&extendedSource2));
        EXPECT_EQ(kVBAPNoError, extendedSource2.SetDivergence(0.6f));

        // Should be identical again
        EXPECT_EQ(false, extendedSource1.HasSameRenderingParams(&extendedSource2));
        
    }


private:

};

TEST_F(IABVBAPExtendedSourceTest, TestAPIs)
{
    TestSetterGetterAPIs();
}


