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

#include <fstream>
#include <math.h>

#include "gtest/gtest.h"
#include "renderer/IABObjectZones/IABObjectZones.h"
#include "common/IABConstants.h"
#include "renderutils/IRendererConfiguration.h"
#include "testcfg.h"

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    // Pass tolerance for zone gain test. Note that zone gain is quantized to 10-bit
    // Use 1/(2 * 1023) as tolerance
    static const float      zoneGainTolerance = 1.0f/(2*1023.0f);
    
    // Number of zones in IABObjectZoneGain9
    static const uint32_t   numZone9Zones = 9;
    
    // Object zone 9 unit test class
    class IABRendererObjectZone9_Test : public testing::Test
    {
        
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabObjectZone9_ = NULL;
            rendererConfig_ = NULL;
        }
        
        void TearDown()
        {
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
            }
        }
        
        // Set up speaker list and number of output channels from test configuration
        bool SetupSpeakersFromConfigFile(std::string iConfig)
        {
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }

            // Creator renderer configuration instance
            rendererConfig_ = RenderUtils::IRendererConfigurationFile::FromBuffer((char*) iConfig.c_str());
            
            if (rendererConfig_ == NULL)
            {
                return false;
            }
            
            // Get speaker list from renderer configuration
            std::vector<RenderUtils::RenderSpeaker>::const_iterator iter;
            
            rendererConfig_->GetSpeakers(speakerList_);
            
            if (0 == (*speakerList_).size())
            {
                return false;
            }
            
            numOutputChannels_ = static_cast<uint32_t>((*speakerList_).size());
            
            return true;
            
        }
        
        // Test speaker zone assignment for a configuration with 5.1 soundfield
        void TestSpeakerZoneAssignment_51WithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase51));
            ASSERT_EQ(28, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            outputIndex.push_back(6);   // LC
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            outputIndex.push_back(7);   // RC
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(10);  // LSS1
            outputIndex.push_back(11);  // LSS2
            outputIndex.push_back(12);  // LSS3
            outputIndex.push_back(13);  // LSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(16);  // RSS1
            outputIndex.push_back(17);  // RSS2
            outputIndex.push_back(18);  // RSS3
            outputIndex.push_back(19);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(8);  // LRS1
            outputIndex.push_back(9);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(14);  // RRS1
            outputIndex.push_back(15);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            outputIndex.clear();
            outputIndex.push_back(20);  // LTS1
            outputIndex.push_back(21);  // LTS2
            outputIndex.push_back(22);  // LTS3
            outputIndex.push_back(23);  // LTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center
            outputIndex.clear();
            outputIndex.push_back(24);  // RTS1
            outputIndex.push_back(25);  // RTS2
            outputIndex.push_back(26);  // RTS3
            outputIndex.push_back(27);  // RTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;
            
            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Test speaker zone assignment for configuration with a 7.1DS soundfield
        void TestSpeakerZoneAssignment_71DSWithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase71DS));
            ASSERT_EQ(28, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(10);  // LSS1
            outputIndex.push_back(11);  // LSS2
            outputIndex.push_back(12);  // LSS3
            outputIndex.push_back(13);  // LSS$
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(16);  // RSS1
            outputIndex.push_back(17);  // RSS2
            outputIndex.push_back(18);  // RSS3
            outputIndex.push_back(19);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(8);  // LRS1
            outputIndex.push_back(9);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(14);  // RRS1
            outputIndex.push_back(15);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            outputIndex.clear();
            outputIndex.push_back(20);  // LTS1
            outputIndex.push_back(21);  // LTS2
            outputIndex.push_back(22);  // LTS3
            outputIndex.push_back(23);  // LTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center
            outputIndex.clear();
            outputIndex.push_back(24);  // RTS1
            outputIndex.push_back(25);  // RTS2
            outputIndex.push_back(26);  // RTS3
            outputIndex.push_back(27);  // RTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;

            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }
        
        // Test speaker zone assignment for configuration with a 7.1SDS soundfield
        void TestSpeakerZoneAssignment_71SDSWithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase71SDS));
            ASSERT_EQ(28, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            outputIndex.push_back(6);   // LC
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            outputIndex.push_back(7);   // RC
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(10);  // LSS1
            outputIndex.push_back(11);  // LSS2
            outputIndex.push_back(12);  // LSS3
            outputIndex.push_back(13);  // LSS$
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(16);  // RSS1
            outputIndex.push_back(17);  // RSS2
            outputIndex.push_back(18);  // RSS3
            outputIndex.push_back(19);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(8);  // LRS1
            outputIndex.push_back(9);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(14);  // RRS1
            outputIndex.push_back(15);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            outputIndex.clear();
            outputIndex.push_back(20);  // LTS1
            outputIndex.push_back(21);  // LTS2
            outputIndex.push_back(22);  // LTS3
            outputIndex.push_back(23);  // LTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center
            outputIndex.clear();
            outputIndex.push_back(24);  // RTS1
            outputIndex.push_back(25);  // RTS2
            outputIndex.push_back(26);  // RTS3
            outputIndex.push_back(27);  // RTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;
            
            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Test speaker zone assignment for configuration with a 9.1OH soundfield
        void TestSpeakerZoneAssignment_91WithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase91));
            ASSERT_EQ(30, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(12);  // LSS1
            outputIndex.push_back(13);  // LSS2
            outputIndex.push_back(14);  // LSS3
            outputIndex.push_back(15);  // LSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(18);  // RSS1
            outputIndex.push_back(19);  // RSS2
            outputIndex.push_back(20);  // RSS3
            outputIndex.push_back(21);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(10);  // LRS1
            outputIndex.push_back(11);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(16);  // RRS1
            outputIndex.push_back(17);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            outputIndex.clear();
            outputIndex.push_back(22);  // LTS1
            outputIndex.push_back(23);  // LTS2
            outputIndex.push_back(24);  // LTS3
            outputIndex.push_back(25);  // LTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center
            outputIndex.clear();
            outputIndex.push_back(26);  // RTS1
            outputIndex.push_back(27);  // RTS2
            outputIndex.push_back(28);  // RTS3
            outputIndex.push_back(29);  // RTS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;
            
            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Test speaker zone assignment for a configuration with Auro 11.1 soundfield
        void TestSpeakerZoneAssignment_11_1WithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase111));
            ASSERT_EQ(40, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(20);  // LSS1
            outputIndex.push_back(21);  // LSS2
            outputIndex.push_back(22);  // LSS3
            outputIndex.push_back(23);  // LSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(32);  // RSS1
            outputIndex.push_back(33);  // RSS2
            outputIndex.push_back(34);  // RSS3
            outputIndex.push_back(35);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(14);  // LRS1
            outputIndex.push_back(15);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(26);  // RRS1
            outputIndex.push_back(27);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            // All speakers with z > 0 and x <= 0.5 are assigned to this zone
            outputIndex.clear();
            outputIndex.push_back(6);   // CH
            outputIndex.push_back(7);   // LH
            outputIndex.push_back(12);  // LRH1
            outputIndex.push_back(13);  // LRH2
            outputIndex.push_back(16);  // LSH1
            outputIndex.push_back(17);  // LSH2
            outputIndex.push_back(18);  // LSH3
            outputIndex.push_back(19);  // LSH4
            outputIndex.push_back(36);  // TSC1
            outputIndex.push_back(37);  // TSC2
			outputIndex.push_back(38);  // TSC3
			outputIndex.push_back(39);  // TSC4
			expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center, no speaker in zone for this config
            // All speakers with z > 0 and x > 0.5 are assigned to this zone
            outputIndex.clear();
            outputIndex.push_back(9);   // RH
            outputIndex.push_back(24);  // RRH1
            outputIndex.push_back(25);  // RRH2
            outputIndex.push_back(28);  // RSH1
            outputIndex.push_back(29);  // RSH2
            outputIndex.push_back(30);  // RSH3
            outputIndex.push_back(31);  // RSH4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;

            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Test speaker zone assignment for a configuration with Auro 13.1 soundfield
        void TestSpeakerZoneAssignment_13_1WithArray()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase131));
            ASSERT_EQ(42, numOutputChannels_);
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Create expected speaker index list for each zone for this test configuration file
            expectedZoneSpeakerMap_.clear();
            std::vector<int32_t> outputIndex;
            
            // Zone1: Screen left of center
            outputIndex.push_back(0);   // L
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftScreen] = outputIndex;
            
            // Zone2: Screen center
            outputIndex.clear();
            outputIndex.push_back(1);   // C
            expectedZoneSpeakerMap_[kIABObjectZone9_CenterScreen] = outputIndex;
            
            // Zone3: Screen right of center
            outputIndex.clear();
            outputIndex.push_back(2);   // R
            expectedZoneSpeakerMap_[kIABObjectZone9_RightScreen] = outputIndex;
            
            // Zone4: Left wall
            outputIndex.clear();
            outputIndex.push_back(22);  // LSS1
            outputIndex.push_back(23);  // LSS2
            outputIndex.push_back(24);  // LSS3
            outputIndex.push_back(25);  // LSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftWall] = outputIndex;
            
            // Zone5: Right wall
            outputIndex.clear();
            outputIndex.push_back(34);  // RSS1
            outputIndex.push_back(35);  // RSS2
            outputIndex.push_back(36);  // RSS3
            outputIndex.push_back(37);  // RSS4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightWall] = outputIndex;
            
            // Zone6: Rear wall left of center
            outputIndex.clear();
            outputIndex.push_back(16);  // LRS1
            outputIndex.push_back(17);  // LRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_LeftRearWall] = outputIndex;
            
            // Zone7: Rear wall right of center
            outputIndex.clear();
            outputIndex.push_back(28);  // RRS1
            outputIndex.push_back(29);  // RRS2
            expectedZoneSpeakerMap_[kIABObjectZone9_RightRearWall] = outputIndex;
            
            // Zone8: Overhead left of center
            // All speakers with z > 0 and x <= 0.5 are assigned to this zone
            outputIndex.clear();
            outputIndex.push_back(8);   // CH
            outputIndex.push_back(9);   // LH
            outputIndex.push_back(14);  // LRH1
            outputIndex.push_back(15);  // LRH2
            outputIndex.push_back(18);  // LSH1
            outputIndex.push_back(19);  // LSH2
            outputIndex.push_back(20);  // LSH3
            outputIndex.push_back(21);  // LSH4
            outputIndex.push_back(38);  // TSC1
            outputIndex.push_back(39);  // TSC2
			outputIndex.push_back(40);  // TSC3
			outputIndex.push_back(41);  // TSC4
			expectedZoneSpeakerMap_[kIABObjectZone9_LeftCeiling] = outputIndex;
            
            // Zone9: Overhead right of center
            // All speakers with z > 0 and x > 0.5 are assigned to this zone
            outputIndex.clear();
            outputIndex.push_back(11);  // RH
            outputIndex.push_back(26);  // RRH1
            outputIndex.push_back(27);  // RRH2
            outputIndex.push_back(30);  // RSH1
            outputIndex.push_back(31);  // RSH2
            outputIndex.push_back(32);  // RSH3
            outputIndex.push_back(33);  // RSH4
            expectedZoneSpeakerMap_[kIABObjectZone9_RightCeiling] = outputIndex;
            
            // Check zone  9 speaker assignement
            CheckSpeakerZoneAssignment();
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Check speaker zone assignment for current test case
        void CheckSpeakerZoneAssignment()
        {
            // Get the map created by the test instance
            ZoneToOutputIndicesMap actualZoneSpeakerMap;
            actualZoneSpeakerMap = iabObjectZone9_->GetZoneOutputIndexMap();
            
            ASSERT_EQ(actualZoneSpeakerMap.size(), expectedZoneSpeakerMap_.size());
            
            // Compare actual against expected
            
            std::map<IABObjectZone9Type, std::vector<int32_t> >::const_iterator iterExpected;
            ZoneToOutputIndicesMap::iterator iterActual = actualZoneSpeakerMap.begin();
            std::vector<int32_t>::const_iterator iterExpectedSpeakers;
            std::vector<int32_t>::iterator iterActualSpeakers;
            
            for (iterExpected = expectedZoneSpeakerMap_.begin(); iterExpected != expectedZoneSpeakerMap_.end(); iterExpected++, iterActual++)
            {
                // Compare zone index
                EXPECT_EQ((uint32_t)iterExpected->first, iterActual->first);
                
                // Compare speaker list size for this zone
                EXPECT_EQ(iterExpected->second.size(), iterActual->second.size());
                iterActualSpeakers = iterActual->second.begin();
                
                // Compare output index list for each zone
				// Disable the following check. This following check replies on same order in index, even when
				// both sets are identical.
				// Order is not irrelevant.
                // for (iterExpectedSpeakers = iterExpected->second.begin(); iterExpectedSpeakers != iterExpected->second.end(); iterExpectedSpeakers++, iterActualSpeakers++)
                // {
                //     EXPECT_EQ(*iterExpectedSpeakers, *iterActualSpeakers);
                // }
            }
        }

        // Test cases when zone 9 control should not apply
        void TestInactiveZone9Cases()
        {
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase91));
            
            ASSERT_EQ(30, numOutputChannels_);
            
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
            }
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Set object speaker gains to a test pattern
            // For all inactive tests, IABObjectZone9 should return the gains unmodified
            
            std::vector<float> objectChannelGains;

            for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
            {
                objectChannelGains.push_back(testSpeakerGains91WithArray[speakerIndex]);
            }
        
            // Test case 1: Object zone control = 0. Zone 9 control is inactive.
            {
                zoneGain9_.objectZoneControl_ = 0;
                
                // Set zone 9 gains to a valid pattern, use no back no overhead pattern
                
                for (uint32_t zoneIndex = 0; zoneIndex < numZone9Zones; zoneIndex++)
                {
                    if (kSupportedZoneGainPatterns[kIABObjectZone9_NoBackNoOverhead][zoneIndex] == 1)
                    {
                        zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(1.0f);
                    }
                    else
                    {
                        zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(0.0f);
                    }
                }

                EXPECT_EQ(kIABNoError, iabObjectZone9_->ProcessZoneGains(zoneGain9_, objectChannelGains));
                
                for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
                {
                    EXPECT_FLOAT_EQ(testSpeakerGains91WithArray[speakerIndex], objectChannelGains[speakerIndex]);
                }
            }
            
            // Test case 2: Zone gain not a unsupported pattern. Zone 9 control defaults to all zones enabled.
            {
                zoneGain9_.objectZoneControl_ = 1;

                // Set zone 9 gains to a unsupported pattern
                for (uint32_t zoneIndex = 0; zoneIndex < numZone9Zones; zoneIndex++)
                {
                    zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(unsupportedZone9GainsPattern[zoneIndex]);
                }
                
                EXPECT_EQ(kIABNoError, iabObjectZone9_->ProcessZoneGains(zoneGain9_, objectChannelGains));
                
                for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
                {
                    EXPECT_FLOAT_EQ(testSpeakerGains91WithArray[speakerIndex], objectChannelGains[speakerIndex]);
                }
            }
            
            // Test case 3: Gain array contains fractional gains. Zone 9 control defaults to all zones enabled.
            // Speaker gains should be un-modified.
            {
                zoneGain9_.objectZoneControl_ = 1;

                // Set zone 9 gains to a unsupported pattern
                for (uint32_t zoneIndex = 0; zoneIndex < numZone9Zones; zoneIndex++)
                {
                    zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(fractionalZone9GainsPattern[zoneIndex]);
                }
                
                EXPECT_EQ(kIABNoError, iabObjectZone9_->ProcessZoneGains(zoneGain9_, objectChannelGains));
                
                for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
                {
                    EXPECT_FLOAT_EQ(testSpeakerGains91WithArray[speakerIndex], objectChannelGains[speakerIndex]);
                }
            }
            
            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        // Test correct zone control for current gain pattern
        void TestGainPatterns(IABObjectZone9Pattern iGainPatternIndex)
        {
            ASSERT_TRUE(iGainPatternIndex <= kIABObjectZone9_FloorNoOverhead);
            
            // Create renderer config from test configuration
            ASSERT_TRUE(SetupSpeakersFromConfigFile(IABConfigWithUseCase91));
            
            ASSERT_EQ(30, numOutputChannels_);
            
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
            }
            
            // Create IABObjectZone9 instance for testing
            iabObjectZone9_ = new IABObjectZone9(*rendererConfig_);
            
            // Split power equally into each speaker in the configuration
            float testGain = std::sqrt(1.0f/numOutputChannels_);
            
            std::vector<float> objectChannelGains;
            
            for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
            {
                objectChannelGains.push_back(testGain);
            }
            
            zoneGain9_.objectZoneControl_ = 1;
            
            // Set zone gains to the test pattern
            for (uint32_t zoneIndex = 0; zoneIndex < numZone9Zones; zoneIndex++)
            {
                if (kSupportedZoneGainPatterns[iGainPatternIndex][zoneIndex] == 1)
                {
                    zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(1.0f);
                }
                else
                {
                    zoneGain9_.zoneGains_[zoneIndex].setIABZoneGain(0.0f);
                }
            }
            
            // Apply zone 9 control
            ASSERT_EQ(kIABNoError, iabObjectZone9_->ProcessZoneGains(zoneGain9_, objectChannelGains));
            
            // Select expected gains for current test pattern
            const float *expectedSpeakerGains = NULL;
            switch (iGainPatternIndex)
            {
                case kIABObjectZone9_NoBackNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsNoBackNoOverhead[0];
                    break;
                    
                case kIABObjectZone9_NoBackPlusOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsNoBackPlusOverhead[0];
                    break;
                    
                case kIABObjectZone9_NoSideNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsNoSideNoOverhead[0];
                    break;
                    
                case kIABObjectZone9_NoSidePlusOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsNoSidePlusOverhead[0];
                    break;
                    
                case kIABObjectZone9_CenterBackNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsCenterBackNoOverhead[0];
                    break;
                    
                case kIABObjectZone9_CenterBackPlusOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsCenterBackPlusOverhead[0];
                    break;
                    
                case kIABObjectZone9_ScreenOnlyNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsScreenOnlyNoOverhead[0];
                    break;
                    
                case kIABObjectZone9_ScreenOnlyPlusOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsScreenOnlyPlusOverhead[0];
                    break;
                    
                case kIABObjectZone9_SurroundNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsSurroundNoOverhead[0];
                    break;
                    
                case kIABObjectZone9_SurroundPlusOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsSurroundPlusOverhead[0];
                    break;
                    
                case kIABObjectZone9_FloorNoOverhead:
                    expectedSpeakerGains = &expectedSpeakerGainsFloorNoOverhead[0];
                default:
                    break;
                    
            }
            
            // Check returned gain gains against expected values
            
            float diff = 0.0f;
            float totalPower = 0.0f;
            
            for (uint32_t speakerIndex = 0; speakerIndex < numOutputChannels_; speakerIndex++)
            {
                diff = fabsf(objectChannelGains[speakerIndex] - *expectedSpeakerGains++);
                EXPECT_LE(diff, zoneGainTolerance);
                
                // Accumulate power to check overall gain
                totalPower += objectChannelGains[speakerIndex] * objectChannelGains[speakerIndex];
            }
            
            totalPower = std::sqrt(totalPower);
            
            // Overall gain should be unity
            EXPECT_FLOAT_EQ(totalPower, 1.0f);

            // Clean up
            if (iabObjectZone9_)
            {
                delete iabObjectZone9_;
                iabObjectZone9_ = NULL;
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
                rendererConfig_ = NULL;
            }
        }

        
    private:
        
        RenderUtils::IRendererConfiguration             *rendererConfig_;
        const std::vector<RenderUtils::RenderSpeaker>   *speakerList_;
        std::map<IABObjectZone9Type, std::vector<int32_t> > expectedZoneSpeakerMap_;
        IABObjectZoneGain9          zoneGain9_;
        IABObjectZone9              *iabObjectZone9_;
        uint32_t                    numOutputChannels_;
        
        static const float fractionalZone9GainsPattern[9];
        
        // Gain pattern for unsupported pattern test
        static const float unsupportedZone9GainsPattern[9];
        
        // Arbitary speaker gains, for testing only, not power normalised
        static const float testSpeakerGains91WithArray[30];
        
        // The following expected gains are for 9.1OH configuration
        
        // Expected gains after zone 9 processing for no back, no overhead zone gain pattern
        static const float expectedSpeakerGainsNoBackNoOverhead[30];
        
        // Expected gains after zone 9 processing for no back, plus overhead zone gain pattern
        static const float expectedSpeakerGainsNoBackPlusOverhead[30];
        
        // Expected gains after zone 9 processing for no side, no overhead zone gain pattern
        static const float expectedSpeakerGainsNoSideNoOverhead[30];
        
        // Expected gains after zone 9 processing for no side, plus overhead zone gain pattern
        static const float expectedSpeakerGainsNoSidePlusOverhead[30];
        
        // Expected gains after zone 9 processing for center and back, no overhead zone gain pattern
        static const float expectedSpeakerGainsCenterBackNoOverhead[30];
        
        // Expected gains after zone 9 processing for center and back, plus overhead zone gain pattern
        static const float expectedSpeakerGainsCenterBackPlusOverhead[30];
        
        // Expected gains after zone 9 processing for creen only, no overhead zone gain pattern
        static const float expectedSpeakerGainsScreenOnlyNoOverhead[30];
        
        // Expected gains after zone 9 processing for screen only, plus overhead zone gain pattern
        static const float expectedSpeakerGainsScreenOnlyPlusOverhead[30];
        
        // Expected gains after zone 9 processing for surround, no overhead zone gain pattern
        static const float expectedSpeakerGainsSurroundNoOverhead[30];
        
        // Expected gains after zone 9 processing for surround, plus overhead zone gain pattern
        static const float expectedSpeakerGainsSurroundPlusOverhead[30];
        
        // Expected gains after zone 9 processing for floor only, no overhead zone gain pattern
        static const float expectedSpeakerGainsFloorNoOverhead[30];

    };
    
    // ********************
    // Run tests
    // ********************

    // *******************************************************
    // Test the assignment of speakers to the correct zone
    // *******************************************************

    // Test speaker zone assignment for configuration with 5.1 soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_51UseCase)
    {
        TestSpeakerZoneAssignment_51WithArray();
    }

    // Test speaker zone assignment for configuration with 7.1DS soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_71DSUseCase)
    {
        TestSpeakerZoneAssignment_71DSWithArray();
    }
    
    // Test speaker zone assignment for configuration with 7.1SDS soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_71SDSUseCase)
    {
        TestSpeakerZoneAssignment_71SDSWithArray();
    }
    
    // Test speaker zone assignment for configuration with 9.1OH soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_91UseCase)
    {
        TestSpeakerZoneAssignment_91WithArray();
    }
    
    // Test speaker zone assignment for configuration with Auro 11.1 soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_11_1UseCase)
    {
        TestSpeakerZoneAssignment_11_1WithArray();
    }
    
    // Test speaker zone assignment for configuration with Auro 13.1 soundfield/use case
    TEST_F(IABRendererObjectZone9_Test, Test_SpeakerZoneAssignment_13_1UseCase)
    {
        TestSpeakerZoneAssignment_13_1WithArray();
    }

    // *******************************************************
    // Test cases when zone 9 control should not apply
    // *******************************************************

    TEST_F(IABRendererObjectZone9_Test, Test_InactiveCases)
    {
        TestInactiveZone9Cases();
    }

    // *******************************************************
    // Test supported gain patterns with 9.1OH configuration
    // *******************************************************
    
    // Test zone gain pattern : no back, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_NoBackNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_NoBackNoOverhead);
    }

    // Test zone gain pattern : no back, plus overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_NoBackPlusOverhead)
    {
        TestGainPatterns(kIABObjectZone9_NoBackPlusOverhead);
    }
    
    // Test zone gain pattern : no side, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_NoSideNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_NoSideNoOverhead);
    }
    
    // Test zone gain pattern : no side, plus overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_NoSidePlusOverhead)
    {
        TestGainPatterns(kIABObjectZone9_NoSidePlusOverhead);
    }
    
    // Test zone gain pattern : center and back, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_CenterBackNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_CenterBackNoOverhead);
    }
    
    // Test zone gain pattern : center and back, plus overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_CenterBackPlusOverhead)
    {
        TestGainPatterns(kIABObjectZone9_CenterBackPlusOverhead);
    }
    
    // Test zone gain pattern : screen only, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_ScreenOnlyNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_ScreenOnlyNoOverhead);
    }
    
    // Test zone gain pattern : screen only, plus overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_ScreenOnlyPlusOverhead)
    {
        TestGainPatterns(kIABObjectZone9_ScreenOnlyPlusOverhead);
    }
    
    // Test zone gain pattern : surround, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_TestGainPatterns_SurroundNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_SurroundNoOverhead);
    }
    
    // Test zone gain pattern : surround, plus overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_SurroundPlusOverhead)
    {
        TestGainPatterns(kIABObjectZone9_SurroundPlusOverhead);
    }
    
    // Test zone gain pattern : floor only, no overhead
    TEST_F(IABRendererObjectZone9_Test, Test_GainPattern_FloorNoOverhead)
    {
        TestGainPatterns(kIABObjectZone9_FloorNoOverhead);
    }
    
    // Gain pattern for fractional gain test
    const float IABRendererObjectZone9_Test::fractionalZone9GainsPattern[9] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    
    // Gain pattern for unsupported pattern test
    const float IABRendererObjectZone9_Test::unsupportedZone9GainsPattern[9] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    
    // Arbitary speaker gains, for testing only, not power normalised
    const float IABRendererObjectZone9_Test::testSpeakerGains91WithArray[30] = {
        0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
        0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
        0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    
    // The following expected speaker gains are for used in supported zone gain pattern tests. Test configuration is 9.1OH.
    // For the 9.1 configuration, there are 30 speakers and power is split equally into them.
    // Power in each speaker = 1.0/30 = 0.182574198f, i.e. all speakers start with this value before zone 9 processng.
    // In the following tables, speakers with value 0.182574198f are unmodified by zone 9 processing and speakers in
    // the disabled zones are set to 0. Speakers with gains > 0.182574198f received gains from other disabled zones.
    
    // 9.1OH speaker index for reference, speakers marked with * are non-VBAP speakers. Object rendering does not use non-VBAP speakers.
    //    0     L
    //    1     C
    //    2     R
    //    3     LSS*
    //    4     RSS*
    //    5     LRS*
    //    6     RRS*
    //    7     LFE*
    //    8     LTS*
    //    9     RTS*
    //    10    LRS1
    //    11    LRS2
    //    12    LSS1
    //    13    LSS2
    //    14    LSS3
    //    15    LSS4
    //    16    RRS1
    //    17    RRS2
    //    18    RSS1
    //    19    RSS2
    //    20    RSS3
    //    21    RSS4
    //    22    LTS1
    //    23    LTS2
    //    24    LTS3
    //    25    LTS4
    //    26    RTS1
    //    27    RTS2
    //    28    RTS3
    //    29    RTS4

    
    // Expected gains after zone 9 processing for no back, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsNoBackNoOverhead[30] = {
		0.223606810f,
		0.182574198f,
		0.223606810f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.0f,
		0.0f,
		0.223606810f,
		0.258198917f,
		0.258198917f,
		0.365148395f,
		0.0f,
		0.0f,
		0.223606810f,
		0.258198917f,
		0.258198917f,
		0.365148395f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};

    // Expected gains after zone 9 processing for no back, plus overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsNoBackPlusOverhead[30] = {
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.316227794f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.316227794f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f };
    
    // Expected gains after zone 9 processing for no side, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsNoSideNoOverhead[30] = {
		0.380058527f,
		0.182574198f,
		0.380058527f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.333333373f,
		0.333333373f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.333333373f,
		0.333333373f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
    
    // Expected gains after zone 9 processing for no side, plus overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsNoSidePlusOverhead[30] = {
		0.298142433f,
		0.182574198f,
		0.298142433f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.268741965f,
		0.268741965f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.268741965f,
		0.268741965f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f
	};
    
    // Expected gains after zone 9 processing for center and back, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsCenterBackNoOverhead[30] = {
		0.0f,
		0.567646265f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.333333373f,
		0.333333373f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.333333373f,
		0.333333373f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
    
    // Expected gains after zone 9 processing for center and back, plus overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsCenterBackPlusOverhead[30] = {
		0.0f,
		0.459468335f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.268741965f,
		0.268741965f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.268741965f,
		0.268741965f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f
	};
    
    // Expected gains after zone 9 processing for screen only, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsScreenOnlyNoOverhead[30] = {
		0.605530083f,
		0.182574198f,
		0.605530083f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
    
    // Expected gains after zone 9 processing for screen only, plus overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsScreenOnlyPlusOverhead[30] = {
		0.483045936f,
		0.182574198f,
		0.483045936f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f
	};
    
    // Expected gains after zone 9 processing for surround, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsSurroundNoOverhead[30] = {
		0.0f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.210818529f,
		0.210818529f,
		0.341565073f,
		0.258198917f,
		0.258198917f,
		0.210818529f,
		0.210818529f,
		0.210818529f,
		0.341565073f,
		0.258198917f,
		0.258198917f,
		0.210818529f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
    
    // Expected gains after zone 9 processing for surround, plus overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsSurroundPlusOverhead[30] = {
		0.0f,
		0.0f,
		0.0f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.288675159f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.288675159f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f
	};
    
    // Expected gains after zone 9 processing for floor only, no overhead zone gain pattern
    const float IABRendererObjectZone9_Test::expectedSpeakerGainsFloorNoOverhead[30] = {
		0.223606810f,
		0.182574198f,
		0.223606810f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.182574198f,
		0.210818529f,
		0.210818529f,
		0.223606810f,
		0.258198917f,
		0.258198917f,
		0.210818529f,
		0.210818529f,
		0.210818529f,
		0.223606810f,
		0.258198917f,
		0.258198917f,
		0.210818529f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
}
