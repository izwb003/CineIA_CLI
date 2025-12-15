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

#include "coreutils/Matrix3.h"

#include "coreutils/CoreDefines.h"

namespace CoreUtils
{

    float Matrix3::invert()
    {
        Matrix3 src = *this;
        return invert(src.m, this->m);
    }

    float Matrix3::invert(float src[3][3], float dest[3][3])
    {

        float dtrmnnt =
            +src[0][0] * (src[1][1] * src[2][2] - src[2][1] * src[1][2])
            -src[0][1] * (src[1][0] * src[2][2] - src[1][2] * src[2][0])
            +src[0][2] * (src[1][0] * src[2][1] - src[1][1] * src[2][0]);

        if (fabs(dtrmnnt) > kEPSILON)
        {

            float inverse = 1/dtrmnnt;

            dest[0][0] = inverse *  (src[1][1] * src[2][2] - src[2][1] * src[1][2]);
            dest[0][1] = inverse * -(src[0][1] * src[2][2] - src[0][2] * src[2][1]);
            dest[0][2] = inverse *  (src[0][1] * src[1][2] - src[0][2] * src[1][1]);
            dest[1][0] = inverse * -(src[1][0] * src[2][2] - src[1][2] * src[2][0]);
            dest[1][1] = inverse *  (src[0][0] * src[2][2] - src[0][2] * src[2][0]);
            dest[1][2] = inverse * -(src[0][0] * src[1][2] - src[1][0] * src[0][2]);
            dest[2][0] = inverse *  (src[1][0] * src[2][1] - src[2][0] * src[1][1]);
            dest[2][1] = inverse * -(src[0][0] * src[2][1] - src[2][0] * src[0][1]);
            dest[2][2] = inverse *  (src[0][0] * src[1][1] - src[1][0] * src[0][1]);
        }

        return dtrmnnt;
    }
} // CoreUtils