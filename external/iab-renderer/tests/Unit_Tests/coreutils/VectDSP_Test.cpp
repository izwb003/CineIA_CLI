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
// VectDSP_Test.cpp
// Unit Test
//
/**
 *
 * Test Objective
 *
 *  - Exercise and validate each of the vector operations against precomputed
 *    outputs (test descriptions provided at methods)
 *
 * Preconditions
 *
 *  - None, self-contained
 *
 * Test Steps
 *
 *  - No additional detail, see test comments
 *
 * Expected Results
 *
 *  - Normal cases exercised and produce verified, pre-calculated
 *    results
 *
 */

#include "gtest/gtest.h"
#include <math.h>
#include <stdint.h>

#ifdef __APPLE__
#define USE_MAC_ACCELERATE
#endif

#include "coreutils/VectDSPInterface.h"

#ifdef USE_MAC_ACCELERATE
#include "coreutils/VectDSPMacAccelerate.h"
#endif

#include "coreutils/VectDSP.h"

#define RAMPBUFFERSIZE	4800
#define TESTBUFFERSIZE	32767

using namespace CoreUtils;

namespace
{

    /**
     *
     * VectDSPCase_Add tests the add function of the VectDSPInterface.
     * The non accelerated implementation is tested on all platforms and mac accelerated implementation is tested on mac only
     *
     * add(const float *iVectorA, const float *iVectorB, float *oVector, long iLength)
     *
     * The add function adds together input vectors and write results to the output vector. iVectorA, iVectorB, oVector
     * are pointers to the input and output vectors respectively. iLength is the length of vectors
     *
     * Test:
     * 1. add two inputs with length = TESTBUFFERSIZE
     * 2. add two inputs with length = 0
     *
     */

    TEST(VectDSP, VectDSPCase_Add)
    {
        VectDSPInterface *nonAccelerated = new VectDSP();

        long length = TESTBUFFERSIZE;
        float vectorA[TESTBUFFERSIZE];				// Input vector A
        float vectorB[TESTBUFFERSIZE];				// Input vector B
        float resultsBuffer[TESTBUFFERSIZE];

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            vectorA[i] = i * 0.25f;
            vectorB[i] = i * 0.5f;
            resultsBuffer[i] = (float) i;			// Pre-set value, expect this to be over-written by add
        }

        // Length = TESTBUFFERSIZE
        nonAccelerated->add(vectorA, vectorB, resultsBuffer, length);

        // Check 3 sample points
        EXPECT_FLOAT_EQ(resultsBuffer[1000], 750.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[10000], 7500.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[32766], 24574.5f);

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;			// Pre-set value, this should NOT be over-written by add with length 0
        }

        // Special cases: Length = 0
        nonAccelerated->add(vectorA, vectorB, resultsBuffer, 0);

        // Expect buffer unchanged
        EXPECT_FLOAT_EQ(resultsBuffer[1000], 1000.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[10000], 10000.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[32766], 32766.0f);

        delete nonAccelerated;

#ifdef USE_MAC_ACCELERATE
        // Do same test with mac accelerate frame work
        VectDSPInterface *macAccelerated = new VectDSPMacAccelerate(TESTBUFFERSIZE);

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;
        }

        // Length = TESTBUFFERSIZE
        macAccelerated->add(vectorA, vectorB, resultsBuffer, length);

        // Check 3 sample points
        EXPECT_FLOAT_EQ(resultsBuffer[1000], 750.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[10000], 7500.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[32766], 24574.5f);

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;
        }

        // Special cases: Length = 0
        macAccelerated->add(vectorA, vectorB, resultsBuffer, 0);

        // Expect buffer unchanged, check 3 sample points
        EXPECT_FLOAT_EQ(resultsBuffer[1000], 1000.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[10000], 10000.0f);
        EXPECT_FLOAT_EQ(resultsBuffer[32766], 32766.0f);

        delete macAccelerated;
