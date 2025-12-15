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

/**
 * VBAP Renderer
 *
 * Simple 3d vector class
 *
 * @file
 * @author Pierre-Anthony Lemieux
 */

#pragma once

#include <ostream>
#include <math.h>

namespace CoreUtils
{


    class Vector3
    {

    public:

        // Floating-point cartesian coordinates
        float x,y,z;

        Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0)
        {
        }

        Vector3() : x(0), y(0), z(0)
        {
        }

        ~Vector3(void)
        {
        }

        inline float getX() const
        {
            return x;
        }
        inline float getY() const
        {
            return y;
        }
        inline float getZ() const
        {
            return z;
        }

        inline float norm() const
        {
            return sqrtf(x*x + y*y + z*z);
        }

        inline float dot(const Vector3& v0) const
        {
            return x * v0.x + y * v0.y + z * v0.z;
        }

        inline Vector3 operator/(const float d) const
        {
            return Vector3(x/d, y/d, z/d);
        }

        Vector3 operator+(const Vector3& v0) const
        {
            return Vector3( x+v0.x, y+v0.y, z+v0.z );
        }

        bool operator==(const Vector3& v0) const
        {
            return ((x == v0.x) && (y == v0.y) && (z == v0.z));
        }
    };

} // CoreUtils
