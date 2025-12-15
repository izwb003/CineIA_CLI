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

#pragma once

#include <string.h>
#include "coreutils/Vector3.h"

namespace CoreUtils
{

    class Matrix3
    {
    public:

        Matrix3()
        {
        }

        Matrix3(float m0[3][3])
        {
            memcpy(m, m0, sizeof(m));
        }

        Matrix3(Vector3 v1, Vector3 v2, Vector3 v3)
        {
            m[0][0] = v1.getX();
            m[0][1] = v1.getY();
            m[0][2] = v1.getZ();
            m[1][0] = v2.getX();
            m[1][1] = v2.getY();
            m[1][2] = v2.getZ();
            m[2][0] = v3.getX();
            m[2][1] = v3.getY();
            m[2][2] = v3.getZ();
        }

        ~Matrix3(void)
        {
        }

        inline Vector3 operator*(const Vector3& v) const
        {
            float x = v.getX();
            float y = v.getY();
            float z = v.getZ();

            return Vector3(x*m[0][0] + y*m[1][0] + z*m[2][0],
                           x*m[0][1] + y*m[1][1] + z*m[2][1],
                           x*m[0][2] + y*m[1][2] + z*m[2][2]);
        }

        float invert();

    private:
        static float invert(float src[3][3], float dest[3][3]);

        float m[3][3];
    };

} // CoreUtils