#endif
    }

    /**
     *
     * VectDSPCase_Mult tests the multiply function of the VectDSPInterface.
     * The non accelerated implementation is tested on all platforms and mac accelerated implementation is tested on mac only
     *
     * mult(const float *iVectorA, const float *iVectorB, float *oVector, long iLength)
     *
     * The mult function multiplies input vectors and write results to the output vector. iVectorA, iVectorB, oVector
     * are pointers to the input and output vectors respectively. iLength is the length of vectors
     *
     * Test:
     * 1. multiply two inputs with length = TESTBUFFERSIZE
     * 2. multiply two inputs with length = 0
     *
     */

    TEST(VectDSP, VectDSPCase_Mult)
    {
        VectDSPInterface *nonAccelerated = new VectDSP();

        long length = TESTBUFFERSIZE;
        float vectorA[TESTBUFFERSIZE];						// Input vector A
        float vectorB[TESTBUFFERSIZE];						// Input vector B
        float resultsBuffer[TESTBUFFERSIZE];

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            vectorA[i] = i * 0.25f;
            vectorB[i] = i  * 0.5f;
            resultsBuffer[i] = (float) i;
        }

        // Length = TESTBUFFERSIZE
        nonAccelerated->mult(vectorA, vectorB, resultsBuffer, length);

        // Check 3 sample points
        EXPECT_LT((fabs(resultsBuffer[1000] - 125000.0f) / 125000.0f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[10000] - 12500000.0f) / 12500000.0f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[32766] - 134201344.5f) / 134201344.5f), 0.001f);

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;					// Pre-set value, this should NOT be over-written by add with length 0
        }

        // Special cases: Length = 0
        nonAccelerated->mult(vectorA, vectorB, resultsBuffer, 0);

        // Expect buffer unchanged, check 3 sample points
        EXPECT_EQ(resultsBuffer[1000], 1000.0f);
        EXPECT_EQ(resultsBuffer[10000], 10000.0f);
        EXPECT_EQ(resultsBuffer[32766], 32766.0f);

        delete nonAccelerated;

#ifdef USE_MAC_ACCELERATE
        // Do same test with mac accelerate frame work
        VectDSPInterface *macAccelerated = new VectDSPMacAccelerate(TESTBUFFERSIZE);

        macAccelerated->mult(vectorA, vectorB, resultsBuffer, length);

        // Check 3 sample points
        EXPECT_LT((fabs(resultsBuffer[1000] - 125000.0f) / 125000.0f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[10000] - 12500000.0f) / 12500000.0f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[32766] - 134201344.5f) / 134201344.5f), 0.001f);

        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;					// Pre-set value, this should NOT be over-written by add with length 0
        }

        // Special cases: Length = 0
        macAccelerated->mult(vectorA, vectorB, resultsBuffer, 0);

        // Expect buffer unchanged, check 3 sample points
        EXPECT_EQ(resultsBuffer[1000], 1000.0f);
        EXPECT_EQ(resultsBuffer[10000], 10000.0f);
        EXPECT_EQ(resultsBuffer[32766], 32766.0f);

        delete macAccelerated;
#endif
    }

    /**
     *
     * VectDSPCase_Ramp tests the ramp function of the VectDSPInterface.
     * The non accelerated implementation is tested on all platforms and mac accelerated implementation is tested on mac only
     *
     * ramp(const float iStartValue, const float iEndValue, float *oVector, long iLength)
     *
     * The ramp function fill a float array with constant stepped values ramping from iStartValue to
     * iEndValue over iLength locations, start writing from pointer position oVector
     *
     * Test:
     * 1. ramping from 0 to 1.0 over 4800 samples
     * 2. ramping with length = 0
     * 3. ramping with length = 1
     */

    TEST(VectDSP, VectDSPCase_Ramp)
    {
        VectDSPInterface *nonAccelerated = new VectDSP();

        float cgain = 0.0f;
        float tgain = 1.0f;
        long realRampPeriod = RAMPBUFFERSIZE;

        float resultsBuffer[RAMPBUFFERSIZE];

        for (int32_t i = 0; i < RAMPBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;
        }

        nonAccelerated->ramp(cgain, tgain, resultsBuffer, realRampPeriod);

        // Check 3 sample points
        // Would expect difference to be under 0.1%.
        // Use 1 % as a functional test error threshold
        EXPECT_LT((fabs(resultsBuffer[100] - 0.020833333f) / 0.020833333f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[3000] - 0.625f) / 0.625f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[RAMPBUFFERSIZE - 1] - tgain) / tgain), 0.001f);

        // Special cases
        cgain = 0.5f;

        // zero ramp length, expect first target buffer element to be set to start value
        resultsBuffer[0] = 1.0f;
        nonAccelerated->ramp(cgain, tgain, resultsBuffer, 0);
        EXPECT_FLOAT_EQ(resultsBuffer[0], 0.5f);

        // 1 ramp length, expect first target buffer element to be set to target value
        resultsBuffer[0] = 1.0f;
        nonAccelerated->ramp(cgain, tgain, resultsBuffer, 1);
        EXPECT_FLOAT_EQ(resultsBuffer[0], 0.5f);

        delete nonAccelerated;

#ifdef USE_MAC_ACCELERATE
        // Do same test with mac accelerate frame work
        VectDSPInterface *macAccelerated = new VectDSPMacAccelerate(RAMPBUFFERSIZE);

        cgain = 0.0f;
        tgain = 1.0f;

        for (int32_t i = 0; i < RAMPBUFFERSIZE; i++)
        {
            resultsBuffer[i] = (float) i;
        }

        macAccelerated->ramp(cgain, tgain, resultsBuffer, realRampPeriod);

        // Check 3 sample points
        // Would expect difference to be under 0.1%.
        // Use 1 % as a functional test error threshold
        EXPECT_LT((fabs(resultsBuffer[100] - 0.020833333f) / 0.020833333f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[3000] - 0.625f) / 0.625f), 0.001f);
        EXPECT_LT((fabs(resultsBuffer[RAMPBUFFERSIZE - 1] - tgain) / tgain), 0.001f);

        // Special cases
        cgain = 0.5f;

        // zero ramp length, expect first target buffer element to be set to start value
        resultsBuffer[0] = 1.0f;
        macAccelerated->ramp(cgain, tgain, resultsBuffer, 0);
        EXPECT_FLOAT_EQ(resultsBuffer[0], 0.5f);

        // 1 ramp length, expect first target buffer element to be set to target value
        resultsBuffer[0] = 1.0f;
        macAccelerated->ramp(cgain, tgain, resultsBuffer, 1);
        EXPECT_FLOAT_EQ(resultsBuffer[0], 0.5f);

        delete macAccelerated;
#endif
    }

    /**
     *
     * VectDSPCase_Fill tests the fill function of the VectDSPInterface.
     * The non accelerated implementation is tested on all platforms and mac accelerated implementation is tested on mac only
     *
     * fill(const float iFillValue, float *oVector, long iLength)
     *
     * The fill function fills ilength consecutive locations of the float array with value iFillValue, starting from pointer position oVector
     * Test filling with four different float values: 0, maximum, minimum and lowest
     */

    TEST(VectDSP, VectDSPCase_Fill)
    {
        int32_t fillStartIndex0 = 1000;							// Start index for to fill 0
        int32_t fillStartIndexMax = 2000;						// start index for to fill maximum float value
        int32_t fillStartIndexMin = 3000;						// Start index for to fill minumumm positive float value
        int32_t fillStartIndexLowest = 4000;					// Start index to fill most negative float value
        float fillValue0 = 0.0f;
        float fillValueMax = std::numeric_limits<float>::max();
        float fillValueMin = std::numeric_limits<float>::min();

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
        // older pre-C++11 compiler, e.g. g++- 4.4.7   does not support std::numeric_limits<float>::lowest()
        // std::numeric_limits<float>::lowest() should be same as negative value of std::numeric_limits<float>::max()
        float fillValueLowest = -fillValueMax;
#else
        float fillValueLowest = std::numeric_limits<float>::lowest();
#endif

        float fillValue;
        long fillLength = 1000;
        float fillBuffer[TESTBUFFERSIZE];

        VectDSPInterface *nonAccelerated = new VectDSP();

        // Pre-fill with test pattern;
        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            fillBuffer[i] = (float) i * 0.25f;
        }

        // Over-write blocks with 0, max float, min float and lowest float
        nonAccelerated->fill(fillValue0, &fillBuffer[fillStartIndex0], fillLength);
        nonAccelerated->fill(fillValueMax, &fillBuffer[fillStartIndexMax], fillLength);
        nonAccelerated->fill(fillValueMin, &fillBuffer[fillStartIndexMin], fillLength);
        nonAccelerated->fill(fillValueLowest, &fillBuffer[fillStartIndexLowest], fillLength);

        // check values
        EXPECT_FLOAT_EQ(fillBuffer[500], 125.0f);
        EXPECT_FLOAT_EQ(fillBuffer[1000], fillValue0);
        EXPECT_FLOAT_EQ(fillBuffer[1999], fillValue0);
        EXPECT_FLOAT_EQ(fillBuffer[2000], fillValueMax);
        EXPECT_FLOAT_EQ(fillBuffer[2999], fillValueMax);
        EXPECT_FLOAT_EQ(fillBuffer[3000], fillValueMin);
        EXPECT_FLOAT_EQ(fillBuffer[3999], fillValueMin);
        EXPECT_FLOAT_EQ(fillBuffer[4000], fillValueLowest);
        EXPECT_FLOAT_EQ(fillBuffer[4999], fillValueLowest);
        EXPECT_FLOAT_EQ(fillBuffer[10000], 2500.0f);

        // Special cases

        // Pre-fill with test pattern;
        fillValue = 1.0f;

        for (int32_t i = 0; i < 10; i++)
        {
            fillBuffer[i] = (float) i * 0.25f;
        }

        // Filll length = 0, expect no change to buffer
        nonAccelerated->fill(fillValue, fillBuffer, 0);

        // Check 3 sample points
        EXPECT_FLOAT_EQ(fillBuffer[0], 0.0f);
        EXPECT_FLOAT_EQ(fillBuffer[5], 1.25f);
        EXPECT_FLOAT_EQ(fillBuffer[9], 2.25f);

        delete nonAccelerated;

