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

#include <cstring>

#include "common/IABConstants.h"
#include "renderer/IABTransform/IABTransform.h"
#include "coreutils/CoreDefines.h"


namespace SMPTE
{
namespace ImmersiveAudioBitstream
{

    // ==========================================================
    // CreatorTransform
    // ----------------
    
    // ------------------------------------------------------------

    /// Normalized room listener position, at center of room
    /// Distance from front of room to listener, if room had depth of 2.0 (+/- 1.0)
    ///
    const float c_normRoomListenerLocation = 1.0;
    
    /// Number of speakers in the normalized room struct for the floor (1/2 of total struct elements - other half is ceiling)
    ///
    const int32_t c_normRoomSpeakersFloor = 8;
    
    // ------------------------------------------------------------
    IABTransform::IABTransform()
    {
		initShoeboxTransform();
        
        initMDAToIABConversionTables();
    }

    // ------------------------------------------------------------
    void IABTransform::initShoeboxTransform()
    {
        // Normalized room corner locations, measured from listener position
        const float c_normRoomCornerCoordinates[c_numRoomCornerCoordinates][2] =    // azimuth, elevation. In same order as t_roomCornerCoordinates
        {
			// Order the room corners as 
			// Left_Front_Low -> Right_Front_Low -> Left_Rear_Low -> Right_Rear_Low -> 
			// Left_Front_High -> Right_Front_High -> Left_Rear_High -> Right_Rear_High

			{-37.5f,      0.0f},          // room corners
            {37.5f,       0.0f},
            {-142.5f,     0.0f},
            {142.5f,      0.0f},
            {-37.5f,      25.97f},
            {37.5f,       25.97f},
            {-142.5f,     25.97f},
            {142.5f,      25.97f}
        };
        
        // Normalized room speaker locations for theatrical room model, measured from listener position
        const float c_normRoomSpeakerCoordinatesTheatrical[c_numRoomSpeakerCoordinates][2] =    // azimuth, elevation. In same order as t_roomSpeakerCoordinates
        {
			// Speakers, not corners
			// Left_Front -> Right_Front -> Left_Rear -> Right_Rear

			{-30.0f,        0.0f},          // speaker boundaries, z = 0
            {30.0f,         0.0f},
            {-150.0f,       0.0f},
            {150.0f,        0.0f},
            
			// Left_Side_Front -> Right_Side_Front -> Left_Side_Rear -> Right_Side_Rear

			{-45.0f,        0.0f},          // speaker side wall boundaries, z = 0
            {45.0f,         0.0f},
            {-135.0f,       0.0f},
            {135.0f,        0.0f},
            
			// Left_Front_Height -> Right_Front_Height -> Left_Rear_Height -> Right_Rear_Height
			// Assuming z = 100?

			{-24.79f,     35.99f},         // NOTE: Algorithm assumes that top speakers are in same order as bottom speakers
            {24.79f,      35.99f},
            {-155.21f,    35.99f},
            {155.21f,     35.99f},

			// For side Height. Same as above, as noted below?

            {-24.79f,     35.99f},         // speaker side wall boundaries, z = 100, same locations as ceiling corner speaker
            {24.79f,      35.99f},
            {-155.21f,    35.99f},
            {155.21f,     35.99f}
        };
        
        // Normalized room speaker locations for mid-height (z=50) plane of theatrical room model, measured from listener position
        // Set elevation of mid-plane so that middle of left and right walls is mapped to height speaker (+/- 90 azimuth, 30 elevation) <--- (Elevation of 21 or 22.2 shown, 1 set @ azimuth = 30, another @45)
        const float c_normRoomSpeakerCoordinatesTheatricalMidPlane[c_numRoomSpeakerCoordinates][2] =    // azimuth, elevation. In same order as t_roomSpeakerCoordinates
        {
            {-30.0f,        0.0f},          // speaker boundaries for floor, z = 0
            {30.0f,         0.0f},
            {-150.0f,       0.0f},
            {150.0f,        0.0f},
            
            {-45.0f,        0.0f},          // speaker side wall boundaries, z = 0
            {45.0f,         0.0f},
            {-135.0f,       0.0f},
            {135.0f,        0.0f},
            
            
            {-30.0f,        21.0f},         // speaker boundaries for mid-plane (z=50) locations
            {30.0f,         21.0f},
            {-150.0f,       21.0f},
            {150.0f,        21.0f},
            
            {-45.0f,        22.2f},         // speaker side wall boundaries, z = 50
            {45.0f,         22.2f},
            {-135.0f,       22.2f},
            {135.0f,        22.2f}
        };
        
        int32_t     i;
        float       cx, cy, cz;
        float       rh;
        float       a[3][3];
        float       aInv[3][3];
        float       b[3];
        float       c[3];
        float       t;
        const float (*pSpeakerCoordinates)[c_numRoomSpeakerCoordinates][2];
        
		// For converting IAB object positions to coordinates in a normalized room,
		// (!!ideally), the conversion process should be "target speaker layout" agnostic.
		// However, the tranform algorithm in MDACreator is clearly target-speaker-dependent.

		// pSpeakerCoordinates can set differently, to  1 of 4 pre-defined 16-speaker 
		// coordinates. As a result, multiple Transform Models can be selected at renderer
		// creation (run-time) in MDACreator.
		//
		// For IAB purpose, the speaker locations for shoebax transform is based on 
		// Theatrical7.1 normalized room
		pSpeakerCoordinates = &c_normRoomSpeakerCoordinatesTheatrical;

        // Set location of listener in room
        m_roomCoordinates.roomListenerLocation = c_normRoomListenerLocation;
        
        // Set y position (in PT coordinates) of front and rear side speakers
        m_roomCoordinates.yFrontSideSpeaker = 75;				// Alert: PT-based numbers, hard-coded
        m_roomCoordinates.yRearSideSpeaker = -75;
        
        // Compute Cartesian room corners using normalized room coordinates and listener position.
        for(i = 0; i < c_numRoomCornerCoordinates; i++)
        {
			// Corners in front half of room, including azimuth = +/-90 degree
            if (c_normRoomCornerCoordinates[i][0] <= 90.0 && c_normRoomCornerCoordinates[i][0] >= -90.0)
            {
                // In front of listener
                cy = m_roomCoordinates.roomListenerLocation;                                 // y = roomListenerLocation directly. (1.0, center line of room)
                cx = cy * tan(c_normRoomCornerCoordinates[i][0] * iabKPI / 180.0f);             // x = y * tan(azimuth)
            }
			else
			// Corners in back half of room, not including azimuth = +/-90 degree
            {
                // In back of listener
                cy = -(2.0f - m_roomCoordinates.roomListenerLocation);                       // y = -(length_of_room - roomListenerLocation), with length_of_room = 2.0. "-" sign as it's behind listener.
                cx = cy * tan(c_normRoomCornerCoordinates[i][0] * iabKPI / 180.0f);             // x = y * tan(azimuth). Sign of x is auto-set, based on azimuth in 3rd or 4th quadrant.
            }
            rh = sqrt(cx * cx + cy * cy);                                                    // rh: diagonal distance of [x y] rectangle
            cz = rh * tan(c_normRoomCornerCoordinates[i][1] * iabKPI / 180.0f);                 // z = rh * tan(elevation)
            
            m_roomCoordinates.roomCorners[i][0] = cx;
            m_roomCoordinates.roomCorners[i][1] = cy;
            m_roomCoordinates.roomCorners[i][2] = cz;
        }
        
        // Compute Cartersian coordinates of speaker locations of normalized room
        for(i = 0; i < c_numRoomSpeakerCoordinates; i++)
        {
            if ((*pSpeakerCoordinates)[i][0] <= 90.0 && (*pSpeakerCoordinates)[i][0] >= -90.0)             // The code below has a potential risk when azimuth = +/-90 degree, as tan(90/-90) = infinity. It seems that, by design, speakers @+/- 90 azimuth have been left out.
            {
                // In front of listener
                cy = m_roomCoordinates.roomListenerLocation;
                cx = cy * tan((*pSpeakerCoordinates)[i][0] * iabKPI / 180.0f);                 // TO-DO: Is this right in general? x can be way outside the wall for azimuth in range [45, 90] degree. The assumption that, "for every speakers, y = roomListenerLocation" surely is incorrect! Try azimuth = 60 degree with unit tests
            }
			else
            {
                // In back of listener
                cy = -(2.0f - m_roomCoordinates.roomListenerLocation);
                cx = cy * tan((*pSpeakerCoordinates)[i][0] * iabKPI / 180.0f);                 // TO-DO: Is this right in general? x can be way outside the wall for azimuth in range [90, 135] degree. The assumption that, "for every speakers, y = roomListenerLocation" surely is incorrect! Try azimuth = 110 degree
            }
            rh = sqrt(cx * cx + cy * cy);
            cz = rh * tan((*pSpeakerCoordinates)[i][1] * iabKPI / 180.0f);                     // TO-DO: This would be wrong when the x is wrong (knock-on from above.)
            
            // Speakers that are at height of listener are on the walls
            // Ceiling speakers may not be on the walls
            if ((*pSpeakerCoordinates)[i][1] > 0.0)                                         // TO-DO: Is this how speakers are "projected" back to room boundary?
            {
                // Intersect with ceiling to determine Cartesian coordinates
                
                // Line from 0, 0, 0 to cx, cy, cz
                a[0][0] = cx;
                a[1][0] = cy;
                a[2][0] = cz;
                
                // Plane of top room corners (only need 3 points to define the plane - using front left, front right, rear left)
                a[0][1] = m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][0] - m_roomCoordinates.roomCorners[c_roomCornerRightFrontTop][0];
                a[1][1] = m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][1] - m_roomCoordinates.roomCorners[c_roomCornerRightFrontTop][1];
                a[2][1] = m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][2] - m_roomCoordinates.roomCorners[c_roomCornerRightFrontTop][2];
                a[0][2] = m_roomCoordinates.roomCorners[c_roomCornerLeftRearTop][0] - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][0];
                a[1][2] = m_roomCoordinates.roomCorners[c_roomCornerLeftRearTop][1] - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][1];
                a[2][2] = m_roomCoordinates.roomCorners[c_roomCornerLeftRearTop][2] - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][2];
                
                // Invert and mulitply to intersect line and plane
                mInvert(a, aInv);
                b[0] = cx - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][0];
                b[1] = cy - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][1];
                b[2] = cz - m_roomCoordinates.roomCorners[c_roomCornerLeftFrontTop][2];
                mMult(aInv, b, c);
                t = c[0];
                cx *= (1 - t);
                cy *= (1 - t);
                cz *= (1 - t);
            }
            
            m_roomCoordinates.roomSpeakers[i][0] = cx;
            m_roomCoordinates.roomSpeakers[i][1] = cy;
            m_roomCoordinates.roomSpeakers[i][2] = cz;
        }
        
        // Calculate Cartesian location of mid-plane corners
        for(i = 0; i < c_numRoomSpeakerCoordinates; i++)
        {
            if (c_normRoomSpeakerCoordinatesTheatricalMidPlane[i][0] <= 90.0 && c_normRoomSpeakerCoordinatesTheatricalMidPlane[i][0] >= -90.0)
            {
                // In front of listener
                cy = m_roomCoordinates.roomListenerLocation;
                cx = cy * tan(c_normRoomSpeakerCoordinatesTheatricalMidPlane[i][0] * iabKPI / 180.0f);
            }
			else
            {
                // In back of listener
                cy = -(2.0f - m_roomCoordinates.roomListenerLocation);
                cx = cy * tan(c_normRoomSpeakerCoordinatesTheatricalMidPlane[i][0] * iabKPI / 180.0f);
            }
            rh = sqrt(cx * cx + cy * cy);
            cz = rh * tan(c_normRoomSpeakerCoordinatesTheatricalMidPlane[i][1] * iabKPI / 180.0f);
            
            // roomSpeakersMidPlane coordinates will represent lower half of room
            m_roomCoordinates.roomSpeakersMidPlane[i][0] = cx;
            m_roomCoordinates.roomSpeakersMidPlane[i][1] = cy;
            m_roomCoordinates.roomSpeakersMidPlane[i][2] = cz;
            
            // roomSpeakers coordinates will represent upper half of room
            if (i >= c_normRoomSpeakersFloor)
            {
                // Replace lower plane of upper half of room with mid-plane
                m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][0] = cx;
                m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][1] = cy;
                m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][2] = cz;
            }
        }
    }
    
    void IABTransform::initMDAToIABConversionTables()
    {
        // Initialise basis matrices
        
        for (int32_t i = 0; i < num_patches; i++)
        {
            CoreUtils::Matrix3 mdaBasis(mda_inv_basis_table[i]);
            CoreUtils::Matrix3 iabBasis(iab_basis_table[i]);
            mda_inv_basis[i] = mdaBasis;
            iab_basis[i] = iabBasis;
        }
    }


	// ------------------------------------------------------------
	//
	iabError IABTransform::TransformIABToSphericalVBAP(
                                                    const IABValueX &iX
                                                  , const IABValueY &iY
                                                  , const IABValueZ &iZ
                                                  , VBAPValueAzimuth &oAzimuth
                                                  , VBAPValueElevation &oElevation
                                                  , VBAPValueRadius &oRadius
                                                  ) const
    {
		// Check input range first
		//
		if (iX < 0.0 || iX > 1.0
			|| iY < 0.0 || iY > 1.0
			|| iZ < 0.0 || iZ > 1.0)
		{
			// out of IAB coordinate range
			return kIABGeneralError;
		}

		// Translate coordinate origin to center of room
		float xCenterOrig = 0.0;
		float yCenterOrig = 1.0;				// Init to front center
		float zCenterOrig = 0.0;

		toRoomCenterOrigin(iX, iY, iZ, xCenterOrig, yCenterOrig, zCenterOrig);

		// Note: shoeboxTransform(), as copied from MDACreator code base, current takes
		// (x y z) input with ProTools automation data range, ie [-100 100] range.
		// Hence, convert x/y/z range from [-1 1] to [-100 100]
		//
		xCenterOrig *= 100.0;
		yCenterOrig *= 100.0;
		zCenterOrig *= 100.0;

		// Work variables for output from shoeboxTransform()
        float azimuth = 0.0;
        float elevation = 0.0;
        float radius = 0.0;

        // Call Transform core algorithm, as-is
        //
        shoeboxTransform(
                           xCenterOrig
                         , yCenterOrig
                         , zCenterOrig
                         , azimuth
                         , elevation
                         , radius
                         );

        oAzimuth = azimuth;
        oElevation = elevation;
        oRadius = radius;

		return kIABNoError;
    }


	// ------------------------------------------------------------
	//
	iabError IABTransform::TransformIABToCartesianVBAP(
		                                            const IABValueX &iX
		                                          , const IABValueY &iY
		                                          , const IABValueZ &iZ
		                                          , PyraMesaValueX &oX
		                                          , PyraMesaValueY &oY
		                                          , PyraMesaValueZ &oZ
		                                          ) const
	{
		iabError rCode = kIABNoError;

		// Work variables for output from TransformIABToSphericalVBAP()
		float azimuth = 0.0;
		float elevation = 0.0;
		float radius = 0.0;

		rCode = TransformIABToSphericalVBAP(iX, iY, iZ, azimuth, elevation, radius);

		// Return if error
		if (kIABNoError != rCode)
			return rCode;

		// Convert Polar back to Cartesian
		oX = radius * cos(elevation) * sin(azimuth);
		oY = radius * cos(elevation) * cos(azimuth);
		oZ = radius * sin(elevation);

		return kIABNoError;
	}

	// ------------------------------------------------------------
	//
	iabError IABTransform::TransformIAB1DSpreadToVBAPExtent(
		const float &iSpreadXYZ
		, float &oAperture
		, float &oDivergence
		) const
	{
		// Check input range first
		//
		if (iSpreadXYZ < 0.0 || iSpreadXYZ > 1.0)
		{
			// out of IAB coordinate range
			return kIABGeneralError;
		}

		// Divergence is set to 0 always
		float divergence = 0.0;

		// Work variable for output from spread1dToAperture()
		float aperture = 0.0;

		// Call spread1dToAperture()
		//
		spread1dToAperture(
			iSpreadXYZ
			, aperture
			);

		oAperture = aperture;
		oDivergence = divergence;

		return kIABNoError;
	}
    
    // ------------------------------------------------------------
    //
    iabError IABTransform::TransformCartesianVBAPToIAB(
                                         const PyraMesaValueX &iVBAPX
                                         , const PyraMesaValueX &iVBAPY
                                         , const PyraMesaValueX &iVBAPZ
                                         , IABValueX &oIABX
                                         , IABValueY &oIABY
                                         , IABValueZ &oIABZ
                                         ) const
    {
        // The code in this function was originally extracted from Pierre's mda_iab_coord_cvt.cpp function mda_iab_coord_cvt()
        // Minor arg name change to align with TransformCartesianVBAPToIAB() parameter names, no other code change
        
        // Count the number of pyramesa patches the position intersects
        float count = 0.0;
        
        CoreUtils::Vector3 coefs(0, 0, 0);      // working coefficent vector for calculating IAB position
        
        CoreUtils::Vector3 iab_pos(0, 0, 0);    // working IAB position
        
        CoreUtils::Vector3 mda_pos(iVBAPX, iVBAPY, iVBAPZ); // input MDA position for conversion

        // Iterate over all pyra-mesa patches, looking for those that intersect with the MDA position
        for (int32_t i = 0; i < num_patches; i++)
        {
            
            // Compute the coordindate of the MDA position within the basis formed by the patch
            // (Conv_Doc) step: B = inv(M) . transpose(O)
            coefs = mda_inv_basis[i] * mda_pos;
            
            // Does the position intersect the patch, i.e. are all coordinates of the position positive within the patch's basis
            // (Conv_Doc) step: if B = inv(M) . transpose(O) < 0, skip to next patch
            if (coefs.getX() < -CoreUtils::kEPSILON || coefs.getY() < -CoreUtils::kEPSILON || coefs.getZ() < -CoreUtils::kEPSILON) continue;
            
            // Compute the number of the patch's basis vectors that contribute to the position
			int32_t numNonZeroCoeffs = ((coefs.getX() > CoreUtils::kEPSILON) + (coefs.getY() > CoreUtils::kEPSILON) + (coefs.getZ() > CoreUtils::kEPSILON));
            
            // Convert the position to the IAB coordinate system assuming that the coordinates in the patch's basis in the MDA space
            // are equal to the coordinates in the patch's basis in the IAB space
            // (Conv_Doc) step: C = G.B, where G = pre-computed iab_basis
            coefs = iab_basis[i] * coefs;
            
            // (Conv_Doc) step: D = (Vi.n/(C.n)) * C
            // iabPatchNormalVector[i])is the pre-computed normal vector to this patch in the cuboid coordinate system
            // Vi.n = iabBasisNormalProjection[i], note that this is a scalar value
            // C.n = coefs.dot(iabPatchNormalVector[i]), note that this is a scalar value
            // The position in the IAB space must be on the unit cube
            coefs = coefs / (coefs.dot(iabPatchNormalVector[i]) / iabBasisNormalProjection[i]);
            
            // Object position can:
            // 1. falls on a vertex : numNonZeroCoeffs = 1, it's done and no further patches need to be checked
            // 2. falls on boundary between two patches : numNonZeroCoeffs = 2, see code below
            // 3. falls on middle of the patch, numNonZeroCoeffs > 2, sum up coordinates and increment count
            //
            if (numNonZeroCoeffs == 2)
            {
                // If the position fell on the boundary between two patches, count the contribution only once
                // There should be one more patch, either already processed or coming up
                // P = P + D/2, count = count + 1/2
                iab_pos = iab_pos + (coefs / 2);
                count += 0.5;
            }
            else
            {
                // P = P + D, count = count + 1
                iab_pos = iab_pos + coefs;
                count++;
            }
            
            // If n = 1, then the position falls on a vertex, finish
            if (numNonZeroCoeffs == 1) break;
        }
        
        if (count == 0)
        {
            return kIABRendererCoordConversionError;
        }
        
        // Average results from the patches
        iab_pos = iab_pos / count;
        
        // Adjust the room-centered IAB position to the corner-relative ST 2098-2 position
        oIABX = (iab_pos.getX() + 1) / 2;
        oIABY = (-iab_pos.getY() + 1) / 2;
        oIABZ = iab_pos.getZ();
        
        // Limit converted values to range [0.0, 1.0]
        if (oIABX < 0.0f)
        {
            oIABX = 0.0f;
        }
        else if (oIABX > 1.0f)
        {
            oIABX = 1.0f;
        }
        
        if (oIABY < 0.0f)
        {
            oIABY = 0.0f;
        }
        else if (oIABY > 1.0f)
        {
            oIABY = 1.0f;
        }
        
        if (oIABZ < 0.0f)
        {
            oIABZ = 0.0f;
        }
        else if (oIABZ > 1.0f)
        {
            oIABZ = 1.0f;
        }
        
        return kIABNoError;
    }
    
    
    // ------------------------------------------------------------
	//
	void IABTransform::toRoomCenterOrigin(float xIn, float yIn, float zIn, float &xOut, float &yOut, float &zOut) const
	{
		// Shift origin from front-left corner to center of (z = 0) floor.
		// No change in height dimension, z.
		xOut = 2.0f * xIn - 1;
		yOut = 1 - 2.0f * yIn;
		zOut = zIn;
	}

	// ------------------------------------------------------------
	//
    void IABTransform::shoeboxTransform(float frontPos, float frPos, float zPos, float &azimuth, float &elevation, float &radius) const
    {
        float       x, y, z;
        float       x1, y1, z1;
        float       x2, y2;
        float       xi, yi, zi;
        float       r;
        float       rObj;
        int32_t     i;
        
        float       scaledSpeakerCoordinates[c_numRoomSpeakerCoordinates][3];      // x, y, z coordinates of speakers scaled to height of object
        
        // scales to range -1 to 1
        x = frontPos / 100.0f;
        y = frPos / 100.0f;
        z = zPos / 100.0f;
        
		// For IAB purpose, the transform is based on Theatrical7.1 normalized room.
		// 
		
		// Scale ceiling speaker coordinates down to height of object
        for(i = 0; i < c_numRoomSpeakerCoordinates; i++)
        {
            // Check for floor or ceiling speaker
            if (i < c_normRoomSpeakersFloor)
            {
				// Scale based on whether object is in lower or upper half of room
				if (z <= 0.5)
                {
                    // Lower half of room
                    scaledSpeakerCoordinates[i][0] = m_roomCoordinates.roomSpeakersMidPlane[i][0];
                    scaledSpeakerCoordinates[i][1] = m_roomCoordinates.roomSpeakersMidPlane[i][1];
                    scaledSpeakerCoordinates[i][2] = m_roomCoordinates.roomSpeakersMidPlane[i][2];
                }
                else
                {
                    // Upper half of room
                    scaledSpeakerCoordinates[i][0] = m_roomCoordinates.roomSpeakers[i][0];
                    scaledSpeakerCoordinates[i][1] = m_roomCoordinates.roomSpeakers[i][1];
                    scaledSpeakerCoordinates[i][2] = m_roomCoordinates.roomSpeakers[i][2];
                }
            }
			else
            {
                // Scale based on whether object is in lower or upper half of room
                if (z <= 0.5)
                {
                    scaledSpeakerCoordinates[i][0] = m_roomCoordinates.roomSpeakersMidPlane[i][0] +
                    ((0.5f - z) / 0.5f) * (m_roomCoordinates.roomSpeakersMidPlane[i - c_normRoomSpeakersFloor][0] - m_roomCoordinates.roomSpeakersMidPlane[i][0]);
                    scaledSpeakerCoordinates[i][1] = m_roomCoordinates.roomSpeakersMidPlane[i][1] +
                    ((0.5f - z) / 0.5f) * (m_roomCoordinates.roomSpeakersMidPlane[i - c_normRoomSpeakersFloor][1] - m_roomCoordinates.roomSpeakersMidPlane[i][1]);
                    scaledSpeakerCoordinates[i][2] = m_roomCoordinates.roomSpeakersMidPlane[i][2] * (z / 0.5f);
                }
				else
                {
                    scaledSpeakerCoordinates[i][0] = m_roomCoordinates.roomSpeakers[i][0] +
                    ((1.0f - z) / 0.5f) * (m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][0] - m_roomCoordinates.roomSpeakers[i][0]);
                    scaledSpeakerCoordinates[i][1] = m_roomCoordinates.roomSpeakers[i][1] +
                    ((1.0f - z) / 0.5f) * (m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][1] - m_roomCoordinates.roomSpeakers[i][1]);
                    scaledSpeakerCoordinates[i][2] = m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][2] +
                    (m_roomCoordinates.roomSpeakers[i][2] - m_roomCoordinates.roomSpeakers[i - c_normRoomSpeakersFloor][2]) * ((z - 0.5f) / 0.5f);
                }
            }
        }
        
        // Scale x and y to plane defined by roomSpeakerCoordinates
        // Assumes room is left/right symmetric
        if (y >= 0)
        {
            // Determine if object is in front of side speaker wedge (line from side speaker to listener)
            float slope = 0.0;
            bool sinf = false;
            float xabs = fabs(x);
            if (xabs != 0)
                slope = y / xabs;
            else
                sinf = true;
            
            // Determine slope of side speaker to listener and scale to corner as z approaches ceiling
            float ySide = m_roomCoordinates.yFrontSideSpeaker / 100.0f;
            if (z > 0.5)
            {
                ySide = ySide + (1.0f - ySide) * ((z - 0.5f) * 2);
            }
            
            float ys1 = scaledSpeakerCoordinates[c_roomRightFrontSideSpeakerTop][1];
            float xs1 = scaledSpeakerCoordinates[c_roomRightFrontSideSpeakerTop][0];
            float ys2 = scaledSpeakerCoordinates[c_roomRightFrontSpeakerTop][1];
            float xs2 = scaledSpeakerCoordinates[c_roomRightFrontSpeakerTop][0];
            
            // Set listener z coordinate
            if ((slope >= 1.0) || sinf)
            {
                y1 = y * ys2;
                x1 = x * xs2;
                
                z1 = scaledSpeakerCoordinates[c_roomRightFrontSpeakerTop][2];
            }
            else if (slope >= ySide)
            {
                // Object is in front of line from side speaker to listener
                
                y1 = xabs * ys2;
                
                if (1.0 - ySide != 0.0)
                {
                    x1 = x * xs2 + (x * (1 - slope)) * ((xs1 - xs2) / (1.0f - ySide));
                    
                    z1 = ((slope - ySide) / (1.0f - ySide)) * scaledSpeakerCoordinates[c_roomRightFrontSpeakerTop][2] +
                    (1 - ((slope - ySide) / (1.0f - ySide))) * scaledSpeakerCoordinates[c_roomRightFrontSideSpeakerTop][2];
                }
                else
                {
                    x1 = x * xs1;
                    
                    z1 = scaledSpeakerCoordinates[c_roomRightFrontSpeakerTop][2];
                }
            }
            else
            {
                // Object is behind line from side speaker to listener (but in front of listener)
                y1 = (y / ySide) * ys1;
                x1 = x * xs1;
                z1 = scaledSpeakerCoordinates[c_roomRightFrontSideSpeakerTop][2];
            }
        }
        else
        {
            // Determine if object is behind side speaker wedge (line from side speaker to listener)
            float slope = 0.0;
            bool sinf = false;
            float xabs = fabs(x);
            if (xabs > kEPSILON)
                slope = -y / xabs;
            else
                sinf = true;
            
            // Determine slope of side speaker to listener and scale to corner as z approaches ceiling
            float ySide = -m_roomCoordinates.yRearSideSpeaker / 100.0f;
            if (z > 0.5)
                ySide = ySide + (1.0f - ySide) * ((z - 0.5f) * 2);
            
            float ys1 = -scaledSpeakerCoordinates[c_roomRightRearSideSpeakerTop][1];
            float xs1 = scaledSpeakerCoordinates[c_roomRightRearSideSpeakerTop][0];
            float ys2 = -scaledSpeakerCoordinates[c_roomRightRearSpeakerTop][1];
            float xs2 = scaledSpeakerCoordinates[c_roomRightRearSpeakerTop][0];
            
            if ((slope >= 1.0) || sinf)
            {
                y1 = y * ys2;
                x1 = x * xs2;
                z1 = scaledSpeakerCoordinates[c_roomRightRearSpeakerTop][2];
            }
            else if (slope >= ySide)
            {
                // Object is behind the line from side speaker to listener
                
                y1 = xabs * -ys2;
                if (1.0 - ySide != 0.0)
                {
                    x1 = x * xs2 + (x * (1 - slope)) * ((xs1 - xs2) / (1.0f - ySide));
                    
                    z1 = ((slope - ySide) / (1.0f - ySide)) * scaledSpeakerCoordinates[c_roomRightRearSpeakerTop][2] +
                    (1 - ((slope - ySide) / (1.0f - ySide))) * scaledSpeakerCoordinates[c_roomRightRearSideSpeakerTop][2];
                }
                else
                {
                    x1 = x * xs1;
                    z1 = scaledSpeakerCoordinates[c_roomRightRearSideSpeakerTop][2];
                }
            }
            else
            {
                // Object is in front of line from side speaker to listener (but behind the listener)
                y1 = (y / ySide) * ys1;
                x1 = x * xs1;
                z1 = scaledSpeakerCoordinates[c_roomRightRearSideSpeakerTop][2];
            }
        }
        
        r = sqrt(x1*x1 + y1*y1 + z1*z1);
        if (r != 0.0)
        {
            elevation = asin(z1 / r);
            azimuth = atan2(x1, y1);
            
            if (elevation < 0.0)
                elevation = 0.0;
        }
        else
        {
            elevation = 0.0;
            azimuth = 0.0;
        }
        
        // Compute radius by first determining the intersection from the listener through the object to the edge of the box
        // Move point to first quadrant
        x2 = x;
        if (x2 < 0)
            x2 = -x2;
        y2 = y;
        if (y2 < 0)
            y2 = -y2;
        
        // PACT-418 - XYZ == 100, radius == 1
        // 
        // Check if intersection is on the ceiling
        // x2 and y2 are in first quadrant
        //
        bool ceiling = false;
        if (z > 0)
        {
            // Check if intersection of listener to object vector with plane of ceiling is within ceiling bounds
            if (x2 / z < 1.0 && y2 / z < 1.0)
                ceiling = true;
        }

        if (ceiling)
        {
            // Intersect with ceiling
            xi = x2 / z;
            yi = y2 / z;
            zi = 1.0;
        }
        else
        {
            if (x2 > y2)
            {
                // Intersect with right wall
                if (x2 != 0)
                {
                    xi = 1.0;
                    yi = y2 / x2;
                    zi = z / x2;
                }
                else
                {
                    // Intersect at right mid-point
                    xi = 1.0;
                    yi = 0.0;
                    zi = 0.0;
                }
            }
            else
            {
                // Intersect with front wall
                if (y2 != 0)
                {
                    xi = x2 / y2;
                    yi = 1.0;
                    zi = z / y2;
                }
                else
                {
                    // Intersect at front mid-point
                    xi = 0.0;
                    yi = 1.0;
                    zi = 0.0;
                }
            }
        }
        
        // Compute length of wall intersection and ratio with length to object
        r = sqrt(xi * xi + yi * yi + zi * zi);
        rObj = sqrt(x2 * x2 + y2 * y2 + z * z);
        if (r != 0.0)
        {
            radius = rObj / r;
        }
        else
        {
            radius = 0.0;
        }
    }

	// ------------------------------------------------------------
	//
	void IABTransform::spread1dToAperture(const float &iSpreadXYZ, float &oAperture) const
	{
		float aperture = 0.0;

		// 1D spread tarnsform algorithm based on MDA
		// Output in radian
		aperture = ((iSpreadXYZ < 0.5) ? (iSpreadXYZ / 2.0f) : (iSpreadXYZ / 2.0f + (iSpreadXYZ - 0.5f))) * iabKPI;

		oAperture = aperture;
	}
    
    // ------------------------------------------------------------
    // Invert a 2x2 matrix
    void IABTransform::mInvert(const float mIn[2][2], float mOut[2][2])
    {
        float       p1;
        float       p2;
        float       det;
        
        p1 = mIn[0][0] * mIn[1][1];
        p2 = mIn[0][1] * mIn[1][0];
        
        memset(mOut, 0, sizeof (float[2][2]));
        
        if (p1 != p2)
        {
            det = p1 - p2;
            mOut[0][0] = mIn[1][1] / det;
            mOut[0][1] = -mIn[0][1] / det;
            mOut[1][0] = -mIn[1][0] / det;
            mOut[1][1] = mIn[0][0] / det;
        }
    }
    
    // ------------------------------------------------------------
    // Invert a 3x3 matrix
    void IABTransform::mInvert(const float mIn[3][3], float mOut[3][3])
    {
        float       det;
        int32_t     i;
        int32_t     j1;
        int32_t     j2;
        
        det = mIn[0][0] * mIn[1][1] * mIn[2][2] +
        mIn[1][0] * mIn[2][1] * mIn[0][2] +
        mIn[2][0] * mIn[0][1] * mIn[1][2] -
        mIn[0][0] * mIn[2][1] * mIn[1][2] -
        mIn[2][0] * mIn[1][1] * mIn[0][2] -
        mIn[1][0] * mIn[0][1] * mIn[2][2];
        
        if (det >= kEPSILON)
        {
            for(i = 0; i < 3; i++)
            {
                j1 = (1 + i) % 3;
                j2 = (2 + i) % 3;
                
                mOut[i][0] = (mIn[1][j1] * mIn[2][j2] - mIn[1][j2] * mIn[2][j1]) / det;
                mOut[i][1] = (mIn[0][j2] * mIn[2][j1] - mIn[0][j1] * mIn[2][j2]) / det;
                mOut[i][2] = (mIn[0][j1] * mIn[1][j2] - mIn[0][j2] * mIn[1][j1]) / det;
            }
        }
    }
    
    // ------------------------------------------------------------
    // Multiple scalar by vector
    inline void IABTransform::vMult(const float a, const float vIn[2], float vOut[2])
    {
        vOut[0] = a * vIn[0];
        vOut[1] = a * vIn[1];
    }
    
    // ------------------------------------------------------------
    // Add two vectors
    inline void IABTransform::vAdd(const float vIn1[2], const float vIn2[2], float vOut[2])
    {
        vOut[0] = vIn1[0] + vIn2[0];
        vOut[1] = vIn1[1] + vIn2[1];
    }
    
    // ------------------------------------------------------------
    // Multiply 2x2 matrix by 2d vector
    inline void IABTransform::mMult(const float mIn[2][2], const float vIn[2], float vOut[2])
    {
        vOut[0] = mIn[0][0] * vIn[0] + mIn[0][1] * vIn[1];
        vOut[1] = mIn[1][0] * vIn[0] + mIn[1][1] * vIn[1];
    }
    
    // ------------------------------------------------------------
    // Multiply 3x3 matrix by 3d vector
    inline void IABTransform::mMult(const float mIn[3][3], const float vIn[3], float vOut[3])
    {
        vOut[0] = mIn[0][0] * vIn[0] + mIn[0][1] * vIn[1] + mIn[0][2] * vIn[2];
        vOut[1] = mIn[1][0] * vIn[0] + mIn[1][1] * vIn[1] + mIn[1][2] * vIn[2];
        vOut[2] = mIn[2][0] * vIn[0] + mIn[2][1] * vIn[1] + mIn[2][2] * vIn[2];
    }
    
    // ------------------------------------------------------------
    inline void IABTransform::mMult(const float mIn1[3][3], const float mIn2[3][3], float mOut[3][3])
    {
        int32_t     i;
        int32_t     j;
        
        for(i = 0; i < 3; i++)
        {
            for(j = 0; j < 3; j++)
            {
                mOut[i][j] = mIn1[i][0] * mIn2[0][j] +
                mIn1[i][1] * mIn2[1][j] +
                mIn1[i][2] * mIn2[2][j];
            }
        }
    }
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
