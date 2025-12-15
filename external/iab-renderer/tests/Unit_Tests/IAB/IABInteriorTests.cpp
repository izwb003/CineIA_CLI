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
#include "IABDataTypes.h"
#include "renderer/IABInterior/IABInterior.h"

#define RAD2DEG(x) (57.295779578f * (x))
#define DEG2RAD(x) (0.0174532925f * (x))

// This custom expect less than tolerance defintion checks the given result, expected values are less than tolerance.
#define EXPECT_LE_TOLERENCE(val1, val2, tolerance)\
EXPECT_LE(fabs(val1 - val2), tolerance)


// Co-ordinate tolerance
const float TOL_COORDINATE_DIFFERENCE = 0.00001f;

// Gain tolerance is set to 10e-3 because the discrete gain is chosen from the predetermined gain table using the gain table index.
// The gain table index is calculated from the co-ordinates, there is a chance that Cross-platform computations slips to adjancent gain table
// indexes, choosing the adjancent gains from the table. The discrete gain step is identified as 10e-3 tolerance.
const float TOL_GAIN_DIFFERENCE = 0.01f;


using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{

    class IABInterior_Test : public testing::Test
    {
        
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            
        }
        
        void TearDown()
        {
            
        }
        
        /*
         * InteriorToMDAextendedSourcesTestData_
         *
         * 12 data {radius, azimuth, elevation, azimuth_left, elevation_left, azimuth_right, elevation_right, azimuth_center, elevation_center, normGain_left, normGain_right, normGain_center}
         *
         * Generated using the interior object to normalised objects conversion algorithm.
         */
        static const float InteriorToMDAextendedSourcesTestData_[1309][12];
        
        IABInterior iabInterior_;
        
        void PolarToCartesian(float iElevation, float iAzhimuth, float iRadius, float &oX, float &oY, float &oZ)
        {
            // Convert Polar back to Cartesian
            oX = iRadius * cos(iElevation) * sin(iAzhimuth);
            oY = iRadius * cos(iElevation) * cos(iAzhimuth);
            oZ = iRadius * sin(iElevation);
        }
        
    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run tests to check for interior object with radius  equal to  1.
    // Test coordinates are passed to interior class for extended source conversion.
    // Results are compared with reference values, those are calculated using the formula.
    TEST_F(IABInterior_Test, TestWithRadius_equal_to_1)
    {
        // Normalized object - objects will be created in a normalized group.
        float elevation = 0.0f;
        float azimuth = 0.0f;
        float radius = 0.0f;
        float aperture = 0.0f;
        float divergence = 0.0f;
        
        // Reference values are calculated using the octave language from the formula given here
        
        std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;
        elevation = DEG2RAD(30.0f), azimuth = DEG2RAD(15.0f), radius = 1.0f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        
        float oX, oY, oZ;
        PolarToCartesian(elevation, azimuth, radius, oX, oY, oZ);
        
        EXPECT_EQ(1, extendedSources.size());
        EXPECT_EQ(oX, extendedSources[0].position_.getX());
        EXPECT_EQ(oY, extendedSources[0].position_.getY());
        EXPECT_EQ(oZ, extendedSources[0].position_.getZ());
        EXPECT_EQ(1.0f, extendedSources[0].extSourceGain_);

		// Clear for re-use
		extendedSources.clear();
        
        elevation = DEG2RAD(0.0f), azimuth = DEG2RAD(0.0f), radius = 1.0f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        
        PolarToCartesian(elevation, azimuth, radius, oX, oY, oZ);
        
        EXPECT_EQ(1, extendedSources.size());
        EXPECT_EQ(oX, extendedSources[0].position_.getX());
        EXPECT_EQ(oY, extendedSources[0].position_.getY());
        EXPECT_EQ(oZ, extendedSources[0].position_.getZ());
        EXPECT_EQ(1.0f, extendedSources[0].extSourceGain_);
        
		extendedSources.clear();
    }
    
    // Run tests to check for interior object with radius less than 1.
    // Test coordinates are passed to interior class for extended source conversion.
    // Results are compared with reference values, those are calculated using the formula.
    TEST_F(IABInterior_Test, TestWithRadius_less_than_1)
    {
        
        // Normalized object - objects will be created in a normalized group.
        float elevation = 0.0f;
        float azimuth = 0.0f;
        float radius = 0.0f;
        float aperture = 0.0f;
        float divergence = 0.0f;
        
        // Reference values are calculated from the formula given here
        
        std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;

        elevation = DEG2RAD(30.0f), azimuth = DEG2RAD(15.0f), radius = 0.8f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        
        EXPECT_EQ(3, extendedSources.size());
        
        float oX, oY, oZ;
        PolarToCartesian(0.41151688f, -0.785398f, 1.0f, oX, oY, oZ);
        
        EXPECT_EQ(3, extendedSources.size());
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eLeftNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eLeftNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eLeftNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.31749839, extendedSources[IABInterior::eLeftNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
        PolarToCartesian(0.41151688f, 0.785398f, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eRightNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eRightNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eRightNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.50975436, extendedSources[IABInterior::eRightNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
        PolarToCartesian(elevation, azimuth, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eProjectedNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eProjectedNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eProjectedNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.17274728, extendedSources[IABInterior::eProjectedNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);

		// Clear for re-use
		extendedSources.clear();

        elevation = DEG2RAD(90.0f), azimuth = DEG2RAD(0.0f), radius = 0.5f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        
        EXPECT_EQ(3, extendedSources.size());
        
        
        PolarToCartesian(0.52359879f, -1.5707964f, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eLeftNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eLeftNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eLeftNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.5, extendedSources[IABInterior::eLeftNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
        PolarToCartesian(0.52359879f, 1.5707964f, 1.0f, oX, oY, oZ);
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eRightNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eRightNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eRightNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.5, extendedSources[IABInterior::eRightNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
        PolarToCartesian(elevation, azimuth, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eProjectedNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eProjectedNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eProjectedNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(0.0, extendedSources[IABInterior::eProjectedNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
    }
    
    // Run tests to check for interior object with radius greater than 1.
    // Test coordinates are passed to interior class for extended source conversion.
    // Results are compared with reference values, those are calculated using the formula.
    TEST_F(IABInterior_Test, TestWithRadius_greater_than_1)
    {
        float elevation = 0.0f;
        float azimuth = 0.0f;
        float radius = 0.0f;
        float aperture = 0.0f;
        float divergence = 0.0f;
        
        // Reference values are calculated from the formula given here
        
        std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;

        elevation = DEG2RAD(30.0f), azimuth = DEG2RAD(15.0f), radius = 1.2f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        EXPECT_EQ(1, extendedSources.size());
        
        float oX, oY, oZ;
        PolarToCartesian(elevation, azimuth, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[0].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[0].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[0].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(1.0f, extendedSources[0].extSourceGain_, TOL_GAIN_DIFFERENCE);

		// Clear for re-use
		extendedSources.clear();

        elevation = DEG2RAD(0.0f), azimuth = DEG2RAD(0.0f), radius = 1.5f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        EXPECT_EQ(1, extendedSources.size());
        
        PolarToCartesian(elevation, azimuth, 1.0f, oX, oY, oZ);
        
        EXPECT_LE_TOLERENCE(oX, extendedSources[0].position_.getX(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oY, extendedSources[0].position_.getY(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(oZ, extendedSources[0].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
        EXPECT_LE_TOLERENCE(1.0f, extendedSources[0].extSourceGain_, TOL_GAIN_DIFFERENCE);
        
		extendedSources.clear();
	}
    
    // Run tests to check for interior object with radius less than 0.
    // Test coordinates are passed to interior class for extended source conversion.
    TEST_F(IABInterior_Test, TestWithRadius_less_than_0)
    {
        
        // Normalized object - objects will be created in a normalized group
        float elevation = 0.0f;
        float azimuth = 0.0f;
        float radius = 0.0f;
        float aperture = 0.0f;
        float divergence = 0.0f;
        std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;

		// Test radius error condition using negative radius value of -0.5f
		elevation = DEG2RAD(30.0f), azimuth = DEG2RAD(15.0f), radius = -0.5f;
        EXPECT_EQ(kIABRendererInvalidRadiusError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                               elevation,
                                                                                               radius,
                                                                                               aperture,
                                                                                               divergence,
                                                                                               extendedSources));
        EXPECT_EQ(0, extendedSources.size());
        
		// Clear for re-use
		extendedSources.clear();

		// for test we pass -0.0f
        radius = -0.0f;
        EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(azimuth,
                                                                                   elevation,
                                                                                   radius,
                                                                                   aperture,
                                                                                   divergence,
                                                                                   extendedSources));
        EXPECT_EQ(3, extendedSources.size());
    }
    
    // Run tests to check the range of radius, elevation and azimuth
    // radius - from 0.1 to 0.9
    // elevation - from 0 to 90 deg with step of 15 deg
    // azimuth - from 0 to 360 deg with step of 22.5 deg
    TEST_F(IABInterior_Test, TestWithTableData)
    {
        std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;
        float oX, oY, oZ;
        float aperture = 0.0f, divergence = 0.0f;

		uint32_t start = 0, end = sizeof(InteriorToMDAextendedSourcesTestData_) / sizeof(InteriorToMDAextendedSourcesTestData_[IABInterior::eLeftNormObject]);
        
        for (uint32_t row = start; row < end; row++)
        {
            EXPECT_EQ(kIABNoError, iabInterior_.MapExtendedSourceToVBAPExtendedSources(InteriorToMDAextendedSourcesTestData_[row][IABInterior::eRightNormObject],
                                                                                       InteriorToMDAextendedSourcesTestData_[row][IABInterior::eProjectedNormObject],
                                                                                       InteriorToMDAextendedSourcesTestData_[row][IABInterior::eLeftNormObject],
                                                                                       aperture,
                                                                                       divergence,
                                                                                       extendedSources));
            
            if (extendedSources.size() == 1)
            {
                
                PolarToCartesian(InteriorToMDAextendedSourcesTestData_[row][4], InteriorToMDAextendedSourcesTestData_[row][3], 1.0f, oX, oY, oZ);
                
                EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eLeftNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eLeftNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eLeftNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(InteriorToMDAextendedSourcesTestData_[row][11], extendedSources[IABInterior::eLeftNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
                
            }
            else if (extendedSources.size() == 3)
            {
                PolarToCartesian(InteriorToMDAextendedSourcesTestData_[row][4], InteriorToMDAextendedSourcesTestData_[row][3], 1.0f, oX, oY, oZ);
                
                EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eLeftNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eLeftNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eLeftNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(InteriorToMDAextendedSourcesTestData_[row][9], extendedSources[IABInterior::eLeftNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
                
                
                PolarToCartesian(InteriorToMDAextendedSourcesTestData_[row][6], InteriorToMDAextendedSourcesTestData_[row][5], 1.0f, oX, oY, oZ);
                
                EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eRightNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eRightNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eRightNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(InteriorToMDAextendedSourcesTestData_[row][10], extendedSources[IABInterior::eRightNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
                
                PolarToCartesian(InteriorToMDAextendedSourcesTestData_[row][8], InteriorToMDAextendedSourcesTestData_[row][7], 1.0f, oX, oY, oZ);
                
                EXPECT_LE_TOLERENCE(oX, extendedSources[IABInterior::eProjectedNormObject].position_.getX(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oY, extendedSources[IABInterior::eProjectedNormObject].position_.getY(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(oZ, extendedSources[IABInterior::eProjectedNormObject].position_.getZ(), TOL_COORDINATE_DIFFERENCE);
                EXPECT_LE_TOLERENCE(InteriorToMDAextendedSourcesTestData_[row][11], extendedSources[IABInterior::eProjectedNormObject].extSourceGain_, TOL_GAIN_DIFFERENCE);
            }
        }
    }

    /*
     * InteriorToMDAextendedSourcesTestData_
     *
     * 12 data (first 3 are inputs, next 9 are outputs) {radius, azimuth, elevation, azimuth_left, elevation_left, azimuth_right, elevation_right, azimuth_projected, elevation_projected, normGain_left, normGain_right, normGain_projected}
     *
     * radius - from 0.0 to 1
     * azimuth - from 0 to 360 deg with step of 22.5 deg
     * elevation - from 0 to 90 deg with step of 15 deg
     *
     * Generated using the interior object to normalised objects conversion algorithm  r
     */
    const float IABInterior_Test::InteriorToMDAextendedSourcesTestData_[1309][12] = {
        {0.000000f, 0.000000f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.000000f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.392699f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 0.785398f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.178097f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.570796f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 1.963495f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.356194f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 2.748894f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.141593f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.534292f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 3.926991f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.319690f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 4.712389f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.105088f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.497787f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 5.890486f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 0.261799f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 0.523599f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 0.785398f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 1.047198f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 1.308997f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.000000f, 6.283185f, 1.570796f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 0.000000f, -1.470629f, 0.000000f, 1.470629f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 0.261799f, -1.474020f, 0.025885f, 1.474020f, 0.025885f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 0.523599f, -1.483976f, 0.050021f, 1.483976f, 0.050021f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 0.785398f, -1.499849f, 0.070770f, 1.499849f, 0.070770f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 1.047198f, -1.520587f, 0.086711f, 1.520587f, 0.086711f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 1.308997f, -1.544790f, 0.096743f, 1.544790f, 0.096743f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.000000f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.392699f, 0.000000f, -1.478276f, 0.000000f, 1.478276f, 0.000000f, 0.392699f, 0.000000f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 0.392699f, 0.261799f, -1.481408f, 0.025885f, 1.481408f, 0.025885f, 0.392699f, 0.261799f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 0.392699f, 0.523599f, -1.490600f, 0.050021f, 1.490600f, 0.050021f, 0.392699f, 0.523599f, 0.488184f, 0.511816f, 0.000000f},
        {0.100000f, 0.392699f, 0.785398f, -1.505257f, 0.070770f, 1.505257f, 0.070770f, 0.392699f, 0.785398f, 0.488184f, 0.511816f, 0.000000f},
        {0.100000f, 0.392699f, 1.047198f, -1.524412f, 0.086711f, 1.524412f, 0.086711f, 0.392699f, 1.047198f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 0.392699f, 1.308997f, -1.546770f, 0.096743f, 1.546770f, 0.096743f, 0.392699f, 1.308997f, 0.496062f, 0.503938f, 0.000000f},
        {0.100000f, 0.392699f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 0.785398f, 0.000000f, -1.500027f, 0.000000f, 1.500027f, 0.000000f, 0.785398f, 0.000000f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 0.785398f, 0.261799f, -1.502419f, 0.025885f, 1.502419f, 0.025885f, 0.785398f, 0.261799f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 0.785398f, 0.523599f, -1.509444f, 0.050021f, 1.509444f, 0.050021f, 0.785398f, 0.523599f, 0.476354f, 0.523646f, 0.000000f},
        {0.100000f, 0.785398f, 0.785398f, -1.520650f, 0.070770f, 1.520650f, 0.070770f, 0.785398f, 0.785398f, 0.480299f, 0.519701f, 0.000000f},
        {0.100000f, 0.785398f, 1.047198f, -1.535300f, 0.086711f, 1.535300f, 0.086711f, 0.785398f, 1.047198f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 0.785398f, 1.308997f, -1.552408f, 0.096743f, 1.552408f, 0.096743f, 0.785398f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 0.785398f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 1.178097f, 0.000000f, -1.532519f, 0.000000f, 1.532519f, 0.000000f, 1.178097f, 0.000000f, 0.464498f, 0.535502f, 0.000000f},
        {0.100000f, 1.178097f, 0.261799f, -1.533811f, 0.025885f, 1.533811f, 0.025885f, 1.178097f, 0.261799f, 0.464498f, 0.535502f, 0.000000f},
        {0.100000f, 1.178097f, 0.523599f, -1.537607f, 0.050021f, 1.537607f, 0.050021f, 1.178097f, 0.523599f, 0.468454f, 0.531546f, 0.000000f},
        {0.100000f, 1.178097f, 0.785398f, -1.543665f, 0.070770f, 1.543665f, 0.070770f, 1.178097f, 0.785398f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 1.178097f, 1.047198f, -1.551589f, 0.086711f, 1.551589f, 0.086711f, 1.178097f, 1.047198f, 0.480299f, 0.519701f, 0.000000f},
        {0.100000f, 1.178097f, 1.308997f, -1.560845f, 0.096743f, 1.560845f, 0.096743f, 1.178097f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 1.178097f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.460539f, 0.539461f, 0.000000f},
        {0.100000f, 1.570796f, 0.261799f, -1.570796f, 0.025885f, 1.570796f, 0.025885f, 1.570796f, 0.261799f, 0.460539f, 0.539461f, 0.000000f},
        {0.100000f, 1.570796f, 0.523599f, -1.570796f, 0.050021f, 1.570796f, 0.050021f, 1.570796f, 0.523599f, 0.464498f, 0.535502f, 0.000000f},
        {0.100000f, 1.570796f, 0.785398f, -1.570796f, 0.070770f, 1.570796f, 0.070770f, 1.570796f, 0.785398f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 1.570796f, 1.047198f, -1.570796f, 0.086711f, 1.570796f, 0.086711f, 1.570796f, 1.047198f, 0.480299f, 0.519701f, 0.000000f},
        {0.100000f, 1.570796f, 1.308997f, -1.570796f, 0.096743f, 1.570796f, 0.096743f, 1.570796f, 1.308997f, 0.488184f, 0.511816f, 0.000000f},
        {0.100000f, 1.570796f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 1.963495f, 0.000000f, -1.609074f, 0.000000f, 1.609074f, 0.000000f, 1.963495f, 0.000000f, 0.464498f, 0.535502f, 0.000000f},
        {0.100000f, 1.963495f, 0.261799f, -1.607782f, 0.025885f, 1.607782f, 0.025885f, 1.963495f, 0.261799f, 0.464498f, 0.535502f, 0.000000f},
        {0.100000f, 1.963495f, 0.523599f, -1.603985f, 0.050021f, 1.603985f, 0.050021f, 1.963495f, 0.523599f, 0.468454f, 0.531546f, 0.000000f},
        {0.100000f, 1.963495f, 0.785398f, -1.597927f, 0.070770f, 1.597927f, 0.070770f, 1.963495f, 0.785398f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 1.963495f, 1.047198f, -1.590004f, 0.086711f, 1.590004f, 0.086711f, 1.963495f, 1.047198f, 0.480299f, 0.519701f, 0.000000f},
        {0.100000f, 1.963495f, 1.308997f, -1.580748f, 0.096743f, 1.580748f, 0.096743f, 1.963495f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 1.963495f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 2.356194f, 0.000000f, -1.641566f, 0.000000f, 1.641566f, 0.000000f, 2.356194f, 0.000000f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 2.356194f, 0.261799f, -1.639174f, 0.025885f, 1.639174f, 0.025885f, 2.356194f, 0.261799f, 0.472406f, 0.527594f, 0.000000f},
        {0.100000f, 2.356194f, 0.523599f, -1.632149f, 0.050021f, 1.632149f, 0.050021f, 2.356194f, 0.523599f, 0.476354f, 0.523646f, 0.000000f},
        {0.100000f, 2.356194f, 0.785398f, -1.620943f, 0.070770f, 1.620943f, 0.070770f, 2.356194f, 0.785398f, 0.480299f, 0.519701f, 0.000000f},
        {0.100000f, 2.356194f, 1.047198f, -1.606292f, 0.086711f, 1.606292f, 0.086711f, 2.356194f, 1.047198f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 2.356194f, 1.308997f, -1.589185f, 0.096743f, 1.589185f, 0.096743f, 2.356194f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 2.356194f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 2.748894f, 0.000000f, -1.663316f, 0.000000f, 1.663316f, 0.000000f, 2.748894f, 0.000000f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 2.748894f, 0.261799f, -1.660185f, 0.025885f, 1.660185f, 0.025885f, 2.748894f, 0.261799f, 0.484242f, 0.515758f, 0.000000f},
        {0.100000f, 2.748894f, 0.523599f, -1.650993f, 0.050021f, 1.650993f, 0.050021f, 2.748894f, 0.523599f, 0.488184f, 0.511816f, 0.000000f},
        {0.100000f, 2.748894f, 0.785398f, -1.636335f, 0.070770f, 1.636335f, 0.070770f, 2.748894f, 0.785398f, 0.488184f, 0.511816f, 0.000000f},
        {0.100000f, 2.748894f, 1.047198f, -1.617181f, 0.086711f, 1.617181f, 0.086711f, 2.748894f, 1.047198f, 0.492123f, 0.507877f, 0.000000f},
        {0.100000f, 2.748894f, 1.308997f, -1.594823f, 0.096743f, 1.594823f, 0.096743f, 2.748894f, 1.308997f, 0.496062f, 0.503938f, 0.000000f},
        {0.100000f, 2.748894f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 0.000000f, -1.670964f, 0.000000f, 1.670964f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 0.261799f, -1.667572f, 0.025885f, 1.667572f, 0.025885f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 0.523599f, -1.657616f, 0.050021f, 1.657616f, 0.050021f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 0.785398f, -1.641744f, 0.070770f, 1.641744f, 0.070770f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 1.047198f, -1.621006f, 0.086711f, 1.621006f, 0.086711f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 1.308997f, -1.596803f, 0.096743f, 1.596803f, 0.096743f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.141593f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.534292f, 0.000000f, -1.663316f, 0.000000f, 1.663316f, 0.000000f, 3.534292f, 0.000000f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 3.534292f, 0.261799f, -1.660185f, 0.025885f, 1.660185f, 0.025885f, 3.534292f, 0.261799f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 3.534292f, 0.523599f, -1.650993f, 0.050021f, 1.650993f, 0.050021f, 3.534292f, 0.523599f, 0.511816f, 0.488184f, 0.000000f},
        {0.100000f, 3.534292f, 0.785398f, -1.636335f, 0.070770f, 1.636335f, 0.070770f, 3.534292f, 0.785398f, 0.511816f, 0.488184f, 0.000000f},
        {0.100000f, 3.534292f, 1.047198f, -1.617181f, 0.086711f, 1.617181f, 0.086711f, 3.534292f, 1.047198f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 3.534292f, 1.308997f, -1.594823f, 0.096743f, 1.594823f, 0.096743f, 3.534292f, 1.308997f, 0.503938f, 0.496062f, 0.000000f},
        {0.100000f, 3.534292f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 3.926991f, 0.000000f, -1.641566f, 0.000000f, 1.641566f, 0.000000f, 3.926991f, 0.000000f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 3.926991f, 0.261799f, -1.639174f, 0.025885f, 1.639174f, 0.025885f, 3.926991f, 0.261799f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 3.926991f, 0.523599f, -1.632149f, 0.050021f, 1.632149f, 0.050021f, 3.926991f, 0.523599f, 0.523646f, 0.476354f, 0.000000f},
        {0.100000f, 3.926991f, 0.785398f, -1.620943f, 0.070770f, 1.620943f, 0.070770f, 3.926991f, 0.785398f, 0.519701f, 0.480299f, 0.000000f},
        {0.100000f, 3.926991f, 1.047198f, -1.606292f, 0.086711f, 1.606292f, 0.086711f, 3.926991f, 1.047198f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 3.926991f, 1.308997f, -1.589185f, 0.096743f, 1.589185f, 0.096743f, 3.926991f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 3.926991f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 4.319690f, 0.000000f, -1.609074f, 0.000000f, 1.609074f, 0.000000f, 4.319690f, 0.000000f, 0.535502f, 0.464498f, 0.000000f},
        {0.100000f, 4.319690f, 0.261799f, -1.607782f, 0.025885f, 1.607782f, 0.025885f, 4.319690f, 0.261799f, 0.535502f, 0.464498f, 0.000000f},
        {0.100000f, 4.319690f, 0.523599f, -1.603985f, 0.050021f, 1.603985f, 0.050021f, 4.319690f, 0.523599f, 0.531546f, 0.468454f, 0.000000f},
        {0.100000f, 4.319690f, 0.785398f, -1.597927f, 0.070770f, 1.597927f, 0.070770f, 4.319690f, 0.785398f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 4.319690f, 1.047198f, -1.590004f, 0.086711f, 1.590004f, 0.086711f, 4.319690f, 1.047198f, 0.519701f, 0.480299f, 0.000000f},
        {0.100000f, 4.319690f, 1.308997f, -1.580748f, 0.096743f, 1.580748f, 0.096743f, 4.319690f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 4.319690f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.539461f, 0.460539f, 0.000000f},
        {0.100000f, 4.712389f, 0.261799f, -1.570796f, 0.025885f, 1.570796f, 0.025885f, 4.712389f, 0.261799f, 0.539461f, 0.460539f, 0.000000f},
        {0.100000f, 4.712389f, 0.523599f, -1.570796f, 0.050021f, 1.570796f, 0.050021f, 4.712389f, 0.523599f, 0.535502f, 0.464498f, 0.000000f},
        {0.100000f, 4.712389f, 0.785398f, -1.570796f, 0.070770f, 1.570796f, 0.070770f, 4.712389f, 0.785398f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 4.712389f, 1.047198f, -1.570796f, 0.086711f, 1.570796f, 0.086711f, 4.712389f, 1.047198f, 0.519701f, 0.480299f, 0.000000f},
        {0.100000f, 4.712389f, 1.308997f, -1.570796f, 0.096743f, 1.570796f, 0.096743f, 4.712389f, 1.308997f, 0.511816f, 0.488184f, 0.000000f},
        {0.100000f, 4.712389f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 5.105088f, 0.000000f, -1.532519f, 0.000000f, 1.532519f, 0.000000f, 5.105088f, 0.000000f, 0.535502f, 0.464498f, 0.000000f},
        {0.100000f, 5.105088f, 0.261799f, -1.533811f, 0.025885f, 1.533811f, 0.025885f, 5.105088f, 0.261799f, 0.535502f, 0.464498f, 0.000000f},
        {0.100000f, 5.105088f, 0.523599f, -1.537607f, 0.050021f, 1.537607f, 0.050021f, 5.105088f, 0.523599f, 0.531546f, 0.468454f, 0.000000f},
        {0.100000f, 5.105088f, 0.785398f, -1.543665f, 0.070770f, 1.543665f, 0.070770f, 5.105088f, 0.785398f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 5.105088f, 1.047198f, -1.551589f, 0.086711f, 1.551589f, 0.086711f, 5.105088f, 1.047198f, 0.519701f, 0.480299f, 0.000000f},
        {0.100000f, 5.105088f, 1.308997f, -1.560845f, 0.096743f, 1.560845f, 0.096743f, 5.105088f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 5.105088f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 5.497787f, 0.000000f, -1.500027f, 0.000000f, 1.500027f, 0.000000f, 5.497787f, 0.000000f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 5.497787f, 0.261799f, -1.502419f, 0.025885f, 1.502419f, 0.025885f, 5.497787f, 0.261799f, 0.527594f, 0.472406f, 0.000000f},
        {0.100000f, 5.497787f, 0.523599f, -1.509444f, 0.050021f, 1.509444f, 0.050021f, 5.497787f, 0.523599f, 0.523646f, 0.476354f, 0.000000f},
        {0.100000f, 5.497787f, 0.785398f, -1.520650f, 0.070770f, 1.520650f, 0.070770f, 5.497787f, 0.785398f, 0.519701f, 0.480299f, 0.000000f},
        {0.100000f, 5.497787f, 1.047198f, -1.535300f, 0.086711f, 1.535300f, 0.086711f, 5.497787f, 1.047198f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 5.497787f, 1.308997f, -1.552408f, 0.096743f, 1.552408f, 0.096743f, 5.497787f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 5.497787f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 5.890486f, 0.000000f, -1.478276f, 0.000000f, 1.478276f, 0.000000f, 5.890486f, 0.000000f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 5.890486f, 0.261799f, -1.481408f, 0.025885f, 1.481408f, 0.025885f, 5.890486f, 0.261799f, 0.515758f, 0.484242f, 0.000000f},
        {0.100000f, 5.890486f, 0.523599f, -1.490600f, 0.050021f, 1.490600f, 0.050021f, 5.890486f, 0.523599f, 0.511816f, 0.488184f, 0.000000f},
        {0.100000f, 5.890486f, 0.785398f, -1.505257f, 0.070770f, 1.505257f, 0.070770f, 5.890486f, 0.785398f, 0.511816f, 0.488184f, 0.000000f},
        {0.100000f, 5.890486f, 1.047198f, -1.524412f, 0.086711f, 1.524412f, 0.086711f, 5.890486f, 1.047198f, 0.507877f, 0.492123f, 0.000000f},
        {0.100000f, 5.890486f, 1.308997f, -1.546770f, 0.096743f, 1.546770f, 0.096743f, 5.890486f, 1.308997f, 0.503938f, 0.496062f, 0.000000f},
        {0.100000f, 5.890486f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 0.000000f, -1.470629f, 0.000000f, 1.470629f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 0.261799f, -1.474020f, 0.025885f, 1.474020f, 0.025885f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 0.523599f, -1.483976f, 0.050021f, 1.483976f, 0.050021f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 0.785398f, -1.499849f, 0.070770f, 1.499849f, 0.070770f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 1.047198f, -1.520587f, 0.086711f, 1.520587f, 0.086711f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 1.308997f, -1.544790f, 0.096743f, 1.544790f, 0.096743f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.100000f, 6.283185f, 1.570796f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 0.000000f, -1.369438f, 0.000000f, 1.369438f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 0.261799f, -1.376125f, 0.051787f, 1.376125f, 0.051787f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 0.523599f, -1.395827f, 0.100167f, 1.395827f, 0.100167f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 0.785398f, -1.427449f, 0.141897f, 1.427449f, 0.141897f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 1.047198f, -1.469086f, 0.174083f, 1.469086f, 0.174083f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 1.308997f, -1.518014f, 0.194407f, 1.518014f, 0.194407f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.000000f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.392699f, 0.000000f, -1.384952f, 0.000000f, 1.384952f, 0.000000f, 0.392699f, 0.000000f, 0.468454f, 0.531546f, 0.000000f},
        {0.200000f, 0.392699f, 0.261799f, -1.391112f, 0.051787f, 1.391112f, 0.051787f, 0.392699f, 0.261799f, 0.468454f, 0.531546f, 0.000000f},
        {0.200000f, 0.392699f, 0.523599f, -1.409268f, 0.100167f, 1.409268f, 0.100167f, 0.392699f, 0.523599f, 0.472406f, 0.527594f, 0.000000f},
        {0.200000f, 0.392699f, 0.785398f, -1.438427f, 0.141897f, 1.438427f, 0.141897f, 0.392699f, 0.785398f, 0.476354f, 0.523646f, 0.000000f},
        {0.200000f, 0.392699f, 1.047198f, -1.476852f, 0.174083f, 1.476852f, 0.174083f, 0.392699f, 1.047198f, 0.484242f, 0.515758f, 0.000000f},
        {0.200000f, 0.392699f, 1.308997f, -1.522035f, 0.194407f, 1.522035f, 0.194407f, 0.392699f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.200000f, 0.392699f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 0.785398f, 0.000000f, -1.428899f, 0.000000f, 1.428899f, 0.000000f, 0.785398f, 0.000000f, 0.444645f, 0.555355f, 0.000000f},
        {0.200000f, 0.785398f, 0.261799f, -1.433580f, 0.051787f, 1.433580f, 0.051787f, 0.785398f, 0.261799f, 0.444645f, 0.555355f, 0.000000f},
        {0.200000f, 0.785398f, 0.523599f, -1.447392f, 0.100167f, 1.447392f, 0.100167f, 0.785398f, 0.523599f, 0.452604f, 0.547396f, 0.000000f},
        {0.200000f, 0.785398f, 0.785398f, -1.469608f, 0.141897f, 1.469608f, 0.141897f, 0.785398f, 0.785398f, 0.460539f, 0.539461f, 0.000000f},
        {0.200000f, 0.785398f, 1.047198f, -1.498939f, 0.174083f, 1.498939f, 0.174083f, 0.785398f, 1.047198f, 0.472406f, 0.527594f, 0.000000f},
        {0.200000f, 0.785398f, 1.308997f, -1.533482f, 0.194407f, 1.533482f, 0.194407f, 0.785398f, 1.308997f, 0.484242f, 0.515758f, 0.000000f},
        {0.200000f, 0.785398f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 1.178097f, 0.000000f, -1.494185f, 0.000000f, 1.494185f, 0.000000f, 1.178097f, 0.000000f, 0.424621f, 0.575379f, 0.000000f},
        {0.200000f, 1.178097f, 0.261799f, -1.496701f, 0.051787f, 1.496701f, 0.051787f, 1.178097f, 0.261799f, 0.428643f, 0.571357f, 0.000000f},
        {0.200000f, 1.178097f, 0.523599f, -1.504130f, 0.100167f, 1.504130f, 0.100167f, 1.178097f, 0.523599f, 0.436660f, 0.563340f, 0.000000f},
        {0.200000f, 1.178097f, 0.785398f, -1.516100f, 0.141897f, 1.516100f, 0.141897f, 1.178097f, 0.785398f, 0.448628f, 0.551372f, 0.000000f},
        {0.200000f, 1.178097f, 1.047198f, -1.531931f, 0.174083f, 1.531931f, 0.174083f, 1.178097f, 1.047198f, 0.464498f, 0.535502f, 0.000000f},
        {0.200000f, 1.178097f, 1.308997f, -1.550605f, 0.194407f, 1.550605f, 0.194407f, 1.178097f, 1.308997f, 0.480299f, 0.519701f, 0.000000f},
        {0.200000f, 1.178097f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.420590f, 0.579410f, 0.000000f},
        {0.200000f, 1.570796f, 0.261799f, -1.570796f, 0.051787f, 1.570796f, 0.051787f, 1.570796f, 0.261799f, 0.424621f, 0.575379f, 0.000000f},
        {0.200000f, 1.570796f, 0.523599f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 1.570796f, 0.523599f, 0.432655f, 0.567345f, 0.000000f},
        {0.200000f, 1.570796f, 0.785398f, -1.570796f, 0.141897f, 1.570796f, 0.141897f, 1.570796f, 0.785398f, 0.444645f, 0.555355f, 0.000000f},
        {0.200000f, 1.570796f, 1.047198f, -1.570796f, 0.174083f, 1.570796f, 0.174083f, 1.570796f, 1.047198f, 0.460539f, 0.539461f, 0.000000f},
        {0.200000f, 1.570796f, 1.308997f, -1.570796f, 0.194407f, 1.570796f, 0.194407f, 1.570796f, 1.308997f, 0.480299f, 0.519701f, 0.000000f},
        {0.200000f, 1.570796f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 1.963495f, 0.000000f, -1.647408f, 0.000000f, 1.647408f, 0.000000f, 1.963495f, 0.000000f, 0.424621f, 0.575379f, 0.000000f},
        {0.200000f, 1.963495f, 0.261799f, -1.644892f, 0.051787f, 1.644892f, 0.051787f, 1.963495f, 0.261799f, 0.428643f, 0.571357f, 0.000000f},
        {0.200000f, 1.963495f, 0.523599f, -1.637462f, 0.100167f, 1.637462f, 0.100167f, 1.963495f, 0.523599f, 0.436660f, 0.563340f, 0.000000f},
        {0.200000f, 1.963495f, 0.785398f, -1.625493f, 0.141897f, 1.625493f, 0.141897f, 1.963495f, 0.785398f, 0.448628f, 0.551372f, 0.000000f},
        {0.200000f, 1.963495f, 1.047198f, -1.609662f, 0.174083f, 1.609662f, 0.174083f, 1.963495f, 1.047198f, 0.464498f, 0.535502f, 0.000000f},
        {0.200000f, 1.963495f, 1.308997f, -1.590987f, 0.194407f, 1.590987f, 0.194407f, 1.963495f, 1.308997f, 0.480299f, 0.519701f, 0.000000f},
        {0.200000f, 1.963495f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 2.356194f, 0.000000f, -1.712693f, 0.000000f, 1.712693f, 0.000000f, 2.356194f, 0.000000f, 0.444645f, 0.555355f, 0.000000f},
        {0.200000f, 2.356194f, 0.261799f, -1.708012f, 0.051787f, 1.708012f, 0.051787f, 2.356194f, 0.261799f, 0.444645f, 0.555355f, 0.000000f},
        {0.200000f, 2.356194f, 0.523599f, -1.694201f, 0.100167f, 1.694201f, 0.100167f, 2.356194f, 0.523599f, 0.452604f, 0.547396f, 0.000000f},
        {0.200000f, 2.356194f, 0.785398f, -1.671984f, 0.141897f, 1.671984f, 0.141897f, 2.356194f, 0.785398f, 0.460539f, 0.539461f, 0.000000f},
        {0.200000f, 2.356194f, 1.047198f, -1.642654f, 0.174083f, 1.642654f, 0.174083f, 2.356194f, 1.047198f, 0.472406f, 0.527594f, 0.000000f},
        {0.200000f, 2.356194f, 1.308997f, -1.608110f, 0.194407f, 1.608110f, 0.194407f, 2.356194f, 1.308997f, 0.484242f, 0.515758f, 0.000000f},
        {0.200000f, 2.356194f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 2.748894f, 0.000000f, -1.756640f, 0.000000f, 1.756640f, 0.000000f, 2.748894f, 0.000000f, 0.468454f, 0.531546f, 0.000000f},
        {0.200000f, 2.748894f, 0.261799f, -1.750481f, 0.051787f, 1.750481f, 0.051787f, 2.748894f, 0.261799f, 0.468454f, 0.531546f, 0.000000f},
        {0.200000f, 2.748894f, 0.523599f, -1.732325f, 0.100167f, 1.732325f, 0.100167f, 2.748894f, 0.523599f, 0.472406f, 0.527594f, 0.000000f},
        {0.200000f, 2.748894f, 0.785398f, -1.703165f, 0.141897f, 1.703165f, 0.141897f, 2.748894f, 0.785398f, 0.476354f, 0.523646f, 0.000000f},
        {0.200000f, 2.748894f, 1.047198f, -1.664740f, 0.174083f, 1.664740f, 0.174083f, 2.748894f, 1.047198f, 0.484242f, 0.515758f, 0.000000f},
        {0.200000f, 2.748894f, 1.308997f, -1.619557f, 0.194407f, 1.619557f, 0.194407f, 2.748894f, 1.308997f, 0.492123f, 0.507877f, 0.000000f},
        {0.200000f, 2.748894f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 0.000000f, -1.772154f, 0.000000f, 1.772154f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 0.261799f, -1.765468f, 0.051787f, 1.765468f, 0.051787f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 0.523599f, -1.745765f, 0.100167f, 1.745765f, 0.100167f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 0.785398f, -1.714144f, 0.141897f, 1.714144f, 0.141897f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 1.047198f, -1.672506f, 0.174083f, 1.672506f, 0.174083f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 1.308997f, -1.623578f, 0.194407f, 1.623578f, 0.194407f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.141593f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.534292f, 0.000000f, -1.756640f, 0.000000f, 1.756640f, 0.000000f, 3.534292f, 0.000000f, 0.531546f, 0.468454f, 0.000000f},
        {0.200000f, 3.534292f, 0.261799f, -1.750481f, 0.051787f, 1.750481f, 0.051787f, 3.534292f, 0.261799f, 0.531546f, 0.468454f, 0.000000f},
        {0.200000f, 3.534292f, 0.523599f, -1.732325f, 0.100167f, 1.732325f, 0.100167f, 3.534292f, 0.523599f, 0.527594f, 0.472406f, 0.000000f},
        {0.200000f, 3.534292f, 0.785398f, -1.703165f, 0.141897f, 1.703165f, 0.141897f, 3.534292f, 0.785398f, 0.523646f, 0.476354f, 0.000000f},
        {0.200000f, 3.534292f, 1.047198f, -1.664740f, 0.174083f, 1.664740f, 0.174083f, 3.534292f, 1.047198f, 0.515758f, 0.484242f, 0.000000f},
        {0.200000f, 3.534292f, 1.308997f, -1.619557f, 0.194407f, 1.619557f, 0.194407f, 3.534292f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.200000f, 3.534292f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 3.926991f, 0.000000f, -1.712693f, 0.000000f, 1.712693f, 0.000000f, 3.926991f, 0.000000f, 0.555355f, 0.444645f, 0.000000f},
        {0.200000f, 3.926991f, 0.261799f, -1.708012f, 0.051787f, 1.708012f, 0.051787f, 3.926991f, 0.261799f, 0.555355f, 0.444645f, 0.000000f},
        {0.200000f, 3.926991f, 0.523599f, -1.694201f, 0.100167f, 1.694201f, 0.100167f, 3.926991f, 0.523599f, 0.547396f, 0.452604f, 0.000000f},
        {0.200000f, 3.926991f, 0.785398f, -1.671984f, 0.141897f, 1.671984f, 0.141897f, 3.926991f, 0.785398f, 0.539461f, 0.460539f, 0.000000f},
        {0.200000f, 3.926991f, 1.047198f, -1.642654f, 0.174083f, 1.642654f, 0.174083f, 3.926991f, 1.047198f, 0.527594f, 0.472406f, 0.000000f},
        {0.200000f, 3.926991f, 1.308997f, -1.608110f, 0.194407f, 1.608110f, 0.194407f, 3.926991f, 1.308997f, 0.515758f, 0.484242f, 0.000000f},
        {0.200000f, 3.926991f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 4.319690f, 0.000000f, -1.647408f, 0.000000f, 1.647408f, 0.000000f, 4.319690f, 0.000000f, 0.575379f, 0.424621f, 0.000000f},
        {0.200000f, 4.319690f, 0.261799f, -1.644892f, 0.051787f, 1.644892f, 0.051787f, 4.319690f, 0.261799f, 0.571357f, 0.428643f, 0.000000f},
        {0.200000f, 4.319690f, 0.523599f, -1.637462f, 0.100167f, 1.637462f, 0.100167f, 4.319690f, 0.523599f, 0.563340f, 0.436660f, 0.000000f},
        {0.200000f, 4.319690f, 0.785398f, -1.625493f, 0.141897f, 1.625493f, 0.141897f, 4.319690f, 0.785398f, 0.551372f, 0.448628f, 0.000000f},
        {0.200000f, 4.319690f, 1.047198f, -1.609662f, 0.174083f, 1.609662f, 0.174083f, 4.319690f, 1.047198f, 0.535502f, 0.464498f, 0.000000f},
        {0.200000f, 4.319690f, 1.308997f, -1.590987f, 0.194407f, 1.590987f, 0.194407f, 4.319690f, 1.308997f, 0.519701f, 0.480299f, 0.000000f},
        {0.200000f, 4.319690f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.579410f, 0.420590f, 0.000000f},
        {0.200000f, 4.712389f, 0.261799f, -1.570796f, 0.051787f, 1.570796f, 0.051787f, 4.712389f, 0.261799f, 0.575379f, 0.424621f, 0.000000f},
        {0.200000f, 4.712389f, 0.523599f, -1.570796f, 0.100167f, 1.570796f, 0.100167f, 4.712389f, 0.523599f, 0.567345f, 0.432655f, 0.000000f},
        {0.200000f, 4.712389f, 0.785398f, -1.570796f, 0.141897f, 1.570796f, 0.141897f, 4.712389f, 0.785398f, 0.555355f, 0.444645f, 0.000000f},
        {0.200000f, 4.712389f, 1.047198f, -1.570796f, 0.174083f, 1.570796f, 0.174083f, 4.712389f, 1.047198f, 0.539461f, 0.460539f, 0.000000f},
        {0.200000f, 4.712389f, 1.308997f, -1.570796f, 0.194407f, 1.570796f, 0.194407f, 4.712389f, 1.308997f, 0.519701f, 0.480299f, 0.000000f},
        {0.200000f, 4.712389f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 5.105088f, 0.000000f, -1.494185f, 0.000000f, 1.494185f, 0.000000f, 5.105088f, 0.000000f, 0.575379f, 0.424621f, 0.000000f},
        {0.200000f, 5.105088f, 0.261799f, -1.496701f, 0.051787f, 1.496701f, 0.051787f, 5.105088f, 0.261799f, 0.571357f, 0.428643f, 0.000000f},
        {0.200000f, 5.105088f, 0.523599f, -1.504130f, 0.100167f, 1.504130f, 0.100167f, 5.105088f, 0.523599f, 0.563340f, 0.436660f, 0.000000f},
        {0.200000f, 5.105088f, 0.785398f, -1.516100f, 0.141897f, 1.516100f, 0.141897f, 5.105088f, 0.785398f, 0.551372f, 0.448628f, 0.000000f},
        {0.200000f, 5.105088f, 1.047198f, -1.531931f, 0.174083f, 1.531931f, 0.174083f, 5.105088f, 1.047198f, 0.535502f, 0.464498f, 0.000000f},
        {0.200000f, 5.105088f, 1.308997f, -1.550605f, 0.194407f, 1.550605f, 0.194407f, 5.105088f, 1.308997f, 0.519701f, 0.480299f, 0.000000f},
        {0.200000f, 5.105088f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 5.497787f, 0.000000f, -1.428899f, 0.000000f, 1.428899f, 0.000000f, 5.497787f, 0.000000f, 0.555355f, 0.444645f, 0.000000f},
        {0.200000f, 5.497787f, 0.261799f, -1.433580f, 0.051787f, 1.433580f, 0.051787f, 5.497787f, 0.261799f, 0.555355f, 0.444645f, 0.000000f},
        {0.200000f, 5.497787f, 0.523599f, -1.447392f, 0.100167f, 1.447392f, 0.100167f, 5.497787f, 0.523599f, 0.547396f, 0.452604f, 0.000000f},
        {0.200000f, 5.497787f, 0.785398f, -1.469608f, 0.141897f, 1.469608f, 0.141897f, 5.497787f, 0.785398f, 0.539461f, 0.460539f, 0.000000f},
        {0.200000f, 5.497787f, 1.047198f, -1.498939f, 0.174083f, 1.498939f, 0.174083f, 5.497787f, 1.047198f, 0.527594f, 0.472406f, 0.000000f},
        {0.200000f, 5.497787f, 1.308997f, -1.533482f, 0.194407f, 1.533482f, 0.194407f, 5.497787f, 1.308997f, 0.515758f, 0.484242f, 0.000000f},
        {0.200000f, 5.497787f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 5.890486f, 0.000000f, -1.384952f, 0.000000f, 1.384952f, 0.000000f, 5.890486f, 0.000000f, 0.531546f, 0.468454f, 0.000000f},
        {0.200000f, 5.890486f, 0.261799f, -1.391112f, 0.051787f, 1.391112f, 0.051787f, 5.890486f, 0.261799f, 0.531546f, 0.468454f, 0.000000f},
        {0.200000f, 5.890486f, 0.523599f, -1.409268f, 0.100167f, 1.409268f, 0.100167f, 5.890486f, 0.523599f, 0.527594f, 0.472406f, 0.000000f},
        {0.200000f, 5.890486f, 0.785398f, -1.438427f, 0.141897f, 1.438427f, 0.141897f, 5.890486f, 0.785398f, 0.523646f, 0.476354f, 0.000000f},
        {0.200000f, 5.890486f, 1.047198f, -1.476852f, 0.174083f, 1.476852f, 0.174083f, 5.890486f, 1.047198f, 0.515758f, 0.484242f, 0.000000f},
        {0.200000f, 5.890486f, 1.308997f, -1.522035f, 0.194407f, 1.522035f, 0.194407f, 5.890486f, 1.308997f, 0.507877f, 0.492123f, 0.000000f},
        {0.200000f, 5.890486f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 0.000000f, -1.369438f, 0.000000f, 1.369438f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 0.261799f, -1.376125f, 0.051787f, 1.376125f, 0.051787f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 0.523599f, -1.395827f, 0.100167f, 1.395827f, 0.100167f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 0.785398f, -1.427449f, 0.141897f, 1.427449f, 0.141897f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 1.047198f, -1.469086f, 0.174083f, 1.469086f, 0.174083f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 1.308997f, -1.518014f, 0.194407f, 1.518014f, 0.194407f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.200000f, 6.283185f, 1.570796f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 0.000000f, -1.266104f, 0.000000f, 1.266104f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 0.261799f, -1.275885f, 0.077724f, 1.275885f, 0.077724f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 0.523599f, -1.304893f, 0.150568f, 1.304893f, 0.150568f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 0.785398f, -1.351982f, 0.213756f, 1.351982f, 0.213756f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 1.047198f, -1.414831f, 0.262823f, 1.414831f, 0.262823f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 1.308997f, -1.489581f, 0.293995f, 1.489581f, 0.293995f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.000000f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.392699f, 0.000000f, -1.289955f, 0.000000f, 1.289955f, 0.000000f, 0.392699f, 0.000000f, 0.452604f, 0.547396f, 0.000000f},
        {0.300000f, 0.392699f, 0.261799f, -1.298929f, 0.077724f, 1.298929f, 0.077724f, 0.392699f, 0.261799f, 0.452604f, 0.547396f, 0.000000f},
        {0.300000f, 0.392699f, 0.523599f, -1.325568f, 0.150568f, 1.325568f, 0.150568f, 0.392699f, 0.523599f, 0.460539f, 0.539461f, 0.000000f},
        {0.300000f, 0.392699f, 0.785398f, -1.368878f, 0.213756f, 1.368878f, 0.213756f, 0.392699f, 0.785398f, 0.464498f, 0.535502f, 0.000000f},
        {0.300000f, 0.392699f, 1.047198f, -1.426789f, 0.262823f, 1.426789f, 0.262823f, 0.392699f, 1.047198f, 0.476354f, 0.523646f, 0.000000f},
        {0.300000f, 0.392699f, 1.308997f, -1.495775f, 0.293995f, 1.495775f, 0.293995f, 0.392699f, 1.308997f, 0.488184f, 0.511816f, 0.000000f},
        {0.300000f, 0.392699f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 0.785398f, 0.000000f, -1.357040f, 0.000000f, 1.357040f, 0.000000f, 0.785398f, 0.000000f, 0.412498f, 0.587502f, 0.000000f},
        {0.300000f, 0.785398f, 0.261799f, -1.363797f, 0.077724f, 1.363797f, 0.077724f, 0.785398f, 0.261799f, 0.416549f, 0.583451f, 0.000000f},
        {0.300000f, 0.785398f, 0.523599f, -1.383896f, 0.150568f, 1.383896f, 0.150568f, 0.785398f, 0.523599f, 0.424621f, 0.575379f, 0.000000f},
        {0.300000f, 0.785398f, 0.785398f, -1.416694f, 0.213756f, 1.416694f, 0.213756f, 0.785398f, 0.785398f, 0.436660f, 0.563340f, 0.000000f},
        {0.300000f, 0.785398f, 1.047198f, -1.460736f, 0.262823f, 1.460736f, 0.262823f, 0.785398f, 1.047198f, 0.456574f, 0.543426f, 0.000000f},
        {0.300000f, 0.785398f, 1.308997f, -1.513400f, 0.293995f, 1.513400f, 0.293995f, 0.785398f, 1.308997f, 0.476354f, 0.523646f, 0.000000f},
        {0.300000f, 0.785398f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 1.178097f, 0.000000f, -1.455738f, 0.000000f, 1.455738f, 0.000000f, 1.178097f, 0.000000f, 0.387939f, 0.612061f, 0.000000f},
        {0.300000f, 1.178097f, 0.261799f, -1.459337f, 0.077724f, 1.459337f, 0.077724f, 1.178097f, 0.261799f, 0.392064f, 0.607936f, 0.000000f},
        {0.300000f, 1.178097f, 0.523599f, -1.470064f, 0.150568f, 1.470064f, 0.150568f, 1.178097f, 0.523599f, 0.400275f, 0.599725f, 0.000000f},
        {0.300000f, 1.178097f, 0.785398f, -1.487630f, 0.213756f, 1.487630f, 0.213756f, 1.178097f, 0.785398f, 0.420590f, 0.579410f, 0.000000f},
        {0.300000f, 1.178097f, 1.047198f, -1.511317f, 0.262823f, 1.511317f, 0.262823f, 1.178097f, 1.047198f, 0.444645f, 0.555355f, 0.000000f},
        {0.300000f, 1.178097f, 1.308997f, -1.539746f, 0.293995f, 1.539746f, 0.293995f, 1.178097f, 1.308997f, 0.468454f, 0.531546f, 0.000000f},
        {0.300000f, 1.178097f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.379643f, 0.620357f, 0.000000f},
        {0.300000f, 1.570796f, 0.261799f, -1.570796f, 0.077724f, 1.570796f, 0.077724f, 1.570796f, 0.261799f, 0.383799f, 0.616201f, 0.000000f},
        {0.300000f, 1.570796f, 0.523599f, -1.570796f, 0.150568f, 1.570796f, 0.150568f, 1.570796f, 0.523599f, 0.396176f, 0.603824f, 0.000000f},
        {0.300000f, 1.570796f, 0.785398f, -1.570796f, 0.213756f, 1.570796f, 0.213756f, 1.570796f, 0.785398f, 0.412498f, 0.587502f, 0.000000f},
        {0.300000f, 1.570796f, 1.047198f, -1.570796f, 0.262823f, 1.570796f, 0.262823f, 1.570796f, 1.047198f, 0.436660f, 0.563340f, 0.000000f},
        {0.300000f, 1.570796f, 1.308997f, -1.570796f, 0.293995f, 1.570796f, 0.293995f, 1.570796f, 1.308997f, 0.468454f, 0.531546f, 0.000000f},
        {0.300000f, 1.570796f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 1.963495f, 0.000000f, -1.685855f, 0.000000f, 1.685855f, 0.000000f, 1.963495f, 0.000000f, 0.387939f, 0.612061f, 0.000000f},
        {0.300000f, 1.963495f, 0.261799f, -1.682256f, 0.077724f, 1.682256f, 0.077724f, 1.963495f, 0.261799f, 0.392064f, 0.607936f, 0.000000f},
        {0.300000f, 1.963495f, 0.523599f, -1.671528f, 0.150568f, 1.671528f, 0.150568f, 1.963495f, 0.523599f, 0.400275f, 0.599725f, 0.000000f},
        {0.300000f, 1.963495f, 0.785398f, -1.653962f, 0.213756f, 1.653962f, 0.213756f, 1.963495f, 0.785398f, 0.420590f, 0.579410f, 0.000000f},
        {0.300000f, 1.963495f, 1.047198f, -1.630275f, 0.262823f, 1.630275f, 0.262823f, 1.963495f, 1.047198f, 0.444645f, 0.555355f, 0.000000f},
        {0.300000f, 1.963495f, 1.308997f, -1.601847f, 0.293995f, 1.601847f, 0.293995f, 1.963495f, 1.308997f, 0.468454f, 0.531546f, 0.000000f},
        {0.300000f, 1.963495f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 2.356194f, 0.000000f, -1.784552f, 0.000000f, 1.784552f, 0.000000f, 2.356194f, 0.000000f, 0.412498f, 0.587502f, 0.000000f},
        {0.300000f, 2.356194f, 0.261799f, -1.777796f, 0.077724f, 1.777796f, 0.077724f, 2.356194f, 0.261799f, 0.416549f, 0.583451f, 0.000000f},
        {0.300000f, 2.356194f, 0.523599f, -1.757697f, 0.150568f, 1.757697f, 0.150568f, 2.356194f, 0.523599f, 0.424621f, 0.575379f, 0.000000f},
        {0.300000f, 2.356194f, 0.785398f, -1.724899f, 0.213756f, 1.724899f, 0.213756f, 2.356194f, 0.785398f, 0.436660f, 0.563340f, 0.000000f},
        {0.300000f, 2.356194f, 1.047198f, -1.680856f, 0.262823f, 1.680856f, 0.262823f, 2.356194f, 1.047198f, 0.456574f, 0.543426f, 0.000000f},
        {0.300000f, 2.356194f, 1.308997f, -1.628193f, 0.293995f, 1.628193f, 0.293995f, 2.356194f, 1.308997f, 0.476354f, 0.523646f, 0.000000f},
        {0.300000f, 2.356194f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 2.748894f, 0.000000f, -1.851637f, 0.000000f, 1.851637f, 0.000000f, 2.748894f, 0.000000f, 0.452604f, 0.547396f, 0.000000f},
        {0.300000f, 2.748894f, 0.261799f, -1.842663f, 0.077724f, 1.842663f, 0.077724f, 2.748894f, 0.261799f, 0.452604f, 0.547396f, 0.000000f},
        {0.300000f, 2.748894f, 0.523599f, -1.816025f, 0.150568f, 1.816025f, 0.150568f, 2.748894f, 0.523599f, 0.460539f, 0.539461f, 0.000000f},
        {0.300000f, 2.748894f, 0.785398f, -1.772714f, 0.213756f, 1.772714f, 0.213756f, 2.748894f, 0.785398f, 0.464498f, 0.535502f, 0.000000f},
        {0.300000f, 2.748894f, 1.047198f, -1.714804f, 0.262823f, 1.714804f, 0.262823f, 2.748894f, 1.047198f, 0.476354f, 0.523646f, 0.000000f},
        {0.300000f, 2.748894f, 1.308997f, -1.645818f, 0.293995f, 1.645818f, 0.293995f, 2.748894f, 1.308997f, 0.488184f, 0.511816f, 0.000000f},
        {0.300000f, 2.748894f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 0.000000f, -1.875489f, 0.000000f, 1.875489f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 0.261799f, -1.865708f, 0.077724f, 1.865708f, 0.077724f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 0.523599f, -1.836699f, 0.150568f, 1.836699f, 0.150568f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 0.785398f, -1.789611f, 0.213756f, 1.789611f, 0.213756f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 1.047198f, -1.726762f, 0.262823f, 1.726762f, 0.262823f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 1.308997f, -1.652012f, 0.293995f, 1.652012f, 0.293995f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.141593f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.534292f, 0.000000f, -1.851637f, 0.000000f, 1.851637f, 0.000000f, 3.534292f, 0.000000f, 0.547396f, 0.452604f, 0.000000f},
        {0.300000f, 3.534292f, 0.261799f, -1.842663f, 0.077724f, 1.842663f, 0.077724f, 3.534292f, 0.261799f, 0.547396f, 0.452604f, 0.000000f},
        {0.300000f, 3.534292f, 0.523599f, -1.816025f, 0.150568f, 1.816025f, 0.150568f, 3.534292f, 0.523599f, 0.539461f, 0.460539f, 0.000000f},
        {0.300000f, 3.534292f, 0.785398f, -1.772714f, 0.213756f, 1.772714f, 0.213756f, 3.534292f, 0.785398f, 0.535502f, 0.464498f, 0.000000f},
        {0.300000f, 3.534292f, 1.047198f, -1.714804f, 0.262823f, 1.714804f, 0.262823f, 3.534292f, 1.047198f, 0.523646f, 0.476354f, 0.000000f},
        {0.300000f, 3.534292f, 1.308997f, -1.645818f, 0.293995f, 1.645818f, 0.293995f, 3.534292f, 1.308997f, 0.511816f, 0.488184f, 0.000000f},
        {0.300000f, 3.534292f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 3.926991f, 0.000000f, -1.784552f, 0.000000f, 1.784552f, 0.000000f, 3.926991f, 0.000000f, 0.587502f, 0.412498f, 0.000000f},
        {0.300000f, 3.926991f, 0.261799f, -1.777796f, 0.077724f, 1.777796f, 0.077724f, 3.926991f, 0.261799f, 0.583451f, 0.416549f, 0.000000f},
        {0.300000f, 3.926991f, 0.523599f, -1.757697f, 0.150568f, 1.757697f, 0.150568f, 3.926991f, 0.523599f, 0.575379f, 0.424621f, 0.000000f},
        {0.300000f, 3.926991f, 0.785398f, -1.724899f, 0.213756f, 1.724899f, 0.213756f, 3.926991f, 0.785398f, 0.563340f, 0.436660f, 0.000000f},
        {0.300000f, 3.926991f, 1.047198f, -1.680856f, 0.262823f, 1.680856f, 0.262823f, 3.926991f, 1.047198f, 0.543426f, 0.456574f, 0.000000f},
        {0.300000f, 3.926991f, 1.308997f, -1.628193f, 0.293995f, 1.628193f, 0.293995f, 3.926991f, 1.308997f, 0.523646f, 0.476354f, 0.000000f},
        {0.300000f, 3.926991f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 4.319690f, 0.000000f, -1.685855f, 0.000000f, 1.685855f, 0.000000f, 4.319690f, 0.000000f, 0.612061f, 0.387939f, 0.000000f},
        {0.300000f, 4.319690f, 0.261799f, -1.682256f, 0.077724f, 1.682256f, 0.077724f, 4.319690f, 0.261799f, 0.607936f, 0.392064f, 0.000000f},
        {0.300000f, 4.319690f, 0.523599f, -1.671528f, 0.150568f, 1.671528f, 0.150568f, 4.319690f, 0.523599f, 0.599725f, 0.400275f, 0.000000f},
        {0.300000f, 4.319690f, 0.785398f, -1.653962f, 0.213756f, 1.653962f, 0.213756f, 4.319690f, 0.785398f, 0.579410f, 0.420590f, 0.000000f},
        {0.300000f, 4.319690f, 1.047198f, -1.630275f, 0.262823f, 1.630275f, 0.262823f, 4.319690f, 1.047198f, 0.555355f, 0.444645f, 0.000000f},
        {0.300000f, 4.319690f, 1.308997f, -1.601847f, 0.293995f, 1.601847f, 0.293995f, 4.319690f, 1.308997f, 0.531546f, 0.468454f, 0.000000f},
        {0.300000f, 4.319690f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.620357f, 0.379643f, 0.000000f},
        {0.300000f, 4.712389f, 0.261799f, -1.570796f, 0.077724f, 1.570796f, 0.077724f, 4.712389f, 0.261799f, 0.616201f, 0.383799f, 0.000000f},
        {0.300000f, 4.712389f, 0.523599f, -1.570796f, 0.150568f, 1.570796f, 0.150568f, 4.712389f, 0.523599f, 0.603824f, 0.396176f, 0.000000f},
        {0.300000f, 4.712389f, 0.785398f, -1.570796f, 0.213756f, 1.570796f, 0.213756f, 4.712389f, 0.785398f, 0.587502f, 0.412498f, 0.000000f},
        {0.300000f, 4.712389f, 1.047198f, -1.570796f, 0.262823f, 1.570796f, 0.262823f, 4.712389f, 1.047198f, 0.563340f, 0.436660f, 0.000000f},
        {0.300000f, 4.712389f, 1.308997f, -1.570796f, 0.293995f, 1.570796f, 0.293995f, 4.712389f, 1.308997f, 0.531546f, 0.468454f, 0.000000f},
        {0.300000f, 4.712389f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 5.105088f, 0.000000f, -1.455738f, 0.000000f, 1.455738f, 0.000000f, 5.105088f, 0.000000f, 0.612061f, 0.387939f, 0.000000f},
        {0.300000f, 5.105088f, 0.261799f, -1.459337f, 0.077724f, 1.459337f, 0.077724f, 5.105088f, 0.261799f, 0.607936f, 0.392064f, 0.000000f},
        {0.300000f, 5.105088f, 0.523599f, -1.470064f, 0.150568f, 1.470064f, 0.150568f, 5.105088f, 0.523599f, 0.599725f, 0.400275f, 0.000000f},
        {0.300000f, 5.105088f, 0.785398f, -1.487630f, 0.213756f, 1.487630f, 0.213756f, 5.105088f, 0.785398f, 0.579410f, 0.420590f, 0.000000f},
        {0.300000f, 5.105088f, 1.047198f, -1.511317f, 0.262823f, 1.511317f, 0.262823f, 5.105088f, 1.047198f, 0.555355f, 0.444645f, 0.000000f},
        {0.300000f, 5.105088f, 1.308997f, -1.539746f, 0.293995f, 1.539746f, 0.293995f, 5.105088f, 1.308997f, 0.531546f, 0.468454f, 0.000000f},
        {0.300000f, 5.105088f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 5.497787f, 0.000000f, -1.357040f, 0.000000f, 1.357040f, 0.000000f, 5.497787f, 0.000000f, 0.587502f, 0.412498f, 0.000000f},
        {0.300000f, 5.497787f, 0.261799f, -1.363797f, 0.077724f, 1.363797f, 0.077724f, 5.497787f, 0.261799f, 0.583451f, 0.416549f, 0.000000f},
        {0.300000f, 5.497787f, 0.523599f, -1.383896f, 0.150568f, 1.383896f, 0.150568f, 5.497787f, 0.523599f, 0.575379f, 0.424621f, 0.000000f},
        {0.300000f, 5.497787f, 0.785398f, -1.416694f, 0.213756f, 1.416694f, 0.213756f, 5.497787f, 0.785398f, 0.563340f, 0.436660f, 0.000000f},
        {0.300000f, 5.497787f, 1.047198f, -1.460736f, 0.262823f, 1.460736f, 0.262823f, 5.497787f, 1.047198f, 0.543426f, 0.456574f, 0.000000f},
        {0.300000f, 5.497787f, 1.308997f, -1.513400f, 0.293995f, 1.513400f, 0.293995f, 5.497787f, 1.308997f, 0.523646f, 0.476354f, 0.000000f},
        {0.300000f, 5.497787f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 5.890486f, 0.000000f, -1.289955f, 0.000000f, 1.289955f, 0.000000f, 5.890486f, 0.000000f, 0.547396f, 0.452604f, 0.000000f},
        {0.300000f, 5.890486f, 0.261799f, -1.298929f, 0.077724f, 1.298929f, 0.077724f, 5.890486f, 0.261799f, 0.547396f, 0.452604f, 0.000000f},
        {0.300000f, 5.890486f, 0.523599f, -1.325568f, 0.150568f, 1.325568f, 0.150568f, 5.890486f, 0.523599f, 0.539461f, 0.460539f, 0.000000f},
        {0.300000f, 5.890486f, 0.785398f, -1.368878f, 0.213756f, 1.368878f, 0.213756f, 5.890486f, 0.785398f, 0.535502f, 0.464498f, 0.000000f},
        {0.300000f, 5.890486f, 1.047198f, -1.426789f, 0.262823f, 1.426789f, 0.262823f, 5.890486f, 1.047198f, 0.523646f, 0.476354f, 0.000000f},
        {0.300000f, 5.890486f, 1.308997f, -1.495775f, 0.293995f, 1.495775f, 0.293995f, 5.890486f, 1.308997f, 0.511816f, 0.488184f, 0.000000f},
        {0.300000f, 5.890486f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 0.000000f, -1.266104f, 0.000000f, 1.266104f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 0.261799f, -1.275885f, 0.077724f, 1.275885f, 0.077724f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 0.523599f, -1.304893f, 0.150568f, 1.304893f, 0.150568f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 0.785398f, -1.351982f, 0.213756f, 1.351982f, 0.213756f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 1.047198f, -1.414831f, 0.262823f, 1.414831f, 0.262823f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 1.308997f, -1.489581f, 0.293995f, 1.489581f, 0.293995f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.300000f, 6.283185f, 1.570796f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 0.000000f, -1.159279f, 0.000000f, 1.159279f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 0.261799f, -1.171839f, 0.103713f, 1.171839f, 0.103713f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 0.523599f, -1.209429f, 0.201358f, 1.209429f, 0.201358f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 0.785398f, -1.271462f, 0.286757f, 1.271462f, 0.286757f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 1.047198f, -1.355946f, 0.353742f, 1.355946f, 0.353742f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 1.308997f, -1.458315f, 0.396693f, 1.458315f, 0.396693f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.000000f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.392699f, 0.000000f, -1.192270f, 0.000000f, 1.192270f, 0.000000f, 0.392699f, 0.000000f, 0.432655f, 0.567345f, 0.000000f},
        {0.400000f, 0.392699f, 0.261799f, -1.203720f, 0.103713f, 1.203720f, 0.103713f, 0.392699f, 0.261799f, 0.436660f, 0.563340f, 0.000000f},
        {0.400000f, 0.392699f, 0.523599f, -1.238049f, 0.201358f, 1.238049f, 0.201358f, 0.392699f, 0.523599f, 0.444645f, 0.555355f, 0.000000f},
        {0.400000f, 0.392699f, 0.785398f, -1.294871f, 0.286757f, 1.294871f, 0.286757f, 0.392699f, 0.785398f, 0.452604f, 0.547396f, 0.000000f},
        {0.400000f, 0.392699f, 1.047198f, -1.372528f, 0.353742f, 1.372528f, 0.353742f, 0.392699f, 1.047198f, 0.468454f, 0.531546f, 0.000000f},
        {0.400000f, 0.392699f, 1.308997f, -1.466910f, 0.396693f, 1.466910f, 0.396693f, 0.392699f, 1.308997f, 0.484242f, 0.515758f, 0.000000f},
        {0.400000f, 0.392699f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 0.785398f, 0.000000f, -1.284040f, 0.000000f, 1.284040f, 0.000000f, 0.785398f, 0.000000f, 0.379643f, 0.620357f, 0.000000f},
        {0.400000f, 0.785398f, 0.261799f, -1.292538f, 0.103713f, 1.292538f, 0.103713f, 0.785398f, 0.261799f, 0.383799f, 0.616201f, 0.000000f},
        {0.400000f, 0.785398f, 0.523599f, -1.318116f, 0.201358f, 1.318116f, 0.201358f, 0.785398f, 0.523599f, 0.396176f, 0.603824f, 0.000000f},
        {0.400000f, 0.785398f, 0.785398f, -1.360741f, 0.286757f, 1.360741f, 0.286757f, 0.785398f, 0.785398f, 0.416549f, 0.583451f, 0.000000f},
        {0.400000f, 0.785398f, 1.047198f, -1.419464f, 0.353742f, 1.419464f, 0.353742f, 0.785398f, 1.047198f, 0.440656f, 0.559344f, 0.000000f},
        {0.400000f, 0.785398f, 1.308997f, -1.491344f, 0.396693f, 1.491344f, 0.396693f, 0.785398f, 1.308997f, 0.468454f, 0.531546f, 0.000000f},
        {0.400000f, 0.785398f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 1.178097f, 0.000000f, -1.417119f, 0.000000f, 1.417119f, 0.000000f, 1.178097f, 0.000000f, 0.345792f, 0.654208f, 0.000000f},
        {0.400000f, 1.178097f, 0.261799f, -1.421587f, 0.103713f, 1.421587f, 0.103713f, 1.178097f, 0.261799f, 0.354365f, 0.645635f, 0.000000f},
        {0.400000f, 1.178097f, 0.523599f, -1.435081f, 0.201358f, 1.435081f, 0.201358f, 1.178097f, 0.523599f, 0.367083f, 0.632917f, 0.000000f},
        {0.400000f, 1.178097f, 0.785398f, -1.457708f, 0.286757f, 1.457708f, 0.286757f, 1.178097f, 0.785398f, 0.387939f, 0.612061f, 0.000000f},
        {0.400000f, 1.178097f, 1.047198f, -1.489117f, 0.353742f, 1.489117f, 0.353742f, 1.178097f, 1.047198f, 0.420590f, 0.579410f, 0.000000f},
        {0.400000f, 1.178097f, 1.308997f, -1.527829f, 0.396693f, 1.527829f, 0.396693f, 1.178097f, 1.308997f, 0.460539f, 0.539461f, 0.000000f},
        {0.400000f, 1.178097f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.337136f, 0.662864f, 0.000000f},
        {0.400000f, 1.570796f, 0.261799f, -1.570796f, 0.103713f, 1.570796f, 0.103713f, 1.570796f, 0.261799f, 0.341474f, 0.658526f, 0.000000f},
        {0.400000f, 1.570796f, 0.523599f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 1.570796f, 0.523599f, 0.354365f, 0.645635f, 0.000000f},
        {0.400000f, 1.570796f, 0.785398f, -1.570796f, 0.286757f, 1.570796f, 0.286757f, 1.570796f, 0.785398f, 0.379643f, 0.620357f, 0.000000f},
        {0.400000f, 1.570796f, 1.047198f, -1.570796f, 0.353742f, 1.570796f, 0.353742f, 1.570796f, 1.047198f, 0.416549f, 0.583451f, 0.000000f},
        {0.400000f, 1.570796f, 1.308997f, -1.570796f, 0.396693f, 1.570796f, 0.396693f, 1.570796f, 1.308997f, 0.456574f, 0.543426f, 0.000000f},
        {0.400000f, 1.570796f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 1.963495f, 0.000000f, -1.724474f, 0.000000f, 1.724474f, 0.000000f, 1.963495f, 0.000000f, 0.345792f, 0.654208f, 0.000000f},
        {0.400000f, 1.963495f, 0.261799f, -1.720006f, 0.103713f, 1.720006f, 0.103713f, 1.963495f, 0.261799f, 0.354365f, 0.645635f, 0.000000f},
        {0.400000f, 1.963495f, 0.523599f, -1.706512f, 0.201358f, 1.706512f, 0.201358f, 1.963495f, 0.523599f, 0.367083f, 0.632917f, 0.000000f},
        {0.400000f, 1.963495f, 0.785398f, -1.683884f, 0.286757f, 1.683884f, 0.286757f, 1.963495f, 0.785398f, 0.387939f, 0.612061f, 0.000000f},
        {0.400000f, 1.963495f, 1.047198f, -1.652475f, 0.353742f, 1.652475f, 0.353742f, 1.963495f, 1.047198f, 0.420590f, 0.579410f, 0.000000f},
        {0.400000f, 1.963495f, 1.308997f, -1.613763f, 0.396693f, 1.613763f, 0.396693f, 1.963495f, 1.308997f, 0.460539f, 0.539461f, 0.000000f},
        {0.400000f, 1.963495f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 2.356194f, 0.000000f, -1.857553f, 0.000000f, 1.857553f, 0.000000f, 2.356194f, 0.000000f, 0.379643f, 0.620357f, 0.000000f},
        {0.400000f, 2.356194f, 0.261799f, -1.849054f, 0.103713f, 1.849054f, 0.103713f, 2.356194f, 0.261799f, 0.383799f, 0.616201f, 0.000000f},
        {0.400000f, 2.356194f, 0.523599f, -1.823477f, 0.201358f, 1.823477f, 0.201358f, 2.356194f, 0.523599f, 0.396176f, 0.603824f, 0.000000f},
        {0.400000f, 2.356194f, 0.785398f, -1.780852f, 0.286757f, 1.780852f, 0.286757f, 2.356194f, 0.785398f, 0.416549f, 0.583451f, 0.000000f},
        {0.400000f, 2.356194f, 1.047198f, -1.722129f, 0.353742f, 1.722129f, 0.353742f, 2.356194f, 1.047198f, 0.440656f, 0.559344f, 0.000000f},
        {0.400000f, 2.356194f, 1.308997f, -1.650248f, 0.396693f, 1.650248f, 0.396693f, 2.356194f, 1.308997f, 0.468454f, 0.531546f, 0.000000f},
        {0.400000f, 2.356194f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 2.748894f, 0.000000f, -1.949323f, 0.000000f, 1.949323f, 0.000000f, 2.748894f, 0.000000f, 0.432655f, 0.567345f, 0.000000f},
        {0.400000f, 2.748894f, 0.261799f, -1.937873f, 0.103713f, 1.937873f, 0.103713f, 2.748894f, 0.261799f, 0.436660f, 0.563340f, 0.000000f},
        {0.400000f, 2.748894f, 0.523599f, -1.903543f, 0.201358f, 1.903543f, 0.201358f, 2.748894f, 0.523599f, 0.444645f, 0.555355f, 0.000000f},
        {0.400000f, 2.748894f, 0.785398f, -1.846721f, 0.286757f, 1.846721f, 0.286757f, 2.748894f, 0.785398f, 0.452604f, 0.547396f, 0.000000f},
        {0.400000f, 2.748894f, 1.047198f, -1.769065f, 0.353742f, 1.769065f, 0.353742f, 2.748894f, 1.047198f, 0.468454f, 0.531546f, 0.000000f},
        {0.400000f, 2.748894f, 1.308997f, -1.674683f, 0.396693f, 1.674683f, 0.396693f, 2.748894f, 1.308997f, 0.484242f, 0.515758f, 0.000000f},
        {0.400000f, 2.748894f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 0.000000f, -1.982313f, 0.000000f, 1.982313f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 0.261799f, -1.969754f, 0.103713f, 1.969754f, 0.103713f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 0.523599f, -1.932163f, 0.201358f, 1.932163f, 0.201358f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 0.785398f, -1.870130f, 0.286757f, 1.870130f, 0.286757f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 1.047198f, -1.785646f, 0.353742f, 1.785646f, 0.353742f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 1.308997f, -1.683277f, 0.396693f, 1.683277f, 0.396693f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.141593f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.534292f, 0.000000f, -1.949323f, 0.000000f, 1.949323f, 0.000000f, 3.534292f, 0.000000f, 0.567345f, 0.432655f, 0.000000f},
        {0.400000f, 3.534292f, 0.261799f, -1.937873f, 0.103713f, 1.937873f, 0.103713f, 3.534292f, 0.261799f, 0.563340f, 0.436660f, 0.000000f},
        {0.400000f, 3.534292f, 0.523599f, -1.903543f, 0.201358f, 1.903543f, 0.201358f, 3.534292f, 0.523599f, 0.555355f, 0.444645f, 0.000000f},
        {0.400000f, 3.534292f, 0.785398f, -1.846721f, 0.286757f, 1.846721f, 0.286757f, 3.534292f, 0.785398f, 0.547396f, 0.452604f, 0.000000f},
        {0.400000f, 3.534292f, 1.047198f, -1.769065f, 0.353742f, 1.769065f, 0.353742f, 3.534292f, 1.047198f, 0.531546f, 0.468454f, 0.000000f},
        {0.400000f, 3.534292f, 1.308997f, -1.674683f, 0.396693f, 1.674683f, 0.396693f, 3.534292f, 1.308997f, 0.515758f, 0.484242f, 0.000000f},
        {0.400000f, 3.534292f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 3.926991f, 0.000000f, -1.857553f, 0.000000f, 1.857553f, 0.000000f, 3.926991f, 0.000000f, 0.620357f, 0.379643f, 0.000000f},
        {0.400000f, 3.926991f, 0.261799f, -1.849054f, 0.103713f, 1.849054f, 0.103713f, 3.926991f, 0.261799f, 0.616201f, 0.383799f, 0.000000f},
        {0.400000f, 3.926991f, 0.523599f, -1.823477f, 0.201358f, 1.823477f, 0.201358f, 3.926991f, 0.523599f, 0.603824f, 0.396176f, 0.000000f},
        {0.400000f, 3.926991f, 0.785398f, -1.780852f, 0.286757f, 1.780852f, 0.286757f, 3.926991f, 0.785398f, 0.583451f, 0.416549f, 0.000000f},
        {0.400000f, 3.926991f, 1.047198f, -1.722129f, 0.353742f, 1.722129f, 0.353742f, 3.926991f, 1.047198f, 0.559344f, 0.440656f, 0.000000f},
        {0.400000f, 3.926991f, 1.308997f, -1.650248f, 0.396693f, 1.650248f, 0.396693f, 3.926991f, 1.308997f, 0.531546f, 0.468454f, 0.000000f},
        {0.400000f, 3.926991f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 4.319690f, 0.000000f, -1.724474f, 0.000000f, 1.724474f, 0.000000f, 4.319690f, 0.000000f, 0.654208f, 0.345792f, 0.000000f},
        {0.400000f, 4.319690f, 0.261799f, -1.720006f, 0.103713f, 1.720006f, 0.103713f, 4.319690f, 0.261799f, 0.645635f, 0.354365f, 0.000000f},
        {0.400000f, 4.319690f, 0.523599f, -1.706512f, 0.201358f, 1.706512f, 0.201358f, 4.319690f, 0.523599f, 0.632917f, 0.367083f, 0.000000f},
        {0.400000f, 4.319690f, 0.785398f, -1.683884f, 0.286757f, 1.683884f, 0.286757f, 4.319690f, 0.785398f, 0.612061f, 0.387939f, 0.000000f},
        {0.400000f, 4.319690f, 1.047198f, -1.652475f, 0.353742f, 1.652475f, 0.353742f, 4.319690f, 1.047198f, 0.579410f, 0.420590f, 0.000000f},
        {0.400000f, 4.319690f, 1.308997f, -1.613763f, 0.396693f, 1.613763f, 0.396693f, 4.319690f, 1.308997f, 0.539461f, 0.460539f, 0.000000f},
        {0.400000f, 4.319690f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.662864f, 0.337136f, 0.000000f},
        {0.400000f, 4.712389f, 0.261799f, -1.570796f, 0.103713f, 1.570796f, 0.103713f, 4.712389f, 0.261799f, 0.658526f, 0.341474f, 0.000000f},
        {0.400000f, 4.712389f, 0.523599f, -1.570796f, 0.201358f, 1.570796f, 0.201358f, 4.712389f, 0.523599f, 0.645635f, 0.354365f, 0.000000f},
        {0.400000f, 4.712389f, 0.785398f, -1.570796f, 0.286757f, 1.570796f, 0.286757f, 4.712389f, 0.785398f, 0.620357f, 0.379643f, 0.000000f},
        {0.400000f, 4.712389f, 1.047198f, -1.570796f, 0.353742f, 1.570796f, 0.353742f, 4.712389f, 1.047198f, 0.583451f, 0.416549f, 0.000000f},
        {0.400000f, 4.712389f, 1.308997f, -1.570796f, 0.396693f, 1.570796f, 0.396693f, 4.712389f, 1.308997f, 0.543426f, 0.456574f, 0.000000f},
        {0.400000f, 4.712389f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 5.105088f, 0.000000f, -1.417119f, 0.000000f, 1.417119f, 0.000000f, 5.105088f, 0.000000f, 0.654208f, 0.345792f, 0.000000f},
        {0.400000f, 5.105088f, 0.261799f, -1.421587f, 0.103713f, 1.421587f, 0.103713f, 5.105088f, 0.261799f, 0.645635f, 0.354365f, 0.000000f},
        {0.400000f, 5.105088f, 0.523599f, -1.435081f, 0.201358f, 1.435081f, 0.201358f, 5.105088f, 0.523599f, 0.632917f, 0.367083f, 0.000000f},
        {0.400000f, 5.105088f, 0.785398f, -1.457708f, 0.286757f, 1.457708f, 0.286757f, 5.105088f, 0.785398f, 0.612061f, 0.387939f, 0.000000f},
        {0.400000f, 5.105088f, 1.047198f, -1.489117f, 0.353742f, 1.489117f, 0.353742f, 5.105088f, 1.047198f, 0.579410f, 0.420590f, 0.000000f},
        {0.400000f, 5.105088f, 1.308997f, -1.527829f, 0.396693f, 1.527829f, 0.396693f, 5.105088f, 1.308997f, 0.539461f, 0.460539f, 0.000000f},
        {0.400000f, 5.105088f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 5.497787f, 0.000000f, -1.284040f, 0.000000f, 1.284040f, 0.000000f, 5.497787f, 0.000000f, 0.620357f, 0.379643f, 0.000000f},
        {0.400000f, 5.497787f, 0.261799f, -1.292538f, 0.103713f, 1.292538f, 0.103713f, 5.497787f, 0.261799f, 0.616201f, 0.383799f, 0.000000f},
        {0.400000f, 5.497787f, 0.523599f, -1.318116f, 0.201358f, 1.318116f, 0.201358f, 5.497787f, 0.523599f, 0.603824f, 0.396176f, 0.000000f},
        {0.400000f, 5.497787f, 0.785398f, -1.360741f, 0.286757f, 1.360741f, 0.286757f, 5.497787f, 0.785398f, 0.583451f, 0.416549f, 0.000000f},
        {0.400000f, 5.497787f, 1.047198f, -1.419464f, 0.353742f, 1.419464f, 0.353742f, 5.497787f, 1.047198f, 0.559344f, 0.440656f, 0.000000f},
        {0.400000f, 5.497787f, 1.308997f, -1.491344f, 0.396693f, 1.491344f, 0.396693f, 5.497787f, 1.308997f, 0.531546f, 0.468454f, 0.000000f},
        {0.400000f, 5.497787f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 5.890486f, 0.000000f, -1.192270f, 0.000000f, 1.192270f, 0.000000f, 5.890486f, 0.000000f, 0.567345f, 0.432655f, 0.000000f},
        {0.400000f, 5.890486f, 0.261799f, -1.203720f, 0.103713f, 1.203720f, 0.103713f, 5.890486f, 0.261799f, 0.563340f, 0.436660f, 0.000000f},
        {0.400000f, 5.890486f, 0.523599f, -1.238049f, 0.201358f, 1.238049f, 0.201358f, 5.890486f, 0.523599f, 0.555355f, 0.444645f, 0.000000f},
        {0.400000f, 5.890486f, 0.785398f, -1.294871f, 0.286757f, 1.294871f, 0.286757f, 5.890486f, 0.785398f, 0.547396f, 0.452604f, 0.000000f},
        {0.400000f, 5.890486f, 1.047198f, -1.372528f, 0.353742f, 1.372528f, 0.353742f, 5.890486f, 1.047198f, 0.531546f, 0.468454f, 0.000000f},
        {0.400000f, 5.890486f, 1.308997f, -1.466910f, 0.396693f, 1.466910f, 0.396693f, 5.890486f, 1.308997f, 0.515758f, 0.484242f, 0.000000f},
        {0.400000f, 5.890486f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 0.000000f, -1.159279f, 0.000000f, 1.159279f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 0.261799f, -1.171839f, 0.103713f, 1.171839f, 0.103713f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 0.523599f, -1.209429f, 0.201358f, 1.209429f, 0.201358f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 0.785398f, -1.271462f, 0.286757f, 1.271462f, 0.286757f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 1.047198f, -1.355946f, 0.353742f, 1.355946f, 0.353742f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 1.308997f, -1.458315f, 0.396693f, 1.458315f, 0.396693f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.400000f, 6.283185f, 1.570796f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 0.000000f, -1.047198f, 0.000000f, 1.047198f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 0.261799f, -1.062078f, 0.129773f, 1.062078f, 0.129773f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 0.523599f, -1.107149f, 0.252680f, 1.107149f, 0.252680f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 0.785398f, -1.183200f, 0.361367f, 1.183200f, 0.361367f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 1.047198f, -1.289761f, 0.447832f, 1.289761f, 0.447832f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 1.308997f, -1.422465f, 0.504035f, 1.422465f, 0.504035f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.000000f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.392699f, 0.000000f, -1.090615f, 0.000000f, 1.090615f, 0.000000f, 0.392699f, 0.000000f, 0.412498f, 0.587502f, 0.000000f},
        {0.500000f, 0.392699f, 0.261799f, -1.104050f, 0.129773f, 1.104050f, 0.129773f, 0.392699f, 0.261799f, 0.416549f, 0.583451f, 0.000000f},
        {0.500000f, 0.392699f, 0.523599f, -1.144862f, 0.252680f, 1.144862f, 0.252680f, 0.392699f, 0.523599f, 0.424621f, 0.575379f, 0.000000f},
        {0.500000f, 0.392699f, 0.785398f, -1.214086f, 0.361367f, 1.214086f, 0.361367f, 0.392699f, 0.785398f, 0.436660f, 0.563340f, 0.000000f},
        {0.500000f, 0.392699f, 1.047198f, -1.311668f, 0.447832f, 1.311668f, 0.447832f, 0.392699f, 1.047198f, 0.456574f, 0.543426f, 0.000000f},
        {0.500000f, 0.392699f, 1.308997f, -1.433830f, 0.504035f, 1.433830f, 0.504035f, 0.392699f, 1.308997f, 0.476354f, 0.523646f, 0.000000f},
        {0.500000f, 0.392699f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 0.785398f, 0.000000f, -1.209429f, 0.000000f, 1.209429f, 0.000000f, 0.785398f, 0.000000f, 0.345792f, 0.654208f, 0.000000f},
        {0.500000f, 0.785398f, 0.261799f, -1.219194f, 0.129773f, 1.219194f, 0.129773f, 0.785398f, 0.261799f, 0.350088f, 0.649912f, 0.000000f},
        {0.500000f, 0.785398f, 0.523599f, -1.249046f, 0.252680f, 1.249046f, 0.252680f, 0.785398f, 0.523599f, 0.367083f, 0.632917f, 0.000000f},
        {0.500000f, 0.785398f, 0.785398f, -1.300247f, 0.361367f, 1.300247f, 0.361367f, 0.785398f, 0.785398f, 0.387939f, 0.612061f, 0.000000f},
        {0.500000f, 0.785398f, 1.047198f, -1.373401f, 0.447832f, 1.373401f, 0.447832f, 0.785398f, 1.047198f, 0.420590f, 0.579410f, 0.000000f},
        {0.500000f, 0.785398f, 1.308997f, -1.466103f, 0.504035f, 1.466103f, 0.504035f, 0.785398f, 1.308997f, 0.456574f, 0.543426f, 0.000000f},
        {0.500000f, 0.785398f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 1.178097f, 0.000000f, -1.378267f, 0.000000f, 1.378267f, 0.000000f, 1.178097f, 0.000000f, 0.306110f, 0.693890f, 0.000000f},
        {0.500000f, 1.178097f, 0.261799f, -1.383311f, 0.129773f, 1.383311f, 0.129773f, 1.178097f, 0.261799f, 0.310618f, 0.689382f, 0.000000f},
        {0.500000f, 1.178097f, 0.523599f, -1.398808f, 0.252680f, 1.398808f, 0.252680f, 1.178097f, 0.523599f, 0.328392f, 0.671608f, 0.000000f},
        {0.500000f, 1.178097f, 0.785398f, -1.425646f, 0.361367f, 1.425646f, 0.361367f, 1.178097f, 0.785398f, 0.358623f, 0.641377f, 0.000000f},
        {0.500000f, 1.178097f, 1.047198f, -1.464459f, 0.447832f, 1.464459f, 0.447832f, 1.178097f, 1.047198f, 0.396176f, 0.603824f, 0.000000f},
        {0.500000f, 1.178097f, 1.308997f, -1.514210f, 0.504035f, 1.514210f, 0.504035f, 1.178097f, 1.308997f, 0.444645f, 0.555355f, 0.000000f},
        {0.500000f, 1.178097f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.292421f, 0.707579f, 0.000000f},
        {0.500000f, 1.570796f, 0.261799f, -1.570796f, 0.129773f, 1.570796f, 0.129773f, 1.570796f, 0.261799f, 0.297013f, 0.702987f, 0.000000f},
        {0.500000f, 1.570796f, 0.523599f, -1.570796f, 0.252680f, 1.570796f, 0.252680f, 1.570796f, 0.523599f, 0.315099f, 0.684901f, 0.000000f},
        {0.500000f, 1.570796f, 0.785398f, -1.570796f, 0.361367f, 1.570796f, 0.361367f, 1.570796f, 0.785398f, 0.345792f, 0.654208f, 0.000000f},
        {0.500000f, 1.570796f, 1.047198f, -1.570796f, 0.447832f, 1.570796f, 0.447832f, 1.570796f, 1.047198f, 0.387939f, 0.612061f, 0.000000f},
        {0.500000f, 1.570796f, 1.308997f, -1.570796f, 0.504035f, 1.570796f, 0.504035f, 1.570796f, 1.308997f, 0.440656f, 0.559344f, 0.000000f},
        {0.500000f, 1.570796f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 1.963495f, 0.000000f, -1.763325f, 0.000000f, 1.763325f, 0.000000f, 1.963495f, 0.000000f, 0.306110f, 0.693890f, 0.000000f},
        {0.500000f, 1.963495f, 0.261799f, -1.758282f, 0.129773f, 1.758282f, 0.129773f, 1.963495f, 0.261799f, 0.310618f, 0.689382f, 0.000000f},
        {0.500000f, 1.963495f, 0.523599f, -1.742784f, 0.252680f, 1.742784f, 0.252680f, 1.963495f, 0.523599f, 0.328392f, 0.671608f, 0.000000f},
        {0.500000f, 1.963495f, 0.785398f, -1.715946f, 0.361367f, 1.715946f, 0.361367f, 1.963495f, 0.785398f, 0.358623f, 0.641377f, 0.000000f},
        {0.500000f, 1.963495f, 1.047198f, -1.677134f, 0.447832f, 1.677134f, 0.447832f, 1.963495f, 1.047198f, 0.396176f, 0.603824f, 0.000000f},
        {0.500000f, 1.963495f, 1.308997f, -1.627383f, 0.504035f, 1.627383f, 0.504035f, 1.963495f, 1.308997f, 0.444645f, 0.555355f, 0.000000f},
        {0.500000f, 1.963495f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 2.356194f, 0.000000f, -1.932163f, 0.000000f, 1.932163f, 0.000000f, 2.356194f, 0.000000f, 0.345792f, 0.654208f, 0.000000f},
        {0.500000f, 2.356194f, 0.261799f, -1.922398f, 0.129773f, 1.922398f, 0.129773f, 2.356194f, 0.261799f, 0.350088f, 0.649912f, 0.000000f},
        {0.500000f, 2.356194f, 0.523599f, -1.892547f, 0.252680f, 1.892547f, 0.252680f, 2.356194f, 0.523599f, 0.362862f, 0.637138f, 0.000000f},
        {0.500000f, 2.356194f, 0.785398f, -1.841346f, 0.361367f, 1.841346f, 0.361367f, 2.356194f, 0.785398f, 0.387939f, 0.612061f, 0.000000f},
        {0.500000f, 2.356194f, 1.047198f, -1.768192f, 0.447832f, 1.768192f, 0.447832f, 2.356194f, 1.047198f, 0.420590f, 0.579410f, 0.000000f},
        {0.500000f, 2.356194f, 1.308997f, -1.675490f, 0.504035f, 1.675490f, 0.504035f, 2.356194f, 1.308997f, 0.456574f, 0.543426f, 0.000000f},
        {0.500000f, 2.356194f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 2.748894f, 0.000000f, -2.050977f, 0.000000f, 2.050977f, 0.000000f, 2.748894f, 0.000000f, 0.412498f, 0.587502f, 0.000000f},
        {0.500000f, 2.748894f, 0.261799f, -2.037543f, 0.129773f, 2.037543f, 0.129773f, 2.748894f, 0.261799f, 0.416549f, 0.583451f, 0.000000f},
        {0.500000f, 2.748894f, 0.523599f, -1.996730f, 0.252680f, 1.996730f, 0.252680f, 2.748894f, 0.523599f, 0.424621f, 0.575379f, 0.000000f},
        {0.500000f, 2.748894f, 0.785398f, -1.927507f, 0.361367f, 1.927507f, 0.361367f, 2.748894f, 0.785398f, 0.436660f, 0.563340f, 0.000000f},
        {0.500000f, 2.748894f, 1.047198f, -1.829925f, 0.447832f, 1.829925f, 0.447832f, 2.748894f, 1.047198f, 0.456574f, 0.543426f, 0.000000f},
        {0.500000f, 2.748894f, 1.308997f, -1.707763f, 0.504035f, 1.707763f, 0.504035f, 2.748894f, 1.308997f, 0.476354f, 0.523646f, 0.000000f},
        {0.500000f, 2.748894f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 0.000000f, -2.094395f, 0.000000f, 2.094395f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 0.261799f, -2.079515f, 0.129773f, 2.079515f, 0.129773f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 0.523599f, -2.034444f, 0.252680f, 2.034444f, 0.252680f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 0.785398f, -1.958393f, 0.361367f, 1.958393f, 0.361367f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 1.047198f, -1.851831f, 0.447832f, 1.851831f, 0.447832f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 1.308997f, -1.719128f, 0.504035f, 1.719128f, 0.504035f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.141593f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.534292f, 0.000000f, -2.050977f, 0.000000f, 2.050977f, 0.000000f, 3.534292f, 0.000000f, 0.587502f, 0.412498f, 0.000000f},
        {0.500000f, 3.534292f, 0.261799f, -2.037543f, 0.129773f, 2.037543f, 0.129773f, 3.534292f, 0.261799f, 0.583451f, 0.416549f, 0.000000f},
        {0.500000f, 3.534292f, 0.523599f, -1.996730f, 0.252680f, 1.996730f, 0.252680f, 3.534292f, 0.523599f, 0.575379f, 0.424621f, 0.000000f},
        {0.500000f, 3.534292f, 0.785398f, -1.927507f, 0.361367f, 1.927507f, 0.361367f, 3.534292f, 0.785398f, 0.563340f, 0.436660f, 0.000000f},
        {0.500000f, 3.534292f, 1.047198f, -1.829925f, 0.447832f, 1.829925f, 0.447832f, 3.534292f, 1.047198f, 0.543426f, 0.456574f, 0.000000f},
        {0.500000f, 3.534292f, 1.308997f, -1.707763f, 0.504035f, 1.707763f, 0.504035f, 3.534292f, 1.308997f, 0.523646f, 0.476354f, 0.000000f},
        {0.500000f, 3.534292f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 3.926991f, 0.000000f, -1.932163f, 0.000000f, 1.932163f, 0.000000f, 3.926991f, 0.000000f, 0.654208f, 0.345792f, 0.000000f},
        {0.500000f, 3.926991f, 0.261799f, -1.922398f, 0.129773f, 1.922398f, 0.129773f, 3.926991f, 0.261799f, 0.649912f, 0.350088f, 0.000000f},
        {0.500000f, 3.926991f, 0.523599f, -1.892547f, 0.252680f, 1.892547f, 0.252680f, 3.926991f, 0.523599f, 0.632917f, 0.367083f, 0.000000f},
        {0.500000f, 3.926991f, 0.785398f, -1.841346f, 0.361367f, 1.841346f, 0.361367f, 3.926991f, 0.785398f, 0.612061f, 0.387939f, 0.000000f},
        {0.500000f, 3.926991f, 1.047198f, -1.768192f, 0.447832f, 1.768192f, 0.447832f, 3.926991f, 1.047198f, 0.579410f, 0.420590f, 0.000000f},
        {0.500000f, 3.926991f, 1.308997f, -1.675490f, 0.504035f, 1.675490f, 0.504035f, 3.926991f, 1.308997f, 0.543426f, 0.456574f, 0.000000f},
        {0.500000f, 3.926991f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 4.319690f, 0.000000f, -1.763325f, 0.000000f, 1.763325f, 0.000000f, 4.319690f, 0.000000f, 0.693890f, 0.306110f, 0.000000f},
        {0.500000f, 4.319690f, 0.261799f, -1.758282f, 0.129773f, 1.758282f, 0.129773f, 4.319690f, 0.261799f, 0.689382f, 0.310618f, 0.000000f},
        {0.500000f, 4.319690f, 0.523599f, -1.742784f, 0.252680f, 1.742784f, 0.252680f, 4.319690f, 0.523599f, 0.671608f, 0.328392f, 0.000000f},
        {0.500000f, 4.319690f, 0.785398f, -1.715946f, 0.361367f, 1.715946f, 0.361367f, 4.319690f, 0.785398f, 0.641377f, 0.358623f, 0.000000f},
        {0.500000f, 4.319690f, 1.047198f, -1.677134f, 0.447832f, 1.677134f, 0.447832f, 4.319690f, 1.047198f, 0.603824f, 0.396176f, 0.000000f},
        {0.500000f, 4.319690f, 1.308997f, -1.627383f, 0.504035f, 1.627383f, 0.504035f, 4.319690f, 1.308997f, 0.555355f, 0.444645f, 0.000000f},
        {0.500000f, 4.319690f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.707579f, 0.292421f, 0.000000f},
        {0.500000f, 4.712389f, 0.261799f, -1.570796f, 0.129773f, 1.570796f, 0.129773f, 4.712389f, 0.261799f, 0.702987f, 0.297013f, 0.000000f},
        {0.500000f, 4.712389f, 0.523599f, -1.570796f, 0.252680f, 1.570796f, 0.252680f, 4.712389f, 0.523599f, 0.684901f, 0.315099f, 0.000000f},
        {0.500000f, 4.712389f, 0.785398f, -1.570796f, 0.361367f, 1.570796f, 0.361367f, 4.712389f, 0.785398f, 0.654208f, 0.345792f, 0.000000f},
        {0.500000f, 4.712389f, 1.047198f, -1.570796f, 0.447832f, 1.570796f, 0.447832f, 4.712389f, 1.047198f, 0.612061f, 0.387939f, 0.000000f},
        {0.500000f, 4.712389f, 1.308997f, -1.570796f, 0.504035f, 1.570796f, 0.504035f, 4.712389f, 1.308997f, 0.559344f, 0.440656f, 0.000000f},
        {0.500000f, 4.712389f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 5.105088f, 0.000000f, -1.378267f, 0.000000f, 1.378267f, 0.000000f, 5.105088f, 0.000000f, 0.693890f, 0.306110f, 0.000000f},
        {0.500000f, 5.105088f, 0.261799f, -1.383311f, 0.129773f, 1.383311f, 0.129773f, 5.105088f, 0.261799f, 0.689382f, 0.310618f, 0.000000f},
        {0.500000f, 5.105088f, 0.523599f, -1.398808f, 0.252680f, 1.398808f, 0.252680f, 5.105088f, 0.523599f, 0.671608f, 0.328392f, 0.000000f},
        {0.500000f, 5.105088f, 0.785398f, -1.425646f, 0.361367f, 1.425646f, 0.361367f, 5.105088f, 0.785398f, 0.641377f, 0.358623f, 0.000000f},
        {0.500000f, 5.105088f, 1.047198f, -1.464459f, 0.447832f, 1.464459f, 0.447832f, 5.105088f, 1.047198f, 0.603824f, 0.396176f, 0.000000f},
        {0.500000f, 5.105088f, 1.308997f, -1.514210f, 0.504035f, 1.514210f, 0.504035f, 5.105088f, 1.308997f, 0.555355f, 0.444645f, 0.000000f},
        {0.500000f, 5.105088f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 5.497787f, 0.000000f, -1.209429f, 0.000000f, 1.209429f, 0.000000f, 5.497787f, 0.000000f, 0.654208f, 0.345792f, 0.000000f},
        {0.500000f, 5.497787f, 0.261799f, -1.219194f, 0.129773f, 1.219194f, 0.129773f, 5.497787f, 0.261799f, 0.649912f, 0.350088f, 0.000000f},
        {0.500000f, 5.497787f, 0.523599f, -1.249046f, 0.252680f, 1.249046f, 0.252680f, 5.497787f, 0.523599f, 0.637138f, 0.362862f, 0.000000f},
        {0.500000f, 5.497787f, 0.785398f, -1.300247f, 0.361367f, 1.300247f, 0.361367f, 5.497787f, 0.785398f, 0.612061f, 0.387939f, 0.000000f},
        {0.500000f, 5.497787f, 1.047198f, -1.373401f, 0.447832f, 1.373401f, 0.447832f, 5.497787f, 1.047198f, 0.579410f, 0.420590f, 0.000000f},
        {0.500000f, 5.497787f, 1.308997f, -1.466103f, 0.504035f, 1.466103f, 0.504035f, 5.497787f, 1.308997f, 0.543426f, 0.456574f, 0.000000f},
        {0.500000f, 5.497787f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 5.890486f, 0.000000f, -1.090615f, 0.000000f, 1.090615f, 0.000000f, 5.890486f, 0.000000f, 0.587502f, 0.412498f, 0.000000f},
        {0.500000f, 5.890486f, 0.261799f, -1.104050f, 0.129773f, 1.104050f, 0.129773f, 5.890486f, 0.261799f, 0.583451f, 0.416549f, 0.000000f},
        {0.500000f, 5.890486f, 0.523599f, -1.144862f, 0.252680f, 1.144862f, 0.252680f, 5.890486f, 0.523599f, 0.575379f, 0.424621f, 0.000000f},
        {0.500000f, 5.890486f, 0.785398f, -1.214086f, 0.361367f, 1.214086f, 0.361367f, 5.890486f, 0.785398f, 0.563340f, 0.436660f, 0.000000f},
        {0.500000f, 5.890486f, 1.047198f, -1.311668f, 0.447832f, 1.311668f, 0.447832f, 5.890486f, 1.047198f, 0.543426f, 0.456574f, 0.000000f},
        {0.500000f, 5.890486f, 1.308997f, -1.433830f, 0.504035f, 1.433830f, 0.504035f, 5.890486f, 1.308997f, 0.523646f, 0.476354f, 0.000000f},
        {0.500000f, 5.890486f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 0.000000f, -1.047198f, 0.000000f, 1.047198f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 0.261799f, -1.062078f, 0.129773f, 1.062078f, 0.129773f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 0.523599f, -1.107149f, 0.252680f, 1.107149f, 0.252680f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 0.785398f, -1.183200f, 0.361367f, 1.183200f, 0.361367f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 1.047198f, -1.289761f, 0.447832f, 1.289761f, 0.447832f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 1.308997f, -1.422465f, 0.504035f, 1.422465f, 0.504035f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.500000f, 6.283185f, 1.570796f, -1.570796f, 0.523599f, 1.570796f, 0.523599f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 0.000000f, -0.927295f, 0.000000f, 0.927295f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 0.261799f, -0.943852f, 0.155922f, 0.943852f, 0.155922f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 0.523599f, -0.994759f, 0.304693f, 0.994759f, 0.304693f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 0.785398f, -1.083180f, 0.438149f, 1.083180f, 0.438149f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 1.047198f, -1.212026f, 0.546401f, 1.212026f, 0.546401f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 1.308997f, -1.379066f, 0.618183f, 1.379066f, 0.618183f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.000000f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.392699f, 0.000000f, -0.983241f, 0.000000f, 0.983241f, 0.000000f, 0.392699f, 0.000000f, 0.387939f, 0.612061f, 0.000000f},
        {0.600000f, 0.392699f, 0.261799f, -0.997964f, 0.155922f, 0.997964f, 0.155922f, 0.392699f, 0.261799f, 0.392064f, 0.607936f, 0.000000f},
        {0.600000f, 0.392699f, 0.523599f, -1.043450f, 0.304693f, 1.043450f, 0.304693f, 0.392699f, 0.523599f, 0.404361f, 0.595639f, 0.000000f},
        {0.600000f, 0.392699f, 0.785398f, -1.123136f, 0.438149f, 1.123136f, 0.438149f, 0.392699f, 0.785398f, 0.420590f, 0.579410f, 0.000000f},
        {0.600000f, 0.392699f, 1.047198f, -1.240423f, 0.546401f, 1.240423f, 0.546401f, 0.392699f, 1.047198f, 0.444645f, 0.555355f, 0.000000f},
        {0.600000f, 0.392699f, 1.308997f, -1.393822f, 0.618183f, 1.393822f, 0.618183f, 0.392699f, 1.308997f, 0.472406f, 0.527594f, 0.000000f},
        {0.600000f, 0.392699f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 0.785398f, 0.000000f, -1.132647f, 0.000000f, 1.132647f, 0.000000f, 0.785398f, 0.000000f, 0.306110f, 0.693890f, 0.000000f},
        {0.600000f, 0.785398f, 0.261799f, -1.143029f, 0.155922f, 1.143029f, 0.155922f, 0.785398f, 0.261799f, 0.310618f, 0.689382f, 0.000000f},
        {0.600000f, 0.785398f, 0.523599f, -1.175410f, 0.304693f, 1.175410f, 0.304693f, 0.785398f, 0.523599f, 0.328392f, 0.671608f, 0.000000f},
        {0.600000f, 0.785398f, 0.785398f, -1.233121f, 0.438149f, 1.233121f, 0.438149f, 0.785398f, 0.785398f, 0.358623f, 0.641377f, 0.000000f},
        {0.600000f, 0.785398f, 1.047198f, -1.319890f, 0.546401f, 1.319890f, 0.546401f, 0.785398f, 1.047198f, 0.396176f, 0.603824f, 0.000000f},
        {0.600000f, 0.785398f, 1.308997f, -1.435641f, 0.618183f, 1.435641f, 0.618183f, 0.785398f, 1.308997f, 0.444645f, 0.555355f, 0.000000f},
        {0.600000f, 0.785398f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 1.178097f, 0.000000f, -1.339119f, 0.000000f, 1.339119f, 0.000000f, 1.178097f, 0.000000f, 0.259399f, 0.740601f, 0.000000f},
        {0.600000f, 1.178097f, 0.261799f, -1.344356f, 0.155922f, 1.344356f, 0.155922f, 1.178097f, 0.261799f, 0.264217f, 0.735783f, 0.000000f},
        {0.600000f, 1.178097f, 0.523599f, -1.360807f, 0.304693f, 1.360807f, 0.304693f, 1.178097f, 0.523599f, 0.283148f, 0.716852f, 0.000000f},
        {0.600000f, 1.178097f, 0.785398f, -1.390526f, 0.438149f, 1.390526f, 0.438149f, 1.178097f, 0.785398f, 0.319555f, 0.680445f, 0.000000f},
        {0.600000f, 1.178097f, 1.047198f, -1.436020f, 0.546401f, 1.436020f, 0.546401f, 1.178097f, 1.047198f, 0.367083f, 0.632917f, 0.000000f},
        {0.600000f, 1.178097f, 1.308997f, -1.497808f, 0.618183f, 1.497808f, 0.618183f, 1.178097f, 1.308997f, 0.428643f, 0.571357f, 0.000000f},
        {0.600000f, 1.178097f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.244725f, 0.755275f, 0.000000f},
        {0.600000f, 1.570796f, 0.261799f, -1.570796f, 0.155922f, 1.570796f, 0.155922f, 1.570796f, 0.261799f, 0.249654f, 0.750346f, 0.000000f},
        {0.600000f, 1.570796f, 0.523599f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 1.570796f, 0.523599f, 0.269000f, 0.731000f, 0.000000f},
        {0.600000f, 1.570796f, 0.785398f, -1.570796f, 0.438149f, 1.570796f, 0.438149f, 1.570796f, 0.785398f, 0.306110f, 0.693890f, 0.000000f},
        {0.600000f, 1.570796f, 1.047198f, -1.570796f, 0.546401f, 1.570796f, 0.546401f, 1.570796f, 1.047198f, 0.358623f, 0.641377f, 0.000000f},
        {0.600000f, 1.570796f, 1.308997f, -1.570796f, 0.618183f, 1.570796f, 0.618183f, 1.570796f, 1.308997f, 0.424621f, 0.575379f, 0.000000f},
        {0.600000f, 1.570796f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 1.963495f, 0.000000f, -1.802473f, 0.000000f, 1.802473f, 0.000000f, 1.963495f, 0.000000f, 0.259399f, 0.740601f, 0.000000f},
        {0.600000f, 1.963495f, 0.261799f, -1.797236f, 0.155922f, 1.797236f, 0.155922f, 1.963495f, 0.261799f, 0.264217f, 0.735783f, 0.000000f},
        {0.600000f, 1.963495f, 0.523599f, -1.780786f, 0.304693f, 1.780786f, 0.304693f, 1.963495f, 0.523599f, 0.283148f, 0.716852f, 0.000000f},
        {0.600000f, 1.963495f, 0.785398f, -1.751066f, 0.438149f, 1.751066f, 0.438149f, 1.963495f, 0.785398f, 0.319555f, 0.680445f, 0.000000f},
        {0.600000f, 1.963495f, 1.047198f, -1.705573f, 0.546401f, 1.705573f, 0.546401f, 1.963495f, 1.047198f, 0.367083f, 0.632917f, 0.000000f},
        {0.600000f, 1.963495f, 1.308997f, -1.643784f, 0.618183f, 1.643784f, 0.618183f, 1.963495f, 1.308997f, 0.428643f, 0.571357f, 0.000000f},
        {0.600000f, 1.963495f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 2.356194f, 0.000000f, -2.008945f, 0.000000f, 2.008945f, 0.000000f, 2.356194f, 0.000000f, 0.306110f, 0.693890f, 0.000000f},
        {0.600000f, 2.356194f, 0.261799f, -1.998563f, 0.155922f, 1.998563f, 0.155922f, 2.356194f, 0.261799f, 0.310618f, 0.689382f, 0.000000f},
        {0.600000f, 2.356194f, 0.523599f, -1.966182f, 0.304693f, 1.966182f, 0.304693f, 2.356194f, 0.523599f, 0.328392f, 0.671608f, 0.000000f},
        {0.600000f, 2.356194f, 0.785398f, -1.908472f, 0.438149f, 1.908472f, 0.438149f, 2.356194f, 0.785398f, 0.358623f, 0.641377f, 0.000000f},
        {0.600000f, 2.356194f, 1.047198f, -1.821702f, 0.546401f, 1.821702f, 0.546401f, 2.356194f, 1.047198f, 0.396176f, 0.603824f, 0.000000f},
        {0.600000f, 2.356194f, 1.308997f, -1.705952f, 0.618183f, 1.705952f, 0.618183f, 2.356194f, 1.308997f, 0.444645f, 0.555355f, 0.000000f},
        {0.600000f, 2.356194f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 2.748894f, 0.000000f, -2.158351f, 0.000000f, 2.158351f, 0.000000f, 2.748894f, 0.000000f, 0.387939f, 0.612061f, 0.000000f},
        {0.600000f, 2.748894f, 0.261799f, -2.143629f, 0.155922f, 2.143629f, 0.155922f, 2.748894f, 0.261799f, 0.392064f, 0.607936f, 0.000000f},
        {0.600000f, 2.748894f, 0.523599f, -2.098142f, 0.304693f, 2.098142f, 0.304693f, 2.748894f, 0.523599f, 0.404361f, 0.595639f, 0.000000f},
        {0.600000f, 2.748894f, 0.785398f, -2.018456f, 0.438149f, 2.018456f, 0.438149f, 2.748894f, 0.785398f, 0.420590f, 0.579410f, 0.000000f},
        {0.600000f, 2.748894f, 1.047198f, -1.901169f, 0.546401f, 1.901169f, 0.546401f, 2.748894f, 1.047198f, 0.444645f, 0.555355f, 0.000000f},
        {0.600000f, 2.748894f, 1.308997f, -1.747771f, 0.618183f, 1.747771f, 0.618183f, 2.748894f, 1.308997f, 0.472406f, 0.527594f, 0.000000f},
        {0.600000f, 2.748894f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 0.000000f, -2.214297f, 0.000000f, 2.214297f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 0.261799f, -2.197740f, 0.155922f, 2.197740f, 0.155922f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 0.523599f, -2.146833f, 0.304693f, 2.146833f, 0.304693f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 0.785398f, -2.058413f, 0.438149f, 2.058413f, 0.438149f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 1.047198f, -1.929567f, 0.546401f, 1.929567f, 0.546401f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 1.308997f, -1.762526f, 0.618183f, 1.762526f, 0.618183f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.141593f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.534292f, 0.000000f, -2.158351f, 0.000000f, 2.158351f, 0.000000f, 3.534292f, 0.000000f, 0.612061f, 0.387939f, 0.000000f},
        {0.600000f, 3.534292f, 0.261799f, -2.143629f, 0.155922f, 2.143629f, 0.155922f, 3.534292f, 0.261799f, 0.607936f, 0.392064f, 0.000000f},
        {0.600000f, 3.534292f, 0.523599f, -2.098142f, 0.304693f, 2.098142f, 0.304693f, 3.534292f, 0.523599f, 0.595639f, 0.404361f, 0.000000f},
        {0.600000f, 3.534292f, 0.785398f, -2.018456f, 0.438149f, 2.018456f, 0.438149f, 3.534292f, 0.785398f, 0.579410f, 0.420590f, 0.000000f},
        {0.600000f, 3.534292f, 1.047198f, -1.901169f, 0.546401f, 1.901169f, 0.546401f, 3.534292f, 1.047198f, 0.555355f, 0.444645f, 0.000000f},
        {0.600000f, 3.534292f, 1.308997f, -1.747771f, 0.618183f, 1.747771f, 0.618183f, 3.534292f, 1.308997f, 0.527594f, 0.472406f, 0.000000f},
        {0.600000f, 3.534292f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 3.926991f, 0.000000f, -2.008945f, 0.000000f, 2.008945f, 0.000000f, 3.926991f, 0.000000f, 0.693890f, 0.306110f, 0.000000f},
        {0.600000f, 3.926991f, 0.261799f, -1.998563f, 0.155922f, 1.998563f, 0.155922f, 3.926991f, 0.261799f, 0.689382f, 0.310618f, 0.000000f},
        {0.600000f, 3.926991f, 0.523599f, -1.966182f, 0.304693f, 1.966182f, 0.304693f, 3.926991f, 0.523599f, 0.671608f, 0.328392f, 0.000000f},
        {0.600000f, 3.926991f, 0.785398f, -1.908472f, 0.438149f, 1.908472f, 0.438149f, 3.926991f, 0.785398f, 0.641377f, 0.358623f, 0.000000f},
        {0.600000f, 3.926991f, 1.047198f, -1.821702f, 0.546401f, 1.821702f, 0.546401f, 3.926991f, 1.047198f, 0.603824f, 0.396176f, 0.000000f},
        {0.600000f, 3.926991f, 1.308997f, -1.705952f, 0.618183f, 1.705952f, 0.618183f, 3.926991f, 1.308997f, 0.555355f, 0.444645f, 0.000000f},
        {0.600000f, 3.926991f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 4.319690f, 0.000000f, -1.802473f, 0.000000f, 1.802473f, 0.000000f, 4.319690f, 0.000000f, 0.740601f, 0.259399f, 0.000000f},
        {0.600000f, 4.319690f, 0.261799f, -1.797236f, 0.155922f, 1.797236f, 0.155922f, 4.319690f, 0.261799f, 0.735783f, 0.264217f, 0.000000f},
        {0.600000f, 4.319690f, 0.523599f, -1.780786f, 0.304693f, 1.780786f, 0.304693f, 4.319690f, 0.523599f, 0.716852f, 0.283148f, 0.000000f},
        {0.600000f, 4.319690f, 0.785398f, -1.751066f, 0.438149f, 1.751066f, 0.438149f, 4.319690f, 0.785398f, 0.680445f, 0.319555f, 0.000000f},
        {0.600000f, 4.319690f, 1.047198f, -1.705573f, 0.546401f, 1.705573f, 0.546401f, 4.319690f, 1.047198f, 0.632917f, 0.367083f, 0.000000f},
        {0.600000f, 4.319690f, 1.308997f, -1.643784f, 0.618183f, 1.643784f, 0.618183f, 4.319690f, 1.308997f, 0.571357f, 0.428643f, 0.000000f},
        {0.600000f, 4.319690f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.755275f, 0.244725f, 0.000000f},
        {0.600000f, 4.712389f, 0.261799f, -1.570796f, 0.155922f, 1.570796f, 0.155922f, 4.712389f, 0.261799f, 0.750346f, 0.249654f, 0.000000f},
        {0.600000f, 4.712389f, 0.523599f, -1.570796f, 0.304693f, 1.570796f, 0.304693f, 4.712389f, 0.523599f, 0.731000f, 0.269000f, 0.000000f},
        {0.600000f, 4.712389f, 0.785398f, -1.570796f, 0.438149f, 1.570796f, 0.438149f, 4.712389f, 0.785398f, 0.693890f, 0.306110f, 0.000000f},
        {0.600000f, 4.712389f, 1.047198f, -1.570796f, 0.546401f, 1.570796f, 0.546401f, 4.712389f, 1.047198f, 0.641377f, 0.358623f, 0.000000f},
        {0.600000f, 4.712389f, 1.308997f, -1.570796f, 0.618183f, 1.570796f, 0.618183f, 4.712389f, 1.308997f, 0.575379f, 0.424621f, 0.000000f},
        {0.600000f, 4.712389f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 5.105088f, 0.000000f, -1.339119f, 0.000000f, 1.339119f, 0.000000f, 5.105088f, 0.000000f, 0.740601f, 0.259399f, 0.000000f},
        {0.600000f, 5.105088f, 0.261799f, -1.344356f, 0.155922f, 1.344356f, 0.155922f, 5.105088f, 0.261799f, 0.735783f, 0.264217f, 0.000000f},
        {0.600000f, 5.105088f, 0.523599f, -1.360807f, 0.304693f, 1.360807f, 0.304693f, 5.105088f, 0.523599f, 0.716852f, 0.283148f, 0.000000f},
        {0.600000f, 5.105088f, 0.785398f, -1.390526f, 0.438149f, 1.390526f, 0.438149f, 5.105088f, 0.785398f, 0.680445f, 0.319555f, 0.000000f},
        {0.600000f, 5.105088f, 1.047198f, -1.436020f, 0.546401f, 1.436020f, 0.546401f, 5.105088f, 1.047198f, 0.632917f, 0.367083f, 0.000000f},
        {0.600000f, 5.105088f, 1.308997f, -1.497808f, 0.618183f, 1.497808f, 0.618183f, 5.105088f, 1.308997f, 0.571357f, 0.428643f, 0.000000f},
        {0.600000f, 5.105088f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 5.497787f, 0.000000f, -1.132647f, 0.000000f, 1.132647f, 0.000000f, 5.497787f, 0.000000f, 0.693890f, 0.306110f, 0.000000f},
        {0.600000f, 5.497787f, 0.261799f, -1.143029f, 0.155922f, 1.143029f, 0.155922f, 5.497787f, 0.261799f, 0.689382f, 0.310618f, 0.000000f},
        {0.600000f, 5.497787f, 0.523599f, -1.175410f, 0.304693f, 1.175410f, 0.304693f, 5.497787f, 0.523599f, 0.671608f, 0.328392f, 0.000000f},
        {0.600000f, 5.497787f, 0.785398f, -1.233121f, 0.438149f, 1.233121f, 0.438149f, 5.497787f, 0.785398f, 0.641377f, 0.358623f, 0.000000f},
        {0.600000f, 5.497787f, 1.047198f, -1.319890f, 0.546401f, 1.319890f, 0.546401f, 5.497787f, 1.047198f, 0.603824f, 0.396176f, 0.000000f},
        {0.600000f, 5.497787f, 1.308997f, -1.435641f, 0.618183f, 1.435641f, 0.618183f, 5.497787f, 1.308997f, 0.555355f, 0.444645f, 0.000000f},
        {0.600000f, 5.497787f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 5.890486f, 0.000000f, -0.983241f, 0.000000f, 0.983241f, 0.000000f, 5.890486f, 0.000000f, 0.612061f, 0.387939f, 0.000000f},
        {0.600000f, 5.890486f, 0.261799f, -0.997964f, 0.155922f, 0.997964f, 0.155922f, 5.890486f, 0.261799f, 0.607936f, 0.392064f, 0.000000f},
        {0.600000f, 5.890486f, 0.523599f, -1.043450f, 0.304693f, 1.043450f, 0.304693f, 5.890486f, 0.523599f, 0.595639f, 0.404361f, 0.000000f},
        {0.600000f, 5.890486f, 0.785398f, -1.123136f, 0.438149f, 1.123136f, 0.438149f, 5.890486f, 0.785398f, 0.579410f, 0.420590f, 0.000000f},
        {0.600000f, 5.890486f, 1.047198f, -1.240423f, 0.546401f, 1.240423f, 0.546401f, 5.890486f, 1.047198f, 0.555355f, 0.444645f, 0.000000f},
        {0.600000f, 5.890486f, 1.308997f, -1.393822f, 0.618183f, 1.393822f, 0.618183f, 5.890486f, 1.308997f, 0.527594f, 0.472406f, 0.000000f},
        {0.600000f, 5.890486f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 0.000000f, -0.927295f, 0.000000f, 0.927295f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 0.261799f, -0.943852f, 0.155922f, 0.943852f, 0.155922f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 0.523599f, -0.994759f, 0.304693f, 0.994759f, 0.304693f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 0.785398f, -1.083180f, 0.438149f, 1.083180f, 0.438149f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 1.047198f, -1.212026f, 0.546401f, 1.212026f, 0.546401f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 1.308997f, -1.379066f, 0.618183f, 1.379066f, 0.618183f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.600000f, 6.283185f, 1.570796f, -1.570796f, 0.643501f, 1.570796f, 0.643501f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 0.000000f, -0.795399f, 0.000000f, 0.795399f, 0.000000f, 0.000000f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 0.261799f, -0.812720f, 0.182179f, 0.812720f, 0.182179f, 0.000000f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 0.523599f, -0.866956f, 0.357571f, 0.866956f, 0.357571f, 0.000000f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 0.785398f, -0.964714f, 0.517806f, 0.964714f, 0.517806f, 0.000000f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 1.047198f, -1.115102f, 0.651296f, 1.115102f, 0.651296f, 0.000000f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 1.308997f, -1.322345f, 0.742522f, 1.322345f, 0.742522f, 0.000000f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.000000f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 0.000000f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.392699f, 0.000000f, -0.867526f, 0.000000f, 0.867526f, 0.000000f, 0.392699f, 0.000000f, 0.358623f, 0.641377f, 0.000000f},
        {0.700000f, 0.392699f, 0.261799f, -0.882540f, 0.182179f, 0.882540f, 0.182179f, 0.392699f, 0.261799f, 0.362862f, 0.637138f, 0.000000f},
        {0.700000f, 0.392699f, 0.523599f, -0.929932f, 0.357571f, 0.929932f, 0.357571f, 0.392699f, 0.523599f, 0.375473f, 0.624527f, 0.000000f},
        {0.700000f, 0.392699f, 0.785398f, -1.016565f, 0.517806f, 1.016565f, 0.517806f, 0.392699f, 0.785398f, 0.396176f, 0.603824f, 0.000000f},
        {0.700000f, 0.392699f, 1.047198f, -1.152081f, 0.651296f, 1.152081f, 0.651296f, 0.392699f, 1.047198f, 0.424621f, 0.575379f, 0.000000f},
        {0.700000f, 0.392699f, 1.308997f, -1.341610f, 0.742522f, 1.341610f, 0.742522f, 0.392699f, 1.308997f, 0.460539f, 0.539461f, 0.000000f},
        {0.700000f, 0.392699f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 0.392699f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 0.785398f, 0.000000f, -1.052991f, 0.000000f, 1.052991f, 0.000000f, 0.785398f, 0.000000f, 0.259399f, 0.740601f, 0.000000f},
        {0.700000f, 0.785398f, 0.261799f, -1.063113f, 0.182179f, 1.063113f, 0.182179f, 0.785398f, 0.261799f, 0.264217f, 0.735783f, 0.000000f},
        {0.700000f, 0.785398f, 0.523599f, -1.095497f, 0.357571f, 1.095497f, 0.357571f, 0.785398f, 0.523599f, 0.283148f, 0.716852f, 0.000000f},
        {0.700000f, 0.785398f, 0.785398f, -1.156217f, 0.517806f, 1.156217f, 0.517806f, 0.785398f, 0.785398f, 0.319555f, 0.680445f, 0.000000f},
        {0.700000f, 0.785398f, 1.047198f, -1.254354f, 0.651296f, 1.254354f, 0.651296f, 0.785398f, 1.047198f, 0.367083f, 0.632917f, 0.000000f},
        {0.700000f, 0.785398f, 1.308997f, -1.396028f, 0.742522f, 1.396028f, 0.742522f, 0.785398f, 1.308997f, 0.428643f, 0.571357f, 0.000000f},
        {0.700000f, 0.785398f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 0.785398f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 1.178097f, 0.000000f, -1.299606f, 0.000000f, 1.299606f, 0.000000f, 1.178097f, 0.000000f, 0.209060f, 0.790940f, 0.000000f},
        {0.700000f, 1.178097f, 0.261799f, -1.304557f, 0.182179f, 1.304557f, 0.182179f, 1.178097f, 0.261799f, 0.214287f, 0.785713f, 0.000000f},
        {0.700000f, 1.178097f, 0.523599f, -1.320539f, 0.357571f, 1.320539f, 0.357571f, 1.178097f, 0.523599f, 0.234748f, 0.765252f, 0.000000f},
        {0.700000f, 1.178097f, 0.785398f, -1.351035f, 0.517806f, 1.351035f, 0.517806f, 1.178097f, 0.785398f, 0.273749f, 0.726251f, 0.000000f},
        {0.700000f, 1.178097f, 1.047198f, -1.401576f, 0.651296f, 1.401576f, 0.651296f, 1.178097f, 1.047198f, 0.332775f, 0.667225f, 0.000000f},
        {0.700000f, 1.178097f, 1.308997f, -1.476554f, 0.742522f, 1.476554f, 0.742522f, 1.178097f, 1.308997f, 0.408435f, 0.591565f, 0.000000f},
        {0.700000f, 1.178097f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 1.178097f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.193088f, 0.806912f, 0.000000f},
        {0.700000f, 1.570796f, 0.261799f, -1.570796f, 0.182179f, 1.570796f, 0.182179f, 1.570796f, 0.261799f, 0.198462f, 0.801538f, 0.000000f},
        {0.700000f, 1.570796f, 0.523599f, -1.570796f, 0.357571f, 1.570796f, 0.357571f, 1.570796f, 0.523599f, 0.219469f, 0.780531f, 0.000000f},
        {0.700000f, 1.570796f, 0.785398f, -1.570796f, 0.517806f, 1.570796f, 0.517806f, 1.570796f, 0.785398f, 0.259399f, 0.740601f, 0.000000f},
        {0.700000f, 1.570796f, 1.047198f, -1.570796f, 0.651296f, 1.570796f, 0.651296f, 1.570796f, 1.047198f, 0.319555f, 0.680445f, 0.000000f},
        {0.700000f, 1.570796f, 1.308997f, -1.570796f, 0.742522f, 1.570796f, 0.742522f, 1.570796f, 1.308997f, 0.400275f, 0.599725f, 0.000000f},
        {0.700000f, 1.570796f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 1.570796f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 1.963495f, 0.000000f, -1.841987f, 0.000000f, 1.841987f, 0.000000f, 1.963495f, 0.000000f, 0.209060f, 0.790940f, 0.000000f},
        {0.700000f, 1.963495f, 0.261799f, -1.837035f, 0.182179f, 1.837035f, 0.182179f, 1.963495f, 0.261799f, 0.214287f, 0.785713f, 0.000000f},
        {0.700000f, 1.963495f, 0.523599f, -1.821054f, 0.357571f, 1.821054f, 0.357571f, 1.963495f, 0.523599f, 0.234748f, 0.765252f, 0.000000f},
        {0.700000f, 1.963495f, 0.785398f, -1.790557f, 0.517806f, 1.790557f, 0.517806f, 1.963495f, 0.785398f, 0.273749f, 0.726251f, 0.000000f},
        {0.700000f, 1.963495f, 1.047198f, -1.740016f, 0.651296f, 1.740016f, 0.651296f, 1.963495f, 1.047198f, 0.332775f, 0.667225f, 0.000000f},
        {0.700000f, 1.963495f, 1.308997f, -1.665039f, 0.742522f, 1.665039f, 0.742522f, 1.963495f, 1.308997f, 0.408435f, 0.591565f, 0.000000f},
        {0.700000f, 1.963495f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 1.963495f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 2.356194f, 0.000000f, -2.088602f, 0.000000f, 2.088602f, 0.000000f, 2.356194f, 0.000000f, 0.259399f, 0.740601f, 0.000000f},
        {0.700000f, 2.356194f, 0.261799f, -2.078480f, 0.182179f, 2.078480f, 0.182179f, 2.356194f, 0.261799f, 0.264217f, 0.735783f, 0.000000f},
        {0.700000f, 2.356194f, 0.523599f, -2.046095f, 0.357571f, 2.046095f, 0.357571f, 2.356194f, 0.523599f, 0.283148f, 0.716852f, 0.000000f},
        {0.700000f, 2.356194f, 0.785398f, -1.985375f, 0.517806f, 1.985375f, 0.517806f, 2.356194f, 0.785398f, 0.319555f, 0.680445f, 0.000000f},
        {0.700000f, 2.356194f, 1.047198f, -1.887239f, 0.651296f, 1.887239f, 0.651296f, 2.356194f, 1.047198f, 0.367083f, 0.632917f, 0.000000f},
        {0.700000f, 2.356194f, 1.308997f, -1.745565f, 0.742522f, 1.745565f, 0.742522f, 2.356194f, 1.308997f, 0.428643f, 0.571357f, 0.000000f},
        {0.700000f, 2.356194f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 2.356194f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 2.748894f, 0.000000f, -2.274067f, 0.000000f, 2.274067f, 0.000000f, 2.748894f, 0.000000f, 0.358623f, 0.641377f, 0.000000f},
        {0.700000f, 2.748894f, 0.261799f, -2.259052f, 0.182179f, 2.259052f, 0.182179f, 2.748894f, 0.261799f, 0.362862f, 0.637138f, 0.000000f},
        {0.700000f, 2.748894f, 0.523599f, -2.211661f, 0.357571f, 2.211661f, 0.357571f, 2.748894f, 0.523599f, 0.375473f, 0.624527f, 0.000000f},
        {0.700000f, 2.748894f, 0.785398f, -2.125028f, 0.517806f, 2.125028f, 0.517806f, 2.748894f, 0.785398f, 0.396176f, 0.603824f, 0.000000f},
        {0.700000f, 2.748894f, 1.047198f, -1.989511f, 0.651296f, 1.989511f, 0.651296f, 2.748894f, 1.047198f, 0.424621f, 0.575379f, 0.000000f},
        {0.700000f, 2.748894f, 1.308997f, -1.799983f, 0.742522f, 1.799983f, 0.742522f, 2.748894f, 1.308997f, 0.460539f, 0.539461f, 0.000000f},
        {0.700000f, 2.748894f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 2.748894f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 0.000000f, -2.346194f, 0.000000f, 2.346194f, 0.000000f, 3.141593f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 0.261799f, -2.328873f, 0.182179f, 2.328873f, 0.182179f, 3.141593f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 0.523599f, -2.274637f, 0.357571f, 2.274637f, 0.357571f, 3.141593f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 0.785398f, -2.176879f, 0.517806f, 2.176879f, 0.517806f, 3.141593f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 1.047198f, -2.026491f, 0.651296f, 2.026491f, 0.651296f, 3.141593f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 1.308997f, -1.819248f, 0.742522f, 1.819248f, 0.742522f, 3.141593f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.141593f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 3.141593f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.534292f, 0.000000f, -2.274067f, 0.000000f, 2.274067f, 0.000000f, 3.534292f, 0.000000f, 0.641377f, 0.358623f, 0.000000f},
        {0.700000f, 3.534292f, 0.261799f, -2.259052f, 0.182179f, 2.259052f, 0.182179f, 3.534292f, 0.261799f, 0.637138f, 0.362862f, 0.000000f},
        {0.700000f, 3.534292f, 0.523599f, -2.211661f, 0.357571f, 2.211661f, 0.357571f, 3.534292f, 0.523599f, 0.624527f, 0.375473f, 0.000000f},
        {0.700000f, 3.534292f, 0.785398f, -2.125028f, 0.517806f, 2.125028f, 0.517806f, 3.534292f, 0.785398f, 0.603824f, 0.396176f, 0.000000f},
        {0.700000f, 3.534292f, 1.047198f, -1.989511f, 0.651296f, 1.989511f, 0.651296f, 3.534292f, 1.047198f, 0.575379f, 0.424621f, 0.000000f},
        {0.700000f, 3.534292f, 1.308997f, -1.799983f, 0.742522f, 1.799983f, 0.742522f, 3.534292f, 1.308997f, 0.539461f, 0.460539f, 0.000000f},
        {0.700000f, 3.534292f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 3.534292f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 3.926991f, 0.000000f, -2.088602f, 0.000000f, 2.088602f, 0.000000f, 3.926991f, 0.000000f, 0.740601f, 0.259399f, 0.000000f},
        {0.700000f, 3.926991f, 0.261799f, -2.078480f, 0.182179f, 2.078480f, 0.182179f, 3.926991f, 0.261799f, 0.735783f, 0.264217f, 0.000000f},
        {0.700000f, 3.926991f, 0.523599f, -2.046095f, 0.357571f, 2.046095f, 0.357571f, 3.926991f, 0.523599f, 0.716852f, 0.283148f, 0.000000f},
        {0.700000f, 3.926991f, 0.785398f, -1.985375f, 0.517806f, 1.985375f, 0.517806f, 3.926991f, 0.785398f, 0.680445f, 0.319555f, 0.000000f},
        {0.700000f, 3.926991f, 1.047198f, -1.887239f, 0.651296f, 1.887239f, 0.651296f, 3.926991f, 1.047198f, 0.632917f, 0.367083f, 0.000000f},
        {0.700000f, 3.926991f, 1.308997f, -1.745565f, 0.742522f, 1.745565f, 0.742522f, 3.926991f, 1.308997f, 0.571357f, 0.428643f, 0.000000f},
        {0.700000f, 3.926991f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 3.926991f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 4.319690f, 0.000000f, -1.841987f, 0.000000f, 1.841987f, 0.000000f, 4.319690f, 0.000000f, 0.790940f, 0.209060f, 0.000000f},
        {0.700000f, 4.319690f, 0.261799f, -1.837035f, 0.182179f, 1.837035f, 0.182179f, 4.319690f, 0.261799f, 0.785713f, 0.214287f, 0.000000f},
        {0.700000f, 4.319690f, 0.523599f, -1.821054f, 0.357571f, 1.821054f, 0.357571f, 4.319690f, 0.523599f, 0.765252f, 0.234748f, 0.000000f},
        {0.700000f, 4.319690f, 0.785398f, -1.790557f, 0.517806f, 1.790557f, 0.517806f, 4.319690f, 0.785398f, 0.726251f, 0.273749f, 0.000000f},
        {0.700000f, 4.319690f, 1.047198f, -1.740016f, 0.651296f, 1.740016f, 0.651296f, 4.319690f, 1.047198f, 0.667225f, 0.332775f, 0.000000f},
        {0.700000f, 4.319690f, 1.308997f, -1.665039f, 0.742522f, 1.665039f, 0.742522f, 4.319690f, 1.308997f, 0.591565f, 0.408435f, 0.000000f},
        {0.700000f, 4.319690f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 4.319690f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.806912f, 0.193088f, 0.000000f},
        {0.700000f, 4.712389f, 0.261799f, -1.570796f, 0.182179f, 1.570796f, 0.182179f, 4.712389f, 0.261799f, 0.801538f, 0.198462f, 0.000000f},
        {0.700000f, 4.712389f, 0.523599f, -1.570796f, 0.357571f, 1.570796f, 0.357571f, 4.712389f, 0.523599f, 0.780531f, 0.219469f, 0.000000f},
        {0.700000f, 4.712389f, 0.785398f, -1.570796f, 0.517806f, 1.570796f, 0.517806f, 4.712389f, 0.785398f, 0.740601f, 0.259399f, 0.000000f},
        {0.700000f, 4.712389f, 1.047198f, -1.570796f, 0.651296f, 1.570796f, 0.651296f, 4.712389f, 1.047198f, 0.680445f, 0.319555f, 0.000000f},
        {0.700000f, 4.712389f, 1.308997f, -1.570796f, 0.742522f, 1.570796f, 0.742522f, 4.712389f, 1.308997f, 0.599725f, 0.400275f, 0.000000f},
        {0.700000f, 4.712389f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 4.712389f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 5.105088f, 0.000000f, -1.299606f, 0.000000f, 1.299606f, 0.000000f, 5.105088f, 0.000000f, 0.790940f, 0.209060f, 0.000000f},
        {0.700000f, 5.105088f, 0.261799f, -1.304557f, 0.182179f, 1.304557f, 0.182179f, 5.105088f, 0.261799f, 0.785713f, 0.214287f, 0.000000f},
        {0.700000f, 5.105088f, 0.523599f, -1.320539f, 0.357571f, 1.320539f, 0.357571f, 5.105088f, 0.523599f, 0.765252f, 0.234748f, 0.000000f},
        {0.700000f, 5.105088f, 0.785398f, -1.351035f, 0.517806f, 1.351035f, 0.517806f, 5.105088f, 0.785398f, 0.726251f, 0.273749f, 0.000000f},
        {0.700000f, 5.105088f, 1.047198f, -1.401576f, 0.651296f, 1.401576f, 0.651296f, 5.105088f, 1.047198f, 0.667225f, 0.332775f, 0.000000f},
        {0.700000f, 5.105088f, 1.308997f, -1.476554f, 0.742522f, 1.476554f, 0.742522f, 5.105088f, 1.308997f, 0.591565f, 0.408435f, 0.000000f},
        {0.700000f, 5.105088f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 5.105088f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 5.497787f, 0.000000f, -1.052991f, 0.000000f, 1.052991f, 0.000000f, 5.497787f, 0.000000f, 0.740601f, 0.259399f, 0.000000f},
        {0.700000f, 5.497787f, 0.261799f, -1.063113f, 0.182179f, 1.063113f, 0.182179f, 5.497787f, 0.261799f, 0.735783f, 0.264217f, 0.000000f},
        {0.700000f, 5.497787f, 0.523599f, -1.095497f, 0.357571f, 1.095497f, 0.357571f, 5.497787f, 0.523599f, 0.716852f, 0.283148f, 0.000000f},
        {0.700000f, 5.497787f, 0.785398f, -1.156217f, 0.517806f, 1.156217f, 0.517806f, 5.497787f, 0.785398f, 0.680445f, 0.319555f, 0.000000f},
        {0.700000f, 5.497787f, 1.047198f, -1.254354f, 0.651296f, 1.254354f, 0.651296f, 5.497787f, 1.047198f, 0.632917f, 0.367083f, 0.000000f},
        {0.700000f, 5.497787f, 1.308997f, -1.396028f, 0.742522f, 1.396028f, 0.742522f, 5.497787f, 1.308997f, 0.571357f, 0.428643f, 0.000000f},
        {0.700000f, 5.497787f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 5.497787f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 5.890486f, 0.000000f, -0.867526f, 0.000000f, 0.867526f, 0.000000f, 5.890486f, 0.000000f, 0.641377f, 0.358623f, 0.000000f},
        {0.700000f, 5.890486f, 0.261799f, -0.882540f, 0.182179f, 0.882540f, 0.182179f, 5.890486f, 0.261799f, 0.637138f, 0.362862f, 0.000000f},
        {0.700000f, 5.890486f, 0.523599f, -0.929932f, 0.357571f, 0.929932f, 0.357571f, 5.890486f, 0.523599f, 0.624527f, 0.375473f, 0.000000f},
        {0.700000f, 5.890486f, 0.785398f, -1.016565f, 0.517806f, 1.016565f, 0.517806f, 5.890486f, 0.785398f, 0.603824f, 0.396176f, 0.000000f},
        {0.700000f, 5.890486f, 1.047198f, -1.152081f, 0.651296f, 1.152081f, 0.651296f, 5.890486f, 1.047198f, 0.575379f, 0.424621f, 0.000000f},
        {0.700000f, 5.890486f, 1.308997f, -1.341610f, 0.742522f, 1.341610f, 0.742522f, 5.890486f, 1.308997f, 0.539461f, 0.460539f, 0.000000f},
        {0.700000f, 5.890486f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 5.890486f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 0.000000f, -0.795399f, 0.000000f, 0.795399f, 0.000000f, 6.283185f, 0.000000f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 0.261799f, -0.812720f, 0.182179f, 0.812720f, 0.182179f, 6.283185f, 0.261799f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 0.523599f, -0.866956f, 0.357571f, 0.866956f, 0.357571f, 6.283185f, 0.523599f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 0.785398f, -0.964714f, 0.517806f, 0.964714f, 0.517806f, 6.283185f, 0.785398f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 1.047198f, -1.115102f, 0.651296f, 1.115102f, 0.651296f, 6.283185f, 1.047198f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 1.308997f, -1.322345f, 0.742522f, 1.322345f, 0.742522f, 6.283185f, 1.308997f, 0.500000f, 0.500000f, 0.000000f},
        {0.700000f, 6.283185f, 1.570796f, -1.570796f, 0.775397f, 1.570796f, 0.775397f, 6.283185f, 1.570796f, 0.500000f, 0.500000f, 0.000000f},
        {0.800000f, 0.000000f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 0.000000f, 0.000000f, 0.401896f, 0.401896f, 0.196207f},
        {0.800000f, 0.000000f, 0.261799f, -0.785398f, 0.208564f, 0.785398f, 0.208564f, 0.000000f, 0.261799f, 0.403903f, 0.403903f, 0.192193f},
        {0.800000f, 0.000000f, 0.523599f, -0.785398f, 0.411517f, 0.785398f, 0.411517f, 0.000000f, 0.523599f, 0.409808f, 0.409808f, 0.180385f},
        {0.800000f, 0.000000f, 0.785398f, -0.814827f, 0.601264f, 0.814827f, 0.601264f, 0.000000f, 0.785398f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 0.000000f, 1.047198f, -0.982794f, 0.765393f, 0.982794f, 0.765393f, 0.000000f, 1.047198f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 0.000000f, 1.308997f, -1.238501f, 0.883148f, 1.238501f, 0.883148f, 0.000000f, 1.308997f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 0.000000f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 0.000000f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 0.392699f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 0.392699f, 0.000000f, 0.256171f, 0.568544f, 0.175285f},
        {0.800000f, 0.392699f, 0.261799f, -0.785398f, 0.208564f, 0.785398f, 0.208564f, 0.392699f, 0.261799f, 0.264391f, 0.562983f, 0.172626f},
        {0.800000f, 0.392699f, 0.523599f, -0.797655f, 0.411517f, 0.797655f, 0.411517f, 0.392699f, 0.523599f, 0.277313f, 0.556021f, 0.166667f},
        {0.800000f, 0.392699f, 0.785398f, -0.884371f, 0.601264f, 0.884371f, 0.601264f, 0.392699f, 0.785398f, 0.302385f, 0.530949f, 0.166667f},
        {0.800000f, 0.392699f, 1.047198f, -1.032730f, 0.765393f, 1.032730f, 0.765393f, 0.392699f, 1.047198f, 0.333563f, 0.499771f, 0.166667f},
        {0.800000f, 0.392699f, 1.308997f, -1.264654f, 0.883148f, 1.264654f, 0.883148f, 0.392699f, 1.308997f, 0.373856f, 0.459477f, 0.166667f},
        {0.800000f, 0.392699f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 0.392699f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 0.785398f, 0.000000f, -0.969532f, 0.000000f, 0.969532f, 0.000000f, 0.785398f, 0.000000f, 0.165385f, 0.667948f, 0.166667f},
        {0.800000f, 0.785398f, 0.261799f, -0.978204f, 0.208564f, 0.978204f, 0.208564f, 0.785398f, 0.261799f, 0.169821f, 0.663512f, 0.166667f},
        {0.800000f, 0.785398f, 0.523599f, -1.006854f, 0.411517f, 1.006854f, 0.411517f, 0.785398f, 0.523599f, 0.187171f, 0.646163f, 0.166667f},
        {0.800000f, 0.785398f, 0.785398f, -1.064352f, 0.601264f, 1.064352f, 0.601264f, 0.785398f, 0.785398f, 0.220181f, 0.613152f, 0.166667f},
        {0.800000f, 0.785398f, 1.047198f, -1.167739f, 0.765393f, 1.167739f, 0.765393f, 0.785398f, 1.047198f, 0.269988f, 0.563345f, 0.166667f},
        {0.800000f, 0.785398f, 1.308997f, -1.338032f, 0.883148f, 1.338032f, 0.883148f, 0.785398f, 1.308997f, 0.336968f, 0.496366f, 0.166667f},
        {0.800000f, 0.785398f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 0.785398f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 1.178097f, 0.000000f, -1.259654f, 0.000000f, 1.259654f, 0.000000f, 1.178097f, 0.000000f, 0.123439f, 0.709894f, 0.166667f},
        {0.800000f, 1.178097f, 0.261799f, -1.263728f, 0.208564f, 1.263728f, 0.208564f, 1.178097f, 0.261799f, 0.128294f, 0.705039f, 0.166667f},
        {0.800000f, 1.178097f, 0.523599f, -1.277320f, 0.411517f, 1.277320f, 0.411517f, 1.178097f, 0.523599f, 0.147212f, 0.686122f, 0.166667f},
        {0.800000f, 1.178097f, 0.785398f, -1.305165f, 0.601264f, 1.305165f, 0.601264f, 1.178097f, 0.785398f, 0.178573f, 0.654761f, 0.166667f},
        {0.800000f, 1.178097f, 1.047198f, -1.356894f, 0.765393f, 1.356894f, 0.765393f, 1.178097f, 1.047198f, 0.232054f, 0.601280f, 0.166667f},
        {0.800000f, 1.178097f, 1.308997f, -1.445633f, 0.883148f, 1.445633f, 0.883148f, 1.178097f, 1.308997f, 0.312894f, 0.520439f, 0.166667f},
        {0.800000f, 1.178097f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 1.178097f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.113567f, 0.719767f, 0.166667f},
        {0.800000f, 1.570796f, 0.261799f, -1.570796f, 0.208564f, 1.570796f, 0.208564f, 1.570796f, 0.261799f, 0.118530f, 0.714803f, 0.166667f},
        {0.800000f, 1.570796f, 0.523599f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 1.570796f, 0.523599f, 0.133098f, 0.700235f, 0.166667f},
        {0.800000f, 1.570796f, 0.785398f, -1.570796f, 0.601264f, 1.570796f, 0.601264f, 1.570796f, 0.785398f, 0.165385f, 0.667948f, 0.166667f},
        {0.800000f, 1.570796f, 1.047198f, -1.570796f, 0.765393f, 1.570796f, 0.765393f, 1.570796f, 1.047198f, 0.220181f, 0.613152f, 0.166667f},
        {0.800000f, 1.570796f, 1.308997f, -1.570796f, 0.883148f, 1.570796f, 0.883148f, 1.570796f, 1.308997f, 0.305902f, 0.527431f, 0.166667f},
        {0.800000f, 1.570796f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 1.570796f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 1.963495f, 0.000000f, -1.881939f, 0.000000f, 1.881939f, 0.000000f, 1.963495f, 0.000000f, 0.123439f, 0.709894f, 0.166667f},
        {0.800000f, 1.963495f, 0.261799f, -1.877865f, 0.208564f, 1.877865f, 0.208564f, 1.963495f, 0.261799f, 0.128294f, 0.705039f, 0.166667f},
        {0.800000f, 1.963495f, 0.523599f, -1.864272f, 0.411517f, 1.864272f, 0.411517f, 1.963495f, 0.523599f, 0.147212f, 0.686122f, 0.166667f},
        {0.800000f, 1.963495f, 0.785398f, -1.836428f, 0.601264f, 1.836428f, 0.601264f, 1.963495f, 0.785398f, 0.178573f, 0.654761f, 0.166667f},
        {0.800000f, 1.963495f, 1.047198f, -1.784698f, 0.765393f, 1.784698f, 0.765393f, 1.963495f, 1.047198f, 0.232054f, 0.601280f, 0.166667f},
        {0.800000f, 1.963495f, 1.308997f, -1.695960f, 0.883148f, 1.695960f, 0.883148f, 1.963495f, 1.308997f, 0.312894f, 0.520439f, 0.166667f},
        {0.800000f, 1.963495f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 1.963495f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 2.356194f, 0.000000f, -2.172061f, 0.000000f, 2.172061f, 0.000000f, 2.356194f, 0.000000f, 0.165385f, 0.667948f, 0.166667f},
        {0.800000f, 2.356194f, 0.261799f, -2.163389f, 0.208564f, 2.163389f, 0.208564f, 2.356194f, 0.261799f, 0.169821f, 0.663512f, 0.166667f},
        {0.800000f, 2.356194f, 0.523599f, -2.134739f, 0.411517f, 2.134739f, 0.411517f, 2.356194f, 0.523599f, 0.187171f, 0.646163f, 0.166667f},
        {0.800000f, 2.356194f, 0.785398f, -2.077241f, 0.601264f, 2.077241f, 0.601264f, 2.356194f, 0.785398f, 0.220181f, 0.613152f, 0.166667f},
        {0.800000f, 2.356194f, 1.047198f, -1.973853f, 0.765393f, 1.973853f, 0.765393f, 2.356194f, 1.047198f, 0.269988f, 0.563345f, 0.166667f},
        {0.800000f, 2.356194f, 1.308997f, -1.803561f, 0.883148f, 1.803561f, 0.883148f, 2.356194f, 1.308997f, 0.336968f, 0.496366f, 0.166667f},
        {0.800000f, 2.356194f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 2.356194f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 2.748894f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 2.748894f, 0.000000f, 0.256171f, 0.568544f, 0.175285f},
        {0.800000f, 2.748894f, 0.261799f, -2.356194f, 0.208564f, 2.356194f, 0.208564f, 2.748894f, 0.261799f, 0.264391f, 0.562983f, 0.172626f},
        {0.800000f, 2.748894f, 0.523599f, -2.343938f, 0.411517f, 2.343938f, 0.411517f, 2.748894f, 0.523599f, 0.277313f, 0.556021f, 0.166667f},
        {0.800000f, 2.748894f, 0.785398f, -2.257222f, 0.601264f, 2.257222f, 0.601264f, 2.748894f, 0.785398f, 0.302385f, 0.530949f, 0.166667f},
        {0.800000f, 2.748894f, 1.047198f, -2.108862f, 0.765393f, 2.108862f, 0.765393f, 2.748894f, 1.047198f, 0.333563f, 0.499771f, 0.166667f},
        {0.800000f, 2.748894f, 1.308997f, -1.876939f, 0.883148f, 1.876939f, 0.883148f, 2.748894f, 1.308997f, 0.373856f, 0.459477f, 0.166667f},
        {0.800000f, 2.748894f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 2.748894f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.141593f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 3.141593f, 0.000000f, 0.401896f, 0.401896f, 0.196207f},
        {0.800000f, 3.141593f, 0.261799f, -2.356194f, 0.208564f, 2.356194f, 0.208564f, 3.141593f, 0.261799f, 0.403903f, 0.403903f, 0.192193f},
        {0.800000f, 3.141593f, 0.523599f, -2.356194f, 0.411517f, 2.356194f, 0.411517f, 3.141593f, 0.523599f, 0.409808f, 0.409808f, 0.180385f},
        {0.800000f, 3.141593f, 0.785398f, -2.326766f, 0.601264f, 2.326766f, 0.601264f, 3.141593f, 0.785398f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.141593f, 1.047198f, -2.158799f, 0.765393f, 2.158799f, 0.765393f, 3.141593f, 1.047198f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.141593f, 1.308997f, -1.903092f, 0.883148f, 1.903092f, 0.883148f, 3.141593f, 1.308997f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.141593f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 3.141593f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.534292f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 3.534292f, 0.000000f, 0.568544f, 0.256171f, 0.175285f},
        {0.800000f, 3.534292f, 0.261799f, -2.356194f, 0.208564f, 2.356194f, 0.208564f, 3.534292f, 0.261799f, 0.562983f, 0.264391f, 0.172626f},
        {0.800000f, 3.534292f, 0.523599f, -2.343938f, 0.411517f, 2.343938f, 0.411517f, 3.534292f, 0.523599f, 0.556021f, 0.277313f, 0.166667f},
        {0.800000f, 3.534292f, 0.785398f, -2.257222f, 0.601264f, 2.257222f, 0.601264f, 3.534292f, 0.785398f, 0.530949f, 0.302385f, 0.166667f},
        {0.800000f, 3.534292f, 1.047198f, -2.108862f, 0.765393f, 2.108862f, 0.765393f, 3.534292f, 1.047198f, 0.499771f, 0.333563f, 0.166667f},
        {0.800000f, 3.534292f, 1.308997f, -1.876939f, 0.883148f, 1.876939f, 0.883148f, 3.534292f, 1.308997f, 0.459477f, 0.373856f, 0.166667f},
        {0.800000f, 3.534292f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 3.534292f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 3.926991f, 0.000000f, -2.172061f, 0.000000f, 2.172061f, 0.000000f, 3.926991f, 0.000000f, 0.667948f, 0.165385f, 0.166667f},
        {0.800000f, 3.926991f, 0.261799f, -2.163389f, 0.208564f, 2.163389f, 0.208564f, 3.926991f, 0.261799f, 0.663512f, 0.169821f, 0.166667f},
        {0.800000f, 3.926991f, 0.523599f, -2.134739f, 0.411517f, 2.134739f, 0.411517f, 3.926991f, 0.523599f, 0.646163f, 0.187171f, 0.166667f},
        {0.800000f, 3.926991f, 0.785398f, -2.077241f, 0.601264f, 2.077241f, 0.601264f, 3.926991f, 0.785398f, 0.613152f, 0.220181f, 0.166667f},
        {0.800000f, 3.926991f, 1.047198f, -1.973853f, 0.765393f, 1.973853f, 0.765393f, 3.926991f, 1.047198f, 0.563345f, 0.269988f, 0.166667f},
        {0.800000f, 3.926991f, 1.308997f, -1.803561f, 0.883148f, 1.803561f, 0.883148f, 3.926991f, 1.308997f, 0.496366f, 0.336968f, 0.166667f},
        {0.800000f, 3.926991f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 3.926991f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 4.319690f, 0.000000f, -1.881939f, 0.000000f, 1.881939f, 0.000000f, 4.319690f, 0.000000f, 0.709894f, 0.123439f, 0.166667f},
        {0.800000f, 4.319690f, 0.261799f, -1.877865f, 0.208564f, 1.877865f, 0.208564f, 4.319690f, 0.261799f, 0.705039f, 0.128294f, 0.166667f},
        {0.800000f, 4.319690f, 0.523599f, -1.864272f, 0.411517f, 1.864272f, 0.411517f, 4.319690f, 0.523599f, 0.686122f, 0.147212f, 0.166667f},
        {0.800000f, 4.319690f, 0.785398f, -1.836428f, 0.601264f, 1.836428f, 0.601264f, 4.319690f, 0.785398f, 0.654761f, 0.178573f, 0.166667f},
        {0.800000f, 4.319690f, 1.047198f, -1.784698f, 0.765393f, 1.784698f, 0.765393f, 4.319690f, 1.047198f, 0.601280f, 0.232054f, 0.166667f},
        {0.800000f, 4.319690f, 1.308997f, -1.695960f, 0.883148f, 1.695960f, 0.883148f, 4.319690f, 1.308997f, 0.520439f, 0.312894f, 0.166667f},
        {0.800000f, 4.319690f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 4.319690f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.719767f, 0.113567f, 0.166667f},
        {0.800000f, 4.712389f, 0.261799f, -1.570796f, 0.208564f, 1.570796f, 0.208564f, 4.712389f, 0.261799f, 0.714803f, 0.118530f, 0.166667f},
        {0.800000f, 4.712389f, 0.523599f, -1.570796f, 0.411517f, 1.570796f, 0.411517f, 4.712389f, 0.523599f, 0.700235f, 0.133098f, 0.166667f},
        {0.800000f, 4.712389f, 0.785398f, -1.570796f, 0.601264f, 1.570796f, 0.601264f, 4.712389f, 0.785398f, 0.667948f, 0.165385f, 0.166667f},
        {0.800000f, 4.712389f, 1.047198f, -1.570796f, 0.765393f, 1.570796f, 0.765393f, 4.712389f, 1.047198f, 0.613152f, 0.220181f, 0.166667f},
        {0.800000f, 4.712389f, 1.308997f, -1.570796f, 0.883148f, 1.570796f, 0.883148f, 4.712389f, 1.308997f, 0.527431f, 0.305902f, 0.166667f},
        {0.800000f, 4.712389f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 4.712389f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 5.105088f, 0.000000f, -1.259654f, 0.000000f, 1.259654f, 0.000000f, 5.105088f, 0.000000f, 0.709894f, 0.123439f, 0.166667f},
        {0.800000f, 5.105088f, 0.261799f, -1.263728f, 0.208564f, 1.263728f, 0.208564f, 5.105088f, 0.261799f, 0.705039f, 0.128294f, 0.166667f},
        {0.800000f, 5.105088f, 0.523599f, -1.277320f, 0.411517f, 1.277320f, 0.411517f, 5.105088f, 0.523599f, 0.686122f, 0.147212f, 0.166667f},
        {0.800000f, 5.105088f, 0.785398f, -1.305165f, 0.601264f, 1.305165f, 0.601264f, 5.105088f, 0.785398f, 0.654761f, 0.178573f, 0.166667f},
        {0.800000f, 5.105088f, 1.047198f, -1.356894f, 0.765393f, 1.356894f, 0.765393f, 5.105088f, 1.047198f, 0.601280f, 0.232054f, 0.166667f},
        {0.800000f, 5.105088f, 1.308997f, -1.445633f, 0.883148f, 1.445633f, 0.883148f, 5.105088f, 1.308997f, 0.520439f, 0.312894f, 0.166667f},
        {0.800000f, 5.105088f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 5.105088f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 5.497787f, 0.000000f, -0.969532f, 0.000000f, 0.969532f, 0.000000f, 5.497787f, 0.000000f, 0.667948f, 0.165385f, 0.166667f},
        {0.800000f, 5.497787f, 0.261799f, -0.978204f, 0.208564f, 0.978204f, 0.208564f, 5.497787f, 0.261799f, 0.663512f, 0.169821f, 0.166667f},
        {0.800000f, 5.497787f, 0.523599f, -1.006854f, 0.411517f, 1.006854f, 0.411517f, 5.497787f, 0.523599f, 0.646163f, 0.187171f, 0.166667f},
        {0.800000f, 5.497787f, 0.785398f, -1.064352f, 0.601264f, 1.064352f, 0.601264f, 5.497787f, 0.785398f, 0.613152f, 0.220181f, 0.166667f},
        {0.800000f, 5.497787f, 1.047198f, -1.167739f, 0.765393f, 1.167739f, 0.765393f, 5.497787f, 1.047198f, 0.563345f, 0.269988f, 0.166667f},
        {0.800000f, 5.497787f, 1.308997f, -1.338032f, 0.883148f, 1.338032f, 0.883148f, 5.497787f, 1.308997f, 0.496366f, 0.336968f, 0.166667f},
        {0.800000f, 5.497787f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 5.497787f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 5.890486f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 5.890486f, 0.000000f, 0.568544f, 0.256171f, 0.175285f},
        {0.800000f, 5.890486f, 0.261799f, -0.785398f, 0.208564f, 0.785398f, 0.208564f, 5.890486f, 0.261799f, 0.562983f, 0.264391f, 0.172626f},
        {0.800000f, 5.890486f, 0.523599f, -0.797655f, 0.411517f, 0.797655f, 0.411517f, 5.890486f, 0.523599f, 0.556021f, 0.277313f, 0.166667f},
        {0.800000f, 5.890486f, 0.785398f, -0.884371f, 0.601264f, 0.884371f, 0.601264f, 5.890486f, 0.785398f, 0.530949f, 0.302385f, 0.166667f},
        {0.800000f, 5.890486f, 1.047198f, -1.032730f, 0.765393f, 1.032730f, 0.765393f, 5.890486f, 1.047198f, 0.499771f, 0.333563f, 0.166667f},
        {0.800000f, 5.890486f, 1.308997f, -1.264654f, 0.883148f, 1.264654f, 0.883148f, 5.890486f, 1.308997f, 0.459477f, 0.373856f, 0.166667f},
        {0.800000f, 5.890486f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 5.890486f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 6.283185f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 6.283185f, 0.000000f, 0.401896f, 0.401896f, 0.196207f},
        {0.800000f, 6.283185f, 0.261799f, -0.785398f, 0.208564f, 0.785398f, 0.208564f, 6.283185f, 0.261799f, 0.403903f, 0.403903f, 0.192193f},
        {0.800000f, 6.283185f, 0.523599f, -0.785398f, 0.411517f, 0.785398f, 0.411517f, 6.283185f, 0.523599f, 0.409808f, 0.409808f, 0.180385f},
        {0.800000f, 6.283185f, 0.785398f, -0.814827f, 0.601264f, 0.814827f, 0.601264f, 6.283185f, 0.785398f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 6.283185f, 1.047198f, -0.982794f, 0.765393f, 0.982794f, 0.765393f, 6.283185f, 1.047198f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 6.283185f, 1.308997f, -1.238501f, 0.883148f, 1.238501f, 0.883148f, 6.283185f, 1.308997f, 0.416667f, 0.416667f, 0.166667f},
        {0.800000f, 6.283185f, 1.570796f, -1.570796f, 0.927295f, 1.570796f, 0.927295f, 6.283185f, 1.570796f, 0.416667f, 0.416667f, 0.166667f},
        {0.900000f, 0.000000f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 0.000000f, 0.000000f, 0.244521f, 0.244521f, 0.510958f},
        {0.900000f, 0.000000f, 0.261799f, -0.785398f, 0.235097f, 0.785398f, 0.235097f, 0.000000f, 0.261799f, 0.248273f, 0.248273f, 0.503454f},
        {0.900000f, 0.000000f, 0.523599f, -0.785398f, 0.466765f, 0.785398f, 0.466765f, 0.000000f, 0.523599f, 0.259855f, 0.259855f, 0.480291f},
        {0.900000f, 0.000000f, 0.785398f, -0.785398f, 0.689817f, 0.785398f, 0.689817f, 0.000000f, 0.785398f, 0.280161f, 0.280161f, 0.439678f},
        {0.900000f, 0.000000f, 1.047198f, -0.785398f, 0.893744f, 0.785398f, 0.893744f, 0.000000f, 1.047198f, 0.310093f, 0.310093f, 0.379814f},
        {0.900000f, 0.000000f, 1.308997f, -1.080013f, 1.053852f, 1.080013f, 1.053852f, 0.000000f, 1.308997f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 0.000000f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 0.000000f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 0.392699f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 0.392699f, 0.000000f, 0.130411f, 0.425127f, 0.444461f},
        {0.900000f, 0.392699f, 0.261799f, -0.785398f, 0.235097f, 0.785398f, 0.235097f, 0.392699f, 0.261799f, 0.134193f, 0.425512f, 0.440296f},
        {0.900000f, 0.392699f, 0.523599f, -0.785398f, 0.466765f, 0.785398f, 0.466765f, 0.392699f, 0.523599f, 0.148684f, 0.424501f, 0.426815f},
        {0.900000f, 0.392699f, 0.785398f, -0.785398f, 0.689817f, 0.785398f, 0.689817f, 0.392699f, 0.785398f, 0.177924f, 0.421120f, 0.400956f},
        {0.900000f, 0.392699f, 1.047198f, -0.845172f, 0.893744f, 0.845172f, 0.893744f, 0.392699f, 1.047198f, 0.218805f, 0.406195f, 0.375000f},
        {0.900000f, 0.392699f, 1.308997f, -1.120270f, 1.053852f, 1.120270f, 1.053852f, 0.392699f, 1.308997f, 0.262869f, 0.362131f, 0.375000f},
        {0.900000f, 0.392699f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 0.392699f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 0.785398f, 0.000000f, -0.880979f, 0.000000f, 0.880979f, 0.000000f, 0.785398f, 0.000000f, 0.073749f, 0.551251f, 0.375000f},
        {0.900000f, 0.785398f, 0.261799f, -0.886537f, 0.235097f, 0.886537f, 0.235097f, 0.785398f, 0.261799f, 0.077602f, 0.547398f, 0.375000f},
        {0.900000f, 0.785398f, 0.523599f, -0.905677f, 0.466765f, 0.905677f, 0.466765f, 0.785398f, 0.523599f, 0.088898f, 0.536102f, 0.375000f},
        {0.900000f, 0.785398f, 0.785398f, -0.947908f, 0.689817f, 0.947908f, 0.689817f, 0.785398f, 0.785398f, 0.113866f, 0.511134f, 0.375000f},
        {0.900000f, 0.785398f, 1.047198f, -1.038052f, 0.893744f, 1.038052f, 0.893744f, 0.785398f, 1.047198f, 0.156034f, 0.468966f, 0.375000f},
        {0.900000f, 0.785398f, 1.308997f, -1.231025f, 1.053852f, 1.231025f, 1.053852f, 0.785398f, 1.308997f, 0.221478f, 0.403522f, 0.375000f},
        {0.900000f, 0.785398f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 0.785398f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 1.178097f, 0.000000f, -1.219181f, 0.000000f, 1.219181f, 0.000000f, 1.178097f, 0.000000f, 0.049619f, 0.575381f, 0.375000f},
        {0.900000f, 1.178097f, 0.261799f, -1.221656f, 0.235097f, 1.221656f, 0.235097f, 1.178097f, 0.261799f, 0.053768f, 0.571232f, 0.375000f},
        {0.900000f, 1.178097f, 0.523599f, -1.230251f, 0.466765f, 1.230251f, 0.466765f, 1.178097f, 0.523599f, 0.061909f, 0.563091f, 0.375000f},
        {0.900000f, 1.178097f, 0.785398f, -1.249576f, 0.689817f, 1.249576f, 0.689817f, 1.178097f, 0.785398f, 0.081410f, 0.543590f, 0.375000f},
        {0.900000f, 1.178097f, 1.047198f, -1.292339f, 0.893744f, 1.292339f, 0.893744f, 1.178097f, 1.047198f, 0.124039f, 0.500961f, 0.375000f},
        {0.900000f, 1.178097f, 1.308997f, -1.389439f, 1.053852f, 1.389439f, 1.053852f, 1.178097f, 1.308997f, 0.199722f, 0.425278f, 0.375000f},
        {0.900000f, 1.178097f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 1.178097f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 1.570796f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 1.570796f, 0.000000f, 0.045416f, 0.579584f, 0.375000f},
        {0.900000f, 1.570796f, 0.261799f, -1.570796f, 0.235097f, 1.570796f, 0.235097f, 1.570796f, 0.261799f, 0.045416f, 0.579584f, 0.375000f},
        {0.900000f, 1.570796f, 0.523599f, -1.570796f, 0.466765f, 1.570796f, 0.466765f, 1.570796f, 0.523599f, 0.053768f, 0.571232f, 0.375000f},
        {0.900000f, 1.570796f, 0.785398f, -1.570796f, 0.689817f, 1.570796f, 0.689817f, 1.570796f, 0.785398f, 0.073749f, 0.551251f, 0.375000f},
        {0.900000f, 1.570796f, 1.047198f, -1.570796f, 0.893744f, 1.570796f, 0.893744f, 1.570796f, 1.047198f, 0.113866f, 0.511134f, 0.375000f},
        {0.900000f, 1.570796f, 1.308997f, -1.570796f, 1.053852f, 1.570796f, 1.053852f, 1.570796f, 1.308997f, 0.191319f, 0.433681f, 0.375000f},
        {0.900000f, 1.570796f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 1.570796f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 1.963495f, 0.000000f, -1.922412f, 0.000000f, 1.922412f, 0.000000f, 1.963495f, 0.000000f, 0.049619f, 0.575381f, 0.375000f},
        {0.900000f, 1.963495f, 0.261799f, -1.919936f, 0.235097f, 1.919936f, 0.235097f, 1.963495f, 0.261799f, 0.053768f, 0.571232f, 0.375000f},
        {0.900000f, 1.963495f, 0.523599f, -1.911341f, 0.466765f, 1.911341f, 0.466765f, 1.963495f, 0.523599f, 0.061909f, 0.563091f, 0.375000f},
        {0.900000f, 1.963495f, 0.785398f, -1.892017f, 0.689817f, 1.892017f, 0.689817f, 1.963495f, 0.785398f, 0.081410f, 0.543590f, 0.375000f},
        {0.900000f, 1.963495f, 1.047198f, -1.849254f, 0.893744f, 1.849254f, 0.893744f, 1.963495f, 1.047198f, 0.124039f, 0.500961f, 0.375000f},
        {0.900000f, 1.963495f, 1.308997f, -1.752154f, 1.053852f, 1.752154f, 1.053852f, 1.963495f, 1.308997f, 0.199722f, 0.425278f, 0.375000f},
        {0.900000f, 1.963495f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 1.963495f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 2.356194f, 0.000000f, -2.260613f, 0.000000f, 2.260613f, 0.000000f, 2.356194f, 0.000000f, 0.073749f, 0.551251f, 0.375000f},
        {0.900000f, 2.356194f, 0.261799f, -2.255056f, 0.235097f, 2.255056f, 0.235097f, 2.356194f, 0.261799f, 0.077602f, 0.547398f, 0.375000f},
        {0.900000f, 2.356194f, 0.523599f, -2.235916f, 0.466765f, 2.235916f, 0.466765f, 2.356194f, 0.523599f, 0.088898f, 0.536102f, 0.375000f},
        {0.900000f, 2.356194f, 0.785398f, -2.193684f, 0.689817f, 2.193684f, 0.689817f, 2.356194f, 0.785398f, 0.113866f, 0.511134f, 0.375000f},
        {0.900000f, 2.356194f, 1.047198f, -2.103541f, 0.893744f, 2.103541f, 0.893744f, 2.356194f, 1.047198f, 0.156034f, 0.468966f, 0.375000f},
        {0.900000f, 2.356194f, 1.308997f, -1.910567f, 1.053852f, 1.910567f, 1.053852f, 2.356194f, 1.308997f, 0.221478f, 0.403522f, 0.375000f},
        {0.900000f, 2.356194f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 2.356194f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 2.748894f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 2.748894f, 0.000000f, 0.130411f, 0.425127f, 0.444461f},
        {0.900000f, 2.748894f, 0.261799f, -2.356194f, 0.235097f, 2.356194f, 0.235097f, 2.748894f, 0.261799f, 0.134193f, 0.425512f, 0.440296f},
        {0.900000f, 2.748894f, 0.523599f, -2.356194f, 0.466765f, 2.356194f, 0.466765f, 2.748894f, 0.523599f, 0.148684f, 0.424501f, 0.426815f},
        {0.900000f, 2.748894f, 0.785398f, -2.356194f, 0.689817f, 2.356194f, 0.689817f, 2.748894f, 0.785398f, 0.177924f, 0.421120f, 0.400956f},
        {0.900000f, 2.748894f, 1.047198f, -2.296421f, 0.893744f, 2.296421f, 0.893744f, 2.748894f, 1.047198f, 0.218805f, 0.406195f, 0.375000f},
        {0.900000f, 2.748894f, 1.308997f, -2.021323f, 1.053852f, 2.021323f, 1.053852f, 2.748894f, 1.308997f, 0.262869f, 0.362131f, 0.375000f},
        {0.900000f, 2.748894f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 2.748894f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 3.141593f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 3.141593f, 0.000000f, 0.244521f, 0.244521f, 0.510958f},
        {0.900000f, 3.141593f, 0.261799f, -2.356194f, 0.235097f, 2.356194f, 0.235097f, 3.141593f, 0.261799f, 0.248273f, 0.248273f, 0.503454f},
        {0.900000f, 3.141593f, 0.523599f, -2.356194f, 0.466765f, 2.356194f, 0.466765f, 3.141593f, 0.523599f, 0.259855f, 0.259855f, 0.480291f},
        {0.900000f, 3.141593f, 0.785398f, -2.356194f, 0.689817f, 2.356194f, 0.689817f, 3.141593f, 0.785398f, 0.280161f, 0.280161f, 0.439678f},
        {0.900000f, 3.141593f, 1.047198f, -2.356194f, 0.893744f, 2.356194f, 0.893744f, 3.141593f, 1.047198f, 0.310093f, 0.310093f, 0.379814f},
        {0.900000f, 3.141593f, 1.308997f, -2.061579f, 1.053852f, 2.061579f, 1.053852f, 3.141593f, 1.308997f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 3.141593f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 3.141593f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 3.534292f, 0.000000f, -2.356194f, 0.000000f, 2.356194f, 0.000000f, 3.534292f, 0.000000f, 0.425127f, 0.130411f, 0.444461f},
        {0.900000f, 3.534292f, 0.261799f, -2.356194f, 0.235097f, 2.356194f, 0.235097f, 3.534292f, 0.261799f, 0.425512f, 0.134193f, 0.440296f},
        {0.900000f, 3.534292f, 0.523599f, -2.356194f, 0.466765f, 2.356194f, 0.466765f, 3.534292f, 0.523599f, 0.424501f, 0.148684f, 0.426815f},
        {0.900000f, 3.534292f, 0.785398f, -2.356194f, 0.689817f, 2.356194f, 0.689817f, 3.534292f, 0.785398f, 0.421120f, 0.177924f, 0.400956f},
        {0.900000f, 3.534292f, 1.047198f, -2.296421f, 0.893744f, 2.296421f, 0.893744f, 3.534292f, 1.047198f, 0.406195f, 0.218805f, 0.375000f},
        {0.900000f, 3.534292f, 1.308997f, -2.021323f, 1.053852f, 2.021323f, 1.053852f, 3.534292f, 1.308997f, 0.362131f, 0.262869f, 0.375000f},
        {0.900000f, 3.534292f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 3.534292f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 3.926991f, 0.000000f, -2.260613f, 0.000000f, 2.260613f, 0.000000f, 3.926991f, 0.000000f, 0.551251f, 0.073749f, 0.375000f},
        {0.900000f, 3.926991f, 0.261799f, -2.255056f, 0.235097f, 2.255056f, 0.235097f, 3.926991f, 0.261799f, 0.547398f, 0.077602f, 0.375000f},
        {0.900000f, 3.926991f, 0.523599f, -2.235916f, 0.466765f, 2.235916f, 0.466765f, 3.926991f, 0.523599f, 0.536102f, 0.088898f, 0.375000f},
        {0.900000f, 3.926991f, 0.785398f, -2.193684f, 0.689817f, 2.193684f, 0.689817f, 3.926991f, 0.785398f, 0.511134f, 0.113866f, 0.375000f},
        {0.900000f, 3.926991f, 1.047198f, -2.103541f, 0.893744f, 2.103541f, 0.893744f, 3.926991f, 1.047198f, 0.468966f, 0.156034f, 0.375000f},
        {0.900000f, 3.926991f, 1.308997f, -1.910567f, 1.053852f, 1.910567f, 1.053852f, 3.926991f, 1.308997f, 0.403522f, 0.221478f, 0.375000f},
        {0.900000f, 3.926991f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 3.926991f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 4.319690f, 0.000000f, -1.922412f, 0.000000f, 1.922412f, 0.000000f, 4.319690f, 0.000000f, 0.575381f, 0.049619f, 0.375000f},
        {0.900000f, 4.319690f, 0.261799f, -1.919936f, 0.235097f, 1.919936f, 0.235097f, 4.319690f, 0.261799f, 0.571232f, 0.053768f, 0.375000f},
        {0.900000f, 4.319690f, 0.523599f, -1.911341f, 0.466765f, 1.911341f, 0.466765f, 4.319690f, 0.523599f, 0.563091f, 0.061909f, 0.375000f},
        {0.900000f, 4.319690f, 0.785398f, -1.892017f, 0.689817f, 1.892017f, 0.689817f, 4.319690f, 0.785398f, 0.543590f, 0.081410f, 0.375000f},
        {0.900000f, 4.319690f, 1.047198f, -1.849254f, 0.893744f, 1.849254f, 0.893744f, 4.319690f, 1.047198f, 0.500961f, 0.124039f, 0.375000f},
        {0.900000f, 4.319690f, 1.308997f, -1.752154f, 1.053852f, 1.752154f, 1.053852f, 4.319690f, 1.308997f, 0.425278f, 0.199722f, 0.375000f},
        {0.900000f, 4.319690f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 4.319690f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 4.712389f, 0.000000f, -1.570796f, 0.000000f, 1.570796f, 0.000000f, 4.712389f, 0.000000f, 0.579584f, 0.045416f, 0.375000f},
        {0.900000f, 4.712389f, 0.261799f, -1.570796f, 0.235097f, 1.570796f, 0.235097f, 4.712389f, 0.261799f, 0.579584f, 0.045416f, 0.375000f},
        {0.900000f, 4.712389f, 0.523599f, -1.570796f, 0.466765f, 1.570796f, 0.466765f, 4.712389f, 0.523599f, 0.571232f, 0.053768f, 0.375000f},
        {0.900000f, 4.712389f, 0.785398f, -1.570796f, 0.689817f, 1.570796f, 0.689817f, 4.712389f, 0.785398f, 0.551251f, 0.073749f, 0.375000f},
        {0.900000f, 4.712389f, 1.047198f, -1.570796f, 0.893744f, 1.570796f, 0.893744f, 4.712389f, 1.047198f, 0.511134f, 0.113866f, 0.375000f},
        {0.900000f, 4.712389f, 1.308997f, -1.570796f, 1.053852f, 1.570796f, 1.053852f, 4.712389f, 1.308997f, 0.433681f, 0.191319f, 0.375000f},
        {0.900000f, 4.712389f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 4.712389f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 5.105088f, 0.000000f, -1.219181f, 0.000000f, 1.219181f, 0.000000f, 5.105088f, 0.000000f, 0.575381f, 0.049619f, 0.375000f},
        {0.900000f, 5.105088f, 0.261799f, -1.221656f, 0.235097f, 1.221656f, 0.235097f, 5.105088f, 0.261799f, 0.571232f, 0.053768f, 0.375000f},
        {0.900000f, 5.105088f, 0.523599f, -1.230251f, 0.466765f, 1.230251f, 0.466765f, 5.105088f, 0.523599f, 0.563091f, 0.061909f, 0.375000f},
        {0.900000f, 5.105088f, 0.785398f, -1.249576f, 0.689817f, 1.249576f, 0.689817f, 5.105088f, 0.785398f, 0.543590f, 0.081410f, 0.375000f},
        {0.900000f, 5.105088f, 1.047198f, -1.292339f, 0.893744f, 1.292339f, 0.893744f, 5.105088f, 1.047198f, 0.500961f, 0.124039f, 0.375000f},
        {0.900000f, 5.105088f, 1.308997f, -1.389439f, 1.053852f, 1.389439f, 1.053852f, 5.105088f, 1.308997f, 0.425278f, 0.199722f, 0.375000f},
        {0.900000f, 5.105088f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 5.105088f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 5.497787f, 0.000000f, -0.880979f, 0.000000f, 0.880979f, 0.000000f, 5.497787f, 0.000000f, 0.551251f, 0.073749f, 0.375000f},
        {0.900000f, 5.497787f, 0.261799f, -0.886537f, 0.235097f, 0.886537f, 0.235097f, 5.497787f, 0.261799f, 0.547398f, 0.077602f, 0.375000f},
        {0.900000f, 5.497787f, 0.523599f, -0.905677f, 0.466765f, 0.905677f, 0.466765f, 5.497787f, 0.523599f, 0.536102f, 0.088898f, 0.375000f},
        {0.900000f, 5.497787f, 0.785398f, -0.947908f, 0.689817f, 0.947908f, 0.689817f, 5.497787f, 0.785398f, 0.511134f, 0.113866f, 0.375000f},
        {0.900000f, 5.497787f, 1.047198f, -1.038052f, 0.893744f, 1.038052f, 0.893744f, 5.497787f, 1.047198f, 0.468966f, 0.156034f, 0.375000f},
        {0.900000f, 5.497787f, 1.308997f, -1.231025f, 1.053852f, 1.231025f, 1.053852f, 5.497787f, 1.308997f, 0.403522f, 0.221478f, 0.375000f},
        {0.900000f, 5.497787f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 5.497787f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 5.890486f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 5.890486f, 0.000000f, 0.425128f, 0.130411f, 0.444461f},
        {0.900000f, 5.890486f, 0.261799f, -0.785398f, 0.235097f, 0.785398f, 0.235097f, 5.890486f, 0.261799f, 0.425512f, 0.134193f, 0.440296f},
        {0.900000f, 5.890486f, 0.523599f, -0.785398f, 0.466765f, 0.785398f, 0.466765f, 5.890486f, 0.523599f, 0.424501f, 0.148684f, 0.426815f},
        {0.900000f, 5.890486f, 0.785398f, -0.785398f, 0.689817f, 0.785398f, 0.689817f, 5.890486f, 0.785398f, 0.421120f, 0.177924f, 0.400956f},
        {0.900000f, 5.890486f, 1.047198f, -0.845172f, 0.893744f, 0.845172f, 0.893744f, 5.890486f, 1.047198f, 0.406195f, 0.218805f, 0.375000f},
        {0.900000f, 5.890486f, 1.308997f, -1.120270f, 1.053852f, 1.120270f, 1.053852f, 5.890486f, 1.308997f, 0.362131f, 0.262869f, 0.375000f},
        {0.900000f, 5.890486f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 5.890486f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 6.283185f, 0.000000f, -0.785398f, 0.000000f, 0.785398f, 0.000000f, 6.283185f, 0.000000f, 0.244521f, 0.244521f, 0.510958f},
        {0.900000f, 6.283185f, 0.261799f, -0.785398f, 0.235097f, 0.785398f, 0.235097f, 6.283185f, 0.261799f, 0.248273f, 0.248273f, 0.503454f},
        {0.900000f, 6.283185f, 0.523599f, -0.785398f, 0.466765f, 0.785398f, 0.466765f, 6.283185f, 0.523599f, 0.259855f, 0.259855f, 0.480291f},
        {0.900000f, 6.283185f, 0.785398f, -0.785398f, 0.689817f, 0.785398f, 0.689817f, 6.283185f, 0.785398f, 0.280161f, 0.280161f, 0.439678f},
        {0.900000f, 6.283185f, 1.047198f, -0.785398f, 0.893744f, 0.785398f, 0.893744f, 6.283185f, 1.047198f, 0.310093f, 0.310093f, 0.379814f},
        {0.900000f, 6.283185f, 1.308997f, -1.080013f, 1.053852f, 1.080013f, 1.053852f, 6.283185f, 1.308997f, 0.312500f, 0.312500f, 0.375000f},
        {0.900000f, 6.283185f, 1.570796f, -1.570796f, 1.119770f, 1.570796f, 1.119770f, 6.283185f, 1.570796f, 0.312500f, 0.312500f, 0.375000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f},
        {1.000000f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 6.283185f, 1.570796f, 0.000000f, 0.000000f, 1.000000f}
    };
    
}