#ifdef USE_MAC_ACCELERATE

        // Do same test with mac  accelerate framework
        VectDSPInterface *macAccelerated = new VectDSPMacAccelerate(TESTBUFFERSIZE);

        // Pre-fill with test pattern;
        for (int32_t i = 0; i < TESTBUFFERSIZE; i++)
        {
            fillBuffer[i] = (float) i * 0.25f;
        }

        // Over-write blocks with 0, max float, min float and lowest float
        macAccelerated->fill(fillValue0, &fillBuffer[fillStartIndex0], fillLength);
        macAccelerated->fill(fillValueMax, &fillBuffer[fillStartIndexMax], fillLength);
        macAccelerated->fill(fillValueMin, &fillBuffer[fillStartIndexMin], fillLength);
        macAccelerated->fill(fillValueLowest, &fillBuffer[fillStartIndexLowest], fillLength);

        // check values
        EXPECT_FLOAT_EQ(fillBuffer[500], 125.0f);
        EXPECT_FLOAT_EQ(fillBuffer[1000], fillValue0);
        EXPECT_FLOAT_EQ(fillBuffer[1999], fillValue0);
        EXPECT_FLOAT_EQ(fillBuffer[2000], fillValueMax);
        EXPECT_FLOAT_EQ(fillBuffer[2999], fillValueMax);
        EXPECT_FLOAT_EQ(fillBuffer[3000], fillValueMin);
        EXPECT_FLOAT_EQ(fillBuffer[3999], fillValueMin);
        EXPECT_FLOAT_EQ(fillBuffer[4000], fillValueLowest);
        EXPECT_FLOAT_EQ(fillBuffer[4999], fillValueLowest);
        EXPECT_FLOAT_EQ(fillBuffer[10000], 2500.0f);

        // Special cases

        // Pre-fill with test pattern;
        fillValue = 1.0f;

        for (int32_t i = 0; i < 10; i++)
        {
            fillBuffer[i] = (float) i * 0.25f;
        }

        // Filll length = 0, expect no change to buffer
        macAccelerated->fill(fillValue, fillBuffer, 0);

        // Check 3 sample points
        EXPECT_FLOAT_EQ(fillBuffer[0], 0.0f);
        EXPECT_FLOAT_EQ(fillBuffer[5], 1.25f);
        EXPECT_FLOAT_EQ(fillBuffer[9], 2.25f);

        delete macAccelerated;
#endif
    }


