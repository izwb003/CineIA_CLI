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

    TEST(AudioDataTest, SetShiftBits)
    {
        dlc::AudioData ad;

        /* default value */

        ASSERT_EQ(ad.getShiftBits(), 0);

        /* other tests */

        ad.setShiftBits(3);

        ASSERT_EQ(ad.getShiftBits(), 3);

        ASSERT_THROW(ad.setShiftBits(32), std::invalid_argument);
    }

    TEST(AudioDataTest, SetSampleRate)
    {
        dlc::AudioData ad;

        /* default value */

        ASSERT_EQ(ad.getSampleRate(), dlc::eSampleRate_48000);

        /* other tests */

        ad.setSampleRate(dlc::eSampleRate_48000);

        ASSERT_EQ(ad.getSampleRate(), dlc::eSampleRate_48000);

        ad.setSampleRate(dlc::eSampleRate_96000);

        ASSERT_EQ(ad.getSampleRate(), dlc::eSampleRate_96000);
    }

    TEST(AudioDataTest, PredRegion48)
    {
        dlc::AudioData ad;

        /* default value */

        ASSERT_EQ(ad.getNumPredRegions48(), 0);

        /* other 48 kHz tests */

        dlc::uint2_t numPredRegions48 = 3;

        ad.setNumPredRegions48(numPredRegions48);

        ASSERT_EQ(ad.getNumPredRegions48(), numPredRegions48);

        for (dlc::uint2_t i = 0; i < numPredRegions48; i++)
        {
            ASSERT_NO_THROW(ad.getPredRegion48(i));
        }

        ASSERT_THROW(ad.getPredRegion48(numPredRegions48), std::out_of_range);

        ASSERT_THROW(ad.setNumPredRegions48(5), std::out_of_range);

        /* test persistence and resizing */

        dlc::PredRegion &p = ad.getPredRegion48(0);

        p.setOrder(1);
        p.setRegionLength(2);
        dlc::uint10_t coeffs[32];
        std::fill(coeffs, coeffs + sizeof(coeffs) / sizeof(coeffs[0]), (dlc::uint10_t) 1);
        p.setKCoeff(coeffs);

        ad.setNumPredRegions48(1);
        ad.setNumPredRegions48(2);

        ASSERT_EQ(p.getOrder(), 1);
        ASSERT_EQ(p.getRegionLength(), 2);
        for (size_t i = 0; i < sizeof(coeffs) / sizeof(coeffs[0]); i++)
        {
            ASSERT_EQ(p.getKCoeff()[i], 1);
        }
    }

    TEST(AudioDataTest, PredRegion96)
    {
        dlc::AudioData ad;

        /* default value */

        ASSERT_EQ(ad.getNumPredRegions96(), 0);

        /* other 96 kHz tests */

        dlc::uint2_t numPredRegions96 = 3;

        ad.setNumPredRegions96(numPredRegions96);

        ASSERT_EQ(ad.getNumPredRegions96(), numPredRegions96);

        for (dlc::uint2_t i = 0; i < numPredRegions96; i++)
        {
            ASSERT_NO_THROW(ad.getPredRegion96(i));
        }

        ASSERT_THROW(ad.getPredRegion96(numPredRegions96), std::out_of_range);

        ASSERT_THROW(ad.setNumPredRegions96(5), std::out_of_range);

        /* test persistence and resizing*/

        dlc::PredRegion &p = ad.getPredRegion96(0);

        p.setOrder(1);
        p.setRegionLength(2);
        dlc::uint10_t coeffs[32];
        std::fill(coeffs, coeffs + sizeof(coeffs) / sizeof(coeffs[0]), (dlc::uint10_t) 1);
        p.setKCoeff(coeffs);

        ad.setNumPredRegions96(1);
        ad.setNumPredRegions96(2);

        ASSERT_EQ(p.getOrder(), 1);
        ASSERT_EQ(p.getRegionLength(), 2);
        for (size_t i = 0; i < sizeof(coeffs) / sizeof(coeffs[0]); i++)
        {
            ASSERT_EQ(p.getKCoeff()[i], 1);
        }
    }

    TEST(AudioDataTest, SubBlock)
    {
        dlc::AudioData ad;

        ad.setSampleRate(dlc::eSampleRate_96000);

        /* default value */

        ASSERT_EQ(ad.getNumDLCSubBlocks(), 0);

        /* set number of subblocks */

        uint8_t numsb = 4;

        ad.setNumDLCSubBlocks(numsb);

        ASSERT_EQ(ad.getNumDLCSubBlocks(), numsb);

        /* can all be retrieved */

        for (uint8_t i = 0; i < numsb; i++)
        {
            ASSERT_NO_THROW(ad.getDLCSubBlock48(i));
            ASSERT_NO_THROW(ad.getDLCSubBlock96(i));

            ASSERT_EQ(NULL, ad.getDLCSubBlock48(i));
            ASSERT_EQ(NULL, ad.getDLCSubBlock96(i));
        }

        /* check if retrieving out of range */

        ASSERT_THROW(ad.getDLCSubBlock48(numsb), std::out_of_range);
        ASSERT_THROW(ad.getDLCSubBlock96(numsb), std::out_of_range);

        /* test persistence and resizing */

        ad.initDLCSubBlock48(0, dlc::eCodeType_PCM_CODE_TYPE, 200);
        ad.initDLCSubBlock48(1, dlc::eCodeType_RICE_CODE_TYPE, 100);

        static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock48(0))->setBitDepth(1);
        static_cast<dlc::RiceResidualsSubBlock*>(ad.getDLCSubBlock48(1))->setRiceRemBits(1);

        ad.initDLCSubBlock96(0, dlc::eCodeType_PCM_CODE_TYPE, 400);
        ad.initDLCSubBlock96(1, dlc::eCodeType_RICE_CODE_TYPE, 200);

        static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock96(0))->setBitDepth(1);
        static_cast<dlc::RiceResidualsSubBlock*>(ad.getDLCSubBlock96(1))->setRiceRemBits(1);

        ad.setNumDLCSubBlocks(8);
        ad.setNumDLCSubBlocks(2);

        ASSERT_EQ(static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock48(0))->getBitDepth(), 1);
        ASSERT_EQ(ad.getDLCSubBlock48(0)->getSize(), 200);

        ASSERT_EQ((static_cast<dlc::RiceResidualsSubBlock*>(ad.getDLCSubBlock48(1))->getRiceRemBits()), 1);
        ASSERT_EQ(ad.getDLCSubBlock48(1)->getSize(), 100);

        ASSERT_EQ(static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock96(0))->getBitDepth(), 1);
        ASSERT_EQ(ad.getDLCSubBlock96(0)->getSize(), 400);

        ASSERT_EQ((static_cast<dlc::RiceResidualsSubBlock*>(ad.getDLCSubBlock96(1))->getRiceRemBits()), 1);
        ASSERT_EQ(ad.getDLCSubBlock96(1)->getSize(), 200);

        /* test sample length of the audio data */

        ASSERT_EQ(300, ad.getSampleCount48());

        ASSERT_EQ(600, ad.getSampleCount96());
    }

}
