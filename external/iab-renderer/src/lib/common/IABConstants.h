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
 * Header file for IABPacker specific constant definitions.
 *
 * @file
 */

#ifndef __IABCONSTANTS_H__
#define	__IABCONSTANTS_H__

#include <limits>
#include <algorithm>

#include "IABDataTypes.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /**
     * Default frame duration in samples (for 48000Hz sampling rate, 24 fps)
     *
     */
    static const uint32_t kIABDefaultFrameDuration = 2000;

    /**
     * "Max_Rendered" limit for 48000Hz sampling rate
     *
     */
    static const IABMaxRenderedRangeType kIABMaxRenderedLimit48000Hz = 128;

    /**
     * "Max_Rendered" limit for 96000Hz sampling rate
     *
     */
    static const IABMaxRenderedRangeType kIABMaxRenderedLimit96000Hz = 64;

    /**
     * Maximum limit on toal number of audio data elements in a frame for 48000Hz sampling rate.
     * Audio data elements are of either AudioDataPCM or AudioDataDLC types.
     *
     */
    static const IABElementCountType kIABMaxAudioDataElementsInFrame48000Hz = 128;

    /**
     * Maximum limit on toal number of audio data elements in a frame for 96000Hz sampling rate.
     * Audio data elements are of either AudioDataPCM or AudioDataDLC types.
     *
     */
    static const IABElementCountType kIABMaxAudioDataElementsInFrame96000Hz = 64;

    /**
     * Default object snap tolerance = 4095, i.e. 4095/4095 = 1.0 or "always snap"
     * In IAB SDK 1.0, default snap tolerance was originally interpreted as 1/4095 and it is changed to
     * 4095/4095. See ST2098-2:2018 Section 11.2 on DEFAULT_OBJ_SNAP_TOL.
     * 
     */
    static const uint16_t kIABDefaultObjectSnapTolerance = 4095;
    
    /**
     * Number of object zones for Zone 9 definition
     *
     */
    static const uint32_t kIABObjectZoneCount9 = 9;

    /**
     * Number of object zones for Zone 19 definition
     *
     */
    static const uint32_t kIABObjectZoneCount19 = 19;

    /**
     * Constant is used in the interior object to 3D norm objects conversion.
     *
     */
    static const float kEPSILON = 8 * std::numeric_limits<float>::epsilon();

    /**
     * Platform-independent defition of Pi.
     */
    static const float iabKPI = 3.141592653589793f;

    /**
     * Default frame version = 1
     * (0 and 2 are forbidden per specification)
     *
     */
    static const uint8_t kIABDefaultFrameVersion = 1;
    
    /**
     * Maximumm number of samples per renderer output channel per IAB frame
     *
     */
	static const uint32_t kIABMaxFrameSampleCount = 4004;
    
    /**
     * Highest positive value for 32-bit signed integer (= 2^31 - 1), casted to float
     *
     */
    static const float kInt32BitMaxValue = (float) 2147483647.0;
    
    /**
     * Lowest negative value for 32-bit signed integer (= -(2^31)), casted to float
     *
     */
    static const float kInt32BitMinValue = (float) -2147483648.0;
    
	/**
     * struct for Cinema_ST2098_2_2018 constraint set parameters
	 * Defines parameter settings for validating against ST2098-2-2018 constraints.
	 */
    struct CS_Cinema_ST2098_2_2018
    {
    public:
        
        // Constructor
		CS_Cinema_ST2098_2_2018()
		{
			// init validUseCases_
			IABUseCaseType useCases_Cinema_ST2098_2_2018[] =
			{
				kIABUseCase_5_1,
				kIABUseCase_7_1_DS,
				kIABUseCase_7_1_SDS,
				kIABUseCase_11_1_HT,
				kIABUseCase_13_1_HT,
				kIABUseCase_9_1_OH,
				kIABUseCase_Always
			};
			validUseCases_.insert(useCases_Cinema_ST2098_2_2018, useCases_Cinema_ST2098_2_2018 + 7);

			// init validGainPrefix_
			IABGainPrefixType gainPrefixes_Cinema_ST2098_2_2018[] =
			{
				kIABGainPrefix_Unity,
				kIABGainPrefix_Silence,
				kIABGainPrefix_InStream
			};
			validGainPrefix_.insert(gainPrefixes_Cinema_ST2098_2_2018, gainPrefixes_Cinema_ST2098_2_2018 + 3);

			// init validDecorCoeffPrefix_
			IABDecorCoeffPrefixType decorCoeffPrefixes_Cinema_ST2098_2_2018[] =
			{
				kIABDecorCoeffPrefix_NoDecor,
				kIABDecorCoeffPrefix_MaxDecor,
				kIABDecorCoeffPrefix_DecorCoeffInStream
			};
			validDecorCoeffPrefix_.insert(decorCoeffPrefixes_Cinema_ST2098_2_2018, decorCoeffPrefixes_Cinema_ST2098_2_2018 + 3);

			// init reservedAudioDescriptionCodes_
			int8_t reserved_AudioDescriptionCodes_Cinema_ST2098_2_2018[] =
			{
				0x40
			};
			reservedAudioDescriptionCodes_.insert(reserved_AudioDescriptionCodes_Cinema_ST2098_2_2018, reserved_AudioDescriptionCodes_Cinema_ST2098_2_2018 + 1);

			// init validVersion_
			validVersion_ = 1;

			// init validSampleRates_
			IABSampleRateType sampleRates_Cinema_ST2098_2_2018[] =
			{
				kIABSampleRate_48000Hz,
				kIABSampleRate_96000Hz
			};
			validSampleRates_.insert(sampleRates_Cinema_ST2098_2_2018, sampleRates_Cinema_ST2098_2_2018 + 2);

			// init validBitDepth_
			IABBitDepthType bitDepths_Cinema_ST2098_2_2018[] =
			{
				kIABBitDepth_16Bit,
				kIABBitDepth_24Bit
			};
			validBitDepth_.insert(bitDepths_Cinema_ST2098_2_2018, bitDepths_Cinema_ST2098_2_2018 + 2);

			// init validFrameRates_
			IABFrameRateType frameRates_Cinema_ST2098_2_2018[] =
			{
				kIABFrameRate_24FPS,
				kIABFrameRate_25FPS,
				kIABFrameRate_30FPS,
				kIABFrameRate_48FPS,
				kIABFrameRate_50FPS,
				kIABFrameRate_60FPS,
				kIABFrameRate_96FPS,
				kIABFrameRate_100FPS,
				kIABFrameRate_120FPS
			};
			validFrameRates_.insert(frameRates_Cinema_ST2098_2_2018, frameRates_Cinema_ST2098_2_2018 + 9);

			// init maxRenderedLimit
			maxRenderedLimit48k_ = 128;
			maxRenderedLimit96k_ = 64;

			// init validChannelIDs_
			IABChannelIDType channelIDs_Cinema_ST2098_2_2018[] =
			{
				kIABChannelID_Left,
				kIABChannelID_LeftCenter,
				kIABChannelID_Center,
				kIABChannelID_RightCenter,
				kIABChannelID_Right,
				kIABChannelID_LeftSideSurround,
				kIABChannelID_LeftSurround,
				kIABChannelID_LeftRearSurround,
				kIABChannelID_RightRearSurround,
				kIABChannelID_RightSideSurround,
				kIABChannelID_RightSurround,
				kIABChannelID_LeftTopSurround,
				kIABChannelID_RightTopSurround,
				kIABChannelID_LFE,
				kIABChannelID_LeftHeight,
				kIABChannelID_RightHeight,
				kIABChannelID_CenterHeight,
				kIABChannelID_LeftSurroundHeight,
				kIABChannelID_RightSurroundHeight,
				kIABChannelID_LeftSideSurroundHeight,
				kIABChannelID_RightSideSurroundHeight,
				kIABChannelID_LeftRearSurroundHeight,
				kIABChannelID_RightRearSurroundHeight,
				kIABChannelID_TopSurround
			};
			validChannelIDs_.insert(channelIDs_Cinema_ST2098_2_2018, channelIDs_Cinema_ST2098_2_2018 + 24);


			// init validZoneGainPrefix_
			IABZoneGainPrefixType zoneGainPrefixes_Cinema_ST2098_2_2018[] =
			{
				kIABZoneGainPrefix_Silence,
				kIABZoneGainPrefix_Unity,
				kIABZoneGainPrefix_InStream
			};
			validZoneGainPrefix_.insert(zoneGainPrefixes_Cinema_ST2098_2_2018, zoneGainPrefixes_Cinema_ST2098_2_2018 + 3);

			// init validObjectSpreadModes_
			IABSpreadModeType objectSpreadModes_Cinema_ST2098_2_2018[] =
			{
				kIABSpreadMode_LowResolution_1D,
				kIABSpreadMode_None,
				kIABSpreadMode_HighResolution_1D,
				kIABSpreadMode_HighResolution_3D
			};
			validObjectSpreadModes_.insert(objectSpreadModes_Cinema_ST2098_2_2018, objectSpreadModes_Cinema_ST2098_2_2018 + 4);

		}
        
        // General constraints
        std::set<IABUseCaseType>            validUseCases_;                 /**< List of valid use case codes */
        std::set<IABGainPrefixType>         validGainPrefix_;               /**< List of valid gain prefix codes */
		std::set<IABDecorCoeffPrefixType>   validDecorCoeffPrefix_;         /**< List of valid Decor coeff prefix codes */
		std::set<int8_t>                    reservedAudioDescriptionCodes_; /**< List of reserved AudioDescription codes */
        
        // Frame level constraints
		uint8_t								validVersion_;                  /**< valid IAB bitstream version */
		std::set<IABSampleRateType>         validSampleRates_;              /**< List of valid sample rate codes */
        std::set<IABBitDepthType>           validBitDepth_;                 /**< List of valid bit depth codes */
        std::set<IABFrameRateType>          validFrameRates_;               /**< List of valid frame rate codes */
        uint32_t                            maxRenderedLimit48k_;           /**< Upper limit on maxRendered value for 48kHz audio */
		uint32_t                            maxRenderedLimit96k_;           /**< Upper limit on maxRendered value for 96kHz audio */
		int32_t                             allowedSubElementLevels_;       /**< Upper limit on maxRendered value. -1 = do not check */
        
        // Constraint on BedDefinition
        std::set<IABChannelIDType>          validChannelIDs_;               /**< List of valid channel ID codes */
        
        // Constraint on ObjectDefinition
        std::set<IABZoneGainPrefixType>     validZoneGainPrefix_;           /**< List of valid zone gain prefix codes */
        std::set<IABSpreadModeType>         validObjectSpreadModes_;        /**< List of valid object spread mode codes */
    }
	static const ConstraintSet_ST2098_2_2018;

	/**
	* struct for additional Cinema_ST429_18_2019 constraint set parameters
	* Additional to CS_Cinema_ST2098_2_2018
	* Defines parameter settings for validating against ST429-18-2019 constraints.
	* Initialisation To be added
	*/
	struct CS_Cinema_ST429_18_2019_SUP
	{
		CS_Cinema_ST429_18_2019_SUP()
        {
            inValidUseCaseLowerBound_ = 0x30;
            inValidUseCaseUpperBound_ = 0xFE;

            // init validBitDepth_
            IABBitDepthType bitDepths_Cinema_ST429_18_2019_SUP[] =
            {
                kIABBitDepth_24Bit
            };
            validBitDepth_.insert(bitDepths_Cinema_ST429_18_2019_SUP, bitDepths_Cinema_ST429_18_2019_SUP + 1);

            // init frameSizeLimits_, in order of frame rate codes 0x00(24fps) to 0x08(120fps)
            uint32_t frameSize[] = { 781250, 750000, 625000, 390625, 375000, 312500, 195313, 187500, 156250 };
            frameSizeLimits_.assign(frameSize, frameSize + 9);
            
            upperBoundForValidChannelID_ = kIABChannelID_DCReserveEnd;
            allowAudioDataPCM_ = false;
            allowUserData_ = false;
        }

        // General constraints
        int32_t        inValidUseCaseLowerBound_;       /**< Lower bound for invalid range */
        int32_t        inValidUseCaseUpperBound_;       /**< Upper bound for invalid range */
        
        // Frame level constraints
        std::set<IABBitDepthType>   validBitDepth_;     /**< List of valid bit depth codes */
        std::vector<uint32_t>       frameSizeLimits_;   /** Vector of frame size limit in bytes. */
        
        // Constraint on BedDefinition channel ID and remap destination channel ID
        IABChannelIDType            upperBoundForValidChannelID_;      /**< Upper bound for valid range. Channel ID codes above this value are invalid */

        // Constraint on AudioDataPCM
        bool                        allowAudioDataPCM_;         /**< Indicates if AudioDataPCM element is allowed in stream */
        
        // Constraint on UserData
        bool                        allowUserData_;             /**< Indicates if UserData element is allowed in stream */
	}
	static const ConstraintSet_ST429_18_2019;

	/**
	* struct for additional DbyCinema constraint set parameters
	* Additional to CS_Cinema_ST2098_2_2018 and CS_Cinema_ST429_18_2019_SUP
	* Defines parameter settings for validating against DbyCinema constraints.
	* Initialisation To be added
	*/
	struct CS_DbyCinema_SUP
	{
		CS_DbyCinema_SUP()
        {
            // init validGainPrefix_
            IABGainPrefixType gainPrefixes_DbyCinema[] =
            {
                kIABGainPrefix_Unity
            };
            validGainPrefix_.insert(gainPrefixes_DbyCinema, gainPrefixes_DbyCinema + 1);

            // init validDecorCoeffPrefix_
            IABDecorCoeffPrefixType decorCoeffPrefixes_DbyCinema[] =
            {
                kIABDecorCoeffPrefix_NoDecor,
                kIABDecorCoeffPrefix_MaxDecor
            };
            validDecorCoeffPrefix_.insert(decorCoeffPrefixes_DbyCinema, decorCoeffPrefixes_DbyCinema + 2);

            // init validUseCases_
            IABUseCaseType useCases_DbyCinema[] =
            {
                kIABUseCase_Always
            };
            validUseCases_.insert(useCases_DbyCinema, useCases_DbyCinema + 1);

            // init max object count and max channel count
            maxObjectCount_ = 118;
            maxBedChannelCount_ = 10;

            // init validSampleRates_
            IABSampleRateType sampleRates_DbyCinema[] =
            {
                kIABSampleRate_48000Hz
            };
            validSampleRates_.insert(sampleRates_DbyCinema, sampleRates_DbyCinema + 1);

            // init validChannelIDs_
            IABChannelIDType channelIDs_DbyCinema[] =
            {
                kIABChannelID_Left,
                kIABChannelID_Center,
                kIABChannelID_Right,
                kIABChannelID_LeftSideSurround,
                kIABChannelID_LeftRearSurround,
                kIABChannelID_RightRearSurround,
                kIABChannelID_RightSideSurround,
                kIABChannelID_LeftTopSurround,
                kIABChannelID_RightTopSurround,
                kIABChannelID_LFE
            };
            validChannelIDs_.insert(channelIDs_DbyCinema, channelIDs_DbyCinema + 10);

            // init validZoneGainPrefix_
            IABZoneGainPrefixType zoneGainPrefixes_DbyCinema[] =
            {
                kIABZoneGainPrefix_Silence,
                kIABZoneGainPrefix_Unity
            };
            validZoneGainPrefix_.insert(zoneGainPrefixes_DbyCinema, zoneGainPrefixes_DbyCinema + 2);

            // init validObjectSpreadModes_
            IABSpreadModeType objectSpreadModes_DbyCinema[] =
            {
                kIABSpreadMode_HighResolution_1D,
            };
            validObjectSpreadModes_.insert(objectSpreadModes_DbyCinema, objectSpreadModes_DbyCinema + 1);

            // init Dby recommended zone gain presets
            IABZoneGainPrefixType zoneGainPresets_DbyCinema[11][9] =
            {
                // Set 1a
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity
				},

                // Set 1b
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				},

                // Set 2a
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity
				},

                // Set 2b
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				},

                // Set 3a
                {
					kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity
				},

                // Set 3b
                {
					kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				},

                // Set 4a
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity
				},

                // Set 4b
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				},

                // Set 5a
                {
					kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity
				},

                // Set 5b
                {
					kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				},

                // Set 6
                {
					kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Unity,
                    kIABZoneGainPrefix_Silence,
                    kIABZoneGainPrefix_Silence
				}
            };

			// init dbyCinemaZoneGainPresets_
			for (uint8_t i = 0; i < 11; i++)
			{
				for (uint8_t k = 0; k < 9; k++)
				{
					dbyCinemaZoneGainPresets_[i][k] = zoneGainPresets_DbyCinema[i][k];
				}
			}
        }

        // General constraints
        std::set<IABGainPrefixType>         validGainPrefix_;               /**< List of valid gain prefix codes */
        std::set<IABDecorCoeffPrefixType>   validDecorCoeffPrefix_;         /**< List of valid Decor coeff prefix codes */
        std::set<IABUseCaseType>            validUseCases_;                 /**< List of valid use case codes */

        // Frame level constraints
        int32_t                             maxObjectCount_;                /**< Maximum number of objects in frame */
        int32_t                             maxBedChannelCount_;            /**< Maximum number of bed channels in frame */
        std::set<IABSampleRateType>         validSampleRates_;              /**< List of valid sample rate codes */

        // Constraint on BedDefinition
        std::set<IABChannelIDType>          validChannelIDs_;               /**< List of valid channel ID codes */

        // Constraint on ObjectDefinition
        std::set<IABZoneGainPrefixType>     validZoneGainPrefix_;           /**< List of valid zone gain prefix codes */
        std::set<IABSpreadModeType>         validObjectSpreadModes_;        /**< List of valid object spread mode codes */
        IABZoneGainPrefixType               dbyCinemaZoneGainPresets_[11][9];   /**< Expected zone gain sequence */
	}
	static const ConstraintSet_DbyCinema;

	/**
     * struct for IMF_ST2098_2_2019 constraint set parameters
	 * Defines parameter settings for validating against ST2098-2-2019 constraints.
	 * Initialisation To be added
	 */
	struct CS_IMF_ST2098_2_2019
	{
        // Constructor
        CS_IMF_ST2098_2_2019()
        {
            // init validUseCases_
            IABUseCaseType useCases_IMF_ST2098_2_2019[] =
            {
                kIABUseCase_5_1,
                kIABUseCase_7_1_DS,
                kIABUseCase_7_1_SDS,
                kIABUseCase_11_1_HT,
                kIABUseCase_13_1_HT,
                kIABUseCase_9_1_OH,

				// ITU-R BS.2051-2 soundfields/use cases.
				// These 3 are valid for IMF_ST2098_2_2019 only (not for IMF_ST2098_2_2018)
				kIABUseCase_2_0_ITUA,
                kIABUseCase_5_1_4_ITUD,
                kIABUseCase_7_1_4_ITUJ,
				kIABUseCase_Always
			};
            validUseCases_.insert(useCases_IMF_ST2098_2_2019, useCases_IMF_ST2098_2_2019 + 10);
            
            // init validGainPrefix_
            IABGainPrefixType gainPrefixes_IMF_ST2098_2_2019[] =
            {
                kIABGainPrefix_Unity,
                kIABGainPrefix_Silence,
                kIABGainPrefix_InStream
            };
            validGainPrefix_.insert(gainPrefixes_IMF_ST2098_2_2019, gainPrefixes_IMF_ST2098_2_2019 + 3);
            
            // init validDecorCoeffPrefix_
            IABDecorCoeffPrefixType decorCoeffPrefixes_IMF_ST2098_2_2019[] =
            {
                kIABDecorCoeffPrefix_NoDecor,
                kIABDecorCoeffPrefix_MaxDecor,
                kIABDecorCoeffPrefix_DecorCoeffInStream
            };
            validDecorCoeffPrefix_.insert(decorCoeffPrefixes_IMF_ST2098_2_2019, decorCoeffPrefixes_IMF_ST2098_2_2019 + 3);
            
            // init reservedAudioDescriptionCodes_
            int8_t reserved_AudioDescriptionCodes_IMF_ST2098_2_2019[] =
            {
                0x40
            };
            reservedAudioDescriptionCodes_.insert(reserved_AudioDescriptionCodes_IMF_ST2098_2_2019, reserved_AudioDescriptionCodes_IMF_ST2098_2_2019 + 1);
            
            // init validVersion_
            validVersion_ = 1;
            
            // init validSampleRates_
            IABSampleRateType sampleRates_IMF_ST2098_2_2019[] =
            {
                kIABSampleRate_48000Hz,
                kIABSampleRate_96000Hz
            };
            validSampleRates_.insert(sampleRates_IMF_ST2098_2_2019, sampleRates_IMF_ST2098_2_2019 + 2);
            
            // init validBitDepth_
            IABBitDepthType bitDepths_IMF_ST2098_2_2019[] =
            {
                kIABBitDepth_16Bit,
                kIABBitDepth_24Bit
            };
            validBitDepth_.insert(bitDepths_IMF_ST2098_2_2019, bitDepths_IMF_ST2098_2_2019 + 2);
            
            // init validFrameRates_
            IABFrameRateType frameRates_IMF_ST2098_2_2019[] =
            {
                kIABFrameRate_24FPS,
                kIABFrameRate_25FPS,
                kIABFrameRate_30FPS,
                kIABFrameRate_48FPS,
                kIABFrameRate_50FPS,
                kIABFrameRate_60FPS,
                kIABFrameRate_96FPS,
                kIABFrameRate_100FPS,
                kIABFrameRate_120FPS,
                kIABFrameRate_23_976FPS
            };
            validFrameRates_.insert(frameRates_IMF_ST2098_2_2019, frameRates_IMF_ST2098_2_2019 + 10);
            
            // init maxRenderedLimit
            maxRenderedLimit48k_ = 128;
            maxRenderedLimit96k_ = 64;
            
            // init validChannelIDs_
            IABChannelIDType channelIDs_IMF_ST2098_2_2019[] =
            {
                kIABChannelID_Left,
                kIABChannelID_LeftCenter,
                kIABChannelID_Center,
                kIABChannelID_RightCenter,
                kIABChannelID_Right,
                kIABChannelID_LeftSideSurround,
                kIABChannelID_LeftSurround,
                kIABChannelID_LeftRearSurround,
                kIABChannelID_RightRearSurround,
                kIABChannelID_RightSideSurround,
                kIABChannelID_RightSurround,
                kIABChannelID_LeftTopSurround,
                kIABChannelID_RightTopSurround,
                kIABChannelID_LFE,
                kIABChannelID_LeftHeight,
                kIABChannelID_RightHeight,
                kIABChannelID_CenterHeight,
                kIABChannelID_LeftSurroundHeight,
                kIABChannelID_RightSurroundHeight,
                kIABChannelID_LeftSideSurroundHeight,
                kIABChannelID_RightSideSurroundHeight,
                kIABChannelID_LeftRearSurroundHeight,
                kIABChannelID_RightRearSurroundHeight,
                kIABChannelID_TopSurround,
                kIABChannelID_LeftTopFront,
                kIABChannelID_RightTopFront,
                kIABChannelID_LeftTopBack,
                kIABChannelID_RightTopBack,
                kIABChannelID_TopSideLeft,
                kIABChannelID_TopSideRight,
                kIABChannelID_LFE1,
                kIABChannelID_LFE2,
                kIABChannelID_FrontLeft,
                kIABChannelID_FrontRight
            };
            validChannelIDs_.insert(channelIDs_IMF_ST2098_2_2019, channelIDs_IMF_ST2098_2_2019 + 34);
            
            
            // init validZoneGainPrefix_
            IABZoneGainPrefixType zoneGainPrefixes_IMF_ST2098_2_2019[] =
            {
                kIABZoneGainPrefix_Silence,
                kIABZoneGainPrefix_Unity,
                kIABZoneGainPrefix_InStream
            };
            validZoneGainPrefix_.insert(zoneGainPrefixes_IMF_ST2098_2_2019, zoneGainPrefixes_IMF_ST2098_2_2019 + 3);
            
            // init validObjectSpreadModes_
            IABSpreadModeType objectSpreadModes_IMF_ST2098_2_2019[] =
            {
                kIABSpreadMode_LowResolution_1D,
                kIABSpreadMode_None,
                kIABSpreadMode_HighResolution_1D,
                kIABSpreadMode_HighResolution_3D
            };
            validObjectSpreadModes_.insert(objectSpreadModes_IMF_ST2098_2_2019, objectSpreadModes_IMF_ST2098_2_2019 + 4);
            
        }
        
        // General constraints
        std::set<IABUseCaseType>            validUseCases_;                 /**< List of valid use case codes */
        std::set<IABGainPrefixType>         validGainPrefix_;               /**< List of valid gain prefix codes */
        std::set<IABDecorCoeffPrefixType>   validDecorCoeffPrefix_;         /**< List of valid Decor coeff prefix codes */
        std::set<int8_t>                    reservedAudioDescriptionCodes_; /**< List of reserved AudioDescription codes */
        
        // Frame level constraints
        uint8_t								validVersion_;                  /**< valid IAB bitstream version */
        std::set<IABSampleRateType>         validSampleRates_;              /**< List of valid sample rate codes */
        std::set<IABBitDepthType>           validBitDepth_;                 /**< List of valid bit depth codes */
        std::set<IABFrameRateType>          validFrameRates_;               /**< List of valid frame rate codes */
        uint32_t                            maxRenderedLimit48k_;           /**< Upper limit on maxRendered value for 48kHz audio */
        uint32_t                            maxRenderedLimit96k_;           /**< Upper limit on maxRendered value for 96kHz audio */
        int32_t                             allowedSubElementLevels_;       /**< Upper limit on maxRendered value. -1 = do not check */
        
        // Constraint on BedDefinition
        std::set<IABChannelIDType>          validChannelIDs_;               /**< List of valid channel ID codes */
        
        // Constraint on ObjectDefinition
        std::set<IABZoneGainPrefixType>     validZoneGainPrefix_;           /**< List of valid zone gain prefix codes */
        std::set<IABSpreadModeType>         validObjectSpreadModes_;        /**< List of valid object spread mode codes */
	}
	static const ConstraintSet_IMF_ST2098_2_2019;

	/**
	* struct for additional IMF_ST2067_201_2019 constraint set parameters
	* Additional to CS_IMF_ST2098_2_2019
	* Defines parameter settings for validating against ST2067-201-2019 constraints.
	* Initialisation To be added
	*/
	struct CS_IMF_ST2067_201_2019_SUP
	{
        // Constructor
		CS_IMF_ST2067_201_2019_SUP()
        {
            // init validBitDepth_
            IABBitDepthType bitDepths_IMF_ST2067_201_2019[] =
            {
                kIABBitDepth_24Bit
            };
            validBitDepth_.insert(bitDepths_IMF_ST2067_201_2019, bitDepths_IMF_ST2067_201_2019 + 1);

            // init validUseCases_
            IABUseCaseType useCases_IMF_ST2067_201_2019[] =
            {
                kIABUseCase_Always
            };
            validUseCases_.insert(useCases_IMF_ST2067_201_2019, useCases_IMF_ST2067_201_2019 + 1);
        }
        
        // General constraints
        std::set<IABBitDepthType>           validBitDepth_;                 /**< List of valid bit depth codes */
        std::set<IABUseCaseType>            validUseCases_;                 /**< List of valid use case codes */

        // No additional constriants on top of IMF_ST2098_2_2019
	}
	static const ConstraintSet_IMF_ST2067_201_2019;

	/**
	* struct for additional DbyIMF constraint set parameters
	* Additional to CS_IMF_ST2098_2_2019 and CS_IMF_ST2067_201_2019_SUP
	* Defines parameter settings for validating against DbyIMF constraints.
	* Initialisation To be added
	*/
	struct CS_DbyIMF_SUP
	{
		CS_DbyIMF_SUP()
		{
			// init validChannelIDs_
			IABChannelIDType channelIDs_DbyIMF[] =
			{
				kIABChannelID_Left,
				kIABChannelID_Center,
				kIABChannelID_Right,
				kIABChannelID_LeftSideSurround,
				kIABChannelID_LeftSurround,
				kIABChannelID_LeftRearSurround,
				kIABChannelID_RightRearSurround,
				kIABChannelID_RightSideSurround,
				kIABChannelID_RightSurround,
				kIABChannelID_LeftTopSurround,
				kIABChannelID_RightTopSurround,
				kIABChannelID_LFE
			};
			validChannelIDs_.insert(channelIDs_DbyIMF, channelIDs_DbyIMF + 12);

			// init validChannelGainPrefix_
			IABGainPrefixType channelGainPrefixes_DbyIMF[] =
			{
				kIABGainPrefix_Unity
			};
			validChannelGainPrefix_.insert(channelGainPrefixes_DbyIMF, channelGainPrefixes_DbyIMF + 1);

			// init validObjectGainPrefix_
			IABGainPrefixType objectGainPrefixes_DbyIMF[] =
			{
				kIABGainPrefix_Unity,
				kIABGainPrefix_Silence
			};
			validObjectGainPrefix_.insert(objectGainPrefixes_DbyIMF, objectGainPrefixes_DbyIMF + 2);

			// init validZoneGainPrefix_
			IABZoneGainPrefixType zoneGainPrefixes_DbyIMF[] =
			{
				kIABZoneGainPrefix_Silence,
				kIABZoneGainPrefix_Unity
			};
			validZoneGainPrefix_.insert(zoneGainPrefixes_DbyIMF, zoneGainPrefixes_DbyIMF + 2);

			// init validObjectSpreadModes_
			IABSpreadModeType objectSpreadModes_DbyIMF[] =
			{
				kIABSpreadMode_HighResolution_1D,
			};
			validObjectSpreadModes_.insert(objectSpreadModes_DbyIMF, objectSpreadModes_DbyIMF + 1);

			// init validDecorCoeffPrefix_
			IABDecorCoeffPrefixType decorCoeffPrefixes_DbyIMF[] =
			{
				kIABDecorCoeffPrefix_NoDecor,
				kIABDecorCoeffPrefix_MaxDecor
			};
			validDecorCoeffPrefix_.insert(decorCoeffPrefixes_DbyIMF, decorCoeffPrefixes_DbyIMF + 2);

			// init DbyIMF recommended zone gain presets
			IABZoneGainPrefixType zoneGainPresets_DbyIMF[12][9] =
			{
				// Set "No back"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "No back plus overhead"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},

				// Set "No sides"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "No sides plus overhead"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},

				// Set "Center back"
				{
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "Center back plus overhead"
				{
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},

				// Set "Screen only"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "Screen only plus overhead"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},

				// Set "Surround only"
				{
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "Surround only plus overhead"
				{
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},

				// Set "Floor"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Silence,
					kIABZoneGainPrefix_Silence
				},

				// Set "Floor plus overhead"
				{
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity,
					kIABZoneGainPrefix_Unity
				},
			};

			// init dbyCinemaZoneGainPresets_
			for (uint8_t i = 0; i < 12; i++)
			{
				for (uint8_t k = 0; k < 9; k++)
				{
					dbyIMFZoneGainPresets_[i][k] = zoneGainPresets_DbyIMF[i][k];
				}
			}
		}

        // Constraint on BedDefinition
        std::set<IABChannelIDType>          validChannelIDs_;               /**< List of valid channel ID codes */
		std::set<IABGainPrefixType>         validChannelGainPrefix_;        /**< List of valid channel gain prefix codes */

        // Constraint on ObjectDefinition
		std::set<IABGainPrefixType>         validObjectGainPrefix_;         /**< List of valid object gain prefix codes */
		std::set<IABZoneGainPrefixType>     validZoneGainPrefix_;           /**< List of valid zone gain prefix codes */
		std::set<IABSpreadModeType>         validObjectSpreadModes_;        /**< List of valid object spread mode codes */
		std::set<IABDecorCoeffPrefixType>   validDecorCoeffPrefix_;         /**< List of valid Decor coeff prefix codes */
		IABZoneGainPrefixType               dbyIMFZoneGainPresets_[12][9];   /**< Expected zone gain sequence */
	}
	static const ConstraintSet_DbyIMF;

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABCONSTANTS_H__
