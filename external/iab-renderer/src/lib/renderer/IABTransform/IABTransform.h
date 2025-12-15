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

#ifndef IABTRANSFORM_H_INCLUDED
#define IABTRANSFORM_H_INCLUDED

#include "renderer/IABTransform/IABTransformDataType.h"
#include "MDAToIABCoordCvtConsts.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    // Set of transform functions originally from creator plugin processor
    //
    class IABTransform
    {
    public:
        
        /// Default construction
        ///
        IABTransform();
        
        /// Destruction
        ///
        virtual ~IABTransform() {}
        
		/// Transforms a set of IAB object position values 
		/// to a set of VBAP position values in Polar format, as used in MDA renderer.
		///
		/// @param[in]  iX  IAB position in room coordinates 0 to +1
		/// @param[in]  iY  IAB position in room coordinates 0 to +1
		/// @param[in]  iZ  IAB position in room coordinates 0 to +1
		/// @param[out] oAzimuth   VBAP object azimuth in radians
		/// @param[out] oElevation VBAP object elevation in radians
		/// @param[out] oRadius    VBAP object radius, as converted and unconstrained
		///
		iabError TransformIABToSphericalVBAP(
			                       const IABValueX &iX
			                     , const IABValueY &iY
			                     , const IABValueZ &iZ
			                     , VBAPValueAzimuth &oAzimuth
			                     , VBAPValueElevation &oElevation
			                     , VBAPValueRadius &oRadius
			                     ) const;

		/// Transforms a set of IAB object position values 
		/// to a set of VBAP object position in Cartesian, using Pyra Mesa.
		///
		/// @param[in]  iX  IAB position in room coordinates 0 to +1
		/// @param[in]  iY  IAB position in room coordinates 0 to +1
		/// @param[in]  iZ  IAB position in room coordinates 0 to +1
		/// @param[out] oX  Output (transformed) VBAP object position X, range [-1 1]
		/// @param[out] oY  Output (transformed) VBAP object position Y, range [-1 1]
		/// @param[out] oZ  Output (transformed) VBAP object position Z, range [-1 1]
		///
		iabError TransformIABToCartesianVBAP(
			                       const IABValueX &iX
			                     , const IABValueY &iY
			                     , const IABValueZ &iZ
			                     , PyraMesaValueX &oX
			                     , PyraMesaValueY &oY
			                     , PyraMesaValueZ &oZ
			                     ) const;

		/// Transforms IAB 1D spread to VBAP extent parameter set of aperture and divergence. 
		///
		/// Note: Current IAB spread (size) trandform algorithm is based on empirical test data
		/// from MDA development. For 1D spread, output divergence is locked to 0. Inclusion of
		/// "oDivergence" in API to support future improvements.
		///
		/// @param[in]  iSpreadXYZ  IAB 1D spread, same value for each of x, y, z dimension. range [0 1]
		/// @param[out] oAperture  Output (transformed) VBAP aperture, in radian, range [0 Pi]
		/// @param[out] oDivergence  Output (transformed) VBAP divengence, in radian, range [0 Pi/2]
		///
		iabError TransformIAB1DSpreadToVBAPExtent(
			const float &iSpreadXYZ
			, float &oAperture
			, float &oDivergence
			) const;
        
        /// Transforms a set of VBAP speaker position values in Cartesian format
        /// to a set of IAB position values.
        ///
        /// @param[in] iVBAPX VBAP speaker x position in normalized room coordinates
        /// @param[in] iVBAPY VBAP speaker y position in normalized room coordinates
        /// @param[in] iVBAPZ VBAP speaker z position in normalized room coordinates
        /// @param[out]  oIABX  IAB x position in room coordinates 0 to +1
        /// @param[out]  oIABY  IAB y position in room coordinates 0 to +1
        /// @param[out]  oIABZ  IAB z position in room coordinates 0 to +1
        ///
        iabError TransformCartesianVBAPToIAB(
                                             const PyraMesaValueX &iVBAPX
                                             , const PyraMesaValueX &iVBAPY
                                             , const PyraMesaValueX &iVBAPZ
                                             , IABValueX &oIABX
                                             , IABValueY &oIABY
                                             , IABValueZ &oIABZ
                                             ) const;

	private:
        
        // Initialize shoebox transform members
        //
        void initShoeboxTransform();
        
        // Initialize MDA to IAB conversion tables
        //
        void initMDAToIABConversionTables();
        
        // Shoebox transform algorithm, to generate position outputs with a unconstrained MDA radius value.
        //
        void shoeboxTransform(float frontPos, float frPos, float zPos,
                              float &azimuth, float &elevation, float &radius) const;

		// Translate (x y z) coordinate from IAB origin (left-front corner) to room-centre
		// origin (Reference Listener Position).
		//
		//  Input (xIn yIn zIn):     IAB coordinate
	    //                           Range: [0 1] (for each dimension)
		//  Output(xOut yOut zOut):  Translated coordinates in ref to room center origin
		//                           Range [-1 1] (for each dimension)
		//
		void toRoomCenterOrigin(float xIn, float yIn, float zIn, float &xOut, float &yOut, float &zOut) const;

		// Translate 1D spread to VBAP aperture
		//
		//  Input iSpreadXYZ:        IAB 1d spread
		//                           Range: [0 1] (for each dimension, same value)
		//  Output oAperture:        Translated aperture
		//                           Range [0 Pi]
		//
		void spread1dToAperture(const float &iSpreadXYZ, float &oAperture) const;



		// ===== SUPPORTING METHODS ======
        
        static void mInvert(const float mIn[2][2], float mOut[2][2]);
        static void mInvert(const float mIn[3][3], float mOut[3][3]);
        static inline void vMult(const float a, const float vIn[2], float vOut[2]);
        static inline void vAdd(const float vIn1[2], const float vIn2[2], float vOut[2]);
        static inline void mMult(const float mIn[2][2], const float vIn[2], float vOut[2]);
        static inline void mMult(const float mIn[3][3], const float vIn[3], float vOut[3]);
        static inline void mMult(const float mIn1[3][3], const float mIn2[3][3], float mOut[3][3]);
        
        // Normalized room corner locations
        //
        enum
        {
            c_roomCornerLeftFrontBottom = 0,
            c_roomCornerRightFrontBottom,
            c_roomCornerLeftRearBottom,
            c_roomCornerRightRearBottom,
            
            c_roomCornerLeftFrontTop,       // NOTE: Transform algorithm assumes that top speakers are in same order as bottom speakers
            c_roomCornerRightFrontTop,
            c_roomCornerLeftRearTop,
            c_roomCornerRightRearTop,
            
            c_numRoomCornerCoordinates
        };
        
        // Normalized room speaker locations
        //
        enum
        {
            c_roomLeftFrontSpeakerBottom = 0,
            c_roomRightFrontSpeakerBottom,
            c_roomLeftRearSpeakerBottom,
            c_roomRightRearSpeakerBottom,
            
            c_roomLeftFrontSideSpeakerBottom,
            c_roomRightFrontSideSpeakerBottom,
            c_roomLeftRearSideSpeakerBottom,
            c_roomRightRearSideSpeakerBottom,
            
            c_roomLeftFrontSpeakerTop,      // NOTE: Transform algorithm assumes that top speakers are in same order as bottom speakers
            c_roomRightFrontSpeakerTop,
            c_roomLeftRearSpeakerTop,
            c_roomRightRearSpeakerTop,
            
            c_roomLeftFrontSideSpeakerTop,
            c_roomRightFrontSideSpeakerTop,
            c_roomLeftRearSideSpeakerTop,
            c_roomRightRearSideSpeakerTop,
            
            c_numRoomSpeakerCoordinates
        };
        
        struct
        {
            float       roomCorners[c_numRoomCornerCoordinates][3];               // x, y, z coordinates of room corners
            float       roomSpeakers[c_numRoomSpeakerCoordinates][3];             // x, y, z coordinates of speakers
            float       roomSpeakersMidPlane[c_numRoomSpeakerCoordinates][3];     // x, y, z coorindates of plane where z = 50
            float       roomListenerLocation;                                     // Listener position from front of room, where room depth is 2.0
            
            float       yFrontSideSpeaker;                                        // Y (PT) position of front side speaker
            float       yRearSideSpeaker;                                         // Y (PT) position of rear side speaker
        }
        m_roomCoordinates;
        
        // Old gcc compiler (gcc4.1.2 used on RH5.11) does not support array initialisation with Matrix3
        // The basis matrices are saved in multi-dimentional float arrays tables in mda_to_iab_coord_cvt_consts.h and actual matrices are then created by the IABTransform class during
        // initialisation, see initMDAToIABConversionTables()
        
        // Inverse of the pyra-mesa patch basis matrix in the MDA coordinate system
        CoreUtils::Matrix3 mda_inv_basis[num_patches];
        
        // Pyra-mesa patch basis matrix in the IAB coordinate system
        CoreUtils::Matrix3 iab_basis[num_patches];
        
    }; // class IABTransform

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif
