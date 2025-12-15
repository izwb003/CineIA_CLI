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
 * IABObjectZones.cpp
 *
 * @file
 */

#include <math.h>

#include "renderer/IABObjectZones/IABObjectZones.h"
#include "common/IABConstants.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    // Configuration speakers are mapped to zones using IAB coordinates converted from
    // their azimuth and elevation angles. This tolerance helps to avoid any issues due
    // to small differences in floating point comparison.
    
    static const float IABZoneMappingTolerance = 0.05f;
    
    // ==================================================
    // IABObjectZone9 class implementation

    // Constructor
    IABObjectZone9::IABObjectZone9(const RenderUtils::IRendererConfiguration &iConfig)
    {
		// Clear all other variables before calling setup
		zoneOutputIndexMap_.clear();
		numOutputChannels_ = 0;
		isInitialised_ = false;

        objectZone9GainPattern_ = kIABObjectZone9_AllZonesEnabled;
		targetSpeakers_.clear();
		numTargetSpeakers_ = 0;

		// Set up zone to output map for all channels/speakers in configSpeakerList
		if (kIABNoError == SetupZoneOutputIndexMap(iConfig))
        {
            isInitialised_ = true;
        }
	}
    
    // Destructor
    IABObjectZone9::~IABObjectZone9()
    {
		zoneOutputIndexMap_.clear();
		numOutputChannels_ = 0;
		isInitialised_ = false;
		targetSpeakers_.clear();
	}
    
	// IABObjectZone9::SetupZoneOutputIndexMap() implementation
	iabError IABObjectZone9::SetupZoneOutputIndexMap(const RenderUtils::IRendererConfiguration &iConfig)
	{
		const std::vector<RenderUtils::RenderSpeaker>*	configSpeakers;
		iConfig.GetPhysicalSpeakers(configSpeakers);

		const std::vector<RenderUtils::RenderSpeaker> &configSpeakerList = *configSpeakers;

		// Report error if iConfigSpeakerList is empty
		if (configSpeakerList.empty())
		{
			return kIABBadArgumentsError;
		}

		// numOutputChannels_ is the size of configSpeakerList
		numOutputChannels_ = static_cast<uint32_t>(configSpeakerList.size());

		// Clear zone9 processing group (zone/sub-zone) output index map
		zoneGroupOutputIndexMap_.clear();

		std::vector<RenderUtils::RenderSpeaker>::const_iterator iter;
		ZoneToOutputIndicesMap::iterator iterZoneMap;
		CoreUtils::Vector3 speakerPos;
		IABValueX speakerIABPosX = 0.0f;
		IABValueY speakerIABPosY = 0.0f;
		IABValueZ speakerIABPosZ = 0.0f;
		IABTransform iabTransform;
		iabError iabReturnCode = kIABNoError;
		uint32_t speakerZoneGroupIndex;
		std::vector<int32_t> outputIndex;

		// Create the map with empty vector and will be filled later
		// kZone9Group_AfterLastGroup represent number of groups (zones and sub-zones) in Zone9 processing algorithm.
		//
		for (uint32_t zone = 0; zone < static_cast<uint32_t>(kZone9Group_AfterLastGroup); zone++)
		{
			zoneGroupOutputIndexMap_[zone] = outputIndex;
		}

		// Search the speaker list and if speaker is a zone speaker, add its output index to the corresponding zone
		// A zone can contain multiple speakers and the output indices are stored in a vector
		for (iter = configSpeakerList.begin(); iter != configSpeakerList.end(); iter++)
		{
			// Exclude non-VBAP speakers
			if (!iConfig.IsVBAPSpeaker(iter->getName()))
			{
				continue;
			}

			// Get speaker position
			speakerPos = iter->getPosition();

			// Convert speaker VBAP coordinates to IAB coordinates
			iabReturnCode = iabTransform.TransformCartesianVBAPToIAB(speakerPos.x, speakerPos.y, speakerPos.z,
				speakerIABPosX, speakerIABPosY, speakerIABPosZ);

			if (kIABNoError != iabReturnCode)
			{
				return iabReturnCode;
			}

			MapSpeakerPositionToZoneIndex(speakerIABPosX, speakerIABPosY, speakerIABPosZ, speakerZoneGroupIndex);
			zoneGroupOutputIndexMap_[speakerZoneGroupIndex].push_back(iter->outputIndex_);
		}

		// Save zone 9 speaker indices
		speakerOutputIndicesLeftScreen_ = zoneGroupOutputIndexMap_[kZone9Group_LeftScreen];
		speakerOutputIndicesCenterScreen_ = zoneGroupOutputIndexMap_[kZone9Group_CenterScreen];
		speakerOutputIndicesRightScreen_ = zoneGroupOutputIndexMap_[kZone9Group_RightScreen];
		speakerOutputIndicesLeftWall_Front_ = zoneGroupOutputIndexMap_[kZone9Group_LeftWall_front];
		speakerOutputIndicesLeftWall_Mid_ = zoneGroupOutputIndexMap_[kZone9Group_LeftWall_mid];
		speakerOutputIndicesLeftWall_Rear_ = zoneGroupOutputIndexMap_[kZone9Group_LeftWall_rear];
		speakerOutputIndicesRightWall_Front_ = zoneGroupOutputIndexMap_[kZone9Group_RightWall_front];
		speakerOutputIndicesRightWall_Mid_ = zoneGroupOutputIndexMap_[kZone9Group_RightWall_mid];
		speakerOutputIndicesRightWall_Rear_ = zoneGroupOutputIndexMap_[kZone9Group_RightWall_rear];
		speakerOutputIndicesLeftRear_ = zoneGroupOutputIndexMap_[kZone9Group_LeftRearWall];
		speakerOutputIndicesCenterRear_ = zoneGroupOutputIndexMap_[kZone9Group_CenterRearWall];
		speakerOutputIndicesRightRear_ = zoneGroupOutputIndexMap_[kZone9Group_RightRearWall];
		speakerOutputIndicesLeftCeiling_Front_ = zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_front];
		speakerOutputIndicesLeftCeiling_Mid_ = zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_mid];
		speakerOutputIndicesLeftCeiling_Rear_ = zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_rear];
		speakerOutputIndicesCenterCeiling_Front_ = zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_front];
		speakerOutputIndicesCenterCeiling_Mid_ = zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_mid];
		speakerOutputIndicesCenterCeiling_Rear_ = zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_rear];
		speakerOutputIndicesRightCeiling_Front_ = zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_front];
		speakerOutputIndicesRightCeiling_Mid_ = zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_mid];
		speakerOutputIndicesRightCeiling_Rear_ = zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_rear];

		// ***** For supporting unit tests only
		// Copy from zoneGroupOutputIndexMap_ to top-level on zoneOutputIndexMap_ map
		zoneOutputIndexMap_.clear();

		// Create the zoneOutputIndexMap_ map with empty vector first
		// Top level map contains 9 zones
		// for (uint32_t zone = 0; zone < 9; zone++)
		// {
		//     zoneOutputIndexMap_[zone] = outputIndex;
		// }

		zoneOutputIndexMap_[kIABObjectZone9_LeftScreen] = zoneGroupOutputIndexMap_[kZone9Group_LeftScreen];
		zoneOutputIndexMap_[kIABObjectZone9_CenterScreen] = zoneGroupOutputIndexMap_[kZone9Group_CenterScreen];
		zoneOutputIndexMap_[kIABObjectZone9_RightScreen] = zoneGroupOutputIndexMap_[kZone9Group_RightScreen];
		zoneOutputIndexMap_[kIABObjectZone9_LeftWall] = zoneGroupOutputIndexMap_[kZone9Group_LeftWall_front];
		zoneOutputIndexMap_[kIABObjectZone9_RightWall] = zoneGroupOutputIndexMap_[kZone9Group_RightWall_front];
		zoneOutputIndexMap_[kIABObjectZone9_LeftRearWall] = zoneGroupOutputIndexMap_[kZone9Group_LeftRearWall];
		zoneOutputIndexMap_[kIABObjectZone9_RightRearWall] = zoneGroupOutputIndexMap_[kZone9Group_RightRearWall];
		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling] = zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_front];
		zoneOutputIndexMap_[kIABObjectZone9_RightCeiling] = zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_front];

		// Add _mid and _ rear to top-level map
		zoneOutputIndexMap_[kIABObjectZone9_LeftWall].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftWall].end()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftWall_mid].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftWall_mid].end());

		zoneOutputIndexMap_[kIABObjectZone9_LeftWall].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftWall].end()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftWall_rear].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftWall_rear].end());

		zoneOutputIndexMap_[kIABObjectZone9_RightWall].insert(zoneOutputIndexMap_[kIABObjectZone9_RightWall].end()
			, zoneGroupOutputIndexMap_[kZone9Group_RightWall_mid].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_RightWall_mid].end());

		zoneOutputIndexMap_[kIABObjectZone9_RightWall].insert(zoneOutputIndexMap_[kIABObjectZone9_RightWall].end()
			, zoneGroupOutputIndexMap_[kZone9Group_RightWall_rear].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_RightWall_rear].end());

		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_mid].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_mid].end());

		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_rear].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_LeftCeiling_rear].end());

		// Additionally append the new center ceiling groups to top level Left ceiling zone (biased)
		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_front].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_front].end());

		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_mid].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_mid].end());

		zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_rear].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_CenterCeiling_rear].end());

		zoneOutputIndexMap_[kIABObjectZone9_RightCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_RightCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_mid].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_mid].end());

		zoneOutputIndexMap_[kIABObjectZone9_RightCeiling].insert(zoneOutputIndexMap_[kIABObjectZone9_RightCeiling].end()
			, zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_rear].begin()
			, zoneGroupOutputIndexMap_[kZone9Group_RightCeiling_rear].end());


		// The following are used for pattern "Screen Only", with or without overhead.
		// Also used during celing energy re-distribution.
		speakerOutputIndicesLeftWall_ = zoneOutputIndexMap_[kIABObjectZone9_LeftWall];
		speakerOutputIndicesRightWall_ = zoneOutputIndexMap_[kIABObjectZone9_RightWall];
		speakerOutputIndicesLeftCeiling_ = zoneOutputIndexMap_[kIABObjectZone9_LeftCeiling];
		speakerOutputIndicesRightCeiling_ = zoneOutputIndexMap_[kIABObjectZone9_RightCeiling];

		return kIABNoError;
	}

	// IABObjectZone9::MapSpeakerPositionToZoneIndex() implementation
	void IABObjectZone9::MapSpeakerPositionToZoneIndex(IABValueX iXCoord, IABValueY iYCoord, IABValueZ iZCoord, uint32_t &oZoneGroupIndex)
	{
		// Developer note:
		// Not checking bounds on the x/y/z incoming coordinates, as the bound of [0, 1.0] is guaranteed by
		// iabTransform.TransformCartesianVBAPToIAB() function

		oZoneGroupIndex = 0;

		// Map speakers to zone 9, at top 9-zone level
		//
		// Note: This map below still applies at 9-zone level, without division into sub-zones.
		//
		// Developer note:
		// 1. Any "interior" coordinates will be mapped to left or right wall

		// Top-level Zones                  Mapping range

		// kZone9Group_LeftScreen           0 <= x < 0.5    y = 0          z = 0
		// kZone9Group_CenterScreen         0 = 0.5         y = 0          z = 0
		// kZone9Group_RightScreen          0.5 < x <= 1    y = 0          z = 0
		// kZone9Group_LeftWall_(ALL)       0 <= x <= 0.5   0 < y < 1      z = 0
		// kZone9Group_RightWall_(ALL)      0.5 < x <= 1    0 < y < 1      z = 0
		// kZone9Group_LeftRearWall         0 <= x <= 0.5   y = 1          z = 0
		// kZone9Group_RightRearWall        0.5 < x <= 1    y = 1          z = 0
		// kZone9Group_LeftCeiling_(ALL)    0 <= x <= 0.5   0 <= y <= 1    z > 0
		// kZone9Group_RightCeiling_(ALL)   0.5 < x <= 1    0 <= y <= 1    z > 0

		// ************ extended mapping into group (sub-zones), only shown where sub-zones are used.
		// Further division into "_front", "_mid" and "_rear" sub-zones are based on following y values

		// Further division to sub-zones    Further mapping range

		// kZone9Group_LeftWall_front       0 < y < 0.33
		// kZone9Group_RightWall_front      0 < y < 0.33
		// kZone9Group_LeftCeiling_front    0 <= y < 0.25
		// kZone9Group_RightCeiling_front   0 <= y < 0.25
		// kZone9Group_LeftWall_mid         0.33 <= y <= 0.67
		// kZone9Group_RightWall_mid        0.33 <= y <= 0.67
		// kZone9Group_LeftCeiling_mid      0.25 <= y <= 0.75
		// kZone9Group_RightCeiling_mid     0.25 <= y <= 0.75
		// kZone9Group_LeftWall_rear        0.67 < y < 1
		// kZone9Group_RightWall_rear       0.67 < y < 1
		// kZone9Group_LeftCeiling_rear     0.75 < y <= 1
		// kZone9Group_RightCeiling_rear    0.75 < y <= 1

		// Define bounds for dividing front, mid, and rear sections.
		const float frontMidBoundFloor = 0.33f;
		const float midRearBoundFloor = 0.67f;

		const float frontMidBoundCeiling = 0.25f;
		const float midRearBoundCeiling = 0.75f;

		// 
		// Added 4 more sub-groups for addressing previous left bias
		//    kZone9Group_CenterRearWall
		//    kZone9Group_CenterCeiling_front
		//    kZone9Group_CenterCeiling_mid
		//    kZone9Group_CenterCeiling_rear

		IABObjectZone9GroupingType zone9GroupIndex = kZone9Group_LeftScreen;

		if (iZCoord >= IABZoneMappingTolerance)
		{
			// Overhead speakers
			if (iXCoord < (0.5 - IABZoneMappingTolerance))
			{
				// In sub-groups left half of ceiling

				if (iYCoord < frontMidBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_LeftCeiling_front;
				}
				else if (iYCoord > midRearBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_LeftCeiling_rear;
				}
				else
				{
					zone9GroupIndex = kZone9Group_LeftCeiling_mid;
				}
			}
			else if (iXCoord > (0.5 + IABZoneMappingTolerance))
			{
				// In sub-groups right half of ceiling

				if (iYCoord < frontMidBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_RightCeiling_front;
				}
				else if (iYCoord > midRearBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_RightCeiling_rear;
				}
				else
				{
					zone9GroupIndex = kZone9Group_RightCeiling_mid;
				}
			}
			else
			{
				// In sub-groups for the center ceiling

				if (iYCoord < frontMidBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_CenterCeiling_front;
				}
				else if (iYCoord > midRearBoundCeiling)
				{
					zone9GroupIndex = kZone9Group_CenterCeiling_rear;
				}
				else
				{
					zone9GroupIndex = kZone9Group_CenterCeiling_mid;
				}
			}
		}
		else    // if (iZCoord >= IABZoneMappingTolerance)
		{
			// Non-overhead speakers
			if (iYCoord < IABZoneMappingTolerance)
			{
				// Screen zones
				if (iXCoord < (0.5 - IABZoneMappingTolerance))
				{
					// In zone screen left of center
					zone9GroupIndex = kZone9Group_LeftScreen;
				}
				else if (iXCoord > (0.5 + IABZoneMappingTolerance))
				{
					// In zone screen right of center
					zone9GroupIndex = kZone9Group_RightScreen;
				}
				else
				{
					// In zone screen center
					zone9GroupIndex = kZone9Group_CenterScreen;
				}
			}
			else if (iYCoord > (1.0 - IABZoneMappingTolerance))
			{
				// Rear wall zones
				if (iXCoord < (0.5 - IABZoneMappingTolerance))
				{
					// In zone left half of rear wall
					zone9GroupIndex = kZone9Group_LeftRearWall;
				}
				else if (iXCoord > (0.5 + IABZoneMappingTolerance))
				{
					// In zone right half of rear wall
					zone9GroupIndex = kZone9Group_RightRearWall;
				}
				else
				{
					zone9GroupIndex = kZone9Group_CenterRearWall;
				}
			}
			else
			{
				// Side wall zones
				if (iXCoord > 0.5f)
				{
					// In zone right wall

					if (iYCoord < frontMidBoundFloor)
					{
						zone9GroupIndex = kZone9Group_RightWall_front;
					}
					else if (iYCoord > midRearBoundFloor)
					{
						zone9GroupIndex = kZone9Group_RightWall_rear;
					}
					else
					{
						zone9GroupIndex = kZone9Group_RightWall_mid;
					}
				}
				else
				{
					// In zone left wall

					if (iYCoord < frontMidBoundFloor)
					{
						zone9GroupIndex = kZone9Group_LeftWall_front;
					}
					else if (iYCoord > midRearBoundFloor)
					{
						zone9GroupIndex = kZone9Group_LeftWall_rear;
					}
					else
					{
						zone9GroupIndex = kZone9Group_LeftWall_mid;
					}
				}
			}
		}

		oZoneGroupIndex = static_cast<uint32_t>(zone9GroupIndex);

	}

	// IABObjectZone9::GetZoneOutputIndexMap() implementation
	const ZoneToOutputIndicesMap& IABObjectZone9::GetZoneOutputIndexMap()
	{
		return zoneOutputIndexMap_;
	}

	// IABObjectZones9::isInitialised() implementation
	bool IABObjectZone9::isInitialised()
	{
		return isInitialised_;
	}

	// IABObjectZone9::MatchZoneGainPattern() implementation
    void IABObjectZone9::MatchZoneGainPattern(const IABObjectZoneGain9 &iZoneGains)
    {
        // Preset to all zones enabled until we find a match
        objectZone9GainPattern_ = kIABObjectZone9_AllZonesEnabled;
        
        // Check if zone gains match a supported pattern
        // If yes, the patterm enum is saved in objectZone9GainPattern_
        
        // Step 1 :  Convert object zone gains to array of 0 and 1
        IABZoneGainPrefixType gainPrefix = kIABZoneGainPrefix_Silence;
        
        uint32_t zoneGains[9];
        
        for (uint32_t i = 0; i < 9; i++)
        {
            gainPrefix = iZoneGains.zoneGains_[i].getIABZoneGainPrefix();
            if (gainPrefix == kIABZoneGainPrefix_Silence)
            {
                zoneGains[i] = 0;
            }
            else if (gainPrefix == kIABZoneGainPrefix_Unity)
            {
                zoneGains[i] = 1;
            }
            else
            {
                // Fractional gain is not supported
                return;
            }
        }
        
        // Step 2: Check if gain pattern is a supported combination/pattern
        for (uint32_t patternIndex = 0; patternIndex < kIABObjectZone9_AllZonesEnabled; patternIndex++)
        {
            uint32_t zoneMatched = 0;

            // Check against each supported pattern in turn
            for (uint32_t i = 0; i < 9; i++)
            {
                if (zoneGains[i] != kSupportedZoneGainPatterns[patternIndex][i])
                {
                    // Found a mismatch, skip rest of this prattern
                    break;
                }
                
                zoneMatched++;
            }
            
            if (zoneMatched == 9)
            {
                // Save pattern index
                objectZone9GainPattern_ = static_cast<IABObjectZone9Pattern>(patternIndex);
                return;
            }
        }
    }

	// IABObjectZone9::SelectTargetSpeakers() implementation
	void IABObjectZone9::SelectTargetSpeakers(std::vector<int32_t> &iFirstPreference,
		                                      std::vector<int32_t> &iSecondPreference,
		                                      std::vector<int32_t> &iThirdPreference)
	{
		// Reset
		targetSpeakers_.clear();
		numTargetSpeakers_ = 0;

		// Assign sub-zone destination in order of preference
		if (iFirstPreference.size() != 0)
		{
			// 1st preference
			targetSpeakers_ = iFirstPreference;
		}
		else if (iSecondPreference.size() != 0)
		{
			// 2nd preference
			targetSpeakers_ = iSecondPreference;
		}
		else if (iThirdPreference.size() != 0)
		{
			// 3rd preference
			targetSpeakers_ = iThirdPreference;
		}
		else;

		// Get size
		numTargetSpeakers_ = static_cast<uint32_t>(targetSpeakers_.size());
	}
    
    // IABObjectZone9::ProcessZoneGains() implementation
    iabError IABObjectZone9::ProcessZoneGains(IABObjectZoneGain9 iZoneGains, std::vector<float> &iObjectChannelGains)
    {
        if ((iZoneGains.objectZoneControl_ == 0) || (!isInitialised_))
        {
            // Return without modifying object channel gains
            return kIABNoError;
        }

        if (iObjectChannelGains.size() != numOutputChannels_)
        {
            return kIABBadArgumentsError;
        }
        
        // Trys to match gains to a supported pattern and save result in objectZone9GainPattern_
        MatchZoneGainPattern(iZoneGains);
        
        if (kIABObjectZone9_AllZonesEnabled == objectZone9GainPattern_)
        {
            // Zone gains do not match a supported pattern, return without modifying object channel gains
            return kIABNoError;
        }

		// Zone9 is now a 2 step-processing.
		//
		// Step 1 - Distribute ceiling energies to floor first for patterns
		// that exclude ceiling (ie. height) speakers
		//
		switch (objectZone9GainPattern_)
		{
		case kIABObjectZone9_NoBackNoOverhead:
		case kIABObjectZone9_NoSideNoOverhead:
		case kIABObjectZone9_CenterBackNoOverhead:
		case kIABObjectZone9_ScreenOnlyNoOverhead:
		case kIABObjectZone9_SurroundNoOverhead:
		case kIABObjectZone9_FloorNoOverhead:
			// Ceiling/heioght to Floor
			RedistributeForFloorOnly(iObjectChannelGains);
			break;

		case kIABObjectZone9_NoBackPlusOverhead:
		case kIABObjectZone9_NoSidePlusOverhead:
		case kIABObjectZone9_CenterBackPlusOverhead:
		case kIABObjectZone9_ScreenOnlyPlusOverhead:
		case kIABObjectZone9_SurroundPlusOverhead:
			// Do nothing
			break;

		default:
			// Not a matched pattern, disable zone 9
			return kIABNoError;
		}

		// Step 2 - Distribute according to floor zone inclusion/exclusion patterns
		// (The patterns are common to pattern pairs with or without ceiling zones.)
		//
		switch (objectZone9GainPattern_)
        {
            case kIABObjectZone9_NoBackNoOverhead:
			case kIABObjectZone9_NoBackPlusOverhead:
				RedistributeForNoBack(iObjectChannelGains);
                break;
                
            case kIABObjectZone9_NoSideNoOverhead:
			case kIABObjectZone9_NoSidePlusOverhead:
				RedistributeForNoSide(iObjectChannelGains);
                break;
                
            case kIABObjectZone9_CenterBackNoOverhead:
			case kIABObjectZone9_CenterBackPlusOverhead:
				RedistributeForCenterBack(iObjectChannelGains);
                break;
                
            case kIABObjectZone9_ScreenOnlyNoOverhead:
			case kIABObjectZone9_ScreenOnlyPlusOverhead:
				RedistributeForScreenOnly(iObjectChannelGains);
                break;
                
            case kIABObjectZone9_SurroundNoOverhead:
			case kIABObjectZone9_SurroundPlusOverhead:
				RedistributeForSurroundOnly(iObjectChannelGains);
                break;
                
            case kIABObjectZone9_FloorNoOverhead:
				// Do nothing (it's already done)
                break;
                
            default:
                return kIABNoError;
        }
        
        return kIABNoError;
    }

    // IABObjectZone9::ComputeTargetPowerFromSourceZone() implementation
    float IABObjectZone9::ComputeTargetPowerFromSourceZone(const std::vector<int32_t> &iSourceSpeakerIndices,
                                                           uint32_t iNumTargets,
                                                           std::vector<float> &iObjectChannelGains)
    {
        float redistPower = 0.0f;
        float gain = 0.0f;
        
        // Calculate source power
        for (size_t index = 0; index < iSourceSpeakerIndices.size(); index++)
        {
            gain = iObjectChannelGains[iSourceSpeakerIndices[index]];
            redistPower += gain * gain;
            
            // Clear source gain
            iObjectChannelGains[iSourceSpeakerIndices[index]] = 0.0f;
        }
        
        redistPower /= iNumTargets;
        return redistPower;
    }
    
    // IABObjectZone9::UpdateTargetGains() implementation
    void IABObjectZone9::UpdateTargetGains(const std::vector<int32_t> &iTargetSpeakerIndices,
                                           float iPowerPerTarget,
                                           std::vector<float> &iObjectChannelGains)
    {
        if (iPowerPerTarget > 0.0f)
        {
            float gain = 0.0f;
            
            for (size_t index = 0; index < iTargetSpeakerIndices.size(); index++)
            {
                gain = iObjectChannelGains[iTargetSpeakerIndices[index]];
                iObjectChannelGains[iTargetSpeakerIndices[index]] = std::sqrt(gain * gain + iPowerPerTarget);
            }
        }
    }

    // IABObjectZone9::RedistributeForNoBack() implementation
    void IABObjectZone9::RedistributeForNoBack(std::vector<float> &iObjectChannelGains)
    {
		// Add center rear group processing, as the first step.
		//
		float redistPowerCenterRearWall = 0.0f;

		// Working group - some targets combine multiple groups
		std::vector<int32_t> aggregateGroup;
		std::vector<int32_t> aggregateGroup_2;
		std::vector<int32_t> aggregateGroup_3;

		// Re-distribute center rear wall first - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesCenterRear_.size())
		{
			// Combine left & right rear wall, as the first preference.
			// (This is the genernal target for center rear group, if present.)
			aggregateGroup = speakerOutputIndicesLeftRear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightRear_.begin(),
				speakerOutputIndicesRightRear_.end());

			// Combine left side rear & right side rear, as the second preference.
			// (Move center rear directly to side_rear, if first preference fails.)
			aggregateGroup = speakerOutputIndicesLeftWall_Rear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightWall_Rear_.begin(),
				speakerOutputIndicesRightWall_Rear_.end());

			// Failing above, combine top level left side & right side, as the third/last preference.
			aggregateGroup = speakerOutputIndicesLeftWall_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightWall_.begin(),
				speakerOutputIndicesRightWall_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				aggregateGroup_2,
				aggregateGroup_3);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterRearWall = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterRear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterRearWall, iObjectChannelGains);
			}
		}

		// Distribute rear speaker power to side wall sub-zones
		float redistPowerLeftRear = 0.0f;
		float redistPowerRightRear = 0.0f;

		// Sub-zone processing
		// Order of target preferences: _rear, _mid, _front

		// Re-distribute left rear zone - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftRear_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesLeftWall_Rear_,
				speakerOutputIndicesLeftWall_Mid_,
				speakerOutputIndicesLeftWall_Front_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftRear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftRear, iObjectChannelGains);
			}
		}

		// Re-distribute right rear zone - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesRightRear_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesRightWall_Rear_,
				speakerOutputIndicesRightWall_Mid_,
				speakerOutputIndicesRightWall_Front_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightRear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightRear, iObjectChannelGains);
			}
		}
    }

    // IABObjectZone9::RedistributeForNoSide() implementation
    void IABObjectZone9::RedistributeForNoSide(std::vector<float> &iObjectChannelGains)
    {
		// Process front, mid, and rear
		float redistPowerLeftWallFront = 0.0f;
		float redistPowerLeftWallMid = 0.0f;
		float redistPowerLeftWallRear = 0.0f;
		float redistPowerRightWallFront = 0.0f;
		float redistPowerRightWallMid = 0.0f;
		float redistPowerRightWallRear = 0.0f;

		// Working group - some targets combine multiple groups
		std::vector<int32_t> aggregateGroup;
		std::vector<int32_t> emptyGroup;
		emptyGroup.clear();

		// Re-distribute left wall front group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftWall_Front_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesLeftScreen_,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftWallFront = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftWall_Front_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftWallFront, iObjectChannelGains);
			}
		}

		// Re-distribute left wall mid group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftWall_Mid_.size())
		{
			// Combine screen left & left rear wall, as the first preference (the only destination)
			aggregateGroup = speakerOutputIndicesLeftScreen_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesLeftRear_.begin(),
				speakerOutputIndicesLeftRear_.end());

			// Add conditional tweak, to prevent everything collapsing to screen left
			//
			if ( (speakerOutputIndicesLeftRear_.size() == 0)
				&& (speakerOutputIndicesLeftWall_Rear_.size() == 0) )
			{
				// Being here means that speakerOutputIndicesLeftWall_Mid_ speakers are
				// actually the rear-most speakers on left side (which is effectively "left-rear" now).
				// To avoid everything collapsing into screen left, do not re-distribute.
				//
				// Forcing numTargetSpeakers_ to be 0.
				numTargetSpeakers_ = 0;
			}
			else
			{
				// Else, call SelectTargetSpeakers() to select target speakers, as normal
				SelectTargetSpeakers(aggregateGroup,
					emptyGroup,
					emptyGroup);
			}

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftWallMid = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftWall_Mid_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftWallMid, iObjectChannelGains);
			}
		}

		// Re-distribute left wall rear group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftWall_Rear_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesLeftRear_,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftWallRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftWall_Rear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftWallRear, iObjectChannelGains);
			}
		}

		// Re-distribute Right wall front group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesRightWall_Front_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesRightScreen_,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightWallFront = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightWall_Front_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightWallFront, iObjectChannelGains);
			}
		}

		// Re-distribute right wall mid group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesRightWall_Mid_.size())
		{
			// Combine screen right & right rear wall, as the first preference (the only destination)
			aggregateGroup = speakerOutputIndicesRightScreen_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightRear_.begin(),
				speakerOutputIndicesRightRear_.end());

			// Add conditional tweak, to prevent everything collapsing to screen right
			//
			if ((speakerOutputIndicesRightRear_.size() == 0)
				&& (speakerOutputIndicesRightWall_Rear_.size() == 0))
			{
				// Being here means that speakerOutputIndicesRightWall_Mid_ speakers are
				// actually the rear-most speakers on right side (which is effectively "right-rear" now).
				// To avoid everything collapsing into screen right, do not re-distribute.
				//
				// Forcing numTargetSpeakers_ to be 0.
				numTargetSpeakers_ = 0;
			}
			else
			{
				// Else, call SelectTargetSpeakers() to select target speakers, as normal
				SelectTargetSpeakers(aggregateGroup,
					emptyGroup,
					emptyGroup);
			}

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightWallMid = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightWall_Mid_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightWallMid, iObjectChannelGains);
			}
		}

		// Re-distribute right wall rear group - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftWall_Rear_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesRightRear_,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightWallRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightWall_Rear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightWallRear, iObjectChannelGains);
			}
		}
    }
    
    // IABObjectZone9::RedistributeForCenterBack() implementation
    void IABObjectZone9::RedistributeForCenterBack(std::vector<float> &iObjectChannelGains)
    {
		// As pattern "CenterBack" is basically "NoSide" followed by "No Screen Left/Right",
		// The implementation is to piggyback RedistributeForNoSide(), and followed by further
		// directing energies from screen left/right to centre zone.
		//

		// Call RedistributeForNoSide() first
		RedistributeForNoSide(iObjectChannelGains);

		// Working variables
		float redistPowerComboLeftRightScreen = 0.0f;

		std::vector<int32_t> combinedScreenLeftRight = speakerOutputIndicesLeftScreen_;
		combinedScreenLeftRight.insert(combinedScreenLeftRight.end(),
			speakerOutputIndicesRightScreen_.begin(),
			speakerOutputIndicesRightScreen_.end());

		// Re-distribute to Center zone, only if source sub-zone contains speakers (energies)
		// Impossible not to have Left/Right, but let's follow the checking
		if (combinedScreenLeftRight.size())
		{
			std::vector<int32_t> emptyGroup;
			emptyGroup.clear();

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesCenterScreen_,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerComboLeftRightScreen = ComputeTargetPowerFromSourceZone(combinedScreenLeftRight,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerComboLeftRightScreen, iObjectChannelGains);
			}
		}
    }
    
    // IABObjectZone9::RedistributeForScreenOnly() implementation
    void IABObjectZone9::RedistributeForScreenOnly(std::vector<float> &iObjectChannelGains)
    {
		// Note: NO changes have been made to the 2x zone9 patterns covered by this method.
		//
		// Note: Add center rear group processing, as the first step.
		//
		float redistPowerCenterRearWall = 0.0f;

		// Working group - some targets combine multiple groups
		std::vector<int32_t> aggregateGroup;
		std::vector<int32_t> emptyGroup;
		emptyGroup.clear();

		// Re-distribute center rear wall first - Only if source zone contains speakers (energies)
		if (speakerOutputIndicesCenterRear_.size())
		{
			// As this is Screen Only pattern, the first preference for center rear is the screen center zone.

			// Combine left & right rear wall, as the second preference.
			// (This is also the genernal target for center rear group.)
			aggregateGroup = speakerOutputIndicesLeftRear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightRear_.begin(),
				speakerOutputIndicesRightRear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesCenterScreen_,
				aggregateGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterRearWall = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterRear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterRearWall, iObjectChannelGains);
			}
		}

        // Redistribute side wall and rear zones to left/right screen zones
        float redistPowerLeftWall = 0.0f;
        float redistPowerRightWall = 0.0f;
        float redistPowerLeftRear = 0.0f;
        float redistPowerRightRear = 0.0f;
        
        // Compute power to be re-distributed
        redistPowerLeftWall = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftWall_, static_cast<uint32_t>(speakerOutputIndicesLeftScreen_.size()), iObjectChannelGains);
        redistPowerRightWall = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightWall_, static_cast<uint32_t>(speakerOutputIndicesRightScreen_.size()), iObjectChannelGains);
        redistPowerLeftRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftRear_, static_cast<uint32_t>(speakerOutputIndicesLeftScreen_.size()), iObjectChannelGains);
        redistPowerRightRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightRear_, static_cast<uint32_t>(speakerOutputIndicesRightScreen_.size()), iObjectChannelGains);

        float powerToLeftScreenTargets = redistPowerLeftWall + redistPowerLeftRear;
        float powerToRightScreenTargets = redistPowerRightWall + redistPowerRightRear;
        
        // Update target speakers
        UpdateTargetGains(speakerOutputIndicesLeftScreen_, powerToLeftScreenTargets, iObjectChannelGains);
        UpdateTargetGains(speakerOutputIndicesRightScreen_, powerToRightScreenTargets, iObjectChannelGains);
    }

    // IABObjectZone9::RedistributeForSurroundOnly() implementation
    void IABObjectZone9::RedistributeForSurroundOnly(std::vector<float> &iObjectChannelGains)
    {
		float redistPowerCenterScreen = 0.0f;
		float redistPowerLeftScreen = 0.0f;
		float redistPowerRightScreen = 0.0f;

		// Working group - some targets combine multiple groups
		std::vector<int32_t> aggregateGroup;
		std::vector<int32_t> emptyGroup;
		emptyGroup.clear();

		// For this pattern of surround-only, energies from all screen zones (left, center and right)
		// are re-distributed to side (wall), in order of preference, to front, then mid, or rear.

		// To ensure that energies from screen center zone be distrbuted evenly to both sides, a pre-step
		// of screen-center to screen-left/right is processed first. This is followed by re-distributing
		// screen-left/right energies to their respective sides, ensuring proper energy movements from
		// all 3 screen zones.
		//

		// Pre-step: re-distribute screen center to screen-left/-right. Only if source zone contains speakers (energies)
		if (speakerOutputIndicesCenterScreen_.size())
		{
			// Combine screen left & left rear wall, as the first preference (the only destination)
			aggregateGroup = speakerOutputIndicesLeftScreen_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightScreen_.begin(),
				speakerOutputIndicesRightScreen_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				emptyGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			//
			// (There is a next-to-impossible scenario where this will not work, that is WHEN/IF
			// screen-left and screen-right zones contain 0 speakers. Except for the target being
			// a mono-center only, it is virtually impossible. In the case where this actually 
			// happens, abort anyway.)
			//
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterScreen = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterScreen_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterScreen, iObjectChannelGains);
			}
		}

		// Re-distribute screen left (now containing part of screen-center). Only if source zone contains speakers (energies)
		if (speakerOutputIndicesLeftScreen_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesLeftWall_Front_,
				speakerOutputIndicesLeftWall_Mid_,
				speakerOutputIndicesLeftWall_Rear_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftScreen = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftScreen_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftScreen, iObjectChannelGains);
			}
		}

		// Re-distribute screen right (now containing part of screen-center). Only if source zone contains speakers (energies)
		if (speakerOutputIndicesRightScreen_.size())
		{
			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesRightWall_Front_,
				speakerOutputIndicesRightWall_Mid_,
				speakerOutputIndicesRightWall_Rear_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightScreen = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightScreen_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightScreen, iObjectChannelGains);
			}
		}
    }

    // IABObjectZone9::RedistributeForFloorOnly() implementation
    void IABObjectZone9::RedistributeForFloorOnly(std::vector<float> &iObjectChannelGains)
    {
		// Distribute ceiling sub-zones to screen left/right and side left/right sub-zones
		// Process front, mid, and rear
		float redistPowerLeftCeilingFront = 0.0f;
		float redistPowerLeftCeilingMid = 0.0f;
		float redistPowerLeftCeilingRear = 0.0f;
		float redistPowerRightCeilingFront = 0.0f;
		float redistPowerRightCeilingMid = 0.0f;
		float redistPowerRightCeilingRear = 0.0f;
		float redistPowerCenterCeilingFront = 0.0f;
		float redistPowerCenterCeilingMid = 0.0f;
		float redistPowerCenterCeilingRear = 0.0f;

		// Working group - some targets combine multiple groups
		std::vector<int32_t> aggregateGroup;
		std::vector<int32_t> aggregateGroup_2;
		std::vector<int32_t> emptyGroup;
		emptyGroup.clear();

		// Re-distribute center ceiling front sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesCenterCeiling_Front_.size())
		{
			// First preference is to go to floor center directly
			//
			// Combine left ceiling and right ceiling fronts, as the second preference
			aggregateGroup = speakerOutputIndicesLeftCeiling_Front_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightCeiling_Front_.begin(),
				speakerOutputIndicesRightCeiling_Front_.end());

			// Combine rest of left ceiling and right ceiling, as the third/last preference
			aggregateGroup_2 = speakerOutputIndicesLeftCeiling_Mid_;
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesRightCeiling_Mid_.begin(),
				speakerOutputIndicesRightCeiling_Mid_.end());
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesLeftCeiling_Rear_.begin(),
				speakerOutputIndicesLeftCeiling_Rear_.end());
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesRightCeiling_Rear_.begin(),
				speakerOutputIndicesRightCeiling_Rear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesCenterScreen_,
				aggregateGroup,
				aggregateGroup_2);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterCeilingFront = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterCeiling_Front_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterCeilingFront, iObjectChannelGains);
			}
		}

		// Re-distribute center ceiling mid sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesCenterCeiling_Mid_.size())
		{
			// Combine all left ceiling and right ceiling, as the first preference
			aggregateGroup = speakerOutputIndicesLeftCeiling_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightCeiling_.begin(),
				speakerOutputIndicesRightCeiling_.end());

			// Combine all left wall and right wall, as the second/last preference
			// This is in case that there is no other ceiling speakers except Overhead/Top
			// In this case, move Overhead irectly to sides (floor)
			aggregateGroup_2 = speakerOutputIndicesLeftWall_;
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesRightWall_.begin(),
				speakerOutputIndicesRightWall_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				aggregateGroup_2,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterCeilingMid = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterCeiling_Mid_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterCeilingMid, iObjectChannelGains);
			}
		}

		// Re-distribute center ceiling rear sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesCenterCeiling_Rear_.size())
		{
			// First preference is to go to floor center surround directly (if present)
			//
			// Combine left ceiling and right ceiling rears, as the second preference
			aggregateGroup = speakerOutputIndicesLeftCeiling_Rear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightCeiling_Rear_.begin(),
				speakerOutputIndicesRightCeiling_Rear_.end());

			// Combine rest of left ceiling and right ceiling, as the third/last preference
			aggregateGroup_2 = speakerOutputIndicesLeftCeiling_Mid_;
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesRightCeiling_Mid_.begin(),
				speakerOutputIndicesRightCeiling_Mid_.end());
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesLeftCeiling_Front_.begin(),
				speakerOutputIndicesLeftCeiling_Front_.end());
			aggregateGroup_2.insert(aggregateGroup_2.end(),
				speakerOutputIndicesRightCeiling_Front_.begin(),
				speakerOutputIndicesRightCeiling_Front_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesCenterRear_,
				aggregateGroup,
				aggregateGroup_2);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerCenterCeilingRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesCenterCeiling_Rear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerCenterCeilingRear, iObjectChannelGains);
			}
		}

		// ********* Now re-distribute Left/Right Celing groups to floor
		// 

		// Re-distribute left ceiling front sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesLeftCeiling_Front_.size())
		{
			// Combine screen-left and LW front, as the first preference
			aggregateGroup = speakerOutputIndicesLeftScreen_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesLeftWall_Front_.begin(),
				speakerOutputIndicesLeftWall_Front_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				speakerOutputIndicesLeftWall_Mid_,
				speakerOutputIndicesLeftWall_Rear_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftCeilingFront = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftCeiling_Front_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftCeilingFront, iObjectChannelGains);
			}
		}

		// Re-distribute left ceiling mid sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesLeftCeiling_Mid_.size())
		{
			// Combine LW front & rear, as the 2nd preference
			aggregateGroup = speakerOutputIndicesLeftWall_Front_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesLeftWall_Rear_.begin(),
				speakerOutputIndicesLeftWall_Rear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesLeftWall_Mid_,
				aggregateGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftCeilingMid = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftCeiling_Mid_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftCeilingMid, iObjectChannelGains);
			}
		}

		// Re-distribute left ceiling rear sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesLeftCeiling_Rear_.size())
		{
			// Combine left rear and LW rear, as the first preference
			aggregateGroup = speakerOutputIndicesLeftRear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesLeftWall_Rear_.begin(),
				speakerOutputIndicesLeftWall_Rear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				speakerOutputIndicesLeftWall_Mid_,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerLeftCeilingRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesLeftCeiling_Rear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerLeftCeilingRear, iObjectChannelGains);
			}
		}

		// Re-distribute Right ceiling front sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesRightCeiling_Front_.size())
		{
			// Combine screen-right and RW front, as the first preference
			aggregateGroup = speakerOutputIndicesRightScreen_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightWall_Front_.begin(),
				speakerOutputIndicesRightWall_Front_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				speakerOutputIndicesRightWall_Mid_,
				speakerOutputIndicesRightWall_Rear_);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightCeilingFront = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightCeiling_Front_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightCeilingFront, iObjectChannelGains);
			}
		}

		// Re-distribute right ceiling mid sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesRightCeiling_Mid_.size())
		{
			// Combine RW front & rear, as the 2nd preference
			aggregateGroup = speakerOutputIndicesRightWall_Front_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightWall_Rear_.begin(),
				speakerOutputIndicesRightWall_Rear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(speakerOutputIndicesRightWall_Mid_,
				aggregateGroup,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightCeilingMid = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightCeiling_Mid_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightCeilingMid, iObjectChannelGains);
			}
		}

		// Re-distribute Right ceiling rear sub-zone - Only if source sub-zone contains speakers (energies)
		if (speakerOutputIndicesRightCeiling_Rear_.size())
		{
			// Combine right rear and RW rear, as the first preference
			aggregateGroup = speakerOutputIndicesRightRear_;
			aggregateGroup.insert(aggregateGroup.end(),
				speakerOutputIndicesRightWall_Rear_.begin(),
				speakerOutputIndicesRightWall_Rear_.end());

			// Call SelectTargetSpeakers() to select target speakers
			SelectTargetSpeakers(aggregateGroup,
				speakerOutputIndicesRightWall_Mid_,
				emptyGroup);

			// Updated: targetSpeakers_ and numTargetSpeakers_

			// Processing energy move if target speakers exist (otherwise, abort and do nothing).
			if (numTargetSpeakers_)
			{
				// Calculate power per target
				redistPowerRightCeilingRear = ComputeTargetPowerFromSourceZone(speakerOutputIndicesRightCeiling_Rear_,
					numTargetSpeakers_,
					iObjectChannelGains);

				// Apply to targets
				UpdateTargetGains(targetSpeakers_, redistPowerRightCeilingRear, iObjectChannelGains);
			}
		}
	}

    // ==================================================
    // End of IABObjectZone9 class methods
    

    // ==================================================
    // IABObjectZone19 class implementation
    
    IABObjectZone19::IABObjectZone19(const RenderUtils::IRendererConfiguration &iConfig)
    {
	}
    
    IABObjectZone19::~IABObjectZone19()
    {
    }
    
    iabError IABObjectZone19::ProcessZoneGains(IABObjectZoneGain19 iZoneGains, std::vector<float> &iObjectChannelGains)
    {
        // To be implemented
        return kIABNotImplementedError;
    }

    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