#ifdef USE_MAC_ACCELERATE

    /**
     *
     * VectDSPCase_Add_Compare runs the Add function from three different implementations of the VectDSPInterface.
     * The maximum difference is determined.
     *
     */

    TEST(VectDSP, VectDSPCase_Add_Compare)
    {
        int32_t maxVectorSize = 32767;

        VectDSPInterface *nonAccelerated = new VectDSP();
        VectDSPInterface *macAcceleratedDouble = new VectDSPMacAccelerate(maxVectorSize);
        VectDSPInterface *macAcceleratedFloat = new VectDSPMacAccelerateFloat32();

        float vgains_NonAccelerated1[maxVectorSize];
        float vgains_AcceleratedDouble1[maxVectorSize];
        float vgains_AcceleratedFloat1[maxVectorSize];

        float vgains_NonAccelerated2[maxVectorSize];
        float vgains_AcceleratedDouble2[maxVectorSize];
        float vgains_AcceleratedFloat2[maxVectorSize];

        float vgains_NonAcceleratedResult[maxVectorSize];
        float vgains_AcceleratedDoubleResult[maxVectorSize];
        float vgains_AcceleratedFloatResult[maxVectorSize];

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            vgains_NonAccelerated1[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble1[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat1[i] = (float) (i + 1) * 0.25f;

            vgains_NonAccelerated2[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble2[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat2[i] = (float) (i + 1) * 0.25f;
        }

        nonAccelerated->add(vgains_NonAccelerated1, vgains_NonAccelerated2, vgains_NonAcceleratedResult, maxVectorSize);
        macAcceleratedDouble->add(vgains_AcceleratedDouble1, vgains_AcceleratedDouble2, vgains_AcceleratedDoubleResult, maxVectorSize);
        macAcceleratedFloat->add(vgains_AcceleratedFloat1, vgains_AcceleratedFloat2, vgains_AcceleratedFloatResult, maxVectorSize);

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            float diff = 0.0f;

            diff = vgains_NonAcceleratedResult[i] - vgains_AcceleratedDoubleResult[i];
            EXPECT_LT(fabsf(diff / vgains_NonAcceleratedResult[i]), 0.001f);

            diff = vgains_NonAcceleratedResult[i] - vgains_AcceleratedFloatResult[i];
            EXPECT_LT(fabsf(diff / vgains_NonAcceleratedResult[i]), 0.001f);

            diff = vgains_AcceleratedDoubleResult[i] - vgains_AcceleratedFloatResult[i];
            EXPECT_LT(fabsf(diff / vgains_AcceleratedDoubleResult[i]), 0.001f);
        }

        delete nonAccelerated;
        delete macAcceleratedDouble;
        delete macAcceleratedFloat;
    }

    /**
     *
     * VectDSPCase_Mult_Compare runs the mult function from three different implementations of the VectDSPInterface.
     * The maximum difference is determined.
     *
     */

    TEST(VectDSP, VectDSPCase_Mult_Compare)
    {
        int32_t maxVectorSize = 32767;

        VectDSPInterface *nonAccelerated = new VectDSP();
        VectDSPInterface *macAcceleratedDouble = new VectDSPMacAccelerate(maxVectorSize);
        VectDSPInterface *macAcceleratedFloat = new VectDSPMacAccelerateFloat32();


        float vgains_NonAccelerated1[maxVectorSize];
        float vgains_AcceleratedDouble1[maxVectorSize];
        float vgains_AcceleratedFloat1[maxVectorSize];

        float vgains_NonAccelerated2[maxVectorSize];
        float vgains_AcceleratedDouble2[maxVectorSize];
        float vgains_AcceleratedFloat2[maxVectorSize];

        float vgains_NonAcceleratedResult[maxVectorSize];
        float vgains_AcceleratedDoubleResult[maxVectorSize];
        float vgains_AcceleratedFloatResult[maxVectorSize];

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            vgains_NonAccelerated1[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble1[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat1[i] = (float) (i + 1) * 0.25f;

            vgains_NonAccelerated2[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble2[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat2[i] = (float) (i + 1) * 0.25f;
        }

        nonAccelerated->mult(vgains_NonAccelerated1, vgains_NonAccelerated2, vgains_NonAcceleratedResult, maxVectorSize);
        macAcceleratedDouble->mult(vgains_AcceleratedDouble1, vgains_AcceleratedDouble2, vgains_AcceleratedDoubleResult, maxVectorSize);
        macAcceleratedFloat->mult(vgains_AcceleratedFloat1, vgains_AcceleratedFloat2, vgains_AcceleratedFloatResult, maxVectorSize);

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            float diff = 0.0f;

            diff = vgains_NonAcceleratedResult[i] - vgains_AcceleratedDoubleResult[i];
            EXPECT_LT(fabsf(diff / vgains_NonAcceleratedResult[i]), 0.001f);

            diff = vgains_NonAcceleratedResult[i] - vgains_AcceleratedFloatResult[i];
            EXPECT_LT(fabsf(diff / vgains_NonAcceleratedResult[i]), 0.001f);

            diff = vgains_AcceleratedDoubleResult[i] - vgains_AcceleratedFloatResult[i];
            EXPECT_LT(fabsf(diff / vgains_AcceleratedDoubleResult[i]), 0.001f);
        }

        delete nonAccelerated;
        delete macAcceleratedDouble;
        delete macAcceleratedFloat;
    }

    /**
     *
     * VectDSPCase_Ramp_Compare runs the ramp function from three different implementations of the VectDSPInterface.
     * The maximum difference is determined.
     *
     */

    TEST(VectDSP, VectDSPCase_Ramp_Compare)
    {
        int32_t maxVectorSize = 32767;

        VectDSPInterface *nonAccelerated = new VectDSP();
        VectDSPInterface *macAcceleratedDouble = new VectDSPMacAccelerate(maxVectorSize);
        VectDSPInterface *macAcceleratedFloat = new VectDSPMacAccelerateFloat32();

        float cgain = 0.1f;
        float tgain = 0.9f;
        long realRampPeriod = 4800;
        float vgains_NonAccelerated[maxVectorSize];
        float vgains_AcceleratedDouble[maxVectorSize];
        float vgains_AcceleratedFloat[maxVectorSize];

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            vgains_NonAccelerated[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat[i] = (float) (i + 1) * 0.25f;
        }

        nonAccelerated->ramp(cgain, tgain, vgains_NonAccelerated, realRampPeriod);
        macAcceleratedDouble->ramp(cgain, tgain, vgains_AcceleratedDouble, realRampPeriod);
        macAcceleratedFloat->ramp(cgain, tgain, vgains_AcceleratedFloat, realRampPeriod);

        for (int32_t i = 0; i < realRampPeriod; i++)
        {
            float diff = 0.0f;

            diff = vgains_NonAccelerated[i] - vgains_AcceleratedDouble[i];
            EXPECT_LT(fabsf(diff / vgains_NonAccelerated[i]), 0.001f);

            diff = vgains_NonAccelerated[i] - vgains_AcceleratedFloat[i];
            EXPECT_LT(fabsf(diff / vgains_NonAccelerated[i]), 0.001f);

            diff = vgains_AcceleratedDouble[i] - vgains_AcceleratedFloat[i];
            EXPECT_LT(fabsf(diff / vgains_AcceleratedDouble[i]), 0.001f);
        }

        delete nonAccelerated;
        delete macAcceleratedDouble;
        delete macAcceleratedFloat;
    }

    /**
     *
     * VectDSPCase_Fill_Compare runs the fill function from three different implementations of the VectDSPInterface.
     * The maximum difference is determined.
     *
     */

    TEST(VectDSP, VectDSPCase_Fill_Compare)
    {
        int32_t maxVectorSize = 32767;

        VectDSPInterface *nonAccelerated = new VectDSP();
        VectDSPInterface *macAcceleratedDouble = new VectDSPMacAccelerate(maxVectorSize);
        VectDSPInterface *macAcceleratedFloat = new VectDSPMacAccelerateFloat32();

        float fill = 0.0f;

        float vgains_NonAccelerated[maxVectorSize];
        float vgains_AcceleratedDouble[maxVectorSize];
        float vgains_AcceleratedFloat[maxVectorSize];

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            vgains_NonAccelerated[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedDouble[i] = (float) (i + 1) * 0.25f;
            vgains_AcceleratedFloat[i] = (float) (i + 1) * 0.25f;
        }

        nonAccelerated->fill(fill, vgains_NonAccelerated, maxVectorSize);
        macAcceleratedDouble->fill(fill, vgains_AcceleratedDouble, maxVectorSize);
        macAcceleratedFloat->fill(fill, vgains_AcceleratedFloat, maxVectorSize);

        for (int32_t i = 0; i < maxVectorSize; i++)
        {
            float diff = 0.0f;

            diff = vgains_NonAccelerated[i] - vgains_AcceleratedDouble[i];
            EXPECT_FLOAT_EQ(diff, 0.0f);

            diff = vgains_NonAccelerated[i] - vgains_AcceleratedFloat[i];
            EXPECT_FLOAT_EQ(diff, 0.0f);

            diff = vgains_AcceleratedDouble[i] - vgains_AcceleratedFloat[i];
            EXPECT_FLOAT_EQ(diff, 0.0f);
        }

        delete nonAccelerated;
        delete macAcceleratedDouble;
        delete macAcceleratedFloat;
    }

#endif

} // namespace

