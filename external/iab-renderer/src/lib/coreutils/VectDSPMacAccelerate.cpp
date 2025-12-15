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


#include "coreutils/VectDSPMacAccelerate.h"

#include <Accelerate/Accelerate.h>

#define DSP_LENGTH_TYPE vDSP_Length

namespace CoreUtils
{

    /**
     *
     * VectDSPMacAccelerate allocates a buffer of type double to store the results of
     * VectDSPMacAccelerate::ramp.
     *
     * See VectDSPMacAccelerate::ramp for additional information about performance.
     */
    VectDSPMacAccelerate::VectDSPMacAccelerate(int pMaxVectorSize)
    {
        buffer_ = new double[pMaxVectorSize];
    }

    VectDSPMacAccelerate::~VectDSPMacAccelerate()
    {
        delete[] buffer_;
    }

    void VectDSPMacAccelerate::add(const float *iVectorA,
                                   const float *iVectorB,
                                   float *oVector,
                                   long iLength)
    {
        vDSP_vadd(iVectorA, 1, iVectorB, 1, oVector, 1, static_cast<DSP_LENGTH_TYPE>(iLength));
    }

    void VectDSPMacAccelerate::mult(const float *iVectorA,
                                    const float *iVectorB,
                                    float *oVector,
                                    long  iLength)
    {
        vDSP_vmul(iVectorA, 1, iVectorB, 1, oVector, 1, static_cast<DSP_LENGTH_TYPE>(iLength));
    }

    /**
     *
     * Accelerated vector based ramping uses the double precision Mac Accelerate API.
     *
     * Since the core VectDSPInterface interface is based in 32-bit floating point,
     * the Mac Accelerate API for converting the resulting vector from double to float is required.
     *
     * Note: The implementation of VectDSPMacAccelerate::ramp compared to VectDSPMacAccelerateFloat32::ramp
     * is approximately 1 order of magnitude slower in computation time. The precision is more accurate.
     *
     */
    void VectDSPMacAccelerate::ramp(const float iStartValue,
                                    const float iEndValue,
                                    float *oVector,
                                    long iLength)
    {
        // Exit early for the following case.
        // This prevents any division-by-0 error that may occur.
        if (iLength < 2)
        {
            *oVector = iStartValue;
            return;
        }

        double start = static_cast<double>(iStartValue);
        double end = static_cast<double>(iEndValue);

        vDSP_vgenD(&start, &end, buffer_, 1,static_cast<DSP_LENGTH_TYPE>(iLength));

        vDSP_vdpsp(buffer_, 1, oVector, 1, static_cast<DSP_LENGTH_TYPE>(iLength));
    }

    void VectDSPMacAccelerate::fill(const float iFillValue,
                                    float *oVector,
                                    long iLength)
    {
        vDSP_vfill(&iFillValue, oVector, 1, static_cast<DSP_LENGTH_TYPE>(iLength));
    }







    VectDSPMacAccelerateFloat32::VectDSPMacAccelerateFloat32() : VectDSPMacAccelerate(0)
    {
    }

    VectDSPMacAccelerateFloat32::~VectDSPMacAccelerateFloat32()
    {
    }

    void VectDSPMacAccelerateFloat32::ramp(const float iStartValue,
                                           const float iEndValue,
                                           float *oVector,
                                           long iLength)
    {
        // Exit early for the following case.
        // This prevents any division-by-0 error that may occur.
        if (iLength < 2)
        {
            *oVector = iStartValue;
            return;
        }

        float diff = iEndValue - iStartValue;
        // vDSP_ramp calculates: c(nk) = a + nb w. n = {0 ... N-1};
        //
        float slope = diff / (iLength - 1);

        // Increasing slope so that target value is reached at N-1.
        //
        vDSP_vramp(&iStartValue, &slope, oVector, 1, static_cast<DSP_LENGTH_TYPE>(iLength));

        // Clamping end value to make sure slope didn't make the end of the ramp overflow.
        //
        oVector[iLength - 1] = iEndValue;
    }

} // CoreUtils
