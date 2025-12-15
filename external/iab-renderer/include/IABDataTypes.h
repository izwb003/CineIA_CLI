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
 * Header file for common IAB data types.
 *
 * @file
 */


#ifndef __IABDATATYPES_H__
#define	__IABDATATYPES_H__

#include <stdint.h>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <set>

#include "IABErrors.h"

#if __linux__ || __APPLE__
#define MT_RENDERER_ENABLED
#endif

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /*****************************************************************************
     *
     * Types
     *
     *****************************************************************************/

    /**
     * Data type for error codes returned by API calls.
     *
     */
    typedef int32_t iabError;

	/**
	 * Represents a 1-bit unsigned integer
     *
	 */
	typedef uint8_t uint1_t;

	/**
	 * Represents a 2-bit unsigned integer
     *
	 */
	typedef uint8_t uint2_t;

	/**
	* Maximum value of a uint2_t value
    *
	*/
	const int32_t UINT2_MAX = (1 << 2) - 1;

	/**
	* Represents a 3-bit unsigned integer
    *
	*/
	typedef uint8_t uint3_t;

	/**
	* Represents a 4-bit unsigned integer
    *
	*/
	typedef uint8_t uint4_t;

	/**
	* Maximum value of a uint4_t value
    *
	*/
	const int32_t UINT4_MAX = (1 << 4) - 1;

	/**
	* Represents a 5-bit unsigned integer
    *
	*/
	typedef uint8_t uint5_t;

	/**
	* Maximum value of a uint5_t value
    *
	*/
	const int32_t UINT5_MAX = (1 << 5) - 1;

	/**
	* Represents a 10-bit unsigned integer
    *
	*/
	typedef uint16_t uint10_t;

	/**
	* Maximum value of a uint10_t value
    *
	*/
	const int32_t UINT10_MAX = (1 << 10) - 1;

	/**
	* Data type for the 32-bit parameter subframe length.
	*
	*/
	typedef uint32_t IABSubframeLengthType;

	/**
     * Data type for the Plex(8)-coded parameter "elementSize_",
     * element payload size in byte.
     *
     * This is typedef'ed to uint64_t, to prevent any overflow caused by,
     * for example, a very large frame element.
     *
     */
    typedef uint64_t IABElementSizeType;

    /**
     * Data type for the Plex(8)-coded parameter "maxRendered_".
     *
     */
    typedef uint32_t IABMaxRenderedRangeType;

    /**
     * Data type for the Plex(8)-coded parameter "subElementCount_".
     *
     */
    typedef uint32_t IABElementCountType;

    /**
     * Data type for the Plex(4)-coded parameter "channelCount_".
     *
     */
    typedef uint16_t IABChannelCountType;

    /**
     * Data type for the Plex(8)-coded parameter "audioDataID_".
     *
     */
    typedef uint32_t IABAudioDataIDType;

    /**
     * Data type for the Plex(8)-coded parameter "metadataID_".
     *
     */
    typedef uint32_t IABMetadataIDType;
    
    /**
     * Data type for the 8-bit parameter bitstream version number
     *
     */
    typedef uint8_t IABVersionNumberType;

    /**
      * Represents the API version.
      *
      */
    typedef struct IABAPIVersionType
    {
        uint32_t	fHigh_;					/**< High version number. */
        uint32_t	fLow_;					/**< Low version number. */
    } IABAPIVersionType;
    
    /**
     * Data type for 16-bit parameter rendered output channel count
     *
     */
    typedef uint16_t IABRenderedOutputChannelCountType;

    /**
     * Data type for 32-bit parameter rendered output sample count
     *
     */
    typedef uint32_t IABRenderedOutputSampleCountType;

    /**
     * Data type for float parameter IAB rendered output sample type
     *
     */
    typedef float IABSampleType;
    
	/**
	* @enum IABContainerSubFrameTagType
	*
	* @brief Enumerate currently defined IAB container SubFrame tages, per beginning of Section 8.
	*
	*/
	enum IABContainerSubFrameTagType
	{
		kIABContainerSubFrameTag_Preamble = 0x01,				/**< Identifies "Preamble" subframe */
		kIABContainerSubFrameTag_IAFrame = 0x02					/**< Identifies "IAFrame" subframe */
	};

	/**
     * @enum IABElementIDType
     *
     * @brief Enumerate currently defined IAB element IDs of table 13.
     *
     */
    enum IABElementIDType
    {
        kIABElementID_IAFrame				= 0x08,				/**< Identifies "Frame Header" element */
        kIABElementID_BedDefinition			= 0x10,				/**< Identifies "Bed Definition" element */
        kIABElementID_BedRemap				= 0x20,				/**< Identifies "Bed Remap" element */
        kIABElementID_ObjectDefinition		= 0x40,				/**< Identifies "Object Definition" element */
        kIABElementID_ObjectZoneDefinition19= 0x80,				/**< Identifies "Extended object 19-zone definition" element */
		kIABElementID_AuthoringToolInfo     = 0x100,			/**< Identifies "Authoring Tool Information" element */
		kIABElementID_UserData              = 0x101,			/**< Identifies "User defined data" element */
		kIABElementID_AudioDataDLC			= 0x200,			/**< Identifies "Audio Data (DLC encoded)" element */
        kIABElementID_AudioDataPCM			= 0x400				/**< Identifies "Audio Data PCM" element */
    };

    /**
     * @enum IABSampleRateType
     *
     * @brief Enumerate defined codes for the supported audio sampling rates, table 4.
     *
     */
    enum IABSampleRateType
    {
        kIABSampleRate_48000Hz		= 0x0,				/**< Identifies "48000 samples per second" sampling rate */
        kIABSampleRate_96000Hz		= 0x1				/**< Identifies "96000 samples per second" sampling rate */
    };

    /**
     * @enum IABBitDepthType
     *
     * @brief Enumerate defined codes for the supported audio bit widths, table 5.
     *
     */
    enum IABBitDepthType
    {
        kIABBitDepth_16Bit			= 0x0,				/**< Identifies "16 bits per audio sample" */
        kIABBitDepth_24Bit			= 0x1				/**< Identifies "24 bits per audio sample" */
    };

    /**
     * @enum IABAssetEncodingType
     *
     * @brief Enumerate defined codes for the supported asset encoding types.
     *
     */
    enum IABAssetEncodingType
    {
        kIABAssetEncodingPCM		= 0x0,
        kIABAssetEncodingDLC		= 0x1,
        kIABNoAssetFound            = 0x2
    };

    /**
     * @enum IABFrameRateType
	 *
     * @brief Enumerate defined codes for the supported IAB frame rates, table 6.
     *
     */
    enum IABFrameRateType
    {
        kIABFrameRate_24FPS			= 0x0,				/**< Identifies "24 frames per second" */
        kIABFrameRate_25FPS			= 0x1,				/**< Identifies "25 frames per second" */
        kIABFrameRate_30FPS			= 0x2,				/**< Identifies "30 frames per second" */
        kIABFrameRate_48FPS			= 0x3,				/**< Identifies "48 frames per second" */
        kIABFrameRate_50FPS			= 0x4,				/**< Identifies "50 frames per second" */
        kIABFrameRate_60FPS			= 0x5,				/**< Identifies "60 frames per second" */
        kIABFrameRate_96FPS			= 0x6,				/**< Identifies "96 frames per second" */
        kIABFrameRate_100FPS		= 0x7,				/**< Identifies "100 frames per second" */
        kIABFrameRate_120FPS		= 0x8,				/**< Identifies "120 frames per second" */
		kIABFrameRate_23_976FPS		= 0x9,				/**< Identifies "23.976 frames per second" */
        
        kIABFrameRate_BoundReserved						/**< Identifies upper bound (including this one) to undefined frame rates. */
	};

    /**
     * @enum IABUseCaseType
     *
     * @brief Enumerate defined use case codes for the supported soundfield/speaker formats/configs, table 1.
     * Speakers are listed as in "front/surround/top" form.
     *
     */
    enum IABUseCaseType
    {
		kIABUseCase_5_1				= 0x01,				/**< Identifies "3/2/0 (5.1)" */
        kIABUseCase_7_1_DS			= 0x02,				/**< Identifies "3/4/0 (7.1DS)" */
        kIABUseCase_7_1_SDS			= 0x03,				/**< Identifies "5/2/0 (7.1SDS in ST 428-12 (annex C) Reference" */
        kIABUseCase_11_1_HT			= 0x04,				/**< Identifies "6/4/1" */
        kIABUseCase_13_1_HT			= 0x05,				/**< Identifies "6/6/1" */
        kIABUseCase_9_1_OH			= 0x06,				/**< Identifies "3/4/2" */

        kIABUseCase_2_0_ITUA		= 0x30,				/**< Identifies ITU-R BS.2051-2 Speaker Layout A, "2/0/0" */
        kIABUseCase_5_1_4_ITUD		= 0x31,				/**< Identifies ITU-R BS.2051-2 Speaker Layout D, "3/2/4" */
        kIABUseCase_7_1_4_ITUJ		= 0x32,				/**< Identifies ITU-R BS.2051-2 Speaker Layout J, "3/4/4" */

        kIABUseCase_Always			= 0xFF,				/**< Identifies "Always Use" */

		kIABUseCase_NoUseCase       = 0xFFFF			/**< Identifies "unmatched target soundfield". Coding method used for capturing an unmatched soundfield from IAB render config file to an IABUseCaseType enum value. Not a SMPTE use case. */
	};

    /**
     * @enum IABChannelIDType
     *
     * @brief Enumerate defined codes for the supported channel IDs, table 8.
     *
     */
    enum IABChannelIDType 
    {
        kIABChannelID_Left					    = 0x0,				/**< Identifies "Left channel" */
        kIABChannelID_LeftCenter				= 0x1,				/**< Identifies "Left Center channel" */
        kIABChannelID_Center					= 0x2,				/**< Identifies "Center channel" */
        kIABChannelID_RightCenter				= 0x3,				/**< Identifies "Right Center channel" */
        kIABChannelID_Right				    	= 0x4,				/**< Identifies "Right channel" */
		kIABChannelID_LeftSideSurround          = 0x5,				/**< Identifies "Left Side Surround channel" */
		kIABChannelID_LeftSurround		    	= 0x6,				/**< Identifies "Left Surround channel" */
        kIABChannelID_LeftRearSurround	    	= 0x7,				/**< Identifies "Left Rear Surround channel" */
        kIABChannelID_RightRearSurround		    = 0x8,				/**< Identifies "Right Rear Surround channel" */
		kIABChannelID_RightSideSurround         = 0x9,				/**< Identifies "Right Side Surround channel" */
		kIABChannelID_RightSurround			    = 0xA,				/**< Identifies "Right Surround channel" */
        kIABChannelID_LeftTopSurround			= 0xB,				/**< Identifies "Left Top Surround channel" */
        kIABChannelID_RightTopSurround	    	= 0xC,				/**< Identifies "Right Top Surround channel" */
        kIABChannelID_LFE						= 0xD,				/**< Identifies "LFE channel" */
        kIABChannelID_LeftHeight				= 0xE,				/**< Identifies "Left Height channel" */
        kIABChannelID_RightHeight				= 0xF,				/**< Identifies "Right Height channel" */
        kIABChannelID_CenterHeight		    	= 0x10,				/**< Identifies "Center Height channel" */
        kIABChannelID_LeftSurroundHeight		= 0x11,				/**< Identifies "Left Surround Height channel" */
        kIABChannelID_RightSurroundHeight		= 0x12,				/**< Identifies "Right Surround Height channel" */
        kIABChannelID_LeftSideSurroundHeight	= 0x13,				/**< Identifies "Left Side Surround Height channel" */
        kIABChannelID_RightSideSurroundHeight	= 0x14,				/**< Identifies "Right Side Surround Height channel" */
        kIABChannelID_LeftRearSurroundHeight	= 0x15,				/**< Identifies "Left Rear Surround Height channel" */
        kIABChannelID_RightRearSurroundHeight	= 0x16,				/**< Identifies "Right Rear Surround Height channel" */
        kIABChannelID_TopSurround				= 0x17,				/**< Identifies "Top Surround channel" */

		kIABChannelID_DCReserveBegin			= 0x18,				/**< Identifies "start of "Reserved for D-Cinema" range" */
		kIABChannelID_DCReserveEnd				= 0x7F,				/**< Identifies "end of "Reserved for D-Cinema" range" */

		kIABChannelID_LeftTopFront				= 0x80,				/**< Identifies "Left Top Front, ITU-R BS.2051-2" */
		kIABChannelID_RightTopFront				= 0x81,				/**< Identifies "Right Top Front, ITU-R BS.2051-2" */
		kIABChannelID_LeftTopBack				= 0x82,				/**< Identifies "Left Top Back, ITU-R BS.2051-2" */
		kIABChannelID_RightTopBack				= 0x83,				/**< Identifies "Right Top Back, ITU-R BS.2051-2" */
		kIABChannelID_TopSideLeft				= 0x84,				/**< Identifies "Top Side Left, ITU-R BS.2051-2" */
		kIABChannelID_TopSideRight				= 0x85,				/**< Identifies "Top Side Right, ITU-R BS.2051-2" */
		kIABChannelID_LFE1						= 0x86,				/**< Identifies "LFE1, ITU-R BS.2051-2" */
		kIABChannelID_LFE2						= 0x87,				/**< Identifies "LFE2, ITU-R BS.2051-2" */
		kIABChannelID_FrontLeft					= 0x88,				/**< Identifies "Front Left, ITU-R BS.2051-2" */
		kIABChannelID_FrontRight				= 0x89				/**< Identifies "Front Right, ITU-R BS.2051-2" */
	};

    /**
     * @enum IABGainPrefixType
     *
     * @brief Enumerate defined codes for the supported IAB gain prefix, table 9.
     *
     */
    enum IABGainPrefixType
    {
        kIABGainPrefix_Unity				= 0x0,				/**< Identifies "Set gain to 1.0" */
        kIABGainPrefix_Silence				= 0x1,				/**< Identifies "Set gain to 0.0" */
        kIABGainPrefix_InStream		    	= 0x2				/**< Identifies "Gain Code follows in the bitstream. Set gain based on Gain Code." */
    };

    /**
     * @enum IABZoneGainPrefixType
     *
     * @brief Enumerate defined codes for the supported IAB Zone gain prefix, table 14.
     *
     */
    enum IABZoneGainPrefixType
    {
        kIABZoneGainPrefix_Silence			= 0x0,				/**< Identifies "Set zone gain to 0.0" */
        kIABZoneGainPrefix_Unity			= 0x1,				/**< Identifies "Set zone gain to 1.0" */
        kIABZoneGainPrefix_InStream	    	= 0x2				/**< Identifies "ZoneGain Code follows in the bitstream. Set zone gain based on ZoneGain Code." */
    };

    /**
     * @enum IABDecorCoeffPrefixType
     *
     * @brief Enumerate defined codes for the supported IAB decorrelation coefficient prefix, tables 10 (channel) & 16 (object).
     *
     */
    enum IABDecorCoeffPrefixType
    {
        kIABDecorCoeffPrefix_NoDecor				= 0x0,				/**< Identifies "No decorrelation" */
        kIABDecorCoeffPrefix_MaxDecor				= 0x1,				/**< Identifies "Maximum decorrelation" */
        kIABDecorCoeffPrefix_DecorCoeffInStream		= 0x2				/**< Identifies "Decorrelation coefficient follows in the bitstream." */
    };

    /**
     * @enum IABAudioDescriptionType
     *
     * @brief Enumerate defined codes for the supported IAB audio description, table 11.
     *
     */
    enum IABAudioDescriptionType
    {
        kIABAudioDescription_NotIndicated	= 0x01,				/**< Identifies "Not Indicated" */
        kIABAudioDescription_Dialog		    = 0x02,				/**< Identifies "Dialog" */
        kIABAudioDescription_Music		    = 0x04,				/**< Identifies "Music" */
        kIABAudioDescription_Effects		= 0x08,				/**< Identifies "Effects" */
        kIABAudioDescription_Foley		    = 0x10,				/**< Identifies "Foley" */
        kIABAudioDescription_Ambience		= 0x20,				/**< Identifies "Ambience" */
        kIABAudioDescription_Reserved		= 0x40,				/**< Identifies "Reserved" */
        kIABAudioDescription_TextInStream	= 0x80				/**< Identifies "Additional Information transmitted as a NULL terminated text string" */
    };

    /**
     * @enum IABSpreadModeType
     *
     * @brief Enumerate defined codes for the supported IAB object spread mode, table 15.
     *
     */
    enum IABSpreadModeType
    {
        kIABSpreadMode_LowResolution_1D 	= 0x0,				/**< Identifies "Equal spreading in each dimension using 8-bit coding" */
		kIABSpreadMode_None                 = 0x1,				/**< Identifies "Point source (no ObjectSpread value is sent)" */
		kIABSpreadMode_HighResolution_1D	= 0x2,				/**< Identifies "Equal spreading in each dimension using 12-bit coding" */
        kIABSpreadMode_HighResolution_3D	= 0x3				/**< Identifies "Specified spreading in each dimension (using 12-bit coding)" */
    };

    /**
     * @enum IABObjectZone9Type
     *
     * @brief Enumerate object 9-zone definition in listed order of table 13.
     *
     */
    enum IABObjectZone9Type
    {
        kIABObjectZone9_LeftScreen = 0,							/**< Identifies "All screen speakers left of center" */
        kIABObjectZone9_CenterScreen = 1,						/**< Identifies "Screen center speakers" */
        kIABObjectZone9_RightScreen = 2,						/**< Identifies "All screen speakers right of center" */
        kIABObjectZone9_LeftWall = 3,							/**< Identifies "All speakers on left wall" */
        kIABObjectZone9_RightWall = 4,							/**< Identifies "All speakers on right wall" */
        kIABObjectZone9_LeftRearWall = 5,						/**< Identifies "All speakers on left half of rear wall" */
        kIABObjectZone9_RightRearWall = 6,						/**< Identifies "All speakers on right half of rear wall" */
        kIABObjectZone9_LeftCeiling = 7,						/**< Identifies "All overhead speakers left of center" */
        kIABObjectZone9_RightCeiling = 8						/**< Identifies "All overhead speakers right of center" */
    };

    /**
     * @enum IABObjectZone19Type
     *
     * @brief Enumerate object 19-zone definition in listed order of table 17.
     *
     */
    enum IABObjectZone19Type
    {
        kIABObjectZone19_LeftScreenMain = 0,					/**< Identifies "Main screen speakers left of center" */
        kIABObjectZone19_CenterScreenMain = 1,					/**< Identifies "Main center screen speakers" */
        kIABObjectZone19_RightScreenMain = 2,					/**< Identifies "Main screen speakers right of center" */
        kIABObjectZone19_LeftScreenHeight = 3,					/**< Identifies "Height screen speakers left of center" */
        kIABObjectZone19_CenterScreenHeight = 4,				/**< Identifies "Height center screen speakers" */
        kIABObjectZone19_RightScreenHeight = 5,					/**< Identifies "Height screen speakers right of center" */
        kIABObjectZone19_LeftBackWallMain = 6,					/**< Identifies "Main back wall speakers left of center" */
        kIABObjectZone19_CenterBackWallMain = 7,				/**< Identifies "Main center back wall speakers" */
        kIABObjectZone19_RightBackWallMain = 8,					/**< Identifies "Main back wall speakers right of center" */
        kIABObjectZone19_LeftBackWallHeight = 9,				/**< Identifies "Height back wall speakers left of center" */
        kIABObjectZone19_CenterBackWallHeight = 10,				/**< Identifies "Height center back wall speakers" */
        kIABObjectZone19_RightBackWallHeight = 11,				/**< Identifies "Height back wall speakers right of center" */
        kIABObjectZone19_LeftWallMain = 12,						/**< Identifies "Main left wall speakers" */
        kIABObjectZone19_LeftWallHeight = 13,					/**< Identifies "Height left wall speakers" */
        kIABObjectZone19_RightWallMain = 14,					/**< Identifies "Main right wall speakers" */
        kIABObjectZone19_RightWallHeight = 15,					/**< Identifies "Height right wall speakers" */
        kIABObjectZone19_LeftOfCeiling = 16,					/**< Identifies "Ceiling speakers left of center" */
        kIABObjectZone19_CenterCeiling = 17,					/**< Identifies "Center ceiling speakers" */
        kIABObjectZone19_RightOfCeiling = 18					/**< Identifies "Ceiling speakers right of center" */
    };

	/**
     * Represents a cartesian position within a unit cube space, including surfaces
     * The origin (0, 0, 0) is set to front left lower corner.
     *
     */
    class CartesianPosInUnitCube
    {
	public:

		CartesianPosInUnitCube() : x_(0.0f), y_(0.0f), z_(0.0f), positionInStream() { setIABObjectPosition(x_, y_, z_); }

		iabError setIABObjectPosition(float iX, float iY, float iZ)
		{
			// Any of the x, y, z dimension must be within the cubic box room.
			if (iX < 0.0f || iX > 1.0f ||
				iY < 0.0f || iY > 1.0f ||
				iZ < 0.0f || iZ > 1.0f )
			{
				return kIABBadArgumentsError;				// At least one of the x, y, z is out of range
			}

			// In range, set.
			x_ = iX;
			y_ = iY;
			z_ = iZ;

            uint16_t x = static_cast<uint16_t>(floor(x_ * 32768.0f + 0.5f) + 32767);	// Convert _x using defined relative distance coding
            uint16_t y = static_cast<uint16_t>(floor(y_ * 32768.0f + 0.5f) + 32767);	// Convert _y using defined relative distance coding
            uint16_t z = static_cast<uint16_t>(floor(z_ * 65535.0f + 0.5f));			// Convert _z using defined relative distance coding

			// Update member variable positionInStream 
			positionInStream.setIABPosition(x, y, z);

			return kIABNoError;
		}
        
        iabError setIABObjectPosition(uint16_t iX, uint16_t iY, uint16_t iZ)
        {
            // Any of the x, y, z dimension must be within the cubic box room.
            if ((iX < 32767) || (iY < 32767))
            {
                return kIABBadArgumentsError;				// At least one of the x, y, z is out of range
            }
            
            // In range, set.
            // Update member variable positionInStream
            positionInStream.setIABPosition(iX, iY, iZ);

            // To do: review recommendation: "should be expressed using integer arithmetic, e.g. x/32768 = x >> 15"
            
            x_ = static_cast<float>((iX - 32767)/32768.0f);		// Convert iX using defined relative distance coding
            y_ = static_cast<float>((iY - 32767)/32768.0f);		// Convert iY using defined relative distance coding
            z_ = static_cast<float>(iZ/65536.0f);               // Convert iZ using defined relative distance coding
            
            return kIABNoError;
        }


		void getIABObjectPosition(float &oX, float &oY, float &oZ) const
		{
			oX = x_;
			oY = y_;
			oZ = z_;
		}

		uint16_t getStreamPosX() const
		{
			return positionInStream.x_;
		}

		uint16_t getStreamPosY() const
		{
			return positionInStream.y_;
		}

		uint16_t getStreamPosZ() const
		{
			return positionInStream.z_;
		}
        
        bool operator== (const CartesianPosInUnitCube &iAnotherPosition) const
        {
            // Compare bitstream values only since the translation  from float to bitstream value and back may give a different value
            
            if ((this->positionInStream.x_ == iAnotherPosition.positionInStream.x_) &&
                (this->positionInStream.y_ == iAnotherPosition.positionInStream.y_) &&
                (this->positionInStream.z_ == iAnotherPosition.positionInStream.z_))
            {
                return true;
            }
            
            return false;
        }

	private:

		float x_;							/**< lateral: left/right position; x_ = 0 corresponds to left wall; x_ = 1 corresponds to right wall. */
		float y_;							/**< longitude: front/back position; y_ = 0 corresponds to front wall; y_ = 1 corresponds to back wall. */
		float z_;							/**< elevation: down/up position; z_ = 0 corresponds to a horizontal plane at of the height of the main screen loudspeakers, the side and rear surround loudspeakers; z_ = 1 corresponds to the ceiling. */

		/**
		* This is a private structure.
		*
		* Represents an IAB object position in bitstream, cartesian coordinate.
		* Mapping from unit cube values to stream values are defined in section 6.4.
		*
		*/
		struct IABObjectPosition
		{
			uint16_t x_;						/**< lateral/left-right, n-bit, mapping to [0, 1] */
			uint16_t y_;						/**< longitude/front-back, n-bit, mapping to [0, 1] */
			uint16_t z_;						/**< elevation/down-up position; n-bit, mapping to [0, 1] */

			IABObjectPosition() : x_(0), y_(0), z_(0) {}

			iabError setIABPosition(uint16_t iX, uint16_t iY, uint16_t iZ)
			{
				// For n = 16-bit, IAB specifies ranger of [32767 65535] for iX or iY, and [0 65535] for iZ
				//
				if ((iX < 32767) || (iY < 32767))
				{
					return kIABBadArgumentsError;				// At least one of the x, y, z is out of range
				}

				// In range, set.
				x_ = iX;
				y_ = iY;
				z_ = iZ;

				return kIABNoError;
			}

			void getIABPosition(uint16_t &oX, uint16_t &oY, uint16_t &oZ) const
			{
				oX = x_;
				oY = y_;
				oZ = z_;
			}
		} positionInStream;
    };

    /**
     * Represents gain and gain coding method. Gain mapping is defined in section 6.5
     * for when (gainPrefix_ == 0x2).
     *
     */
    class IABGain
    {
    
    public:
        IABGain() : gainPrefix_(kIABGainPrefix_Unity), gain_(1.0f), gainInStream_(0) {}
        
        iabError setIABGain(float iGain)
        {
            // Ensure gain is within range
            if ((iGain < 0.0f) || (iGain > 1.0f))
            {
                return kIABBadArgumentsError;
            }
            
            gain_ = iGain;

            // Update member variable gainInStream_
            if (0 == gain_)
            {
                gainInStream_ = 0x3FF;   // 0.0f maps to 0x3FF
                gainPrefix_ = kIABGainPrefix_Silence;

            }
            else if (1.0 == gain_)
            {
                gainInStream_ = 0;      // 1.0f maps to 0
                gainPrefix_ = kIABGainPrefix_Unity;
            }
            else
            {
                // 0 < gain < 1.0: -64 * log2(Gain)
                gainInStream_ = static_cast<uint16_t>(floor(-64 * log2(iGain) + 0.5f));
                gainPrefix_ = kIABGainPrefix_InStream;
            }
            
            return kIABNoError;
        }
        
        iabError setIABGainFromStreamValue(uint16_t iGainInStream)
        {
            // Ensure iGainInStream is within range
            if (iGainInStream > 0x3FF)
            {
                return kIABBadArgumentsError;
            }

            gainInStream_ = iGainInStream;

            // Update member variables
            if (0x3FF == iGainInStream)
            {
                gain_ = 0.0f;
                gainPrefix_ = kIABGainPrefix_Silence;
            }
            else if (0 == iGainInStream)
            {
                gain_ = 1.0f;
                gainPrefix_ = kIABGainPrefix_Unity;
            }
            else
            {
                // 0 < iGainInStream < 0x3FF, gain = 2 ^ -(G/64.0)
                gain_ = std::pow(2.0f, -(iGainInStream/64.0f));
                gainPrefix_ = kIABGainPrefix_InStream;
            }
            
            return kIABNoError;
        }

        
        float getIABGain() const
        {
            return gain_;
        }
        
		IABGainPrefixType getIABGainPrefix() const
		{
			return gainPrefix_;
		}

		uint16_t getIABGainInStreamValue() const
		{
			return gainInStream_;
		}
        
        bool operator== (const IABGain &iAnotherGain) const
        {
            if ((this->gainPrefix_ == iAnotherGain.gainPrefix_) && (this->gainInStream_ == iAnotherGain.gainInStream_))
            {
                return true;
            }
            
            return false;
        }

	private:

        IABGainPrefixType gainPrefix_;		/**< 2-bit, method for coding the gain values. See table 9. */
        float gain_;						/**< 0.0 to 1.0 */
        uint16_t gainInStream_;				/**< 10-bit gain, when (gainPrefix_ == 0x2) */

        
    };

    /**
     * Represents zone gain and gain coding method. Gain mapping is defined in section 11.5.14, per v4.34 specification.
     * for when (gainPrefix_ == 0x2).
     *
     */
    
    class IABObjectZoneGain
    {
        
    public:
        IABObjectZoneGain() : gainPrefix_(kIABZoneGainPrefix_Silence), gain_(0.0f), gainInStream_(0) {}
        
		/**
		* Sets zone gain value.
		*
		* @param[in] iGain zone gain to set to, a linear scale factor in range of [0.0 1.0].
		* Note that due to quantization, the value set may be different from iGain.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		iabError setIABZoneGain(float iGain)
        {
            // Ensure gain is within range
            if ((iGain < 0.0f) || (iGain > 1.0f))
            {
                return kIABBadArgumentsError;
            }
            
            gain_ = iGain;
            
            // Update member variable gainInStream_
            if (0.0f == gain_)
            {
                gainInStream_ = 0x0;							// 0.0f maps to 0x000
                gainPrefix_ = kIABZoneGainPrefix_Silence;
            }
            else if (1.0f == gain_)
            {
                gainInStream_ = 0x3FF;							// 1.0f maps to 0x3FF
                gainPrefix_ = kIABZoneGainPrefix_Unity;
            }
            else
            {
                // 0 < gain < 1.0: gainInStream_ = iGain * 1023
                gainInStream_ = static_cast<uint16_t>(floor(iGain * 1023.0f + 0.5f));
				gain_ = gainInStream_ / 1023.0f;
                gainPrefix_ = kIABZoneGainPrefix_InStream;
            }
            
            return kIABNoError;
        }
        
        iabError setIABZoneGainFromStreamValue(uint16_t iGainInStream)
        {
            // Ensure gain is within range
            if ((iGainInStream > 0x3FF))
            {
                return kIABBadArgumentsError;
            }
            
            gainInStream_ = iGainInStream;
            
            // Update member variables
            if (0x3FF == iGainInStream)
            {
                gain_ = 1.0f;									// 1.0f maps to 0x3FF
                gainPrefix_ = kIABZoneGainPrefix_Unity;
            }
            else if (0 == iGainInStream)
            {
                gain_ = 0.0f;									// 0.0f maps to 0
                gainPrefix_ = kIABZoneGainPrefix_Silence;
            }
            else
            {
                // 0 < iGainInStream < 0x3FF, gain_ = iGainInStream / 1023.0f
                gainInStream_ = iGainInStream;
                gain_ = iGainInStream / 1023.0f;
                gainPrefix_ = kIABZoneGainPrefix_InStream;
            }
            
            return kIABNoError;
        }

        
        float getIABZoneGain() const
        {
            return gain_;
        }
        
		IABZoneGainPrefixType getIABZoneGainPrefix() const
		{
			return gainPrefix_;
		}

		uint16_t getIABZoneGainInStreamValue() const
		{
			return gainInStream_;
		}
        
        bool operator== (const IABObjectZoneGain &iAnotherZoneGain) const
        {
            // Compare bitstream values only since the translation  from float to bitstream value and back may give a different value
            
            if ((this->gainPrefix_ == iAnotherZoneGain.gainPrefix_) && (this->gainInStream_ == iAnotherZoneGain.gainInStream_))
            {
                return true;
            }
            
            return false;
        }


	private:
        
        IABZoneGainPrefixType gainPrefix_;		/**< 2-bit, method for coding the gain values. See table 14. */
        float gain_;							/**< 0.0 to 1.0 */
        uint16_t gainInStream_;					/**< 10-bit gain, when (gainPrefix_ == 0x2) */
        
        
    };

    /**
     * Represents zone control and a set of zone gains corresponding to 9-zone definition (table 13)
     *
     */
    struct IABObjectZoneGain9
    {
        uint8_t objectZoneControl_;			/**< 1-bit, indicating presence of zone control gains. */
        IABObjectZoneGain zoneGains_[9];	/**< An array of 9 elements of IABObjectZoneGain */
    };

    /**
     * Represents zone control and a set of zone gains corresponding to 19-zone definition (table 17)
     *
     */
    struct IABObjectZoneGain19
    {
        uint8_t objectZone19InfoExists_;	/**< 1-bit, indicating presence of 19-zone control gains. */
        IABObjectZoneGain zoneGains_[19];	/**< An array of 19 elements of IABObjectZoneGain */
    };

    /**
     * Represents object snap info presence, non-default tolerance presence, and tolerance value.
     *
     */
    struct IABObjectSnap
    {
        uint8_t objectSnapPresent_;			/**< 1-bit, indicates presence of additional snap info. */
        uint8_t objectSnapTolExists_;		/**< 1-bit, indicates presence of non-default snap tolerance value. */
        uint16_t objectSnapTolerance_;		/**< 12-bit, meaningful only when (objectSnapTolExists_ == 0x1 && objectSnapPresent_ == 0x1). */
        uint8_t reservedBit_;				/**< 1-bit, set to 0, in stream only when (objectSnapPresent_ == 0x1). */
    };

    /**
     * Represents object spread mode, and spread amount(s). Refer to table 15.
     * Resolution of spreadXYZ_ depends on spreadMode_.
     * spreadY_ and spreadZ_ are valid only when (spreadMode_ == 0x3).
     *
     */
    
    class IABObjectSpread
    {
    public:
        
        IABObjectSpread()
        {
            spreadMode_ = kIABSpreadMode_None;
            spreadXYZ_ = 0.0f;
            spreadY_ = 0.0f;
            spreadZ_ = 0.0f;
            spreadXYZInStream_ = 0;
            spreadYInStream_ = 0;
            spreadZInStream_ = 0;
        }
        
        iabError setIABObjectSpread(IABSpreadModeType iSpreadMode, float iSpreadXYZ, float iSpreadY, float iSpreadZ)
        {
            // Any of the x, y, z dimension must be within [0, 1]
            if (iSpreadXYZ < 0.0f || iSpreadXYZ > 1.0f ||
                iSpreadY < 0.0f || iSpreadY > 1.0f ||
                iSpreadZ < 0.0f || iSpreadZ > 1.0f )
            {
                return kIABBadArgumentsError;				// At least one of the x, y, z is out of range
            }
            
            spreadXYZ_ = iSpreadXYZ;
            spreadY_ = iSpreadY;
            spreadZ_ = iSpreadZ;
            
            if (iSpreadMode != kIABSpreadMode_HighResolution_3D)
            {
                // In 1D modes, spreadXYZ_ is used for all 3, make y_, z_ same as spreadXYZ_ for consistency
                spreadY_ = spreadXYZ_;
                spreadZ_ = spreadXYZ_;
            }
            
            if (iSpreadMode == kIABSpreadMode_LowResolution_1D)
            {
                // Convert spreadXYZ_ using defined relative distance coding, n = 8-bit. Rnage [0, 255]
                spreadXYZInStream_ = static_cast<uint16_t>(floor(spreadXYZ_ * 255.0f + 0.5f));
                
                // In low res 1D mode, only spreadXYZInStream_ is written to stream , make y & z same as spreadXYZInStream_ for consistency
                spreadYInStream_ = spreadXYZInStream_;
                spreadZInStream_ = spreadXYZInStream_;

            }
            else if (iSpreadMode == kIABSpreadMode_HighResolution_1D)
            {
                // Convert spreadXYZ_ using defined relative distance coding, n = 12-bit, range [0, 4095]
                spreadXYZInStream_ = static_cast<uint16_t>(floor(spreadXYZ_ * 4095.0f + 0.5f));

                // In Hi res 1D mode, only spreadXYZInStream_ is written to stream , make y & z same as spreadXYZInStream_ for consistency
                spreadYInStream_ = spreadXYZInStream_;
                spreadZInStream_ = spreadXYZInStream_;
            }
            else if (iSpreadMode == kIABSpreadMode_HighResolution_3D)
            {
                // Convert individudal spread values using defined relative distance coding, n = 12-bit, for x, y, z
                spreadXYZInStream_ = static_cast<uint16_t>(floor(spreadXYZ_ * 4095.0f + 0.5f));
                spreadYInStream_ = static_cast<uint16_t>(floor(spreadY_ * 4095.0f + 0.5f));
                spreadZInStream_ = static_cast<uint16_t>(floor(spreadZ_ * 4095.0f + 0.5f));
            }
			else if (iSpreadMode == kIABSpreadMode_None)
			{
				spreadXYZ_ = 0.0f;
				spreadY_ = 0.0f;
				spreadZ_ = 0.0f;
				spreadXYZInStream_ = 0;
				spreadYInStream_ = 0;
				spreadZInStream_ = 0;
			}
			else
			{
				return kIABBadArgumentsError;
			}
            
			spreadMode_ = iSpreadMode;
			return kIABNoError;
        }
        
        iabError setIABObjectSpread(IABSpreadModeType iSpreadMode, uint16_t iSpreadXYZ, uint16_t iSpreadY, uint16_t iSpreadZ)
        {
            if (iSpreadMode == kIABSpreadMode_LowResolution_1D)
            {
                // In low res 1D mode, only spreadXYZInStream_ is read/written to stream
                
                // Check x range [0, 255], ignore y & z
                if (iSpreadXYZ > 255)
                {
                    return kIABBadArgumentsError;		// Input x is out of range
                }
                
                // make y & z same as spreadXYZInStream_ for consistency
                spreadXYZInStream_ = iSpreadXYZ;
                spreadYInStream_ = iSpreadXYZ;
                spreadZInStream_ = iSpreadXYZ;
                
                // Convert
                spreadXYZ_ = static_cast<float>((iSpreadXYZ)/255.0f);
                spreadY_ = spreadXYZ_;
                spreadZ_ = spreadXYZ_;
            }
            else if (iSpreadMode == kIABSpreadMode_HighResolution_1D)
            {
                // In Hi res 1D mode, only spreadXYZInStream_ is read/written to stream

                // Check range [0, 4095], ignore y & z
                if (iSpreadXYZ > 4095)
                {
                    return kIABBadArgumentsError;		// Input x is out of range
                }
                
                // make y & z same as spreadXYZInStream_ for consistency
                spreadXYZInStream_ = iSpreadXYZ;
                spreadYInStream_ = iSpreadXYZ;
                spreadZInStream_ = iSpreadXYZ;
                
                // Convert
                spreadXYZ_ = static_cast<float>((iSpreadXYZ)/4095.0f);
                spreadY_ = spreadXYZ_;
                spreadZ_ = spreadXYZ_;
            }
            else if (iSpreadMode == kIABSpreadMode_HighResolution_3D)
            {
                // Check range x, y, z [0, 4095]
                if (iSpreadXYZ > 4095 || iSpreadY > 4095 || iSpreadZ > 4095)
                {
                    return kIABBadArgumentsError;		// Input x is out of range
                }
                
                spreadXYZInStream_ = iSpreadXYZ;
                spreadYInStream_ = iSpreadY;
                spreadZInStream_ = iSpreadZ;
                
                // Convert
                spreadXYZ_ = static_cast<float>(iSpreadXYZ/4095.0f);
                spreadY_ = static_cast<float>(iSpreadY/4095.0f);
                spreadZ_ = static_cast<float>(iSpreadZ/4095.0f);
            }
			else if (iSpreadMode == kIABSpreadMode_None)
			{
				spreadXYZ_ = 0.0f;
				spreadY_ = 0.0f;
				spreadZ_ = 0.0f;
				spreadXYZInStream_ = 0;
				spreadYInStream_ = 0;
				spreadZInStream_ = 0;
			}
			else
            {
                return kIABBadArgumentsError;   // unknown spread mode
            }
            
            spreadMode_ = iSpreadMode;
            return kIABNoError;
        }


        void getIABObjectSpread(float &oSpreadX, float &oSpreadY, float &oSpreadZ) const
        {
            oSpreadX = spreadXYZ_;
            oSpreadY = spreadY_;
            oSpreadZ = spreadZ_;
        }
        
        IABSpreadModeType getIABObjectSpreadMode() const
        {
            return spreadMode_;
        }
        
        uint16_t getStreamSpreadXYZ() const
        {
            return spreadXYZInStream_;
        }

        uint16_t getStreamSpreadY() const
        {
            return spreadYInStream_;
        }

        uint16_t getStreamSpreadZ() const
        {
            return spreadZInStream_;
        }
        
        bool operator== (const IABObjectSpread iAnotherObjectSpread) const
        {
            // Compare bitstream values only since the translation  from float to bitstream value and back may give a different value
            
            if ((this->spreadMode_ == iAnotherObjectSpread.spreadMode_) && (this->spreadXYZInStream_ == iAnotherObjectSpread.spreadXYZInStream_) &&
                (this->spreadYInStream_ == iAnotherObjectSpread.spreadYInStream_) && (this->spreadZInStream_ == iAnotherObjectSpread.spreadZInStream_))
            {
                return true;
            }
            
            return false;
        }
        
    private:

		IABSpreadModeType spreadMode_;				/**< 2-bit, specifies type of spreading info to use. See table 15. */
        float spreadXYZ_;							/**< [0.0, 1.0], spread amount for X/Y/Z, when (spreadMode_ == 0x1/0x2). For X only when (spreadMode_ == 0x3) */
        float spreadY_;								/**< [0.0, 1.0] spread amount for Y, when (spreadMode_ == 0x3). */
        float spreadZ_;								/**< [0.0, 1.0] spread amount for Z, when (spreadMode_ == 0x3). */
        
        uint16_t spreadXYZInStream_;				/**< 8-/12-bit spread amount for X/Y/Z, when (spreadMode_ == 0x1/0x2). For X only when (spreadMode_ == 0x3) */
        uint16_t spreadYInStream_;					/**< 12-bit spread amount for Y, when (spreadMode_ == 0x3). */
        uint16_t spreadZInStream_;					/**< 12-bit spread amount for Z, when (spreadMode_ == 0x3). */
        
    };

    /**
     * Represents decor coeff and its coding method. decorCoef_ included in bitstream only
     * when (decorCoefPrefix_ == 0x2).
     *
     */
    struct IABDecorCoeff
    {
        IABDecorCoeffPrefixType decorCoefPrefix_;	/**< 2-bit, method for coding the decor values. See tables 10 (channel) and 16 (object). */
        uint8_t decorCoef_;						    /**< 8-bit decor coefficient, when (decorCoefPrefix_ == 0x2) */
    };

    /**
     * Represents audio description code, and a NULL-terminated ASCII text string when (audioDescription_ & 0x80 != 0).
     *
     */
    struct IABAudioDescription
    {
        IABAudioDescriptionType audioDescription_;	/**< 8-bit, description of audio content. See tables 11 */
		std::string audioDescriptionText_;		    /**< containing string from/to NULL-terminated ASCII text, when (audioDescription_ & 0x80 != 0) */
    };

	/**
	* Represents a set of remap gain coefficients that are applied to source channels 
	* to derive a destination channel.
	* Gain coefficients are represented using IABGain.
	* The channel ID of destination is specified using IABChannelIDType.
	* The number of source channel (> 0) is represented by numberOfSources.
	* Source channel ID, and listed order are entirely determined by client.
	*
	*/
	class IABRemapCoeff
	{

	public:
		IABRemapCoeff() : DestinationChannelID_(kIABChannelID_Center), numberOfSources_(0), remapGainCoeffs_() {}

		IABRemapCoeff(IABChannelIDType destChannelID, int16_t numSourceChannel) : 
			DestinationChannelID_(destChannelID), numberOfSources_(numSourceChannel)
		{
			IABGain initRemapCoeff;
			initRemapCoeff.setIABGain(0.0f);						// Init gain to 0.0f (silence)

			// Reserve size to prevent re-allocation during looping
			remapGainCoeffs_.reserve(numSourceChannel);

			for (uint16_t i = 0; i < numSourceChannel; i++)
			{
				remapGainCoeffs_.push_back(initRemapCoeff);
			}
		}

		~IABRemapCoeff() { remapGainCoeffs_.clear(); }

		iabError setDestinationChannelID(IABChannelIDType iChannelID)
		{
			DestinationChannelID_ = iChannelID;
			return kIABNoError;
		}

		iabError setRemapCoeffVector(std::vector<IABGain> iRemapCoeffArray)
		{
			remapGainCoeffs_.clear();								// Clear existing value
			remapGainCoeffs_ = iRemapCoeffArray;

			numberOfSources_ = static_cast<uint16_t>(remapGainCoeffs_.size());
			return kIABNoError;
		}

		iabError setRemapCoeff(IABGain iRemapCoeff, uint16_t index)
		{
			if (index > (remapGainCoeffs_.size() - 1))
			{
				return kIABBadArgumentsError;
			}

			remapGainCoeffs_[index] = iRemapCoeff;

			return kIABNoError;
		}

		IABChannelIDType getDestinationChannelID()
		{
			return DestinationChannelID_;
		}

		uint16_t getRemapSourceNumber()
		{
			return numberOfSources_;
		}

		std::vector<IABGain>& getRemapCoeffVector()
		{
			return remapGainCoeffs_;
		}

		iabError getRemapCoeff(IABGain& oRemapCoeff, uint16_t index)
		{
			if (index > (remapGainCoeffs_.size() - 1))
			{
				return kIABBadArgumentsError;
			}

			oRemapCoeff = remapGainCoeffs_[index];

			return kIABNoError;
		}

	private:

		IABChannelIDType DestinationChannelID_;					/**< Channel ID code for remap destination channel*/
		uint16_t numberOfSources_;								/**< Numnber of source channels for remapping to destination channel */
		std::vector<IABGain> remapGainCoeffs_;					/**< pointers to "numberOfSources_" of remap coefficients */
	};

	/**
	*
	* Simple relationship table for a channel id and audio id - typically
	* for bed definitions
	*
	*/
	typedef std::map<IABChannelIDType, IABAudioDataIDType> IABChannelAudioLookup;

	/**
	*
	* Identifies the linkage between bed channels and audio data identifiers used
	* with the bed for building beds
	*
	*/
    class IABBedMappingInfo
	{

	public:

		/// Construction requires immediately setting the bed use case and metadata
		/// identifier for the bed information
		///
		IABBedMappingInfo(IABUseCaseType useCase, IABMetadataIDType metadataID) :
			useCase_(useCase), metadataID_(metadataID) {}

		/// Destruction
		///
		~IABBedMappingInfo() {}

		/// Returns the use case (e.g. bed type) identified by this association
		///
		IABUseCaseType getUseCase() const { return useCase_; }

		/// Returns the Metadata ID associated with this mapping
		///
		IABMetadataIDType getMetadataID() const { return metadataID_; }

		IABUseCaseType    useCase_;
		IABMetadataIDType metadataID_;
        IABChannelAudioLookup lookupMap_;

	};

    // This is created so we could pass pan info to IAB packer in a single data structure
    struct IABObjectPanningParameters
    {
        uint1_t                 panInfoExists_;
        CartesianPosInUnitCube  position_;
        IABGain                 objectGain_;
        IABObjectSpread         spread_;
    };
    
    
    /*****************************************************************************
     *
     * Validator Types
     *
     *****************************************************************************/
    
	 /**
	 * Constraint sets supported by the validator
	 *
	 */
	enum SupportedConstraintsSet
	{
		kConstraints_set_Cinema_ST2098_2_2018,
		kConstraints_set_Cinema_ST429_18_2019,
		kConstraints_set_DbyCinema,
		kConstraints_set_IMF_ST2098_2_2019,
		kConstraints_set_IMF_ST2067_201_2019,
		kConstraints_set_DbyIMF
	};

	/**
     * Represent result of validation against a constraint set.
     *
     */
    enum ValidationResult
    {
        kValid,				/**< Indicates validation is complete and validator has not encountered any non-compilance condition */
        kValidWithWarning,  /**< Indicates validation is complete. The validator has not encountered any non-compilance condition, but has encountered warning-level issues */
        kInvalid			/**< Indicates validator has encountered one or more items not meeting the constraint set */
    };
    
	/**
	* Kinds of event reported by the validator
	*/
	enum ValidatorEventKind
	{
		ErrorEvent,			/**< Indicates an error event */
		WarningEvent		/**< Indicates a warning event */
	};

	/**
	* Represent single validation issue, error or warning, with specific error/warning code
	* A list of events can be compiled to generate validation report.
	*/
	typedef struct ValidationIssue
	{
		ValidationIssue()
		{
			reset();
		}

		void reset()
		{
            isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			event_ = ErrorEvent;
			errorCode_ = kIABNoError;
			frameIndex_ = 0;
			id_ = -1;				// default to "kIssueID_IAFrame"
		}

        SupportedConstraintsSet     isBeingValidated_;  /**< Indicates constraint set under which the issue was reported */
		ValidatorEventKind event_;                      /**< Indicates whether it's an error or a warning */
		iabError errorCode_;                            /**< Error or warning code */
		int32_t frameIndex_;                            /**< Frame index number in which it occurs */
		int32_t id_;                                    /**< IDs for identifying bed/object/essence/others. It can be MetaID, AudioID, or other validator pre-defined values */

	} ValidationIssue;

	/**
	* Supplementary issue IDs identifying kind of issues reported.
	* Containing negative enum values only.
	* To be used for "ValidationIssue.id_" field.
	* 
	* Note, positive values are used from MetaIDs or AudioIDs inherently specified in object/bed/DLC/PCM.
	* For IAB elements that do not have such in-built IDs, eg. IAFrame, etc, following is defined for use
	* with IABValidator lib to assist identification of issues. 
	*/
	enum ValidationSuppIssueID {
		kIssueID_IAFrame = -1,
		kIssueID_AuthoringToolInfo = -2,
		kIssueID_UserData = -3,
		kIssueID_ObjectZoneDefinition19 = -4
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABDATATYPES_H__
