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

#include <fstream>      // std::ofstream

#include "gtest/gtest.h"
#include "common/IABConstants.h"
#include "renderer/IABTransform/IABTransformDataType.h"
#include "renderer/IABTransform/IABTransform.h"
#include "math.h"

#define TOL_DISTANCE	0.000001f							// 10^(-6)

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// IAB Transform Tests
// ------------------------------------------------------------------------------------------------


using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
	// Test class for IAB object coordinate transform test

	class IABTransformTest : public testing::Test
	{
	protected:
		virtual void SetUp()
		{
			// Instantiate IABTransform
			iabTransform_ = new IABTransform();
		}

		virtual void TearDown()
		{
			delete iabTransform_;
		}

		// Tests cases/functions

		// Sample tests contain
		// 1) Out-of-range input (IAB coordinates)
		// 2) Two selected positions, eg. front-left corner etc
		//
		void SampleTests()
		{
			float       xIn, yIn, zIn;								// input, IAB x y z coordinates
			float       xOut, yOut, zOut;							// output, transformed MDA x y z from DUT
			float       xExpected, yExpected, zExpected;			// Ref/expected x y z, independently generated using Octave script based on prescribed algorithm
			float       xD, yD, zD;									// To store x y z deviation between output and ref

			// Testing out-of-range IAB coordinates as input

			// Test 1: Out-of range x coordinate, expect error
			//
			xIn = 1.01f;
			yIn = 0.0f;
			zIn = 0.0f;
			EXPECT_NE(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

			// Test 2: Out-of range y coordinate, expect error
			//
			xIn = 0.0f;
			yIn = 1.01f;
			EXPECT_NE(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

			// Test 3: Out-of range z coordinate, expect error
			//
			yIn = 0.0f;
			zIn = 1.01f;
			EXPECT_NE(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

			// Test 4: Reset z to 0, 
			// Lower-front-left corner, valid, in-range position, expect no error
			//
			zIn = 0.0f;
			EXPECT_EQ(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

			// Ref expected transformed values - copy first row of "IABToMDAPositionTransformTests_"
			xExpected = IABToMDAPositionTransformTests_[0][3];
			yExpected = IABToMDAPositionTransformTests_[0][4];
			zExpected = IABToMDAPositionTransformTests_[0][5];

			// Due to cross-platform differences, pass/fail is tolerance/bound based
			// Calculate deviation in each of x, y and z component
			xD = std::fabs(xOut - xExpected);
			yD = std::fabs(yOut - yExpected);
			zD = std::fabs(zOut - zExpected);

			// Deviation in each component must be within prescribed tolerance limit
			EXPECT_LE(xD, tolerance_);
			EXPECT_LE(yD, tolerance_);
			EXPECT_LE(zD, tolerance_);

			// Test 5: Upper-back-right corner, valid, in-range position, expect no error
			//
			xIn = 1.0f;
			yIn = 1.0f;
			zIn = 1.0f;
			EXPECT_EQ(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

			// Ref expected transformed values - copy first row of "IABToMDAPositionTransformTests_"
			xExpected = IABToMDAPositionTransformTests_[727][3];
			yExpected = IABToMDAPositionTransformTests_[727][4];
			zExpected = IABToMDAPositionTransformTests_[727][5];

			// Due to cross-platform differences, pass/fail is tolerance/bound based
			// Calculate deviation in each of x, y and z component
			xD = std::fabs(xOut - xExpected);
			yD = std::fabs(yOut - yExpected);
			zD = std::fabs(zOut - zExpected);

			// Deviation in each component must be within prescribed tolerance limit
			EXPECT_LE(xD, tolerance_);
			EXPECT_LE(yD, tolerance_);
			EXPECT_LE(zD, tolerance_);
		}

		// Test all positions contained in "IABToMDAPositionTransformTests_"
		// 728 positions in total
		//
		void FullSweepThroughRoomTest()
		{
			int32_t     i;
			int32_t     numPositions = sizeof IABToMDAPositionTransformTests_ / sizeof IABToMDAPositionTransformTests_[0];	// 728

			float       xIn, yIn, zIn;								// input, IAB x y z coordinates
			float       xOut, yOut, zOut;							// output, transformed MDA x y z from DUT
			float       xExpected, yExpected, zExpected;			// Ref/expected x y z, independently generated using Octave script based on prescribed algorithm
			float       xD, yD, zD;									// To store x y z deviation between output and ref

			for (i = 0; i < numPositions; i++)
			{
				// Copy in IAB coordinates as input
				xIn = IABToMDAPositionTransformTests_[i][0];
				yIn = IABToMDAPositionTransformTests_[i][1];
				zIn = IABToMDAPositionTransformTests_[i][2];

				// Copy ref/expected transformed values
				xExpected = IABToMDAPositionTransformTests_[i][3];
				yExpected = IABToMDAPositionTransformTests_[i][4];
				zExpected = IABToMDAPositionTransformTests_[i][5];

				// All positions in the array are in range
				EXPECT_EQ(iabTransform_->TransformIABToCartesianVBAP(xIn, yIn, zIn, xOut, yOut, zOut), kIABNoError);

				// Due to cross-platform differences, pass/fail is tolerance/bound based
				// Calculate deviation in each of x, y and z component
				xD = std::fabs(xOut - xExpected);
				yD = std::fabs(yOut - yExpected);
				zD = std::fabs(zOut - zExpected);

				// Deviation in each component must be within prescribed tolerance limit
				//
				EXPECT_LE(xD, tolerance_);
				EXPECT_LE(yD, tolerance_);
				EXPECT_LE(zD, tolerance_);

			}
		}

		// Sample tests contain
		// 1) Out-of-range input (IAB 1d spread)
		// 2) Two selected, in-range 1d spread values
		//
		void Sample1dSpreadTests()
		{
			float       spreadIn;									// input, IAB 1d spread
			float       apertureOut, divergenceOut;					// output, aperture (and divergence = 0 per current algorithm) 
			float       spreadConvertedBack;						// Store converted back spread from apertureOut. Reverse conversion to spread1dToAperture().

			// Testing out-of-range IAB spread as input

			// Test 1: Out-of range x coordinate, expect error
			//
			spreadIn = 1.01f;
			EXPECT_NE(iabTransform_->TransformIAB1DSpreadToVBAPExtent(spreadIn, apertureOut, divergenceOut), kIABNoError);

			// Test 2: test sample value of 0.357
			//
			spreadIn = 0.357f;
			EXPECT_EQ(iabTransform_->TransformIAB1DSpreadToVBAPExtent(spreadIn, apertureOut, divergenceOut), kIABNoError);
			EXPECT_EQ(0.0f, divergenceOut);							// Divergence fixed to 0
			apertureTospread1d(apertureOut, spreadConvertedBack);	// Reverse convert aperture
			EXPECT_LE(std::fabs(spreadConvertedBack - spreadIn), tolerance_);	// Diff either 0 or very close to 0. Use tolerance comparison to allow floatingP discrenpancy

			// Test 3: test sample value of 0.789
			//
			spreadIn = 0.789f;
			EXPECT_EQ(iabTransform_->TransformIAB1DSpreadToVBAPExtent(spreadIn, apertureOut, divergenceOut), kIABNoError);
			EXPECT_EQ(0.0f, divergenceOut);							// Divergence fixed to 0
			apertureTospread1d(apertureOut, spreadConvertedBack);	// Reverse convert aperture
			EXPECT_LE(std::fabs(spreadConvertedBack - spreadIn), tolerance_);	// Diff either 0 or very close to 0. Use tolerance comparison to allow floatingP discrenpancy
		}

		// Full range test spread transform to aperture
		// Granuarity set by stepSpread
		//
		void FullRange1dSpreadTest()
		{
			float       stepSpread = 0.01f;							// Set stepSpread to 0.01f. Full range contains 100 test points.
			float       apertureOut, divergenceOut;					// output, aperture (and divergence = 0 per current algorithm) 
			float       spreadConvertedBack;						// Store converted back spread from apertureOut. Reverse conversion to spread1dToAperture().

			// Test full range of IAB 1d spread [0, 1]
			for (float spreadIn = 0.0f; spreadIn <= 1.0f; (spreadIn += stepSpread))
			{
				EXPECT_EQ(iabTransform_->TransformIAB1DSpreadToVBAPExtent(spreadIn, apertureOut, divergenceOut), kIABNoError);
				EXPECT_EQ(0.0f, divergenceOut);										// Divergence fixed to 0
				apertureTospread1d(apertureOut, spreadConvertedBack);				// Reverse convert aperture
				EXPECT_LE(std::fabs(spreadConvertedBack - spreadIn), tolerance_);	// Diff either 0 or very close to 0. Use tolerance comparison to allow floatingP discrenpancy
			}
		}
        
        // ------------------------------------------------------------------------------------------------
        // Convert each MDA PyraMesa vertex to IAB coordinate and compare against the expected value
        //
        void MDAToIABConversionPyraMesaVerticesTest()
        {
            float iab_x;
            float iab_y;
            float iab_z;
            
            for (int32_t i = 0; i < num_vertices; i++)
            {
				int32_t retval = iabTransform_->TransformCartesianVBAPToIAB(vertices[i][3], vertices[i][4], vertices[i][5], iab_x, iab_y, iab_z);
                
                ASSERT_EQ(retval, 0) << "Incomplete hull";
                
                ASSERT_NEAR(iab_x, (vertices[i][0] + 1)/2, kEPSILON);
                ASSERT_NEAR(iab_y, (-vertices[i][1] + 1)/2, kEPSILON);
                ASSERT_NEAR(iab_z, vertices[i][2], kEPSILON);
            }
        }

        // ------------------------------------------------------------------------------------------------
        // Sweep MDA coordinate through full elevation and azimuth range. As MDA radius = 1.0, the
        // converted IAB coordinates should be on the boundary, i.e. one or more x, y, z components
        // = 0.0 or 1.0
        void MDAToIABConversionBoundsTest()
        {
            float iab_x;
            float iab_y;
            float iab_z;
            
            float mda_x;
            float mda_y;
            float mda_z;
            
            const int32_t elevation_res = 64;
            
            for (int32_t i = 0; i < elevation_res; i++)
            {
                double elevation = ((float)i) / (elevation_res - 1.0) * iabKPI / 2.0;
                
				int32_t azimuth_res = i == (elevation_res - 1) ? 1 : (int32_t) round((double)(elevation_res)* cos(elevation));
                
                for (int32_t j = 0; j < azimuth_res; j++)
                {
                    double azimuth = (((double)j) / azimuth_res) * 2.0 * iabKPI;
                    
                    mda_x = (float) (cos(elevation) * sin(azimuth));
                    mda_y = (float)(cos(elevation) * cos(azimuth));
                    mda_z = (float)(sin(elevation));
                    
					int32_t retval = iabTransform_->TransformCartesianVBAPToIAB(mda_x, mda_y, mda_z, iab_x, iab_y, iab_z);
                    
                    ASSERT_EQ(retval, 0) << "Incomplete hull";
                    
                    ASSERT_TRUE(std::abs(iab_x - 1) < kEPSILON || std::abs(iab_x) < kEPSILON || std::abs(iab_y - 1) < kEPSILON || std::abs(iab_y) < kEPSILON || std::abs(iab_z - 1) < kEPSILON || std::abs(iab_z) < kEPSILON) << "IAB position not on cube surface";
                    
                    ASSERT_FALSE(iab_x > 1.0 + kEPSILON || iab_x < -kEPSILON || iab_y < -kEPSILON || iab_y > 1.0 + kEPSILON || iab_z < -kEPSILON || iab_z > 1.0 + kEPSILON) << "IAB position outside the unit cube";
                    
                    ASSERT_FALSE((mda_x < -kEPSILON && iab_x > 0.5)
                                 || (mda_x > kEPSILON && iab_x < 0.5)
                                 || (mda_y < -kEPSILON && iab_y < 0.5)
                                 || (mda_y > kEPSILON && iab_y > 0.5)) << "IAB position does not match polarity of MDA position";
                    
                }
            }
        }
        
    private:
        
        // Private function used for verifying converted aperture values from
		// TransformIAB1DSpreadToVBAPExtent() (ie forward conversion)
		// Coversion algorithm reverse convert aperture back to 1d spread
		// (For unit test uses only)
		//
		void apertureTospread1d(float iAperture, float &o1dSpread) const
		{
			float aperture = iAperture;

			// convert paerture from radian to unit range first
			aperture /= iabKPI;

			// Reverse convert back to 1d spread
			o1dSpread = (aperture <= 0.25f) ? (aperture * 2.0f) : ((aperture + 0.5f) * 2.0f / 3.0f);
		}

		// member variables

		IABTransform *iabTransform_;				// pointer to an IABTransform instance

    	// IABToMDAPositionTransformTests_
    	// 
    	// {iab_x, iab_y, iab_z, mda_x, mda_y, mda_z}
    	// 
    	// Generated using the pyra-mesa transform
    	// 
    	// Contains 728 transform positions (rows)
    	// Each row contains 6 coodinates (columns). The first 3 represent (x y z) input
    	// in IAB space. Columns 4-6 as (x y z) in MDA normlized room, using pyra-mesa.
    	//
    	static const float IABToMDAPositionTransformTests_[728][6];

		// Test tolerance
		static const float tolerance_;

	};

	// ------------------------------------------------------------------------------------------------
	// Random tests...
	//
	TEST_F(IABTransformTest, Selective_Test)
	{
		SampleTests();
	}

    // ------------------------------------------------------------------------------------------------
	// Sweeping through / test all positions as included in test data array 
	// "IABToMDAPositionTransformTests_"
	//
    TEST_F(IABTransformTest, Full_Sweep_Through_Room_Test)
    {
		FullSweepThroughRoomTest();
	}

	// ------------------------------------------------------------------------------------------------
	// Sample 1d spread tests...
	//
	TEST_F(IABTransformTest, Selective_1dSpread_Test)
	{
		Sample1dSpreadTests();
	}

	// ------------------------------------------------------------------------------------------------
	// Sweeping through / test all positions as included in test data array 
	// "IABToMDAPositionTransformTests_"
	//
	TEST_F(IABTransformTest, Full_Range_1dSpread_Transform_Test)
	{
		FullRange1dSpreadTest();
	}
    
    // ------------------------------------------------------------------------------------------------
    // Test MDA PyraMesa vertices conversion to IAB coordinates
    //
    TEST_F(IABTransformTest, MDA_To_IAB_Conversion_PyraMesaVertices_Test)
    {
        MDAToIABConversionPyraMesaVerticesTest();
    }

    // ------------------------------------------------------------------------------------------------
    // Test MDA coordinate bounds conversion to IAB coordinates
    //
    TEST_F(IABTransformTest, MDA_To_IAB_Conversion_Bounds_Test)
    {
        MDAToIABConversionBoundsTest();
    }


	// =========================================================================================
    // Unit Test data, Input and expected output
    //

	// IABToMDAPositionTransformTests_
	// 
	// {iab_x, iab_y, iab_z, mda_x, mda_y, mda_z}
	// 
	// Generated using the pyra-mesa transform
	// 
	// Contains 728 transform positions (rows)
	// Each row contains 6 coodinates (columns). The first 3 represent (x y z) input
	// in IAB space. Columns 4-6 as (x y z) in MDA normlized room, using pyra-mesa.
	//
	// Array initialization
	const float IABTransformTest::IABToMDAPositionTransformTests_[728][6] = {
		{ 0.000000f, 0.000000f, 0.000000f, -0.500000f, 0.866025f, 0.000000f },
		{ 0.000000f, 0.000000f, 0.125000f, -0.497713f, 0.862065f, 0.095527f },
		{ 0.000000f, 0.000000f, 0.250000f, -0.491037f, 0.850502f, 0.188492f },
		{ 0.000000f, 0.000000f, 0.375000f, -0.480484f, 0.832222f, 0.276661f },
		{ 0.000000f, 0.000000f, 0.500000f, -0.466790f, 0.808504f, 0.358368f },
		{ 0.000000f, 0.000000f, 0.625000f, -0.441592f, 0.797353f, 0.411370f },
		{ 0.000000f, 0.000000f, 0.750000f, -0.412211f, 0.781888f, 0.467688f },
		{ 0.000000f, 0.000000f, 0.875000f, -0.378206f, 0.761242f, 0.526754f },
		{ 0.000000f, 0.000000f, 1.000000f, -0.339259f, 0.734560f, 0.587644f },
		{ 0.000000f, 0.125000f, 0.000000f, -0.707107f, 0.707107f, 0.000000f },
		{ 0.000000f, 0.125000f, 0.125000f, -0.703455f, 0.703455f, 0.101496f },
		{ 0.000000f, 0.125000f, 0.250000f, -0.692831f, 0.692831f, 0.199927f },
		{ 0.000000f, 0.125000f, 0.375000f, -0.676146f, 0.676146f, 0.292668f },
		{ 0.000000f, 0.125000f, 0.500000f, -0.654689f, 0.654689f, 0.377841f },
		{ 0.000000f, 0.125000f, 0.625000f, -0.624555f, 0.647493f, 0.436673f },
		{ 0.000000f, 0.125000f, 0.750000f, -0.575019f, 0.643161f, 0.505665f },
		{ 0.000000f, 0.125000f, 0.875000f, -0.499138f, 0.639089f, 0.585172f },
		{ 0.000000f, 0.125000f, 1.000000f, -0.388153f, 0.630319f, 0.672336f },
		{ 0.000000f, 0.250000f, 0.000000f, -0.832050f, 0.554700f, 0.000000f },
		{ 0.000000f, 0.250000f, 0.125000f, -0.826119f, 0.550746f, 0.119194f },
		{ 0.000000f, 0.250000f, 0.250000f, -0.809057f, 0.539371f, 0.233465f },
		{ 0.000000f, 0.250000f, 0.375000f, -0.782828f, 0.521885f, 0.338845f },
		{ 0.000000f, 0.250000f, 0.500000f, -0.750054f, 0.500036f, 0.432878f },
		{ 0.000000f, 0.250000f, 0.625000f, -0.713098f, 0.492858f, 0.498580f },
		{ 0.000000f, 0.250000f, 0.750000f, -0.655214f, 0.488573f, 0.576187f },
		{ 0.000000f, 0.250000f, 0.875000f, -0.567682f, 0.484568f, 0.665531f },
		{ 0.000000f, 0.250000f, 1.000000f, -0.439701f, 0.476018f, 0.761624f },
		{ 0.000000f, 0.375000f, 0.000000f, -0.948683f, 0.316228f, 0.000000f },
		{ 0.000000f, 0.375000f, 0.125000f, -0.939919f, 0.313306f, 0.135614f },
		{ 0.000000f, 0.375000f, 0.250000f, -0.915016f, 0.305005f, 0.264041f },
		{ 0.000000f, 0.375000f, 0.375000f, -0.877575f, 0.292525f, 0.379856f },
		{ 0.000000f, 0.375000f, 0.500000f, -0.832124f, 0.277375f, 0.480243f },
		{ 0.000000f, 0.375000f, 0.625000f, -0.788535f, 0.272498f, 0.551324f },
		{ 0.000000f, 0.375000f, 0.750000f, -0.723134f, 0.269609f, 0.635915f },
		{ 0.000000f, 0.375000f, 0.875000f, -0.625416f, 0.266925f, 0.733216f },
		{ 0.000000f, 0.375000f, 1.000000f, -0.482618f, 0.261240f, 0.835963f },
		{ 0.000000f, 0.500000f, 0.000000f, -1.000000f, 0.000000f, 0.000000f },
		{ 0.000000f, 0.500000f, 0.125000f, -0.989751f, 0.000000f, 0.142804f },
		{ 0.000000f, 0.500000f, 0.250000f, -0.960797f, 0.000000f, 0.277252f },
		{ 0.000000f, 0.500000f, 0.375000f, -0.917718f, 0.000000f, 0.397232f },
		{ 0.000000f, 0.500000f, 0.500000f, -0.866108f, 0.000000f, 0.499857f },
		{ 0.000000f, 0.500000f, 0.625000f, -0.819550f, 0.000000f, 0.573008f },
		{ 0.000000f, 0.500000f, 0.750000f, -0.750941f, 0.000000f, 0.660369f },
		{ 0.000000f, 0.500000f, 0.875000f, -0.648962f, 0.000000f, 0.760821f },
		{ 0.000000f, 0.500000f, 1.000000f, -0.499980f, 0.000000f, 0.866037f },
		{ 0.000000f, 0.625000f, 0.000000f, -0.948683f, -0.316228f, 0.000000f },
		{ 0.000000f, 0.625000f, 0.125000f, -0.939919f, -0.313306f, 0.135614f },
		{ 0.000000f, 0.625000f, 0.250000f, -0.915016f, -0.305005f, 0.264041f },
		{ 0.000000f, 0.625000f, 0.375000f, -0.877575f, -0.292525f, 0.379856f },
		{ 0.000000f, 0.625000f, 0.500000f, -0.832124f, -0.277375f, 0.480243f },
		{ 0.000000f, 0.625000f, 0.625000f, -0.788535f, -0.272498f, 0.551324f },
		{ 0.000000f, 0.625000f, 0.750000f, -0.723134f, -0.269609f, 0.635915f },
		{ 0.000000f, 0.625000f, 0.875000f, -0.625416f, -0.266925f, 0.733216f },
		{ 0.000000f, 0.625000f, 1.000000f, -0.482618f, -0.261240f, 0.835963f },
		{ 0.000000f, 0.750000f, 0.000000f, -0.832050f, -0.554700f, 0.000000f },
		{ 0.000000f, 0.750000f, 0.125000f, -0.826119f, -0.550746f, 0.119194f },
		{ 0.000000f, 0.750000f, 0.250000f, -0.809057f, -0.539371f, 0.233465f },
		{ 0.000000f, 0.750000f, 0.375000f, -0.782828f, -0.521885f, 0.338845f },
		{ 0.000000f, 0.750000f, 0.500000f, -0.750054f, -0.500036f, 0.432878f },
		{ 0.000000f, 0.750000f, 0.625000f, -0.713098f, -0.492858f, 0.498580f },
		{ 0.000000f, 0.750000f, 0.750000f, -0.655214f, -0.488573f, 0.576187f },
		{ 0.000000f, 0.750000f, 0.875000f, -0.567682f, -0.484568f, 0.665531f },
		{ 0.000000f, 0.750000f, 1.000000f, -0.439701f, -0.476018f, 0.761624f },
		{ 0.000000f, 0.875000f, 0.000000f, -0.707107f, -0.707107f, 0.000000f },
		{ 0.000000f, 0.875000f, 0.125000f, -0.703455f, -0.703455f, 0.101496f },
		{ 0.000000f, 0.875000f, 0.250000f, -0.692831f, -0.692831f, 0.199927f },
		{ 0.000000f, 0.875000f, 0.375000f, -0.676146f, -0.676146f, 0.292668f },
		{ 0.000000f, 0.875000f, 0.500000f, -0.654689f, -0.654689f, 0.377841f },
		{ 0.000000f, 0.875000f, 0.625000f, -0.624555f, -0.647493f, 0.436673f },
		{ 0.000000f, 0.875000f, 0.750000f, -0.575019f, -0.643161f, 0.505665f },
		{ 0.000000f, 0.875000f, 0.875000f, -0.499138f, -0.639089f, 0.585172f },
		{ 0.000000f, 0.875000f, 1.000000f, -0.388153f, -0.630319f, 0.672336f },
		{ 0.000000f, 1.000000f, 0.000000f, -0.500000f, -0.866025f, 0.000000f },
		{ 0.000000f, 1.000000f, 0.125000f, -0.497713f, -0.862065f, 0.095527f },
		{ 0.000000f, 1.000000f, 0.250000f, -0.491037f, -0.850502f, 0.188492f },
		{ 0.000000f, 1.000000f, 0.375000f, -0.480484f, -0.832222f, 0.276661f },
		{ 0.000000f, 1.000000f, 0.500000f, -0.466790f, -0.808504f, 0.358368f },
		{ 0.000000f, 1.000000f, 0.625000f, -0.441592f, -0.797353f, 0.411370f },
		{ 0.000000f, 1.000000f, 0.750000f, -0.412211f, -0.781888f, 0.467688f },
		{ 0.000000f, 1.000000f, 0.875000f, -0.378206f, -0.761242f, 0.526754f },
		{ 0.000000f, 1.000000f, 1.000000f, -0.339259f, -0.734560f, 0.587644f },
		{ 0.125000f, 0.000000f, 0.000000f, -0.397360f, 0.917663f, 0.000000f },
		{ 0.125000f, 0.000000f, 0.125000f, -0.395321f, 0.912955f, 0.101166f },
		{ 0.125000f, 0.000000f, 0.250000f, -0.389388f, 0.899254f, 0.199296f },
		{ 0.125000f, 0.000000f, 0.375000f, -0.380068f, 0.877729f, 0.291789f },
		{ 0.125000f, 0.000000f, 0.500000f, -0.368076f, 0.850035f, 0.376776f },
		{ 0.125000f, 0.000000f, 0.625000f, -0.346295f, 0.833709f, 0.430127f },
		{ 0.125000f, 0.000000f, 0.750000f, -0.321333f, 0.812679f, 0.486106f },
		{ 0.125000f, 0.000000f, 0.875000f, -0.292970f, 0.786241f, 0.544053f },
		{ 0.125000f, 0.000000f, 1.000000f, -0.261103f, 0.753783f, 0.603023f },
		{ 0.125000f, 0.125000f, 0.000000f, -0.375000f, 0.649519f, 0.000000f },
		{ 0.125000f, 0.125000f, 0.125000f, -0.371967f, 0.644266f, 0.095190f },
		{ 0.125000f, 0.125000f, 0.250000f, -0.363293f, 0.629241f, 0.185940f },
		{ 0.125000f, 0.125000f, 0.375000f, -0.350093f, 0.606378f, 0.268776f },
		{ 0.125000f, 0.125000f, 0.500000f, -0.333817f, 0.578188f, 0.341708f },
		{ 0.125000f, 0.125000f, 0.625000f, -0.311338f, 0.562162f, 0.386707f },
		{ 0.125000f, 0.125000f, 0.750000f, -0.285802f, 0.542113f, 0.432355f },
		{ 0.125000f, 0.125000f, 0.875000f, -0.300126f, 0.604085f, 0.557342f },
		{ 0.125000f, 0.125000f, 1.000000f, -0.301211f, 0.652179f, 0.695654f },
		{ 0.125000f, 0.250000f, 0.000000f, -0.560557f, 0.498273f, 0.000000f },
		{ 0.125000f, 0.250000f, 0.125000f, -0.554851f, 0.493201f, 0.106740f },
		{ 0.125000f, 0.250000f, 0.250000f, -0.538724f, 0.478866f, 0.207276f },
		{ 0.125000f, 0.250000f, 0.375000f, -0.514714f, 0.457523f, 0.297057f },
		{ 0.125000f, 0.250000f, 0.500000f, -0.485922f, 0.431930f, 0.373920f },
		{ 0.125000f, 0.250000f, 0.625000f, -0.454898f, 0.419204f, 0.424071f },
		{ 0.125000f, 0.250000f, 0.750000f, -0.408959f, 0.406598f, 0.479511f },
		{ 0.125000f, 0.250000f, 0.875000f, -0.401953f, 0.457471f, 0.628315f },
		{ 0.125000f, 0.250000f, 1.000000f, -0.344675f, 0.497524f, 0.796033f },
		{ 0.125000f, 0.375000f, 0.000000f, -0.685359f, 0.304604f, 0.000000f },
		{ 0.125000f, 0.375000f, 0.125000f, -0.675008f, 0.300003f, 0.129856f },
		{ 0.125000f, 0.375000f, 0.250000f, -0.646560f, 0.287360f, 0.248766f },
		{ 0.125000f, 0.375000f, 0.375000f, -0.606218f, 0.269430f, 0.349867f },
		{ 0.125000f, 0.375000f, 0.500000f, -0.560628f, 0.249168f, 0.431407f },
		{ 0.125000f, 0.375000f, 0.625000f, -0.519861f, 0.239535f, 0.484631f },
		{ 0.125000f, 0.375000f, 0.750000f, -0.463182f, 0.230254f, 0.543089f },
		{ 0.125000f, 0.375000f, 0.875000f, -0.450810f, 0.256538f, 0.704686f },
		{ 0.125000f, 0.375000f, 1.000000f, -0.381947f, 0.275663f, 0.882115f },
		{ 0.125000f, 0.500000f, 0.000000f, -0.750000f, 0.000000f, 0.000000f },
		{ 0.125000f, 0.500000f, 0.125000f, -0.736495f, 0.000000f, 0.141685f },
		{ 0.125000f, 0.500000f, 0.250000f, -0.699977f, 0.000000f, 0.269318f },
		{ 0.125000f, 0.500000f, 0.375000f, -0.649581f, 0.000000f, 0.374893f },
		{ 0.125000f, 0.500000f, 0.500000f, -0.594389f, 0.000000f, 0.457386f },
		{ 0.125000f, 0.500000f, 0.625000f, -0.548592f, 0.000000f, 0.511416f },
		{ 0.125000f, 0.500000f, 0.750000f, -0.486685f, 0.000000f, 0.570647f },
		{ 0.125000f, 0.500000f, 0.875000f, -0.471532f, 0.000000f, 0.737077f },
		{ 0.125000f, 0.500000f, 1.000000f, -0.397342f, 0.000000f, 0.917671f },
		{ 0.125000f, 0.625000f, 0.000000f, -0.685359f, -0.304604f, 0.000000f },
		{ 0.125000f, 0.625000f, 0.125000f, -0.675008f, -0.300003f, 0.129856f },
		{ 0.125000f, 0.625000f, 0.250000f, -0.646560f, -0.287360f, 0.248766f },
		{ 0.125000f, 0.625000f, 0.375000f, -0.606218f, -0.269430f, 0.349867f },
		{ 0.125000f, 0.625000f, 0.500000f, -0.560628f, -0.249168f, 0.431407f },
		{ 0.125000f, 0.625000f, 0.625000f, -0.519861f, -0.239535f, 0.484631f },
		{ 0.125000f, 0.625000f, 0.750000f, -0.463182f, -0.230254f, 0.543089f },
		{ 0.125000f, 0.625000f, 0.875000f, -0.450810f, -0.256538f, 0.704686f },
		{ 0.125000f, 0.625000f, 1.000000f, -0.381947f, -0.275663f, 0.882115f },
		{ 0.125000f, 0.750000f, 0.000000f, -0.560557f, -0.498273f, 0.000000f },
		{ 0.125000f, 0.750000f, 0.125000f, -0.554851f, -0.493201f, 0.106740f },
		{ 0.125000f, 0.750000f, 0.250000f, -0.538724f, -0.478866f, 0.207276f },
		{ 0.125000f, 0.750000f, 0.375000f, -0.514714f, -0.457523f, 0.297057f },
		{ 0.125000f, 0.750000f, 0.500000f, -0.485922f, -0.431930f, 0.373920f },
		{ 0.125000f, 0.750000f, 0.625000f, -0.454898f, -0.419204f, 0.424071f },
		{ 0.125000f, 0.750000f, 0.750000f, -0.408959f, -0.406598f, 0.479511f },
		{ 0.125000f, 0.750000f, 0.875000f, -0.401953f, -0.457471f, 0.628315f },
		{ 0.125000f, 0.750000f, 1.000000f, -0.344675f, -0.497524f, 0.796033f },
		{ 0.125000f, 0.875000f, 0.000000f, -0.375000f, -0.649519f, 0.000000f },
		{ 0.125000f, 0.875000f, 0.125000f, -0.371967f, -0.644266f, 0.095190f },
		{ 0.125000f, 0.875000f, 0.250000f, -0.363293f, -0.629241f, 0.185940f },
		{ 0.125000f, 0.875000f, 0.375000f, -0.350093f, -0.606378f, 0.268776f },
		{ 0.125000f, 0.875000f, 0.500000f, -0.333817f, -0.578188f, 0.341708f },
		{ 0.125000f, 0.875000f, 0.625000f, -0.311338f, -0.562162f, 0.386707f },
		{ 0.125000f, 0.875000f, 0.750000f, -0.285802f, -0.542113f, 0.432355f },
		{ 0.125000f, 0.875000f, 0.875000f, -0.300126f, -0.604085f, 0.557342f },
		{ 0.125000f, 0.875000f, 1.000000f, -0.301211f, -0.652179f, 0.695654f },
		{ 0.125000f, 1.000000f, 0.000000f, -0.397360f, -0.917663f, 0.000000f },
		{ 0.125000f, 1.000000f, 0.125000f, -0.395321f, -0.912955f, 0.101166f },
		{ 0.125000f, 1.000000f, 0.250000f, -0.389388f, -0.899254f, 0.199296f },
		{ 0.125000f, 1.000000f, 0.375000f, -0.380068f, -0.877729f, 0.291789f },
		{ 0.125000f, 1.000000f, 0.500000f, -0.368076f, -0.850035f, 0.376776f },
		{ 0.125000f, 1.000000f, 0.625000f, -0.346295f, -0.833709f, 0.430127f },
		{ 0.125000f, 1.000000f, 0.750000f, -0.321333f, -0.812679f, 0.486106f },
		{ 0.125000f, 1.000000f, 0.875000f, -0.292970f, -0.786241f, 0.544053f },
		{ 0.125000f, 1.000000f, 1.000000f, -0.261103f, -0.753783f, 0.603023f },
		{ 0.250000f, 0.000000f, 0.000000f, -0.277350f, 0.960769f, 0.000000f },
		{ 0.250000f, 0.000000f, 0.125000f, -0.275791f, 0.955370f, 0.105866f },
		{ 0.250000f, 0.000000f, 0.250000f, -0.271269f, 0.939702f, 0.208261f },
		{ 0.250000f, 0.000000f, 0.375000f, -0.264201f, 0.915220f, 0.304252f },
		{ 0.250000f, 0.000000f, 0.500000f, -0.255175f, 0.883952f, 0.391810f },
		{ 0.250000f, 0.000000f, 0.625000f, -0.238961f, 0.862950f, 0.445213f },
		{ 0.250000f, 0.000000f, 0.750000f, -0.220644f, 0.837041f, 0.500678f },
		{ 0.250000f, 0.000000f, 0.875000f, -0.200143f, 0.805685f, 0.557507f },
		{ 0.250000f, 0.000000f, 1.000000f, -0.177462f, 0.768477f, 0.614777f },
		{ 0.250000f, 0.125000f, 0.000000f, -0.269408f, 0.699942f, 0.000000f },
		{ 0.250000f, 0.125000f, 0.125000f, -0.266883f, 0.693382f, 0.102447f },
		{ 0.250000f, 0.125000f, 0.250000f, -0.259713f, 0.674754f, 0.199389f },
		{ 0.250000f, 0.125000f, 0.375000f, -0.248949f, 0.646788f, 0.286688f },
		{ 0.250000f, 0.125000f, 0.500000f, -0.235905f, 0.612899f, 0.362221f },
		{ 0.250000f, 0.125000f, 0.625000f, -0.218270f, 0.591172f, 0.406663f },
		{ 0.250000f, 0.125000f, 0.750000f, -0.198719f, 0.565399f, 0.450926f },
		{ 0.250000f, 0.125000f, 0.875000f, -0.206962f, 0.624851f, 0.576501f },
		{ 0.250000f, 0.125000f, 1.000000f, -0.206068f, 0.669264f, 0.713878f },
		{ 0.250000f, 0.250000f, 0.000000f, -0.250000f, 0.433013f, 0.000000f },
		{ 0.250000f, 0.250000f, 0.125000f, -0.245519f, 0.425251f, 0.094246f },
		{ 0.250000f, 0.250000f, 0.250000f, -0.233395f, 0.404252f, 0.179184f },
		{ 0.250000f, 0.250000f, 0.375000f, -0.216652f, 0.375252f, 0.249495f },
		{ 0.250000f, 0.250000f, 0.500000f, -0.198300f, 0.343466f, 0.304481f },
		{ 0.250000f, 0.250000f, 0.625000f, -0.224775f, 0.405860f, 0.418782f },
		{ 0.250000f, 0.250000f, 0.750000f, -0.240229f, 0.455670f, 0.545120f },
		{ 0.250000f, 0.250000f, 0.875000f, -0.244470f, 0.492062f, 0.680981f },
		{ 0.250000f, 0.250000f, 1.000000f, -0.237763f, 0.514803f, 0.823679f },
		{ 0.250000f, 0.375000f, 0.000000f, -0.416025f, 0.277350f, 0.000000f },
		{ 0.250000f, 0.375000f, 0.125000f, -0.404528f, 0.269686f, 0.116733f },
		{ 0.250000f, 0.375000f, 0.250000f, -0.375027f, 0.250018f, 0.216439f },
		{ 0.250000f, 0.375000f, 0.375000f, -0.337570f, 0.225047f, 0.292233f },
		{ 0.250000f, 0.375000f, 0.500000f, -0.300055f, 0.200037f, 0.346341f },
		{ 0.250000f, 0.375000f, 0.625000f, -0.337317f, 0.233137f, 0.471687f },
		{ 0.250000f, 0.375000f, 0.750000f, -0.347830f, 0.259366f, 0.611755f },
		{ 0.250000f, 0.375000f, 0.875000f, -0.325497f, 0.277841f, 0.763204f },
		{ 0.250000f, 0.375000f, 1.000000f, -0.265623f, 0.287562f, 0.920192f },
		{ 0.250000f, 0.500000f, 0.000000f, -0.500000f, 0.000000f, 0.000000f },
		{ 0.250000f, 0.500000f, 0.125000f, -0.480399f, 0.000000f, 0.138626f },
		{ 0.250000f, 0.500000f, 0.250000f, -0.433054f, 0.000000f, 0.249928f },
		{ 0.250000f, 0.500000f, 0.375000f, -0.378026f, 0.000000f, 0.327255f },
		{ 0.250000f, 0.500000f, 0.500000f, -0.327398f, 0.000000f, 0.377903f },
		{ 0.250000f, 0.500000f, 0.625000f, -0.363558f, 0.000000f, 0.508381f },
		{ 0.250000f, 0.500000f, 0.750000f, -0.370702f, 0.000000f, 0.651982f },
		{ 0.250000f, 0.500000f, 0.875000f, -0.343262f, 0.000000f, 0.804858f },
		{ 0.250000f, 0.500000f, 1.000000f, -0.277337f, 0.000000f, 0.960773f },
		{ 0.250000f, 0.625000f, 0.000000f, -0.416025f, -0.277350f, 0.000000f },
		{ 0.250000f, 0.625000f, 0.125000f, -0.404528f, -0.269686f, 0.116733f },
		{ 0.250000f, 0.625000f, 0.250000f, -0.375027f, -0.250018f, 0.216439f },
		{ 0.250000f, 0.625000f, 0.375000f, -0.337570f, -0.225047f, 0.292233f },
		{ 0.250000f, 0.625000f, 0.500000f, -0.300055f, -0.200037f, 0.346341f },
		{ 0.250000f, 0.625000f, 0.625000f, -0.337317f, -0.233137f, 0.471687f },
		{ 0.250000f, 0.625000f, 0.750000f, -0.347830f, -0.259366f, 0.611755f },
		{ 0.250000f, 0.625000f, 0.875000f, -0.325497f, -0.277841f, 0.763204f },
		{ 0.250000f, 0.625000f, 1.000000f, -0.265623f, -0.287562f, 0.920192f },
		{ 0.250000f, 0.750000f, 0.000000f, -0.250000f, -0.433013f, 0.000000f },
		{ 0.250000f, 0.750000f, 0.125000f, -0.245519f, -0.425251f, 0.094246f },
		{ 0.250000f, 0.750000f, 0.250000f, -0.233395f, -0.404252f, 0.179184f },
		{ 0.250000f, 0.750000f, 0.375000f, -0.216652f, -0.375252f, 0.249495f },
		{ 0.250000f, 0.750000f, 0.500000f, -0.198300f, -0.343466f, 0.304481f },
		{ 0.250000f, 0.750000f, 0.625000f, -0.224775f, -0.405860f, 0.418782f },
		{ 0.250000f, 0.750000f, 0.750000f, -0.240229f, -0.455670f, 0.545120f },
		{ 0.250000f, 0.750000f, 0.875000f, -0.244470f, -0.492062f, 0.680981f },
		{ 0.250000f, 0.750000f, 1.000000f, -0.237763f, -0.514803f, 0.823679f },
		{ 0.250000f, 0.875000f, 0.000000f, -0.269408f, -0.699942f, 0.000000f },
		{ 0.250000f, 0.875000f, 0.125000f, -0.266883f, -0.693382f, 0.102447f },
		{ 0.250000f, 0.875000f, 0.250000f, -0.259713f, -0.674754f, 0.199389f },
		{ 0.250000f, 0.875000f, 0.375000f, -0.248949f, -0.646788f, 0.286688f },
		{ 0.250000f, 0.875000f, 0.500000f, -0.235905f, -0.612899f, 0.362221f },
		{ 0.250000f, 0.875000f, 0.625000f, -0.218270f, -0.591172f, 0.406663f },
		{ 0.250000f, 0.875000f, 0.750000f, -0.198719f, -0.565399f, 0.450926f },
		{ 0.250000f, 0.875000f, 0.875000f, -0.206962f, -0.624851f, 0.576501f },
		{ 0.250000f, 0.875000f, 1.000000f, -0.206068f, -0.669264f, 0.713878f },
		{ 0.250000f, 1.000000f, 0.000000f, -0.277350f, -0.960769f, 0.000000f },
		{ 0.250000f, 1.000000f, 0.125000f, -0.275791f, -0.955370f, 0.105866f },
		{ 0.250000f, 1.000000f, 0.250000f, -0.271269f, -0.939702f, 0.208261f },
		{ 0.250000f, 1.000000f, 0.375000f, -0.264201f, -0.915220f, 0.304252f },
		{ 0.250000f, 1.000000f, 0.500000f, -0.255175f, -0.883952f, 0.391810f },
		{ 0.250000f, 1.000000f, 0.625000f, -0.238961f, -0.862950f, 0.445213f },
		{ 0.250000f, 1.000000f, 0.750000f, -0.220644f, -0.837041f, 0.500678f },
		{ 0.250000f, 1.000000f, 0.875000f, -0.200143f, -0.805685f, 0.557507f },
		{ 0.250000f, 1.000000f, 1.000000f, -0.177462f, -0.768477f, 0.614777f },
		{ 0.375000f, 0.000000f, 0.000000f, -0.142857f, 0.989743f, 0.000000f },
		{ 0.375000f, 0.000000f, 0.125000f, -0.142006f, 0.983844f, 0.109022f },
		{ 0.375000f, 0.000000f, 0.250000f, -0.139540f, 0.966759f, 0.214257f },
		{ 0.375000f, 0.000000f, 0.375000f, -0.135701f, 0.940161f, 0.312543f },
		{ 0.375000f, 0.000000f, 0.500000f, -0.130822f, 0.906360f, 0.401742f },
		{ 0.375000f, 0.000000f, 0.625000f, -0.122124f, 0.882045f, 0.455064f },
		{ 0.375000f, 0.000000f, 0.750000f, -0.112393f, 0.852754f, 0.510077f },
		{ 0.375000f, 0.000000f, 0.875000f, -0.101610f, 0.818067f, 0.566075f },
		{ 0.375000f, 0.000000f, 1.000000f, -0.089798f, 0.777716f, 0.622169f },
		{ 0.375000f, 0.125000f, 0.000000f, -0.141737f, 0.736485f, 0.000000f },
		{ 0.375000f, 0.125000f, 0.125000f, -0.140268f, 0.728854f, 0.107688f },
		{ 0.375000f, 0.125000f, 0.250000f, -0.136122f, 0.707309f, 0.209009f },
		{ 0.375000f, 0.125000f, 0.375000f, -0.129960f, 0.675291f, 0.299321f },
		{ 0.375000f, 0.125000f, 0.500000f, -0.122588f, 0.636987f, 0.376457f },
		{ 0.375000f, 0.125000f, 0.625000f, -0.112775f, 0.610893f, 0.420228f },
		{ 0.375000f, 0.125000f, 0.750000f, -0.102083f, 0.580899f, 0.463288f },
		{ 0.375000f, 0.125000f, 0.875000f, -0.105723f, 0.638387f, 0.588990f },
		{ 0.375000f, 0.125000f, 1.000000f, -0.104715f, 0.680183f, 0.725524f },
		{ 0.375000f, 0.250000f, 0.000000f, -0.138675f, 0.480384f, 0.000000f },
		{ 0.375000f, 0.250000f, 0.125000f, -0.135634f, 0.469851f, 0.104130f },
		{ 0.375000f, 0.250000f, 0.250000f, -0.127587f, 0.441976f, 0.195905f },
		{ 0.375000f, 0.250000f, 0.375000f, -0.116866f, 0.404837f, 0.269165f },
		{ 0.375000f, 0.250000f, 0.500000f, -0.105572f, 0.365714f, 0.324204f },
		{ 0.375000f, 0.250000f, 0.625000f, -0.118270f, 0.427104f, 0.440703f },
		{ 0.375000f, 0.250000f, 0.750000f, -0.125021f, 0.474282f, 0.567386f },
		{ 0.375000f, 0.250000f, 0.875000f, -0.125978f, 0.507131f, 0.701835f },
		{ 0.375000f, 0.250000f, 1.000000f, -0.121485f, 0.526076f, 0.841716f },
		{ 0.375000f, 0.375000f, 0.000000f, -0.125000f, 0.216506f, 0.000000f },
		{ 0.375000f, 0.375000f, 0.125000f, -0.116698f, 0.202126f, 0.089592f },
		{ 0.375000f, 0.375000f, 0.250000f, -0.099150f, 0.171733f, 0.152240f },
		{ 0.375000f, 0.375000f, 0.375000f, -0.122937f, 0.212932f, 0.283146f },
		{ 0.375000f, 0.375000f, 0.500000f, -0.136434f, 0.236311f, 0.418978f },
		{ 0.375000f, 0.375000f, 0.625000f, -0.146723f, 0.264928f, 0.546727f },
		{ 0.375000f, 0.375000f, 0.750000f, -0.149420f, 0.283422f, 0.678119f },
		{ 0.375000f, 0.375000f, 0.875000f, -0.145655f, 0.293170f, 0.811456f },
		{ 0.375000f, 0.375000f, 1.000000f, -0.136471f, 0.295486f, 0.945549f },
		{ 0.375000f, 0.500000f, 0.000000f, -0.250000f, 0.000000f, 0.000000f },
		{ 0.375000f, 0.500000f, 0.125000f, -0.216527f, 0.000000f, 0.124964f },
		{ 0.375000f, 0.500000f, 0.250000f, -0.163699f, 0.000000f, 0.188951f },
		{ 0.375000f, 0.500000f, 0.375000f, -0.187554f, 0.000000f, 0.324729f },
		{ 0.375000f, 0.500000f, 0.500000f, -0.198744f, 0.000000f, 0.458804f },
		{ 0.375000f, 0.500000f, 0.625000f, -0.210430f, 0.000000f, 0.588510f },
		{ 0.375000f, 0.500000f, 0.750000f, -0.205090f, 0.000000f, 0.721414f },
		{ 0.375000f, 0.500000f, 0.875000f, -0.182486f, 0.000000f, 0.855759f },
		{ 0.375000f, 0.500000f, 1.000000f, -0.142850f, 0.000000f, 0.989744f },
		{ 0.375000f, 0.625000f, 0.000000f, -0.125000f, -0.216506f, 0.000000f },
		{ 0.375000f, 0.625000f, 0.125000f, -0.116698f, -0.202126f, 0.089592f },
		{ 0.375000f, 0.625000f, 0.250000f, -0.099150f, -0.171733f, 0.152240f },
		{ 0.375000f, 0.625000f, 0.375000f, -0.122937f, -0.212932f, 0.283146f },
		{ 0.375000f, 0.625000f, 0.500000f, -0.136434f, -0.236311f, 0.418978f },
		{ 0.375000f, 0.625000f, 0.625000f, -0.146723f, -0.264928f, 0.546727f },
		{ 0.375000f, 0.625000f, 0.750000f, -0.149420f, -0.283422f, 0.678119f },
		{ 0.375000f, 0.625000f, 0.875000f, -0.145655f, -0.293170f, 0.811456f },
		{ 0.375000f, 0.625000f, 1.000000f, -0.136471f, -0.295486f, 0.945549f },
		{ 0.375000f, 0.750000f, 0.000000f, -0.138675f, -0.480384f, 0.000000f },
		{ 0.375000f, 0.750000f, 0.125000f, -0.135634f, -0.469851f, 0.104130f },
		{ 0.375000f, 0.750000f, 0.250000f, -0.127587f, -0.441976f, 0.195905f },
		{ 0.375000f, 0.750000f, 0.375000f, -0.116866f, -0.404837f, 0.269165f },
		{ 0.375000f, 0.750000f, 0.500000f, -0.105572f, -0.365714f, 0.324204f },
		{ 0.375000f, 0.750000f, 0.625000f, -0.118270f, -0.427104f, 0.440703f },
		{ 0.375000f, 0.750000f, 0.750000f, -0.125021f, -0.474282f, 0.567386f },
		{ 0.375000f, 0.750000f, 0.875000f, -0.125978f, -0.507131f, 0.701835f },
		{ 0.375000f, 0.750000f, 1.000000f, -0.121485f, -0.526076f, 0.841716f },
		{ 0.375000f, 0.875000f, 0.000000f, -0.141737f, -0.736485f, 0.000000f },
		{ 0.375000f, 0.875000f, 0.125000f, -0.140268f, -0.728854f, 0.107688f },
		{ 0.375000f, 0.875000f, 0.250000f, -0.136122f, -0.707309f, 0.209009f },
		{ 0.375000f, 0.875000f, 0.375000f, -0.129960f, -0.675291f, 0.299321f },
		{ 0.375000f, 0.875000f, 0.500000f, -0.122588f, -0.636987f, 0.376457f },
		{ 0.375000f, 0.875000f, 0.625000f, -0.112775f, -0.610893f, 0.420228f },
		{ 0.375000f, 0.875000f, 0.750000f, -0.102083f, -0.580899f, 0.463288f },
		{ 0.375000f, 0.875000f, 0.875000f, -0.105723f, -0.638387f, 0.588990f },
		{ 0.375000f, 0.875000f, 1.000000f, -0.104715f, -0.680183f, 0.725524f },
		{ 0.375000f, 1.000000f, 0.000000f, -0.142857f, -0.989743f, 0.000000f },
		{ 0.375000f, 1.000000f, 0.125000f, -0.142006f, -0.983844f, 0.109022f },
		{ 0.375000f, 1.000000f, 0.250000f, -0.139540f, -0.966759f, 0.214257f },
		{ 0.375000f, 1.000000f, 0.375000f, -0.135701f, -0.940161f, 0.312543f },
		{ 0.375000f, 1.000000f, 0.500000f, -0.130822f, -0.906360f, 0.401742f },
		{ 0.375000f, 1.000000f, 0.625000f, -0.122124f, -0.882045f, 0.455064f },
		{ 0.375000f, 1.000000f, 0.750000f, -0.112393f, -0.852754f, 0.510077f },
		{ 0.375000f, 1.000000f, 0.875000f, -0.101610f, -0.818067f, 0.566075f },
		{ 0.375000f, 1.000000f, 1.000000f, -0.089798f, -0.777716f, 0.622169f },
		{ 0.500000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f },
		{ 0.500000f, 0.000000f, 0.125000f, 0.000000f, 0.993916f, 0.110138f },
		{ 0.500000f, 0.000000f, 0.250000f, 0.000000f, 0.976311f, 0.216374f },
		{ 0.500000f, 0.000000f, 0.375000f, 0.000000f, 0.948938f, 0.315461f },
		{ 0.500000f, 0.000000f, 0.500000f, 0.000000f, 0.914217f, 0.405225f },
		{ 0.500000f, 0.000000f, 0.625000f, 0.000000f, 0.888697f, 0.458496f },
		{ 0.500000f, 0.000000f, 0.750000f, 0.000000f, 0.858192f, 0.513329f },
		{ 0.500000f, 0.000000f, 0.875000f, 0.000000f, 0.822323f, 0.569020f },
		{ 0.500000f, 0.000000f, 1.000000f, 0.000000f, 0.780871f, 0.624693f },
		{ 0.500000f, 0.125000f, 0.000000f, 0.000000f, 0.750000f, 0.000000f },
		{ 0.500000f, 0.125000f, 0.125000f, 0.000000f, 0.741945f, 0.109622f },
		{ 0.500000f, 0.125000f, 0.250000f, 0.000000f, 0.719255f, 0.212539f },
		{ 0.500000f, 0.125000f, 0.375000f, 0.000000f, 0.685663f, 0.303919f },
		{ 0.500000f, 0.125000f, 0.500000f, 0.000000f, 0.645670f, 0.381589f },
		{ 0.500000f, 0.125000f, 0.625000f, 0.000000f, 0.617918f, 0.425061f },
		{ 0.500000f, 0.125000f, 0.750000f, 0.000000f, 0.586356f, 0.467640f },
		{ 0.500000f, 0.125000f, 0.875000f, 0.000000f, 0.643099f, 0.593337f },
		{ 0.500000f, 0.125000f, 1.000000f, 0.000000f, 0.683943f, 0.729535f },
		{ 0.500000f, 0.250000f, 0.000000f, 0.000000f, 0.500000f, 0.000000f },
		{ 0.500000f, 0.250000f, 0.125000f, 0.000000f, 0.488155f, 0.108187f },
		{ 0.500000f, 0.250000f, 0.250000f, 0.000000f, 0.457109f, 0.202612f },
		{ 0.500000f, 0.250000f, 0.375000f, 0.000000f, 0.416370f, 0.276833f },
		{ 0.500000f, 0.250000f, 0.500000f, 0.000000f, 0.374149f, 0.331681f },
		{ 0.500000f, 0.250000f, 0.625000f, 0.000000f, 0.434963f, 0.448812f },
		{ 0.500000f, 0.250000f, 0.750000f, 0.000000f, 0.481012f, 0.575437f },
		{ 0.500000f, 0.250000f, 0.875000f, 0.000000f, 0.512470f, 0.709225f },
		{ 0.500000f, 0.250000f, 1.000000f, 0.000000f, 0.530001f, 0.847997f },
		{ 0.500000f, 0.375000f, 0.000000f, 0.000000f, 0.250000f, 0.000000f },
		{ 0.500000f, 0.375000f, 0.125000f, 0.000000f, 0.228554f, 0.101306f },
		{ 0.500000f, 0.375000f, 0.250000f, 0.000000f, 0.187074f, 0.165841f },
		{ 0.500000f, 0.375000f, 0.375000f, 0.000000f, 0.225388f, 0.299709f },
		{ 0.500000f, 0.375000f, 0.500000f, 0.000000f, 0.245633f, 0.435505f },
		{ 0.500000f, 0.375000f, 0.625000f, 0.000000f, 0.272545f, 0.562445f },
		{ 0.500000f, 0.375000f, 0.750000f, 0.000000f, 0.289220f, 0.691991f },
		{ 0.500000f, 0.375000f, 0.875000f, 0.000000f, 0.297318f, 0.822938f },
		{ 0.500000f, 0.375000f, 1.000000f, 0.000000f, 0.298277f, 0.954479f },
		{ 0.500000f, 0.500000f, 0.125000f, 0.000000f, 0.000000f, 0.125000f },
		{ 0.500000f, 0.500000f, 0.250000f, 0.000000f, 0.000000f, 0.250000f },
		{ 0.500000f, 0.500000f, 0.375000f, 0.000000f, 0.000000f, 0.375000f },
		{ 0.500000f, 0.500000f, 0.500000f, 0.000000f, 0.000000f, 0.500000f },
		{ 0.500000f, 0.500000f, 0.625000f, 0.000000f, 0.000000f, 0.625000f },
		{ 0.500000f, 0.500000f, 0.750000f, 0.000000f, 0.000000f, 0.750000f },
		{ 0.500000f, 0.500000f, 0.875000f, 0.000000f, 0.000000f, 0.875000f },
		{ 0.500000f, 0.500000f, 1.000000f, 0.000000f, 0.000000f, 1.000000f },
		{ 0.500000f, 0.625000f, 0.000000f, 0.000000f, -0.250000f, 0.000000f },
		{ 0.500000f, 0.625000f, 0.125000f, 0.000000f, -0.228554f, 0.101306f },
		{ 0.500000f, 0.625000f, 0.250000f, 0.000000f, -0.187074f, 0.165841f },
		{ 0.500000f, 0.625000f, 0.375000f, 0.000000f, -0.225388f, 0.299709f },
		{ 0.500000f, 0.625000f, 0.500000f, 0.000000f, -0.245633f, 0.435505f },
		{ 0.500000f, 0.625000f, 0.625000f, 0.000000f, -0.272545f, 0.562445f },
		{ 0.500000f, 0.625000f, 0.750000f, 0.000000f, -0.289220f, 0.691991f },
		{ 0.500000f, 0.625000f, 0.875000f, 0.000000f, -0.297318f, 0.822938f },
		{ 0.500000f, 0.625000f, 1.000000f, 0.000000f, -0.298277f, 0.954479f },
		{ 0.500000f, 0.750000f, 0.000000f, 0.000000f, -0.500000f, 0.000000f },
		{ 0.500000f, 0.750000f, 0.125000f, 0.000000f, -0.488155f, 0.108187f },
		{ 0.500000f, 0.750000f, 0.250000f, 0.000000f, -0.457109f, 0.202612f },
		{ 0.500000f, 0.750000f, 0.375000f, 0.000000f, -0.416370f, 0.276833f },
		{ 0.500000f, 0.750000f, 0.500000f, 0.000000f, -0.374149f, 0.331681f },
		{ 0.500000f, 0.750000f, 0.625000f, 0.000000f, -0.434963f, 0.448812f },
		{ 0.500000f, 0.750000f, 0.750000f, 0.000000f, -0.481012f, 0.575437f },
		{ 0.500000f, 0.750000f, 0.875000f, 0.000000f, -0.512470f, 0.709225f },
		{ 0.500000f, 0.750000f, 1.000000f, 0.000000f, -0.530001f, 0.847997f },
		{ 0.500000f, 0.875000f, 0.000000f, 0.000000f, -0.750000f, 0.000000f },
		{ 0.500000f, 0.875000f, 0.125000f, 0.000000f, -0.741945f, 0.109622f },
		{ 0.500000f, 0.875000f, 0.250000f, 0.000000f, -0.719255f, 0.212539f },
		{ 0.500000f, 0.875000f, 0.375000f, 0.000000f, -0.685663f, 0.303919f },
		{ 0.500000f, 0.875000f, 0.500000f, 0.000000f, -0.645670f, 0.381589f },
		{ 0.500000f, 0.875000f, 0.625000f, 0.000000f, -0.617918f, 0.425061f },
		{ 0.500000f, 0.875000f, 0.750000f, 0.000000f, -0.586356f, 0.467640f },
		{ 0.500000f, 0.875000f, 0.875000f, 0.000000f, -0.643099f, 0.593337f },
		{ 0.500000f, 0.875000f, 1.000000f, 0.000000f, -0.683943f, 0.729535f },
		{ 0.500000f, 1.000000f, 0.000000f, 0.000000f, -1.000000f, 0.000000f },
		{ 0.500000f, 1.000000f, 0.125000f, 0.000000f, -0.993916f, 0.110138f },
		{ 0.500000f, 1.000000f, 0.250000f, 0.000000f, -0.976311f, 0.216374f },
		{ 0.500000f, 1.000000f, 0.375000f, 0.000000f, -0.948938f, 0.315461f },
		{ 0.500000f, 1.000000f, 0.500000f, 0.000000f, -0.914217f, 0.405225f },
		{ 0.500000f, 1.000000f, 0.625000f, 0.000000f, -0.888697f, 0.458496f },
		{ 0.500000f, 1.000000f, 0.750000f, 0.000000f, -0.858192f, 0.513329f },
		{ 0.500000f, 1.000000f, 0.875000f, 0.000000f, -0.822323f, 0.569020f },
		{ 0.500000f, 1.000000f, 1.000000f, 0.000000f, -0.780871f, 0.624693f },
		{ 0.625000f, 0.000000f, 0.000000f, 0.142857f, 0.989743f, 0.000000f },
		{ 0.625000f, 0.000000f, 0.125000f, 0.142006f, 0.983844f, 0.109022f },
		{ 0.625000f, 0.000000f, 0.250000f, 0.139540f, 0.966759f, 0.214257f },
		{ 0.625000f, 0.000000f, 0.375000f, 0.135701f, 0.940161f, 0.312543f },
		{ 0.625000f, 0.000000f, 0.500000f, 0.130822f, 0.906360f, 0.401742f },
		{ 0.625000f, 0.000000f, 0.625000f, 0.122124f, 0.882045f, 0.455064f },
		{ 0.625000f, 0.000000f, 0.750000f, 0.112393f, 0.852754f, 0.510077f },
		{ 0.625000f, 0.000000f, 0.875000f, 0.101610f, 0.818067f, 0.566075f },
		{ 0.625000f, 0.000000f, 1.000000f, 0.089798f, 0.777716f, 0.622169f },
		{ 0.625000f, 0.125000f, 0.000000f, 0.141737f, 0.736485f, 0.000000f },
		{ 0.625000f, 0.125000f, 0.125000f, 0.140268f, 0.728854f, 0.107688f },
		{ 0.625000f, 0.125000f, 0.250000f, 0.136122f, 0.707309f, 0.209009f },
		{ 0.625000f, 0.125000f, 0.375000f, 0.129960f, 0.675291f, 0.299321f },
		{ 0.625000f, 0.125000f, 0.500000f, 0.122588f, 0.636987f, 0.376457f },
		{ 0.625000f, 0.125000f, 0.625000f, 0.112775f, 0.610893f, 0.420228f },
		{ 0.625000f, 0.125000f, 0.750000f, 0.102083f, 0.580899f, 0.463288f },
		{ 0.625000f, 0.125000f, 0.875000f, 0.105723f, 0.638387f, 0.588990f },
		{ 0.625000f, 0.125000f, 1.000000f, 0.104715f, 0.680183f, 0.725524f },
		{ 0.625000f, 0.250000f, 0.000000f, 0.138675f, 0.480384f, 0.000000f },
		{ 0.625000f, 0.250000f, 0.125000f, 0.135634f, 0.469851f, 0.104130f },
		{ 0.625000f, 0.250000f, 0.250000f, 0.127587f, 0.441976f, 0.195905f },
		{ 0.625000f, 0.250000f, 0.375000f, 0.116866f, 0.404837f, 0.269165f },
		{ 0.625000f, 0.250000f, 0.500000f, 0.105572f, 0.365714f, 0.324204f },
		{ 0.625000f, 0.250000f, 0.625000f, 0.118270f, 0.427104f, 0.440703f },
		{ 0.625000f, 0.250000f, 0.750000f, 0.125021f, 0.474282f, 0.567386f },
		{ 0.625000f, 0.250000f, 0.875000f, 0.125978f, 0.507131f, 0.701835f },
		{ 0.625000f, 0.250000f, 1.000000f, 0.121485f, 0.526076f, 0.841716f },
		{ 0.625000f, 0.375000f, 0.000000f, 0.125000f, 0.216506f, 0.000000f },
		{ 0.625000f, 0.375000f, 0.125000f, 0.116698f, 0.202126f, 0.089592f },
		{ 0.625000f, 0.375000f, 0.250000f, 0.099150f, 0.171733f, 0.152240f },
		{ 0.625000f, 0.375000f, 0.375000f, 0.122937f, 0.212932f, 0.283146f },
		{ 0.625000f, 0.375000f, 0.500000f, 0.136434f, 0.236311f, 0.418978f },
		{ 0.625000f, 0.375000f, 0.625000f, 0.146723f, 0.264928f, 0.546727f },
		{ 0.625000f, 0.375000f, 0.750000f, 0.149420f, 0.283422f, 0.678119f },
		{ 0.625000f, 0.375000f, 0.875000f, 0.145655f, 0.293170f, 0.811456f },
		{ 0.625000f, 0.375000f, 1.000000f, 0.136471f, 0.295486f, 0.945549f },
		{ 0.625000f, 0.500000f, 0.000000f, 0.250000f, 0.000000f, 0.000000f },
		{ 0.625000f, 0.500000f, 0.125000f, 0.216527f, 0.000000f, 0.124964f },
		{ 0.625000f, 0.500000f, 0.250000f, 0.163699f, 0.000000f, 0.188951f },
		{ 0.625000f, 0.500000f, 0.375000f, 0.187554f, 0.000000f, 0.324729f },
		{ 0.625000f, 0.500000f, 0.500000f, 0.198744f, 0.000000f, 0.458804f },
		{ 0.625000f, 0.500000f, 0.625000f, 0.210430f, 0.000000f, 0.588510f },
		{ 0.625000f, 0.500000f, 0.750000f, 0.205090f, 0.000000f, 0.721414f },
		{ 0.625000f, 0.500000f, 0.875000f, 0.182486f, 0.000000f, 0.855759f },
		{ 0.625000f, 0.500000f, 1.000000f, 0.142850f, 0.000000f, 0.989744f },
		{ 0.625000f, 0.625000f, 0.000000f, 0.125000f, -0.216506f, 0.000000f },
		{ 0.625000f, 0.625000f, 0.125000f, 0.116698f, -0.202126f, 0.089592f },
		{ 0.625000f, 0.625000f, 0.250000f, 0.099150f, -0.171733f, 0.152240f },
		{ 0.625000f, 0.625000f, 0.375000f, 0.122937f, -0.212932f, 0.283146f },
		{ 0.625000f, 0.625000f, 0.500000f, 0.136434f, -0.236311f, 0.418978f },
		{ 0.625000f, 0.625000f, 0.625000f, 0.146723f, -0.264928f, 0.546727f },
		{ 0.625000f, 0.625000f, 0.750000f, 0.149420f, -0.283422f, 0.678119f },
		{ 0.625000f, 0.625000f, 0.875000f, 0.145655f, -0.293170f, 0.811456f },
		{ 0.625000f, 0.625000f, 1.000000f, 0.136471f, -0.295486f, 0.945549f },
		{ 0.625000f, 0.750000f, 0.000000f, 0.138675f, -0.480384f, 0.000000f },
		{ 0.625000f, 0.750000f, 0.125000f, 0.135634f, -0.469851f, 0.104130f },
		{ 0.625000f, 0.750000f, 0.250000f, 0.127587f, -0.441976f, 0.195905f },
		{ 0.625000f, 0.750000f, 0.375000f, 0.116866f, -0.404837f, 0.269165f },
		{ 0.625000f, 0.750000f, 0.500000f, 0.105572f, -0.365714f, 0.324204f },
		{ 0.625000f, 0.750000f, 0.625000f, 0.118270f, -0.427104f, 0.440703f },
		{ 0.625000f, 0.750000f, 0.750000f, 0.125021f, -0.474282f, 0.567386f },
		{ 0.625000f, 0.750000f, 0.875000f, 0.125978f, -0.507131f, 0.701835f },
		{ 0.625000f, 0.750000f, 1.000000f, 0.121485f, -0.526076f, 0.841716f },
		{ 0.625000f, 0.875000f, 0.000000f, 0.141737f, -0.736485f, 0.000000f },
		{ 0.625000f, 0.875000f, 0.125000f, 0.140268f, -0.728854f, 0.107688f },
		{ 0.625000f, 0.875000f, 0.250000f, 0.136122f, -0.707309f, 0.209009f },
		{ 0.625000f, 0.875000f, 0.375000f, 0.129960f, -0.675291f, 0.299321f },
		{ 0.625000f, 0.875000f, 0.500000f, 0.122588f, -0.636987f, 0.376457f },
		{ 0.625000f, 0.875000f, 0.625000f, 0.112775f, -0.610893f, 0.420228f },
		{ 0.625000f, 0.875000f, 0.750000f, 0.102083f, -0.580899f, 0.463288f },
		{ 0.625000f, 0.875000f, 0.875000f, 0.105723f, -0.638387f, 0.588990f },
		{ 0.625000f, 0.875000f, 1.000000f, 0.104715f, -0.680183f, 0.725524f },
		{ 0.625000f, 1.000000f, 0.000000f, 0.142857f, -0.989743f, 0.000000f },
		{ 0.625000f, 1.000000f, 0.125000f, 0.142006f, -0.983844f, 0.109022f },
		{ 0.625000f, 1.000000f, 0.250000f, 0.139540f, -0.966759f, 0.214257f },
		{ 0.625000f, 1.000000f, 0.375000f, 0.135701f, -0.940161f, 0.312543f },
		{ 0.625000f, 1.000000f, 0.500000f, 0.130822f, -0.906360f, 0.401742f },
		{ 0.625000f, 1.000000f, 0.625000f, 0.122124f, -0.882045f, 0.455064f },
		{ 0.625000f, 1.000000f, 0.750000f, 0.112393f, -0.852754f, 0.510077f },
		{ 0.625000f, 1.000000f, 0.875000f, 0.101610f, -0.818067f, 0.566075f },
		{ 0.625000f, 1.000000f, 1.000000f, 0.089798f, -0.777716f, 0.622169f },
		{ 0.750000f, 0.000000f, 0.000000f, 0.277350f, 0.960769f, 0.000000f },
		{ 0.750000f, 0.000000f, 0.125000f, 0.275791f, 0.955370f, 0.105866f },
		{ 0.750000f, 0.000000f, 0.250000f, 0.271269f, 0.939702f, 0.208261f },
		{ 0.750000f, 0.000000f, 0.375000f, 0.264201f, 0.915220f, 0.304252f },
		{ 0.750000f, 0.000000f, 0.500000f, 0.255175f, 0.883952f, 0.391810f },
		{ 0.750000f, 0.000000f, 0.625000f, 0.238961f, 0.862950f, 0.445213f },
		{ 0.750000f, 0.000000f, 0.750000f, 0.220644f, 0.837041f, 0.500678f },
		{ 0.750000f, 0.000000f, 0.875000f, 0.200143f, 0.805685f, 0.557507f },
		{ 0.750000f, 0.000000f, 1.000000f, 0.177462f, 0.768477f, 0.614777f },
		{ 0.750000f, 0.125000f, 0.000000f, 0.269408f, 0.699942f, 0.000000f },
		{ 0.750000f, 0.125000f, 0.125000f, 0.266883f, 0.693382f, 0.102447f },
		{ 0.750000f, 0.125000f, 0.250000f, 0.259713f, 0.674754f, 0.199389f },
		{ 0.750000f, 0.125000f, 0.375000f, 0.248949f, 0.646788f, 0.286688f },
		{ 0.750000f, 0.125000f, 0.500000f, 0.235905f, 0.612899f, 0.362221f },
		{ 0.750000f, 0.125000f, 0.625000f, 0.218270f, 0.591172f, 0.406663f },
		{ 0.750000f, 0.125000f, 0.750000f, 0.198719f, 0.565399f, 0.450926f },
		{ 0.750000f, 0.125000f, 0.875000f, 0.206962f, 0.624851f, 0.576501f },
		{ 0.750000f, 0.125000f, 1.000000f, 0.206068f, 0.669264f, 0.713878f },
		{ 0.750000f, 0.250000f, 0.000000f, 0.250000f, 0.433013f, 0.000000f },
		{ 0.750000f, 0.250000f, 0.125000f, 0.245519f, 0.425251f, 0.094246f },
		{ 0.750000f, 0.250000f, 0.250000f, 0.233395f, 0.404252f, 0.179184f },
		{ 0.750000f, 0.250000f, 0.375000f, 0.216652f, 0.375252f, 0.249495f },
		{ 0.750000f, 0.250000f, 0.500000f, 0.198300f, 0.343466f, 0.304481f },
		{ 0.750000f, 0.250000f, 0.625000f, 0.224775f, 0.405860f, 0.418782f },
		{ 0.750000f, 0.250000f, 0.750000f, 0.240229f, 0.455670f, 0.545120f },
		{ 0.750000f, 0.250000f, 0.875000f, 0.244470f, 0.492062f, 0.680981f },
		{ 0.750000f, 0.250000f, 1.000000f, 0.237763f, 0.514803f, 0.823679f },
		{ 0.750000f, 0.375000f, 0.000000f, 0.416025f, 0.277350f, 0.000000f },
		{ 0.750000f, 0.375000f, 0.125000f, 0.404528f, 0.269686f, 0.116733f },
		{ 0.750000f, 0.375000f, 0.250000f, 0.375027f, 0.250018f, 0.216439f },
		{ 0.750000f, 0.375000f, 0.375000f, 0.337570f, 0.225047f, 0.292233f },
		{ 0.750000f, 0.375000f, 0.500000f, 0.300055f, 0.200037f, 0.346341f },
		{ 0.750000f, 0.375000f, 0.625000f, 0.337317f, 0.233137f, 0.471687f },
		{ 0.750000f, 0.375000f, 0.750000f, 0.347830f, 0.259366f, 0.611755f },
		{ 0.750000f, 0.375000f, 0.875000f, 0.325497f, 0.277841f, 0.763204f },
		{ 0.750000f, 0.375000f, 1.000000f, 0.265623f, 0.287562f, 0.920192f },
		{ 0.750000f, 0.500000f, 0.000000f, 0.500000f, 0.000000f, 0.000000f },
		{ 0.750000f, 0.500000f, 0.125000f, 0.480399f, 0.000000f, 0.138626f },
		{ 0.750000f, 0.500000f, 0.250000f, 0.433054f, 0.000000f, 0.249928f },
		{ 0.750000f, 0.500000f, 0.375000f, 0.378026f, 0.000000f, 0.327255f },
		{ 0.750000f, 0.500000f, 0.500000f, 0.327398f, 0.000000f, 0.377903f },
		{ 0.750000f, 0.500000f, 0.625000f, 0.363558f, 0.000000f, 0.508381f },
		{ 0.750000f, 0.500000f, 0.750000f, 0.370702f, 0.000000f, 0.651982f },
		{ 0.750000f, 0.500000f, 0.875000f, 0.343262f, 0.000000f, 0.804858f },
		{ 0.750000f, 0.500000f, 1.000000f, 0.277337f, 0.000000f, 0.960773f },
		{ 0.750000f, 0.625000f, 0.000000f, 0.416025f, -0.277350f, 0.000000f },
		{ 0.750000f, 0.625000f, 0.125000f, 0.404528f, -0.269686f, 0.116733f },
		{ 0.750000f, 0.625000f, 0.250000f, 0.375027f, -0.250018f, 0.216439f },
		{ 0.750000f, 0.625000f, 0.375000f, 0.337570f, -0.225047f, 0.292233f },
		{ 0.750000f, 0.625000f, 0.500000f, 0.300055f, -0.200037f, 0.346341f },
		{ 0.750000f, 0.625000f, 0.625000f, 0.337317f, -0.233137f, 0.471687f },
		{ 0.750000f, 0.625000f, 0.750000f, 0.347830f, -0.259366f, 0.611755f },
		{ 0.750000f, 0.625000f, 0.875000f, 0.325497f, -0.277841f, 0.763204f },
		{ 0.750000f, 0.625000f, 1.000000f, 0.265623f, -0.287562f, 0.920192f },
		{ 0.750000f, 0.750000f, 0.000000f, 0.250000f, -0.433013f, 0.000000f },
		{ 0.750000f, 0.750000f, 0.125000f, 0.245519f, -0.425251f, 0.094246f },
		{ 0.750000f, 0.750000f, 0.250000f, 0.233395f, -0.404252f, 0.179184f },
		{ 0.750000f, 0.750000f, 0.375000f, 0.216652f, -0.375252f, 0.249495f },
		{ 0.750000f, 0.750000f, 0.500000f, 0.198300f, -0.343466f, 0.304481f },
		{ 0.750000f, 0.750000f, 0.625000f, 0.224775f, -0.405860f, 0.418782f },
		{ 0.750000f, 0.750000f, 0.750000f, 0.240229f, -0.455670f, 0.545120f },
		{ 0.750000f, 0.750000f, 0.875000f, 0.244470f, -0.492062f, 0.680981f },
		{ 0.750000f, 0.750000f, 1.000000f, 0.237763f, -0.514803f, 0.823679f },
		{ 0.750000f, 0.875000f, 0.000000f, 0.269408f, -0.699942f, 0.000000f },
		{ 0.750000f, 0.875000f, 0.125000f, 0.266883f, -0.693382f, 0.102447f },
		{ 0.750000f, 0.875000f, 0.250000f, 0.259713f, -0.674754f, 0.199389f },
		{ 0.750000f, 0.875000f, 0.375000f, 0.248949f, -0.646788f, 0.286688f },
		{ 0.750000f, 0.875000f, 0.500000f, 0.235905f, -0.612899f, 0.362221f },
		{ 0.750000f, 0.875000f, 0.625000f, 0.218270f, -0.591172f, 0.406663f },
		{ 0.750000f, 0.875000f, 0.750000f, 0.198719f, -0.565399f, 0.450926f },
		{ 0.750000f, 0.875000f, 0.875000f, 0.206962f, -0.624851f, 0.576501f },
		{ 0.750000f, 0.875000f, 1.000000f, 0.206068f, -0.669264f, 0.713878f },
		{ 0.750000f, 1.000000f, 0.000000f, 0.277350f, -0.960769f, 0.000000f },
		{ 0.750000f, 1.000000f, 0.125000f, 0.275791f, -0.955370f, 0.105866f },
		{ 0.750000f, 1.000000f, 0.250000f, 0.271269f, -0.939702f, 0.208261f },
		{ 0.750000f, 1.000000f, 0.375000f, 0.264201f, -0.915220f, 0.304252f },
		{ 0.750000f, 1.000000f, 0.500000f, 0.255175f, -0.883952f, 0.391810f },
		{ 0.750000f, 1.000000f, 0.625000f, 0.238961f, -0.862950f, 0.445213f },
		{ 0.750000f, 1.000000f, 0.750000f, 0.220644f, -0.837041f, 0.500678f },
		{ 0.750000f, 1.000000f, 0.875000f, 0.200143f, -0.805685f, 0.557507f },
		{ 0.750000f, 1.000000f, 1.000000f, 0.177462f, -0.768477f, 0.614777f },
		{ 0.875000f, 0.000000f, 0.000000f, 0.397360f, 0.917663f, 0.000000f },
		{ 0.875000f, 0.000000f, 0.125000f, 0.395321f, 0.912955f, 0.101166f },
		{ 0.875000f, 0.000000f, 0.250000f, 0.389388f, 0.899254f, 0.199296f },
		{ 0.875000f, 0.000000f, 0.375000f, 0.380068f, 0.877729f, 0.291789f },
		{ 0.875000f, 0.000000f, 0.500000f, 0.368076f, 0.850035f, 0.376776f },
		{ 0.875000f, 0.000000f, 0.625000f, 0.346295f, 0.833709f, 0.430127f },
		{ 0.875000f, 0.000000f, 0.750000f, 0.321333f, 0.812679f, 0.486106f },
		{ 0.875000f, 0.000000f, 0.875000f, 0.292970f, 0.786241f, 0.544053f },
		{ 0.875000f, 0.000000f, 1.000000f, 0.261103f, 0.753783f, 0.603023f },
		{ 0.875000f, 0.125000f, 0.000000f, 0.375000f, 0.649519f, 0.000000f },
		{ 0.875000f, 0.125000f, 0.125000f, 0.371967f, 0.644266f, 0.095190f },
		{ 0.875000f, 0.125000f, 0.250000f, 0.363293f, 0.629241f, 0.185940f },
		{ 0.875000f, 0.125000f, 0.375000f, 0.350093f, 0.606378f, 0.268776f },
		{ 0.875000f, 0.125000f, 0.500000f, 0.333817f, 0.578188f, 0.341708f },
		{ 0.875000f, 0.125000f, 0.625000f, 0.311338f, 0.562162f, 0.386707f },
		{ 0.875000f, 0.125000f, 0.750000f, 0.285802f, 0.542113f, 0.432355f },
		{ 0.875000f, 0.125000f, 0.875000f, 0.300126f, 0.604085f, 0.557342f },
		{ 0.875000f, 0.125000f, 1.000000f, 0.301211f, 0.652179f, 0.695654f },
		{ 0.875000f, 0.250000f, 0.000000f, 0.560557f, 0.498273f, 0.000000f },
		{ 0.875000f, 0.250000f, 0.125000f, 0.554851f, 0.493201f, 0.106740f },
		{ 0.875000f, 0.250000f, 0.250000f, 0.538724f, 0.478866f, 0.207276f },
		{ 0.875000f, 0.250000f, 0.375000f, 0.514714f, 0.457523f, 0.297057f },
		{ 0.875000f, 0.250000f, 0.500000f, 0.485922f, 0.431930f, 0.373920f },
		{ 0.875000f, 0.250000f, 0.625000f, 0.454898f, 0.419204f, 0.424071f },
		{ 0.875000f, 0.250000f, 0.750000f, 0.408959f, 0.406598f, 0.479511f },
		{ 0.875000f, 0.250000f, 0.875000f, 0.401953f, 0.457471f, 0.628315f },
		{ 0.875000f, 0.250000f, 1.000000f, 0.344675f, 0.497524f, 0.796033f },
		{ 0.875000f, 0.375000f, 0.000000f, 0.685359f, 0.304604f, 0.000000f },
		{ 0.875000f, 0.375000f, 0.125000f, 0.675008f, 0.300003f, 0.129856f },
		{ 0.875000f, 0.375000f, 0.250000f, 0.646560f, 0.287360f, 0.248766f },
		{ 0.875000f, 0.375000f, 0.375000f, 0.606218f, 0.269430f, 0.349867f },
		{ 0.875000f, 0.375000f, 0.500000f, 0.560628f, 0.249168f, 0.431407f },
		{ 0.875000f, 0.375000f, 0.625000f, 0.519861f, 0.239535f, 0.484631f },
		{ 0.875000f, 0.375000f, 0.750000f, 0.463182f, 0.230254f, 0.543089f },
		{ 0.875000f, 0.375000f, 0.875000f, 0.450810f, 0.256538f, 0.704686f },
		{ 0.875000f, 0.375000f, 1.000000f, 0.381947f, 0.275663f, 0.882115f },
		{ 0.875000f, 0.500000f, 0.000000f, 0.750000f, 0.000000f, 0.000000f },
		{ 0.875000f, 0.500000f, 0.125000f, 0.736495f, 0.000000f, 0.141685f },
		{ 0.875000f, 0.500000f, 0.250000f, 0.699977f, 0.000000f, 0.269318f },
		{ 0.875000f, 0.500000f, 0.375000f, 0.649581f, 0.000000f, 0.374893f },
		{ 0.875000f, 0.500000f, 0.500000f, 0.594389f, 0.000000f, 0.457386f },
		{ 0.875000f, 0.500000f, 0.625000f, 0.548592f, 0.000000f, 0.511416f },
		{ 0.875000f, 0.500000f, 0.750000f, 0.486685f, 0.000000f, 0.570647f },
		{ 0.875000f, 0.500000f, 0.875000f, 0.471532f, 0.000000f, 0.737077f },
		{ 0.875000f, 0.500000f, 1.000000f, 0.397342f, 0.000000f, 0.917671f },
		{ 0.875000f, 0.625000f, 0.000000f, 0.685359f, -0.304604f, 0.000000f },
		{ 0.875000f, 0.625000f, 0.125000f, 0.675008f, -0.300003f, 0.129856f },
		{ 0.875000f, 0.625000f, 0.250000f, 0.646560f, -0.287360f, 0.248766f },
		{ 0.875000f, 0.625000f, 0.375000f, 0.606218f, -0.269430f, 0.349867f },
		{ 0.875000f, 0.625000f, 0.500000f, 0.560628f, -0.249168f, 0.431407f },
		{ 0.875000f, 0.625000f, 0.625000f, 0.519861f, -0.239535f, 0.484631f },
		{ 0.875000f, 0.625000f, 0.750000f, 0.463182f, -0.230254f, 0.543089f },
		{ 0.875000f, 0.625000f, 0.875000f, 0.450810f, -0.256538f, 0.704686f },
		{ 0.875000f, 0.625000f, 1.000000f, 0.381947f, -0.275663f, 0.882115f },
		{ 0.875000f, 0.750000f, 0.000000f, 0.560557f, -0.498273f, 0.000000f },
		{ 0.875000f, 0.750000f, 0.125000f, 0.554851f, -0.493201f, 0.106740f },
		{ 0.875000f, 0.750000f, 0.250000f, 0.538724f, -0.478866f, 0.207276f },
		{ 0.875000f, 0.750000f, 0.375000f, 0.514714f, -0.457523f, 0.297057f },
		{ 0.875000f, 0.750000f, 0.500000f, 0.485922f, -0.431930f, 0.373920f },
		{ 0.875000f, 0.750000f, 0.625000f, 0.454898f, -0.419204f, 0.424071f },
		{ 0.875000f, 0.750000f, 0.750000f, 0.408959f, -0.406598f, 0.479511f },
		{ 0.875000f, 0.750000f, 0.875000f, 0.401953f, -0.457471f, 0.628315f },
		{ 0.875000f, 0.750000f, 1.000000f, 0.344675f, -0.497524f, 0.796033f },
		{ 0.875000f, 0.875000f, 0.000000f, 0.375000f, -0.649519f, 0.000000f },
		{ 0.875000f, 0.875000f, 0.125000f, 0.371967f, -0.644266f, 0.095190f },
		{ 0.875000f, 0.875000f, 0.250000f, 0.363293f, -0.629241f, 0.185940f },
		{ 0.875000f, 0.875000f, 0.375000f, 0.350093f, -0.606378f, 0.268776f },
		{ 0.875000f, 0.875000f, 0.500000f, 0.333817f, -0.578188f, 0.341708f },
		{ 0.875000f, 0.875000f, 0.625000f, 0.311338f, -0.562162f, 0.386707f },
		{ 0.875000f, 0.875000f, 0.750000f, 0.285802f, -0.542113f, 0.432355f },
		{ 0.875000f, 0.875000f, 0.875000f, 0.300126f, -0.604085f, 0.557342f },
		{ 0.875000f, 0.875000f, 1.000000f, 0.301211f, -0.652179f, 0.695654f },
		{ 0.875000f, 1.000000f, 0.000000f, 0.397360f, -0.917663f, 0.000000f },
		{ 0.875000f, 1.000000f, 0.125000f, 0.395321f, -0.912955f, 0.101166f },
		{ 0.875000f, 1.000000f, 0.250000f, 0.389388f, -0.899254f, 0.199296f },
		{ 0.875000f, 1.000000f, 0.375000f, 0.380068f, -0.877729f, 0.291789f },
		{ 0.875000f, 1.000000f, 0.500000f, 0.368076f, -0.850035f, 0.376776f },
		{ 0.875000f, 1.000000f, 0.625000f, 0.346295f, -0.833709f, 0.430127f },
		{ 0.875000f, 1.000000f, 0.750000f, 0.321333f, -0.812679f, 0.486106f },
		{ 0.875000f, 1.000000f, 0.875000f, 0.292970f, -0.786241f, 0.544053f },
		{ 0.875000f, 1.000000f, 1.000000f, 0.261103f, -0.753783f, 0.603023f },
		{ 1.000000f, 0.000000f, 0.000000f, 0.500000f, 0.866025f, 0.000000f },
		{ 1.000000f, 0.000000f, 0.125000f, 0.497713f, 0.862065f, 0.095527f },
		{ 1.000000f, 0.000000f, 0.250000f, 0.491037f, 0.850502f, 0.188492f },
		{ 1.000000f, 0.000000f, 0.375000f, 0.480484f, 0.832222f, 0.276661f },
		{ 1.000000f, 0.000000f, 0.500000f, 0.466790f, 0.808504f, 0.358368f },
		{ 1.000000f, 0.000000f, 0.625000f, 0.441592f, 0.797353f, 0.411370f },
		{ 1.000000f, 0.000000f, 0.750000f, 0.412211f, 0.781888f, 0.467688f },
		{ 1.000000f, 0.000000f, 0.875000f, 0.378206f, 0.761242f, 0.526754f },
		{ 1.000000f, 0.000000f, 1.000000f, 0.339259f, 0.734560f, 0.587644f },
		{ 1.000000f, 0.125000f, 0.000000f, 0.707107f, 0.707107f, 0.000000f },
		{ 1.000000f, 0.125000f, 0.125000f, 0.703455f, 0.703455f, 0.101496f },
		{ 1.000000f, 0.125000f, 0.250000f, 0.692831f, 0.692831f, 0.199927f },
		{ 1.000000f, 0.125000f, 0.375000f, 0.676146f, 0.676146f, 0.292668f },
		{ 1.000000f, 0.125000f, 0.500000f, 0.654689f, 0.654689f, 0.377841f },
		{ 1.000000f, 0.125000f, 0.625000f, 0.624555f, 0.647493f, 0.436673f },
		{ 1.000000f, 0.125000f, 0.750000f, 0.575019f, 0.643161f, 0.505665f },
		{ 1.000000f, 0.125000f, 0.875000f, 0.499138f, 0.639089f, 0.585172f },
		{ 1.000000f, 0.125000f, 1.000000f, 0.388153f, 0.630319f, 0.672336f },
		{ 1.000000f, 0.250000f, 0.000000f, 0.832050f, 0.554700f, 0.000000f },
		{ 1.000000f, 0.250000f, 0.125000f, 0.826119f, 0.550746f, 0.119194f },
		{ 1.000000f, 0.250000f, 0.250000f, 0.809057f, 0.539371f, 0.233465f },
		{ 1.000000f, 0.250000f, 0.375000f, 0.782828f, 0.521885f, 0.338845f },
		{ 1.000000f, 0.250000f, 0.500000f, 0.750054f, 0.500036f, 0.432878f },
		{ 1.000000f, 0.250000f, 0.625000f, 0.713098f, 0.492858f, 0.498580f },
		{ 1.000000f, 0.250000f, 0.750000f, 0.655214f, 0.488573f, 0.576187f },
		{ 1.000000f, 0.250000f, 0.875000f, 0.567682f, 0.484568f, 0.665531f },
		{ 1.000000f, 0.250000f, 1.000000f, 0.439701f, 0.476018f, 0.761624f },
		{ 1.000000f, 0.375000f, 0.000000f, 0.948683f, 0.316228f, 0.000000f },
		{ 1.000000f, 0.375000f, 0.125000f, 0.939919f, 0.313306f, 0.135614f },
		{ 1.000000f, 0.375000f, 0.250000f, 0.915016f, 0.305005f, 0.264041f },
		{ 1.000000f, 0.375000f, 0.375000f, 0.877575f, 0.292525f, 0.379856f },
		{ 1.000000f, 0.375000f, 0.500000f, 0.832124f, 0.277375f, 0.480243f },
		{ 1.000000f, 0.375000f, 0.625000f, 0.788535f, 0.272498f, 0.551324f },
		{ 1.000000f, 0.375000f, 0.750000f, 0.723134f, 0.269609f, 0.635915f },
		{ 1.000000f, 0.375000f, 0.875000f, 0.625416f, 0.266925f, 0.733216f },
		{ 1.000000f, 0.375000f, 1.000000f, 0.482618f, 0.261240f, 0.835963f },
		{ 1.000000f, 0.500000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f },
		{ 1.000000f, 0.500000f, 0.125000f, 0.989751f, 0.000000f, 0.142804f },
		{ 1.000000f, 0.500000f, 0.250000f, 0.960797f, 0.000000f, 0.277252f },
		{ 1.000000f, 0.500000f, 0.375000f, 0.917718f, 0.000000f, 0.397232f },
		{ 1.000000f, 0.500000f, 0.500000f, 0.866108f, 0.000000f, 0.499857f },
		{ 1.000000f, 0.500000f, 0.625000f, 0.819550f, 0.000000f, 0.573008f },
		{ 1.000000f, 0.500000f, 0.750000f, 0.750941f, 0.000000f, 0.660369f },
		{ 1.000000f, 0.500000f, 0.875000f, 0.648962f, 0.000000f, 0.760821f },
		{ 1.000000f, 0.500000f, 1.000000f, 0.499980f, 0.000000f, 0.866037f },
		{ 1.000000f, 0.625000f, 0.000000f, 0.948683f, -0.316228f, 0.000000f },
		{ 1.000000f, 0.625000f, 0.125000f, 0.939919f, -0.313306f, 0.135614f },
		{ 1.000000f, 0.625000f, 0.250000f, 0.915016f, -0.305005f, 0.264041f },
		{ 1.000000f, 0.625000f, 0.375000f, 0.877575f, -0.292525f, 0.379856f },
		{ 1.000000f, 0.625000f, 0.500000f, 0.832124f, -0.277375f, 0.480243f },
		{ 1.000000f, 0.625000f, 0.625000f, 0.788535f, -0.272498f, 0.551324f },
		{ 1.000000f, 0.625000f, 0.750000f, 0.723134f, -0.269609f, 0.635915f },
		{ 1.000000f, 0.625000f, 0.875000f, 0.625416f, -0.266925f, 0.733216f },
		{ 1.000000f, 0.625000f, 1.000000f, 0.482618f, -0.261240f, 0.835963f },
		{ 1.000000f, 0.750000f, 0.000000f, 0.832050f, -0.554700f, 0.000000f },
		{ 1.000000f, 0.750000f, 0.125000f, 0.826119f, -0.550746f, 0.119194f },
		{ 1.000000f, 0.750000f, 0.250000f, 0.809057f, -0.539371f, 0.233465f },
		{ 1.000000f, 0.750000f, 0.375000f, 0.782828f, -0.521885f, 0.338845f },
		{ 1.000000f, 0.750000f, 0.500000f, 0.750054f, -0.500036f, 0.432878f },
		{ 1.000000f, 0.750000f, 0.625000f, 0.713098f, -0.492858f, 0.498580f },
		{ 1.000000f, 0.750000f, 0.750000f, 0.655214f, -0.488573f, 0.576187f },
		{ 1.000000f, 0.750000f, 0.875000f, 0.567682f, -0.484568f, 0.665531f },
		{ 1.000000f, 0.750000f, 1.000000f, 0.439701f, -0.476018f, 0.761624f },
		{ 1.000000f, 0.875000f, 0.000000f, 0.707107f, -0.707107f, 0.000000f },
		{ 1.000000f, 0.875000f, 0.125000f, 0.703455f, -0.703455f, 0.101496f },
		{ 1.000000f, 0.875000f, 0.250000f, 0.692831f, -0.692831f, 0.199927f },
		{ 1.000000f, 0.875000f, 0.375000f, 0.676146f, -0.676146f, 0.292668f },
		{ 1.000000f, 0.875000f, 0.500000f, 0.654689f, -0.654689f, 0.377841f },
		{ 1.000000f, 0.875000f, 0.625000f, 0.624555f, -0.647493f, 0.436673f },
		{ 1.000000f, 0.875000f, 0.750000f, 0.575019f, -0.643161f, 0.505665f },
		{ 1.000000f, 0.875000f, 0.875000f, 0.499138f, -0.639089f, 0.585172f },
		{ 1.000000f, 0.875000f, 1.000000f, 0.388153f, -0.630319f, 0.672336f },
		{ 1.000000f, 1.000000f, 0.000000f, 0.500000f, -0.866025f, 0.000000f },
		{ 1.000000f, 1.000000f, 0.125000f, 0.497713f, -0.862065f, 0.095527f },
		{ 1.000000f, 1.000000f, 0.250000f, 0.491037f, -0.850502f, 0.188492f },
		{ 1.000000f, 1.000000f, 0.375000f, 0.480484f, -0.832222f, 0.276661f },
		{ 1.000000f, 1.000000f, 0.500000f, 0.466790f, -0.808504f, 0.358368f },
		{ 1.000000f, 1.000000f, 0.625000f, 0.441592f, -0.797353f, 0.411370f },
		{ 1.000000f, 1.000000f, 0.750000f, 0.412211f, -0.781888f, 0.467688f },
		{ 1.000000f, 1.000000f, 0.875000f, 0.378206f, -0.761242f, 0.526754f },
		{ 1.000000f, 1.000000f, 1.000000f, 0.339259f, -0.734560f, 0.587644f }
	};

	// Initialize test tolerance (max limit)
	const float IABTransformTest::tolerance_ = TOL_DISTANCE;

}
