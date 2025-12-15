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
// Tests the VirtualSourceTree class
// Unit Test
//
/**
 *
 * Test Objective
 *
 *  - Unit test to validate the averageGainsOverRange function used in
 *    the VBAP renderer through a set of nominal, programmed cases, the
 *    function returns the number of virtual sources that were averaged
 *
 * Preconditions
 *
 *  - None, the test is self-contained
 *
 * Test Steps
 *
 *  - Virtual source renderer is configured with a virtual
 *    source tree containing a predefined set of animated
 *    theta values, fixed phi, and speaker gains set to
 *    0.0 fullscale
 *  - Average gains function is evaluated over full range,
 *    top index, and bottom index
 *  - For the full range, the range is split in half and then
 *    confirmed against itself for the full range
 *  -
 *
 * Expected Results
 *
 *  - Returned values of the range split vs continuous compares equal
 *    from the same function (self comparison)
 *  - Top and bottom indices evaluate to 1
 *
 */
#include <cmath>

#include "gtest/gtest.h"
#include "renderutils/VirtualSources.h"
#include "coreutils/CoreDefines.h"

using namespace RenderUtils;

class VirtualSourcesTreeTest : public testing::Test
{
public:

    VirtualSourcesTreeTest()
    {
        rendererVirtualSource_ = NULL;
    }

protected:

    virtual void SetUp()
    {
        int gainCount = 8;
        int thetaDivs = 128;
        float phi = CoreUtils::kPI / 4.0f;

        int n = (int) floor(thetaDivs * std::sin(phi));

        rendererVirtualSource_ = new LongitudeVirtualSources();

        rendererVirtualSource_->fMaxThetaIndex = n - 1;
        rendererVirtualSource_->fDeltaTheta = 2 * CoreUtils::kPI / (float(n));
        rendererVirtualSource_->fPhi = phi;
        rendererVirtualSource_->fPhiIndex = 5;

        VirtualSourceTree* vst  = new VirtualSourceTree(gainCount);
        rendererVirtualSource_->fVirtualSources = vst;

        std::vector<VirtualSource> vsv(n);

        for (int j = 0; j < n ; j++)
        {
            float theta = rendererVirtualSource_->fDeltaTheta*((float)j);

            vsv[j].fThetaIndex = j;
            vsv[j].fTheta = theta;
            vsv[j].fSpeakerGains.resize(gainCount, 0.0f);
        }

        rendererVirtualSource_->fVirtualSources->build(vsv.begin(), vsv.end());
    }

    virtual void TearDown()
    {
        if (rendererVirtualSource_)
        {
            if (rendererVirtualSource_->fVirtualSources)
            {
                delete rendererVirtualSource_->fVirtualSources;
            }

            delete rendererVirtualSource_;
        }
    }

    // Need to be protected as the base class implementation
    // needs to access these member variables
    //
    LongitudeVirtualSources* rendererVirtualSource_;

private:

};

TEST_F(VirtualSourcesTreeTest, AverageGainsOverRangeAll)
{
    std::vector<float> tmp(rendererVirtualSource_->fVirtualSources->fRoot.fSpeakerGains.size(), 0.0f);
    int one = rendererVirtualSource_->fVirtualSources->averageGainsOverRange(0, rendererVirtualSource_->fMaxThetaIndex, 0, rendererVirtualSource_->fMaxThetaIndex, tmp);
    int two = rendererVirtualSource_->fVirtualSources->averageGainsOverRange(0, rendererVirtualSource_->fMaxThetaIndex / 2, 0, rendererVirtualSource_->fMaxThetaIndex, tmp);

    two += rendererVirtualSource_->fVirtualSources->averageGainsOverRange(rendererVirtualSource_->fMaxThetaIndex / 2 + 1,  rendererVirtualSource_->fMaxThetaIndex, 0, rendererVirtualSource_->fMaxThetaIndex, tmp) ;

    EXPECT_EQ(one, two);
}

TEST_F(VirtualSourcesTreeTest, AverageGainsOverRangeOneTop)
{
    std::vector<float> tmp(rendererVirtualSource_->fVirtualSources->fRoot.fSpeakerGains.size(), 0.0f);
    int one = rendererVirtualSource_->fVirtualSources->averageGainsOverRange(rendererVirtualSource_->fMaxThetaIndex,  rendererVirtualSource_->fMaxThetaIndex, 0, rendererVirtualSource_->fMaxThetaIndex, tmp);

    EXPECT_EQ(one, 1);
}

TEST_F(VirtualSourcesTreeTest, AverageGainsOverRangeOneBottom)
{
    std::vector<float> tmp(rendererVirtualSource_->fVirtualSources->fRoot.fSpeakerGains.size(), 0.0f);
    int one = rendererVirtualSource_->fVirtualSources->averageGainsOverRange(0, 0, 0, rendererVirtualSource_->fMaxThetaIndex, tmp);

    EXPECT_EQ(one, 1);
}
