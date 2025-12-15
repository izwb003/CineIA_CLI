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

    TEST(RiceResidual, Quotient)
    {
        dlc::RiceResidual rr;

        /* default value */

        ASSERT_EQ(rr.getQuotient(), 0);

        /* other tests */

        rr.setQuotient(3);

        ASSERT_EQ(rr.getQuotient(), 3);
    }

    TEST(RiceResidual, Order)
    {
        dlc::RiceResidual rr;

        /* default value */

        ASSERT_EQ(rr.getRemainder(), 0);

        /* other tests */

        rr.setRemainder(3);

        ASSERT_EQ(rr.getRemainder(), 3);
    }

    TEST(RiceResidual, Sign)
    {
        dlc::RiceResidual rr;

        /* default value */

        ASSERT_EQ(rr.getSign(), 1);

        /* other tests */

        rr.setSign(-1);

        ASSERT_EQ(rr.getSign(), -1);

        rr.setSign(-10);

        ASSERT_EQ(rr.getSign(), -1);

        rr.setSign(10);

        ASSERT_EQ(rr.getSign(), 1);

        rr.setSign(1);

        ASSERT_EQ(rr.getSign(), 1);

        ASSERT_THROW(rr.setSign(0), std::out_of_range);
    }

}
