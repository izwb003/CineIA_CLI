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
 * IABObjectZones.h
 *
 * @file
 */

#ifndef __IABOBJECTZONES_H__
#define __IABOBJECTZONES_H__

#include "IABDataTypes.h"
#include "renderutils/IRendererConfiguration.h"
#include "renderer/IABTransform/IABTransform.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    // Enum identifying supported gain preset pattern
    enum IABObjectZone9Pattern
    {
        kIABObjectZone9_NoBackNoOverhead            = 0,    /**< All zones except back or overhead */
        kIABObjectZone9_NoBackPlusOverhead          = 1,    /**< All zones except back */
        kIABObjectZone9_NoSideNoOverhead            = 2,    /**< All zones except side or overhead */
        kIABObjectZone9_NoSidePlusOverhead          = 3,    /**< All zones except side */
        kIABObjectZone9_CenterBackNoOverhead        = 4,	/**< Screen center and back zones only */
        kIABObjectZone9_CenterBackPlusOverhead      = 5,	/**< Screen center, back and overhead zones only */
        kIABObjectZone9_ScreenOnlyNoOverhead        = 6,    /**< Screen zones only */
        kIABObjectZone9_ScreenOnlyPlusOverhead      = 7,	/**< Screen and overhead zones only */
        kIABObjectZone9_SurroundNoOverhead          = 8,	/**< Side and back zones only */
        kIABObjectZone9_SurroundPlusOverhead        = 9,	/**< Side, back and overhead zones only */
        kIABObjectZone9_FloorNoOverhead             = 10,   /**< All zones except overhead */
        kIABObjectZone9_AllZonesEnabled             = 11	/**< For default setting, zone control inactive */
    };
    
    // Patterns listed in the guidelines doc.
    // Use integer 0, 1 for simplicity, since supported zone gains are either unity or silence.
    // These match the enum order of IABObjectZone9Pattern
    //
    static const uint32_t kSupportedZoneGainPatterns[12][9] =
    {   {1,1,1,1,1,0,0,0,0},    // kIABObjectZone9_NoBackNoOverhead
        {1,1,1,1,1,0,0,1,1},    // kIABObjectZone9_NoBackPlusOverhead
        {1,1,1,0,0,1,1,0,0},    // kIABObjectZone9_NoSideNoOverhead
        {1,1,1,0,0,1,1,1,1},    // kIABObjectZone9_NoSidePlusOverhead
        {0,1,0,0,0,1,1,0,0},    // kIABObjectZone9_CenterBackNoOverhead
        {0,1,0,0,0,1,1,1,1},    // kIABObjectZone9_CenterBackPlusOverhead
        {1,1,1,0,0,0,0,0,0},    // kIABObjectZone9_ScreenOnlyNoOverhead
        {1,1,1,0,0,0,0,1,1},    // kIABObjectZone9_ScreenOnlyPlusOverhead
        {0,0,0,1,1,1,1,0,0},    // kIABObjectZone9_SurroundNoOverhead
        {0,0,0,1,1,1,1,1,1},    // kIABObjectZone9_SurroundPlusOverhead
        {1,1,1,1,1,1,1,0,0},    // kIABObjectZone9_FloorNoOverhead
        {1,1,1,1,1,1,1,1,1}     // kIABObjectZone9_AllZonesEnabled
    };


	// Map for a zone to a list of output/speaker indices.
	// Zone index (uint32_t) as key.
	typedef std::map<uint32_t, std::vector<int32_t> > ZoneToOutputIndicesMap;

	/**
     * IAB base class for renderer object zone control
     */
    class IABObjectZones
    {
    public:
        
		/// Constructor
		IABObjectZones() {}
	
        /// Destructor
		virtual ~IABObjectZones() {}
        
        /**
         * Update object channel gains according to zone9 gains. Only zone gain combinations matching a supported preset
         * pattern will be processed. Zone gain combinations not match any of the presets will be treated as
         * "All zones enabled" i.e. zone control inactive and iObjectChannelGains will be returned un-modified.
         *
         * @param[in] iZoneGains zone9 gain.
         *
         * @param[in, out] iObjectChannelGains vector of object gain to be updated. The caller is responsible for ensuring that the
         * vector size and sequence match the rendered output list.
         */
        virtual iabError ProcessZoneGains(IABObjectZoneGain9 iZoneGains, std::vector<float> &iObjectChannelGains) { return kIABNotImplementedError; }

        /**
         * Overloading method for zone 19.
         *
         * @param[in] iZoneGains zone19 gain.
         *
         * @param[in, out] iObjectChannelGains vector of object gain to be updated. The caller is responsible for ensuring that the
         * vector size and sequence match the rendered output list.
         */
        virtual iabError ProcessZoneGains(IABObjectZoneGain19 iZoneGains, std::vector<float> &iObjectChannelGains) { return kIABNotImplementedError; }
	};
    
    /**
     * IAB class for renderer object zone 9 control
     */
    class IABObjectZone9 : public virtual IABObjectZones
    {
        
    public:
        
        // Constructor
        IABObjectZone9(const RenderUtils::IRendererConfiguration &iConfig);
        
        // Destructor
        ~IABObjectZone9();
        
        /**
         * Process object zone 9 gains. If zone 9 is activated, update object channel gains according to the gain pattern,
         * otherwise leave object channel gains un-modified.
         *
         * @param[in] iZoneGains zone9 gain array.
         *
         * @param[in] iObjectChannelGains vector of object gain to be updated. The caller is responsible for ensuring that the
         * vector size and sequence match the rendered output list.
         */
        iabError ProcessZoneGains(IABObjectZoneGain9 iZoneGains, std::vector<float> &iObjectChannelGains);

		/**
		* (Unit test use only)
		*
		* Gets zone to output index mapping.
		*
		* This method is provided to support unit tests and generally would not be used by the IAB renderer.
		* It allows a unit test to verify zone output index mapping for a test configuration.
		*
		* Returns a reference to zoneOutputIndexMap_.
		*/
		const ZoneToOutputIndicesMap& GetZoneOutputIndexMap();

		/**
		* Checks if instance has been initialised successfully.
		*
		* Returns true if the instance has beed initialised successfully. False indicates initialisation error
		* and zone control will be disabled.
		*/
		bool isInitialised();

	private:

		// *** Class internal data type for Zone9 processing
		//

		// Zones or sub-zones (groups) that are used for zone9 algorithm
		// Note that 4 zones are further divided into sub-zones to improve front-rear resolution.
		// These are: Left-Wall, Right-Wall, Left-Overhead and Right-Overhead.
		//
		// Each of the zones, or sub-zones after division, is a group.
		//
		enum IABObjectZone9GroupingType
		{
			kZone9Group_LeftScreen = 0,							// All screen speakers left of center" */
			kZone9Group_CenterScreen = 1,						// Screen center speakers" */
			kZone9Group_RightScreen = 2,						// All screen speakers right of center" */
			kZone9Group_LeftWall_front = 3,						// All speakers on front-section of left wall" */
			kZone9Group_LeftWall_mid = 4,						// All speakers on mid-section of left wall" */
			kZone9Group_LeftWall_rear = 5,						// All speakers on rear-section of left wall" */
			kZone9Group_RightWall_front = 6,					// All speakers on front-section of right wall" */
			kZone9Group_RightWall_mid = 7,						// All speakers on mid-section of right wall" */
			kZone9Group_RightWall_rear = 8,						// All speakers on rear-section of right wall" */
			kZone9Group_LeftRearWall = 9,						// All speakers on left half of rear wall" */
			kZone9Group_CenterRearWall = 10,					// All speakers on center rear wall" */
			kZone9Group_RightRearWall = 11,						// All speakers on right half of rear wall" */
			kZone9Group_LeftCeiling_front = 12,					// All overhead speakers left of center, front-section" */
			kZone9Group_LeftCeiling_mid = 13,					// All overhead speakers left of center, mid-section" */
			kZone9Group_LeftCeiling_rear = 14,					// All overhead speakers left of center, rear-section" */
			kZone9Group_CenterCeiling_front = 15,				// All overhead speakers in the center, front-section" */
			kZone9Group_CenterCeiling_mid = 16,					// All overhead speakers in the center, mid-section" */
			kZone9Group_CenterCeiling_rear = 17,				// All overhead speakers in the of center, rear-section" */
			kZone9Group_RightCeiling_front = 18,				// All overhead speakers right of center, front-section" */
			kZone9Group_RightCeiling_mid = 19,					// All overhead speakers right of center, mid-section" */
			kZone9Group_RightCeiling_rear = 20,					// All overhead speakers right of center, rear-section" */

			kZone9Group_AfterLastGroup
		};

		// *** Private methods
		//

		// Assigns config file speaker channel indices to the corresponding group (zones/sub-zones) and stores information
		// in zoneGroupOutputIndexMap_.
		// In the config file, every speaker has a renderer output index (RenderSpeaker class data member "ch_") 
		// indicating its "slot" in the renderer output buffer block. This method calls SetupZoneOutputIndexMap() 
		// to determine if a config file speaker belongs to a particular group. Non-VBAP speakers are excluded.
		iabError SetupZoneOutputIndexMap(const RenderUtils::IRendererConfiguration &iConfig);

		// Maps a speaker to zone group (zone/sub-zone) using its IAB coordinates.
		void MapSpeakerPositionToZoneIndex(IABValueX iXCoord, IABValueY iYCoord, IABValueZ iZCoord, uint32_t &oZoneGroupIndex);

		// Trys to match gains to a supported pattern, unmatched pattern will be interpreted as all zones enabled
        void MatchZoneGainPattern(const IABObjectZoneGain9 &iZoneGains);
        
        // Internal function. Choose first valid target group (zone/sub-zone) of speakers.
		// Chosen target speakers are stored targetSpeakers_. Number of speakers in list
		// is sotred in numTargetSpeakers_.
        void SelectTargetSpeakers(std::vector<int32_t> &iFirstPreference,
								  std::vector<int32_t> &iSecondPreference,
			                      std::vector<int32_t> &iThirdPreference);

        // Computes power from source speakers for distribution to other target speakers.
        // iSourceSpeakerIndices list of source speaker indices to compute power from.
        // iNumTargets number of target speakers to distribute the power to.
        // iObjectChannelGains object channel gain array containing actual channel gains.
        float ComputeTargetPowerFromSourceZone(const std::vector<int32_t> &iSourceSpeakerIndices,
                                               uint32_t iNumTargets,
                                               std::vector<float> &iObjectChannelGains);
        
        // Update target speaker gains with power from source zones
        // iTargetSpeakerIndices list of target speaker indices to distribute power to.
        // iPowerPerTarget power to be added to each target speaker.
        // iObjectChannelGains object channel gain array containing actual channel gains.
        void UpdateTargetGains(const std::vector<int32_t> &iTargetSpeakerIndices,
                               float iPowerPerTarget,
                               std::vector<float> &iObjectChannelGains);
        
        // Redistribute power from disabled zones to enabled zones for no back cases
        void RedistributeForNoBack(std::vector<float> &iObjectChannelGains);

        // Redistribute power from disabled zones to enabled zones for no side cases
        void RedistributeForNoSide(std::vector<float> &iObjectChannelGains);

        // Redistribute power from disabled zones to enabled zones for center and back cases
        void RedistributeForCenterBack(std::vector<float> &iObjectChannelGains);

        // Redistribute power from disabled zones to enabled zones for screen only cases
        void RedistributeForScreenOnly(std::vector<float> &iObjectChannelGains);

        // Redistribute power from disabled zones to enabled zones for surround only cases
        void RedistributeForSurroundOnly(std::vector<float> &iObjectChannelGains);

        // Redistribute power from disabled zones to enabled zones for floor only case
        void RedistributeForFloorOnly(std::vector<float> &iObjectChannelGains);

		// *** Private member variables
		//

		// Preset pattern matched by current object zone gain combination
		IABObjectZone9Pattern objectZone9GainPattern_;

		// Internal working variables for selected target group of speakers, in order of design preference.
		// Selection is made through call to internal function SelectTargetSpeakers().
		std::vector<int32_t> targetSpeakers_;
		uint32_t numTargetSpeakers_;

		// Target speakers divided into groups (zones and sub-zones).
		// Map key is a customised group (zone/sub-zone) index for zone9 algorithm, and map value is a 
		// vector of all speaker channel indices (one for each speaker in the config file) belonging to 
		// the group (zone/sub-zone.)
		// The 4 zones that are divided into sub-zone are: LWall, RWall, LOh and ROh.
		// See IABObjectZone9GroupingType enum for range.
		// The map is set up by SetupZoneOutputIndexMap().
		ZoneToOutputIndicesMap zoneGroupOutputIndexMap_;

		// (This map is used to support unit tests only.)
		// Similar to zoneGroupOutputIndexMap_, but without division into sub-zones. As a result, this map 
		// stores all config file speaker channel indices found for each of the original 9 top-level zones.
		ZoneToOutputIndicesMap zoneOutputIndexMap_;

		// Number of output channels/speakers in target config
		uint32_t numOutputChannels_;

		// Flag to indicate if zone instance has been initialised successfully or not
		bool isInitialised_;

		// Vectors to track speaker indices in each group (zone/sub-zone) in Zone9 algorithm. They are initialised with 
		// the renderer configuration during instatiation and only VBAP speakers are used for object zone control.
		std::vector<int32_t> speakerOutputIndicesLeftScreen_;
		std::vector<int32_t> speakerOutputIndicesCenterScreen_;
		std::vector<int32_t> speakerOutputIndicesRightScreen_;
		std::vector<int32_t> speakerOutputIndicesLeftWall_Front_;
		std::vector<int32_t> speakerOutputIndicesLeftWall_Mid_;
		std::vector<int32_t> speakerOutputIndicesLeftWall_Rear_;
		std::vector<int32_t> speakerOutputIndicesRightWall_Front_;
		std::vector<int32_t> speakerOutputIndicesRightWall_Mid_;
		std::vector<int32_t> speakerOutputIndicesRightWall_Rear_;
		std::vector<int32_t> speakerOutputIndicesLeftRear_;
		std::vector<int32_t> speakerOutputIndicesCenterRear_;
		std::vector<int32_t> speakerOutputIndicesRightRear_;
		std::vector<int32_t> speakerOutputIndicesLeftCeiling_Front_;
		std::vector<int32_t> speakerOutputIndicesLeftCeiling_Mid_;
		std::vector<int32_t> speakerOutputIndicesLeftCeiling_Rear_;
		std::vector<int32_t> speakerOutputIndicesCenterCeiling_Front_;
		std::vector<int32_t> speakerOutputIndicesCenterCeiling_Mid_;
		std::vector<int32_t> speakerOutputIndicesCenterCeiling_Rear_;
		std::vector<int32_t> speakerOutputIndicesRightCeiling_Front_;
		std::vector<int32_t> speakerOutputIndicesRightCeiling_Mid_;
		std::vector<int32_t> speakerOutputIndicesRightCeiling_Rear_;

		// The following are used for pattern "Screen Only", with or without overhead.
		std::vector<int32_t> speakerOutputIndicesLeftWall_;
		std::vector<int32_t> speakerOutputIndicesRightWall_;
		std::vector<int32_t> speakerOutputIndicesLeftCeiling_;
		std::vector<int32_t> speakerOutputIndicesRightCeiling_;
	};
    
    /**
     * IAB class for renderer object zone 19 control
	 * (Not implemented)
     */
    class IABObjectZone19 : public virtual IABObjectZones
    {
        
    public:
        
        // Constructor
        IABObjectZone19(const RenderUtils::IRendererConfiguration &iConfig);
        
        // Destructor
        ~IABObjectZone19();
        
        /**
         * Update object channel gains with object zone 19 gains
         *
         * @param[in] iZoneGains zone19 gain.
         *
         * @param[in] iObjectChannelGains vector of object gain to be updated. The caller is responsible for ensuring that the
         * vector size and sequence match the rendered output list.
         */
        iabError ProcessZoneGains(IABObjectZoneGain19 iZoneGains, std::vector<float> &iObjectChannelGains);

    };

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABOBJECTZONES_H__
