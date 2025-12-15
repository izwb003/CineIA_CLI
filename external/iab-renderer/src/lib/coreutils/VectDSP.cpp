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
 *
 * Platform-specific implementations of accelerated vector math functions.
 *
 * @file
 */


#include "coreutils/VectDSP.h"

namespace CoreUtils
{

    void VectDSP::add(const float *iVectorA,
                      const float *iVectorB,
                      float *oVector,
                      long iLength)
    {
        for (long i = 0; i < iLength; i++)
        {
            *oVector++ = *iVectorA++ + *iVectorB++;
        }
    }

    void VectDSP::mult(const float *iVectorA,
                       const float *iVectorB,
                       float *oVector,
                       long  iLength)
    {
        for (long i = 0; i < iLength; i++)
        {
            *oVector++ = *iVectorA++ **iVectorB++;
        }
    }

    void VectDSP::ramp(const float iStartValue,
                       const float iEndValue,
                       float *oVector,
                       long iLength)
    {
        // Exit early for the following case.
        // This prevents any division-by-0 error that may occur in "s = i / rampLength;" of the loop below.
        if (iLength < 2)
        {
            *oVector = iStartValue;
            return;
        }

        // Shortening ramp length by one to make room for target value.
        //
        float rampLength = static_cast<float>(iLength - 1);

        float s = 0;

        for (long i = 0; i < iLength; i++)
        {
            s = i / rampLength;

            *oVector++ = (iStartValue * (1.0f - s)) + (iEndValue * s);
        }
    }

    void VectDSP::fill(const float iFillValue,
                       float *oVector,
                       long iLength)
    {
        for (long i=0; i < iLength; i++)
        {
            *oVector++ = iFillValue;
        }
    }

} // CoreUtils
