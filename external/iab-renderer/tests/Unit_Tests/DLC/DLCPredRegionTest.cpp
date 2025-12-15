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

    TEST(PredRegion, RegionLength)
    {
        dlc::PredRegion pr;

        /* default value */

        ASSERT_EQ(pr.getRegionLength(), 0);

        /* other tests */

        pr.setRegionLength(3);

        ASSERT_EQ(pr.getRegionLength(), 3);

        ASSERT_THROW(pr.setRegionLength(32), std::out_of_range);
    }

    TEST(PredRegion, Order)
    {
        dlc::PredRegion pr;

        /* default value */

        ASSERT_EQ(pr.getOrder(), 0);

        /* other tests */

        pr.setOrder(3);

        ASSERT_EQ(pr.getOrder(), 3);

        ASSERT_THROW(pr.setOrder(43), std::out_of_range);
    }

    TEST(PredRegion, KCoeff)
    {
        dlc::PredRegion pr;

        /* default value */

        for (int32_t i = 0; i < 32; i++)
        {
            ASSERT_EQ(pr.getKCoeff()[i], 0);
        }

        /* set/get */

        dlc::uint10_t coeffs[32];
        std::fill(coeffs, coeffs + sizeof(coeffs) / sizeof(coeffs[0]), (dlc::uint10_t) 1);
        pr.setKCoeff(coeffs);

        for (int32_t i = 0; i < 32; i++)
        {
            ASSERT_EQ(pr.getKCoeff()[i], 1);
        }
    }

}
