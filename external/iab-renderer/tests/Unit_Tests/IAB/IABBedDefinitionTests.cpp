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
#include "common/IABElements.h"
#include <vector>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    
    // IABBedDefinition element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABBedDefiniton_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {

            bedDefinitionMetaID_ = 123;
            bedChannelAudioDataID_ = 1;  // use same ID for all bed channels to simplify testing
            
            // These members will be changed per test case
            conditionalBed_ = 1;
            bedUseCase_ = kIABUseCase_7_1_DS;
            audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
            audioDescription_.audioDescriptionText_.clear();
			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second
			sampleRate_ = kIABSampleRate_48000Hz;
			bitDepth_ = kIABBitDepth_24Bit;

			iabPackerBedDefinition_ = NULL;
			iabParserBedDefinition_ = NULL;
		}
        
		// **********************************************
        // IABBedDefinition element setters and getters API tests
        // **********************************************
        
        void TestSetterGetterAPIs()
        {
            IABBedDefinitionInterface* bedDefinitionInterface = NULL;
            bedDefinitionInterface = IABBedDefinitionInterface::Create(frameRate_);
            ASSERT_TRUE(NULL != bedDefinitionInterface);
            
            // Test metadata ID
            IABMetadataIDType metaID = 1;
            bedDefinitionInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0);   // expect to default to zero
            
            // Set metadata ID and verify
            EXPECT_EQ(bedDefinitionInterface->SetMetadataID(0xFFFFFFFF), kIABNoError);
            bedDefinitionInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0xFFFFFFFF);
            
            // Test conditional bed
            // Check default
            uint1_t conditionalBed = 0;
            bedDefinitionInterface->GetConditionalBed(conditionalBed);
            EXPECT_EQ(conditionalBed, 0);   // Default to 0
            
            // Set to zero and verify
            EXPECT_EQ(bedDefinitionInterface->SetConditionalBed(0), kIABNoError);
            bedDefinitionInterface->GetConditionalBed(conditionalBed);
            EXPECT_EQ(conditionalBed, 0);
            
            // Test bed use case
            // Check default value
            IABUseCaseType bedUseCase;
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_9_1_OH);
            
            // Set to kIABUseCase_5_1 and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_5_1), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_5_1);
            
            // Set to kIABUseCase_7_1_DS and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_7_1_DS), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_7_1_DS);
            
            // Set to kIABUseCase_7_1_SDS and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_7_1_SDS), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_7_1_SDS);
            
            // Set to kIABUseCase_11_1_HT and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_11_1_HT), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_11_1_HT);
            
            // Set to kIABUseCase_13_1_HT and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_13_1_HT), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_13_1_HT);
            
            // Set to kIABUseCase_9_1_OH and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_9_1_OH), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_9_1_OH);
            
            // Set to kIABUseCase_Always and verify
            EXPECT_EQ(bedDefinitionInterface->SetBedUseCase(kIABUseCase_Always), kIABNoError);
            bedDefinitionInterface->GetBedUseCase(bedUseCase);
            EXPECT_EQ(bedUseCase, kIABUseCase_Always);
            
            // Test bed channels
            // After added to bedDefinition with SetBedChannels call, these IABChannels will be deleted by IABBedDefinition destructor
            // No need to explicitly delete them at end of test
            
            IABChannel *channelL = new IABChannel();
            IABChannel *channelLc = new IABChannel();
            IABChannel *channelC = new IABChannel();
            IABChannel *channelRc = new IABChannel();
            IABChannel *channelR = new IABChannel();
            IABChannel *channelLs = new IABChannel();
            IABChannel *channelLss = new IABChannel();
            IABChannel *channelLsr = new IABChannel();
            IABChannel *channelRsr = new IABChannel();
            IABChannel *channelRs = new IABChannel();
            IABChannel *channelRss = new IABChannel();
            IABChannel *channelLts = new IABChannel();
            IABChannel *channelRts = new IABChannel();
            IABChannel *channelLFE = new IABChannel();
            IABChannel *channelRh = new IABChannel();
            IABChannel *channelCh = new IABChannel();
            IABChannel *channelLsh = new IABChannel();
            IABChannel *channelRsh = new IABChannel();
            IABChannel *channelLssh = new IABChannel();
            IABChannel *channelRssh = new IABChannel();
            IABChannel *channelLrsh = new IABChannel();
            IABChannel *channelRrsh = new IABChannel();
            IABChannel *channelTs = new IABChannel();
            
            channelL->SetChannelID(kIABChannelID_Left);
            channelLc->SetChannelID(kIABChannelID_LeftCenter);
            channelC->SetChannelID(kIABChannelID_Center);
            channelRc->SetChannelID(kIABChannelID_RightCenter);
            channelR->SetChannelID(kIABChannelID_Right);
            channelLs->SetChannelID(kIABChannelID_LeftSurround);
            channelLss->SetChannelID(kIABChannelID_LeftSideSurround);
            channelLsr->SetChannelID(kIABChannelID_LeftRearSurround);
            channelRsr->SetChannelID(kIABChannelID_RightRearSurround);
            channelRs->SetChannelID(kIABChannelID_RightSurround);
            channelRss->SetChannelID(kIABChannelID_RightSideSurround);
            channelLts->SetChannelID(kIABChannelID_LeftTopSurround);
            channelRts->SetChannelID(kIABChannelID_RightTopSurround);
            channelLFE->SetChannelID(kIABChannelID_LFE);
            channelRh->SetChannelID(kIABChannelID_RightHeight);
            channelCh->SetChannelID(kIABChannelID_CenterHeight);
            channelLsh->SetChannelID(kIABChannelID_LeftSurroundHeight);
            channelRsh->SetChannelID(kIABChannelID_RightSurroundHeight);
            channelLssh->SetChannelID(kIABChannelID_LeftSideSurroundHeight);
            channelRssh->SetChannelID(kIABChannelID_RightSideSurroundHeight);
            channelLrsh->SetChannelID(kIABChannelID_LeftRearSurroundHeight);
            channelRrsh->SetChannelID(kIABChannelID_RightRearSurroundHeight);
            channelTs->SetChannelID(kIABChannelID_TopSurround);
            
            std::vector<IABChannel*> bedChannels;
            std::vector<IABChannel*> bedChannelsGet;
            
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelLc);
            bedChannels.push_back(channelC);
            bedChannels.push_back(channelRc);
            bedChannels.push_back(channelR);
            bedChannels.push_back(channelLs);
            bedChannels.push_back(channelLss);
            bedChannels.push_back(channelLsr);
            bedChannels.push_back(channelRsr);
            bedChannels.push_back(channelRs);
            bedChannels.push_back(channelRss);
            bedChannels.push_back(channelLts);
            bedChannels.push_back(channelRts);
            bedChannels.push_back(channelLFE);
            bedChannels.push_back(channelRh);
            bedChannels.push_back(channelCh);
            bedChannels.push_back(channelLsh);
            bedChannels.push_back(channelRsh);
            bedChannels.push_back(channelLssh);
            bedChannels.push_back(channelRssh);
            bedChannels.push_back(channelLrsh);
            bedChannels.push_back(channelRrsh);
            bedChannels.push_back(channelTs);
            
            IABChannelCountType channelCount = 0;
            
            // check default
            bedDefinitionInterface->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 0);
            
            bedDefinitionInterface->GetBedChannels(bedChannelsGet);
            EXPECT_EQ(bedChannelsGet.size(), 0);
            
            // Set and Get
            EXPECT_EQ(bedDefinitionInterface->SetBedChannels(bedChannels), kIABNoError);
            bedDefinitionInterface->GetBedChannels(bedChannelsGet);
            
            // Check count
            bedDefinitionInterface->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 23);
            EXPECT_EQ(bedChannelsGet.size(), 23);
            
            // Check returned channel list
            IABChannelIDType id;
            bedChannelsGet[0]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_Left);
            
            bedChannelsGet[1]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftCenter);
            
            bedChannelsGet[2]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_Center);
            
            bedChannelsGet[3]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightCenter);
            
            bedChannelsGet[4]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_Right);
            
            bedChannelsGet[5]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftSurround);
            
            bedChannelsGet[6]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftSideSurround);
            
            bedChannelsGet[7]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftRearSurround);
            
            bedChannelsGet[8]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightRearSurround);
            
            bedChannelsGet[9]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightSurround);
            
            bedChannelsGet[10]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightSideSurround);
            
            bedChannelsGet[11]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftTopSurround);
            
            bedChannelsGet[12]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightTopSurround);
            
            bedChannelsGet[13]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LFE);
            
            bedChannelsGet[14]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightHeight);
            
            bedChannelsGet[15]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_CenterHeight);
            
            bedChannelsGet[16]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftSurroundHeight);
            
            bedChannelsGet[17]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightSurroundHeight);
            
            bedChannelsGet[18]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftSideSurroundHeight);
            
            bedChannelsGet[19]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightSideSurroundHeight);
            
            bedChannelsGet[20]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_LeftRearSurroundHeight);
            
            bedChannelsGet[21]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_RightRearSurroundHeight);
            
            bedChannelsGet[22]->GetChannelID(id);
            EXPECT_EQ(id, kIABChannelID_TopSurround);
            
            // Valid sub-element type for bed definition, these will be deleted by IABBedDefinition destructor
            IABBedDefinition *bedDefinitionElement = new IABBedDefinition(frameRate_);
            IABBedRemap *remapElement = new IABBedRemap(5, 7, frameRate_);
            
            // Invalid sub-element type for bed definition
            IABFrame *frameElement = new IABFrame();
            IABObjectZoneDefinition19 *objectZoneElement = new IABObjectZoneDefinition19(frameRate_);
            IABObjectDefinition *objectDefinitionElement = new IABObjectDefinition(frameRate_);
			IABAudioDataDLC *dlcAudioElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRate_, sampleRate_));
            IABAudioDataPCM *pcmAudioElement = new IABAudioDataPCM(frameRate_, sampleRate_, bitDepth_);

            IABElementCountType count;
            std::vector<IABElement*> subElements;
            std::vector<IABElement*> invalidSubElements;
            std::vector<IABElement*> subElementsGet;
            
            // check default
            bedDefinitionInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 0);
            
            bedDefinitionInterface->GetSubElements(subElementsGet);
            EXPECT_EQ(subElementsGet.size(), 0);
            
            // Invalid sub element tests
            invalidSubElements.push_back(frameElement);
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(objectDefinitionElement);
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(objectZoneElement);
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(dlcAudioElement);
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(pcmAudioElement);
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            // Valid sub element tests
            subElements.push_back(bedDefinitionElement);
            subElements.push_back(remapElement);
            
            EXPECT_EQ(bedDefinitionInterface->SetSubElements(subElements), kIABNoError);
            bedDefinitionInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 2);
            
            bedDefinitionInterface->GetSubElements(subElementsGet);
            EXPECT_EQ(subElementsGet.size(), 2);
            
            IABElementIDType elementID;
            subElementsGet[0]->GetElementID(elementID);
            EXPECT_EQ(elementID, kIABElementID_BedDefinition);
            subElementsGet[1]->GetElementID(elementID);
            EXPECT_EQ(elementID, kIABElementID_BedRemap);
            
            //UpdateSubElements private method test
            // Set 2 valid sub-elements first
            std::vector<IABElement*> validsubelements_1;
            validsubelements_1.push_back(bedDefinitionElement);
            validsubelements_1.push_back(remapElement);
            
            EXPECT_EQ(kIABNoError, bedDefinitionInterface->SetSubElements(validsubelements_1));
            
            // set 1 valid sub-element
            std::vector<IABElement*> validsubelements_2;
            validsubelements_2.push_back(remapElement);
            
            EXPECT_EQ(kIABNoError, bedDefinitionInterface->SetSubElements(validsubelements_2));
            
            // verify that only second set valid sub-elements present.
            std::vector<IABElement*> ret_subelments;
            bedDefinitionInterface->GetSubElements(ret_subelments);
            EXPECT_EQ(remapElement, ret_subelments.at(0));
            EXPECT_EQ(1, ret_subelments.size());
            
            // Duplicate test
            std::vector<IABElement*> duplicate_subelements;
            duplicate_subelements.push_back(remapElement);
            duplicate_subelements.push_back(remapElement);
            EXPECT_EQ(kIABDuplicateSubElementsError, bedDefinitionInterface->SetSubElements(duplicate_subelements));
            
            // Delete invalid sub elements that have not been added to the frame
            
            delete frameElement;
            frameElement = NULL;
            
            delete objectDefinitionElement;
            objectDefinitionElement = NULL;
            
            delete objectZoneElement;
            objectZoneElement = NULL;
            
            delete dlcAudioElement;
            dlcAudioElement = NULL;
            
            delete pcmAudioElement;
            pcmAudioElement = NULL;
            
            // Test audio description
            
            IABAudioDescription audioDescription;
            IABAudioDescription audioDescriptionGet;
            
            audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
            audioDescription.audioDescriptionText_.clear();
            
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_NotIndicated, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Dialog;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Dialog, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Music;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Music, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Effects;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Effects, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Foley;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Foley, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Ambience;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Ambience, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_TextInStream;
            std::string desc("audio description contents");
            std::string desc2;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_TextInStream, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));
            
            IABAudioDescriptionType combinedCode1 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Dialog + kIABAudioDescription_TextInStream);
            audioDescription.audioDescription_ = combinedCode1;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(combinedCode1, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));
            
            IABAudioDescriptionType combinedCode2 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Music + kIABAudioDescription_TextInStream);
            audioDescription.audioDescription_ = combinedCode2;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(bedDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            bedDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(combinedCode2, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));

			// Test dynamic packing flag
			// Default = true
			EXPECT_EQ(bedDefinitionInterface->IsIncludedForPacking(), true);

			bedDefinitionInterface->DisablePacking();
			EXPECT_EQ(bedDefinitionInterface->IsIncludedForPacking(), false);

			bedDefinitionInterface->EnablePacking();
			EXPECT_EQ(bedDefinitionInterface->IsIncludedForPacking(), true);

            IABBedDefinitionInterface::Delete(bedDefinitionInterface);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************

        void SetupPackerBedDefinition()
        {
            ASSERT_EQ(iabPackerBedDefinition_->SetMetadataID(bedDefinitionMetaID_), kIABNoError);
            ASSERT_EQ(iabPackerBedDefinition_->SetConditionalBed(conditionalBed_), kIABNoError);
            ASSERT_EQ(iabPackerBedDefinition_->SetBedUseCase(bedUseCase_), kIABNoError);

            // Test 5.1, 7.1 and 9.1 only
            switch (bedUseCase_)
            {
                case kIABUseCase_5_1:
                    SetupPackerBed51Channels();
                    break;

                case kIABUseCase_9_1_OH:
                    SetupPackerBed91Channels();
                    break;

                // Set up 7.1 channels in bitstream for other use case
                case kIABUseCase_7_1_DS:
                default:
                    SetupPackerBed71Channels();
                    
            }
            
            // Add audio description: no description in bitstream
            ASSERT_EQ(iabPackerBedDefinition_->SetAudioDescription(audioDescription_), kIABNoError);
            
            // Set up bed definition sub-elements. Note that we are testing serialize/de-serialize only. No actual downmix test
            std::vector<IABElement*> bedSubElements;
            
            // Valid sub-element type for bed definition, these will be deleted by IABBedDefinition destructor
            IABBedDefinition *bedDefinitionSubElement = new IABBedDefinition(frameRate_);
            
            // Set meta data IDto an arbitrary value of 123
            bedDefinitionSubElement->SetMetadataID(123);
            // Set up a two channel bed definition for basic checks
            
            IABChannel *channelL = new IABChannel();
            IABChannel *channelR = new IABChannel();
            
            std::vector<IABChannel*> bedChannels;
            
            // Use the same audio ID for all bed channels
            channelL->SetChannelID(kIABChannelID_Left);
            channelL->SetAudioDataID(bedChannelAudioDataID_);
            
            channelR->SetChannelID(kIABChannelID_Right);
            channelR->SetAudioDataID(bedChannelAudioDataID_);
            
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelR);
            
            ASSERT_EQ(bedDefinitionSubElement->SetBedChannels(bedChannels), kIABNoError);
            
            bedSubElements.push_back(bedDefinitionSubElement);
            
            // Set up a bed remap element
            uint8_t numPanSubBlocks = 8;            // 8 subBlocks for 24FPS
            uint8_t numSourceChannels = 8;          // 7.1 source
            uint8_t numDestinationChannels = 6;     // 5.1 destination
            IABBedRemap *remapSubElement = new IABBedRemap(numDestinationChannels, numSourceChannels, frameRate_);
            
            // Create a vector to store a 7.1 channel list to use as destination channels
            std::vector<IABChannelIDType> destinationList;
            destinationList.push_back(kIABChannelID_Left);
            destinationList.push_back(kIABChannelID_Center);
            destinationList.push_back(kIABChannelID_Right);
            destinationList.push_back(kIABChannelID_LeftSideSurround);
            destinationList.push_back(kIABChannelID_RightSideSurround);
            destinationList.push_back(kIABChannelID_LeftRearSurround);
            destinationList.push_back(kIABChannelID_RightRearSurround);
            destinationList.push_back(kIABChannelID_LFE);

            // Set meta data ID to 456, an arbitrary test value
            EXPECT_EQ(remapSubElement->SetMetadataID(456), kIABNoError);
            
            // Set use case to target config
            EXPECT_EQ(remapSubElement->SetRemapUseCase(kIABUseCase_5_1), kIABNoError);
            
            std::vector<IABBedRemapSubBlock*> bedRemapSubBlocksSet;
            std::vector<IABRemapCoeff*> remapCoeffArray;
            std::vector<IABGain> remapGainCoeffs;
            
            // Test setters and set up a bed remap for serialisation, use 8 subBlocks for 24FPS
            for (uint16_t i = 0; i < numPanSubBlocks; i++)
            {
                // NOTE: These will be deleted by IABBedRemap destructor, no need to delete them explictly at end of test
                IABBedRemapSubBlock* bedRemapSubBlock = new IABBedRemapSubBlock(numDestinationChannels, numSourceChannels);
                
                // Set exist flag
                EXPECT_EQ(bedRemapSubBlock->SetRemapInfoExists(1), kIABNoError);
                
                remapCoeffArray.clear();
                
                // Iterator for destination channel list
                std::vector<IABChannelIDType>::iterator iterDest = destinationList.begin();
                
                for (uint8_t j = 0; j < numDestinationChannels; j++)
                {
                    // Gain coeffiecent array for each destination channel
                    IABRemapCoeff *destChannelRemapCoeff = new IABRemapCoeff();
                    remapGainCoeffs.clear();
                    IABGain gainCoeff;
                    float gain = 0.1f;
                    
                    // Set up some non-zero gains at 0.1f increments
                    for (uint8_t k = 0; k < numSourceChannels; k++)
                    {
                        gainCoeff.setIABGain(gain);
                        remapGainCoeffs.push_back(gainCoeff);
                        gain += 0.1f;
                    }
                    
                    // Set destination channel ID
                    EXPECT_EQ(destChannelRemapCoeff->setDestinationChannelID(*iterDest++), kIABNoError);
                    
                    // Add gain array
                    EXPECT_EQ(destChannelRemapCoeff->setRemapCoeffVector(remapGainCoeffs), kIABNoError);
                    
                    // Add destination to remap coeff array
                    remapCoeffArray.push_back(destChannelRemapCoeff);
                }
                
                // Add coefficient array to curent subBlock
                EXPECT_EQ(bedRemapSubBlock->SetRemapCoeffArray(remapCoeffArray), kIABNoError);
                
                // Check added array
                EXPECT_EQ(bedRemapSubBlock->checkRemapCoeffArray(), kIABNoError);
                
                // Add remap subBlock to vector
                bedRemapSubBlocksSet.push_back(bedRemapSubBlock);
                
            }
            
            // Add remap subBlocks to bed remap element, all set up for serialisation
            EXPECT_EQ(remapSubElement->SetRemapSubBlocks(bedRemapSubBlocksSet), kIABNoError);
            
            // Add remap element to bed sub-element vector
            bedSubElements.push_back(remapSubElement);
            
            // Add sub-elements to bed definition
            IABElementCountType count;
            EXPECT_EQ(iabPackerBedDefinition_->SetSubElements(bedSubElements), kIABNoError);
            iabPackerBedDefinition_->GetSubElementCount(count);
            ASSERT_EQ(count, 2);
        }
        
        void SetupPackerBed51Channels()
        {
            // Set up for 5.1 bed
            // After added to bedDefinition with SetBedChannels call, these IABChannels will be deleted by IABBedDefinition destructor
            // No need to explicitly delete them at end of test
            
            IABChannel *channelL = new IABChannel();
            IABChannel *channelR = new IABChannel();
            IABChannel *channelC = new IABChannel();
            IABChannel *channelLs = new IABChannel();
            IABChannel *channelRs = new IABChannel();
            IABChannel *channelLFE = new IABChannel();
            
            std::vector<IABChannel*> bedChannels;
            
            // Use the same audio ID for all bed channels
            channelL->SetChannelID(kIABChannelID_Left);
            channelL->SetAudioDataID(bedChannelAudioDataID_);
            
            channelR->SetChannelID(kIABChannelID_Right);
            channelR->SetAudioDataID(bedChannelAudioDataID_);
            
            channelC->SetChannelID(kIABChannelID_Center);
            channelC->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLs->SetChannelID(kIABChannelID_LeftSurround);
            channelLs->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRs->SetChannelID(kIABChannelID_RightSurround);
            channelRs->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLFE->SetChannelID(kIABChannelID_LFE);
            channelLFE->SetAudioDataID(bedChannelAudioDataID_);
            
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelR);
            bedChannels.push_back(channelC);
            bedChannels.push_back(channelLs);
            bedChannels.push_back(channelRs);
            bedChannels.push_back(channelLFE);
            
            IABChannelCountType channelCount = 0;
            ASSERT_EQ(iabPackerBedDefinition_->SetBedChannels(bedChannels), kIABNoError);
            iabPackerBedDefinition_->GetChannelCount(channelCount);
            ASSERT_EQ(channelCount, 6);
            
        }
        
        void SetupPackerBed71Channels()
        {
            // Set up for 7.1 bed
            // After added to bedDefinition with SetBedChannels call, these IABChannels will be deleted by IABBedDefinition destructor
            // No need to explicitly delete them at end of test
            
            IABChannel *channelL = new IABChannel();
            IABChannel *channelR = new IABChannel();
            IABChannel *channelC = new IABChannel();
            IABChannel *channelLss = new IABChannel();
            IABChannel *channelRss = new IABChannel();
            IABChannel *channelLsr = new IABChannel();
            IABChannel *channelRsr = new IABChannel();
            IABChannel *channelLFE = new IABChannel();
            
            std::vector<IABChannel*> bedChannels;
            
            // Use the same audio ID for all bed channels
            channelL->SetChannelID(kIABChannelID_Left);
            channelL->SetAudioDataID(bedChannelAudioDataID_);
            
            channelR->SetChannelID(kIABChannelID_Right);
            channelR->SetAudioDataID(bedChannelAudioDataID_);
            
            channelC->SetChannelID(kIABChannelID_Center);
            channelC->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLss->SetChannelID(kIABChannelID_LeftSideSurround);
            channelLss->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRss->SetChannelID(kIABChannelID_RightSideSurround);
            channelRss->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLsr->SetChannelID(kIABChannelID_LeftRearSurround);
            channelLsr->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRsr->SetChannelID(kIABChannelID_RightRearSurround);
            channelRsr->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLFE->SetChannelID(kIABChannelID_LFE);
            channelLFE->SetAudioDataID(bedChannelAudioDataID_);
            
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelR);
            bedChannels.push_back(channelC);
            bedChannels.push_back(channelLss);
            bedChannels.push_back(channelRss);
            bedChannels.push_back(channelLsr);
            bedChannels.push_back(channelRsr);
            bedChannels.push_back(channelLFE);
            
            IABChannelCountType channelCount = 0;
            ASSERT_EQ(iabPackerBedDefinition_->SetBedChannels(bedChannels), kIABNoError);
            iabPackerBedDefinition_->GetChannelCount(channelCount);
            ASSERT_EQ(channelCount, 8);
            
        }

        void SetupPackerBed91Channels()
        {
            // NOTE: currently using Ls, Rs because this seems to be what the SMPTE example stream uses for 9.1 config
            // To be verfied
            
            // Set up for 9.1 bed
            // After added to bedDefinition with SetBedChannels call, these IABChannels will be deleted by IABBedDefinition destructor
            // No need to explicitly delete them at end of test
            
            IABChannel *channelL = new IABChannel();
            IABChannel *channelR = new IABChannel();
            IABChannel *channelC = new IABChannel();
            IABChannel *channelLss = new IABChannel();
            IABChannel *channelRss = new IABChannel();
            IABChannel *channelLsr = new IABChannel();
            IABChannel *channelRsr = new IABChannel();
            IABChannel *channelLFE = new IABChannel();
            IABChannel *channelLts = new IABChannel();
            IABChannel *channelRts = new IABChannel();

            
            std::vector<IABChannel*> bedChannels;
            
            // Use the same audio ID for all bed channels
            channelL->SetChannelID(kIABChannelID_Left);
            channelL->SetAudioDataID(bedChannelAudioDataID_);
            
            channelR->SetChannelID(kIABChannelID_Right);
            channelR->SetAudioDataID(bedChannelAudioDataID_);
            
            channelC->SetChannelID(kIABChannelID_Center);
            channelC->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLss->SetChannelID(kIABChannelID_LeftSideSurround);
            channelLss->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRss->SetChannelID(kIABChannelID_RightSideSurround);
            channelRss->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLsr->SetChannelID(kIABChannelID_LeftRearSurround);
            channelLsr->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRsr->SetChannelID(kIABChannelID_RightRearSurround);
            channelRsr->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLFE->SetChannelID(kIABChannelID_LFE);
            channelLFE->SetAudioDataID(bedChannelAudioDataID_);
            
            channelLts->SetChannelID(kIABChannelID_LeftTopSurround);
            channelLts->SetAudioDataID(bedChannelAudioDataID_);
            
            channelRts->SetChannelID(kIABChannelID_RightTopSurround);
            channelRts->SetAudioDataID(bedChannelAudioDataID_);
            
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelR);
            bedChannels.push_back(channelC);
            bedChannels.push_back(channelLss);
            bedChannels.push_back(channelRss);
            bedChannels.push_back(channelLsr);
            bedChannels.push_back(channelRsr);
            bedChannels.push_back(channelLFE);
            bedChannels.push_back(channelLts);
            bedChannels.push_back(channelRts);
            
            IABChannelCountType channelCount = 0;
            ASSERT_EQ(iabPackerBedDefinition_->SetBedChannels(bedChannels), kIABNoError);
            iabPackerBedDefinition_->GetChannelCount(channelCount);
            ASSERT_EQ(channelCount, 10);
            
        }


        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        void VerifyDeSerializedBedDefinition()
        {
            IABMetadataIDType           bedDefinitionMetaID;
            uint1_t                     conditionalBed;     // no bed use case in bitstream
            IABUseCaseType              bedUseCase;

            iabParserBedDefinition_->GetMetadataID(bedDefinitionMetaID);
            iabParserBedDefinition_->GetConditionalBed(conditionalBed);
            iabParserBedDefinition_->GetBedUseCase(bedUseCase);
            
            EXPECT_EQ(bedDefinitionMetaID, bedDefinitionMetaID_);
            EXPECT_EQ(conditionalBed, conditionalBed_);
            
            if (1 == conditionalBed)
            {
                EXPECT_EQ(bedUseCase, bedUseCase_);
            }
            else
            {
                bedUseCase = kIABUseCase_7_1_DS;
            }
            
            switch (bedUseCase)
            {
                case kIABUseCase_5_1:
                    VerifyDeSerializedBed51Channels();
                    break;
                    
                case kIABUseCase_9_1_OH:
                    VerifyDeSerializedBed91Channels();
                    break;
                    
                    // Set up 7.1 channels in bitstream for other use case
                case kIABUseCase_7_1_DS:
                default:
                    VerifyDeSerializedBed71Channels();
            }
            
            IABAudioDescription audioDescription;
            iabParserBedDefinition_->GetAudioDescription(audioDescription);

            EXPECT_EQ(audioDescription.audioDescription_, audioDescription_.audioDescription_);
            
            if ((kIABAudioDescription_TextInStream & audioDescription.audioDescription_) == 0)
            {
				EXPECT_EQ(audioDescription.audioDescriptionText_.size(), 0);
            }
            else
            {
                EXPECT_EQ(0, audioDescription.audioDescriptionText_.compare(audioDescription_.audioDescriptionText_));
            }
            
            // Check bed sub-Elements, should be two
            IABElementCountType count;
            std::vector<IABElement*> bedSubElements;

            iabParserBedDefinition_->GetSubElementCount(count);
            ASSERT_EQ(count, 2);
            
            iabParserBedDefinition_->GetSubElements(bedSubElements);
            ASSERT_EQ(bedSubElements.size(), 2);
            
            IABBedDefinition *bedDefinitionSubElement = dynamic_cast<IABBedDefinition*>(bedSubElements[0]);
			ASSERT_TRUE(NULL != bedDefinitionSubElement);
            
            // Do basic checks on de-serialized bed definition element
            IABMetadataIDType metaID = 1;
            IABChannelCountType channelCount = 0;
            bedDefinitionSubElement->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 123);
            bedDefinitionSubElement->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 2);
            
            // Do basic checks on de-serialized bed remap element
            // Note that detailed tests are performed in its own unit test

            IABUseCaseType remapUseCase;
            IABBedRemap *remapSubElement = dynamic_cast<IABBedRemap*>(bedSubElements[1]);
            
            ASSERT_TRUE(remapSubElement != NULL);
            remapSubElement->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 456);
            remapSubElement->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_5_1);
            remapSubElement->GetSourceChannels(channelCount);
            EXPECT_EQ(channelCount, 8);
            remapSubElement->GetDestinationChannels(channelCount);
            EXPECT_EQ(channelCount, 6);
        }
        
        void VerifyDeSerializedBed51Channels()
        {
            IABAudioDataIDType          audioDataID;  // use same ID for all bed channels to simplify testing
            std::vector<IABChannel*>    bedChannels;
            IABChannelCountType         channelCount;
            IABChannelIDType            channelID;

            iabParserBedDefinition_->GetBedChannels(bedChannels);
            iabParserBedDefinition_->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 6);
            EXPECT_EQ(bedChannels.size(), 6);
            
            // Left channel
            bedChannels[0]->GetChannelID(channelID);
            bedChannels[0]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Left);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Right channel
            bedChannels[1]->GetChannelID(channelID);
            bedChannels[1]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Right);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Center channel
            bedChannels[2]->GetChannelID(channelID);
            bedChannels[2]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Center);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Ls channel
            bedChannels[3]->GetChannelID(channelID);
            bedChannels[3]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rs channel
            bedChannels[4]->GetChannelID(channelID);
            bedChannels[4]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // LFE channel
            bedChannels[5]->GetChannelID(channelID);
            bedChannels[5]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LFE);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
        }
        
        void VerifyDeSerializedBed71Channels()
        {
            IABAudioDataIDType          audioDataID;  // use same ID for all bed channels to simplify testing
            std::vector<IABChannel*>    bedChannels;
            IABChannelCountType         channelCount;
            IABChannelIDType            channelID;

            iabParserBedDefinition_->GetBedChannels(bedChannels);
            iabParserBedDefinition_->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 8);
            EXPECT_EQ(bedChannels.size(), 8);
            
            // Left channel
            bedChannels[0]->GetChannelID(channelID);
            bedChannels[0]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Left);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Right channel
            bedChannels[1]->GetChannelID(channelID);
            bedChannels[1]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Right);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Center channel
            bedChannels[2]->GetChannelID(channelID);
            bedChannels[2]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Center);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Lss channel
            bedChannels[3]->GetChannelID(channelID);
            bedChannels[3]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftSideSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rss channel
            bedChannels[4]->GetChannelID(channelID);
            bedChannels[4]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightSideSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Lsr channel
            bedChannels[5]->GetChannelID(channelID);
            bedChannels[5]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftRearSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rsr channel
            bedChannels[6]->GetChannelID(channelID);
            bedChannels[6]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightRearSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // LFE channel
            bedChannels[7]->GetChannelID(channelID);
            bedChannels[7]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LFE);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
        }

        void VerifyDeSerializedBed91Channels()
        {
            IABAudioDataIDType          audioDataID;  // use same ID for all bed channels to simplify testing
            std::vector<IABChannel*>    bedChannels;
            IABChannelCountType         channelCount;
            IABChannelIDType            channelID;

            iabParserBedDefinition_->GetBedChannels(bedChannels);
            iabParserBedDefinition_->GetChannelCount(channelCount);
            EXPECT_EQ(channelCount, 10);
            EXPECT_EQ(bedChannels.size(), 10);
            
            // Left channel
            bedChannels[0]->GetChannelID(channelID);
            bedChannels[0]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Left);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Right channel
            bedChannels[1]->GetChannelID(channelID);
            bedChannels[1]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Right);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Center channel
            bedChannels[2]->GetChannelID(channelID);
            bedChannels[2]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_Center);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Lss channel
            bedChannels[3]->GetChannelID(channelID);
            bedChannels[3]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftSideSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rss channel
            bedChannels[4]->GetChannelID(channelID);
            bedChannels[4]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightSideSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Lsr channel
            bedChannels[5]->GetChannelID(channelID);
            bedChannels[5]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftRearSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rsr channel
            bedChannels[6]->GetChannelID(channelID);
            bedChannels[6]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightRearSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // LFE channel
            bedChannels[7]->GetChannelID(channelID);
            bedChannels[7]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LFE);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Lts channel
            bedChannels[8]->GetChannelID(channelID);
            bedChannels[8]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_LeftTopSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
            // Rts channel
            bedChannels[9]->GetChannelID(channelID);
            bedChannels[9]->GetAudioDataID(audioDataID);
            EXPECT_EQ(channelID, kIABChannelID_RightTopSurround);
            EXPECT_EQ(audioDataID, bedChannelAudioDataID_);
            
        }

        // **********************************************
        // Function to test Serialize() and DeSerialize()
        // **********************************************

        void TestSerializeDeSerialize()
        {
            // When conditional bed is set to 0, bedUseCase is not written to stream
            // When conditional bed is set to 1, a valid bedUseCase code should be written to stream.
            // This is rendering information and does not necessarily reflect what bed channels are in the bitstream
            
            conditionalBed_ = 1;
            audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
            audioDescription_.audioDescriptionText_.clear();

            // but with a 7.1 bed channels in stream
            
            // Test case: 5.1
            bedUseCase_ = kIABUseCase_5_1;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 7.1
            bedUseCase_ = kIABUseCase_7_1_DS;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 9.1
            bedUseCase_ = kIABUseCase_9_1_OH;
            RunSerializeDeSerializeTestCase();
            
            // The following bed use case will have the correct use case code written to bitstream but with 7.1 bed channels in the bitstream for testing
            // Test case: 7_1_SDS
            bedUseCase_ = kIABUseCase_7_1_SDS;
            RunSerializeDeSerializeTestCase();

            // Test case: 11_1_HT
            bedUseCase_ = kIABUseCase_11_1_HT;
            RunSerializeDeSerializeTestCase();

            // Test case: 13_1_HT
            bedUseCase_ = kIABUseCase_13_1_HT;
            RunSerializeDeSerializeTestCase();

			// Test case: ITU-A (2.0)
			bedUseCase_ = kIABUseCase_2_0_ITUA;
			RunSerializeDeSerializeTestCase();

			// Test case: ITU-D (5.1.4)
			bedUseCase_ = kIABUseCase_5_1_4_ITUD;
			RunSerializeDeSerializeTestCase();

			// Test case: ITU-J (7.1.4)
			bedUseCase_ = kIABUseCase_7_1_4_ITUJ;
			RunSerializeDeSerializeTestCase();

			// Test case: Always
            bedUseCase_ = kIABUseCase_Always;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 7.1 channel in stream with with conditionalBed_ = 0
            conditionalBed_ = 0;
            bedUseCase_ = kIABUseCase_7_1_DS;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Dialog
            audioDescription_.audioDescription_ = kIABAudioDescription_Dialog;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Music
            audioDescription_.audioDescription_ = kIABAudioDescription_Music;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Effects
            audioDescription_.audioDescription_ = kIABAudioDescription_Effects;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Foley
            audioDescription_.audioDescription_ = kIABAudioDescription_Foley;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Ambience
            audioDescription_.audioDescription_ = kIABAudioDescription_Ambience;
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_TextInStream
            audioDescription_.audioDescription_ = kIABAudioDescription_TextInStream;
            audioDescription_.audioDescriptionText_ = "Bed definition audio description test string";
            RunSerializeDeSerializeTestCase();
            
            // Test selected combination of audio description types
            
            // Test case: description = kIABAudioDescription_TextInStream with kIABAudioDescription_Dialog
            IABAudioDescriptionType combinedCode1 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Dialog + kIABAudioDescription_TextInStream);
            audioDescription_.audioDescription_ = combinedCode1;
            audioDescription_.audioDescriptionText_ = "This is a dialog plus text in stream audio description";
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_TextInStream with kIABAudioDescription_Music
            IABAudioDescriptionType combinedCode2 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Music + kIABAudioDescription_TextInStream);
            audioDescription_.audioDescription_ = combinedCode2;
            audioDescription_.audioDescriptionText_ = "This is a music plus text in stream audio description";
            RunSerializeDeSerializeTestCase();
        }
        
        void RunSerializeDeSerializeTestCase()
        {
			// Create IAB bed definition element (packer) to Serialize stream buffer
			iabPackerBedDefinition_ = new IABBedDefinition(frameRate_);
			ASSERT_TRUE(NULL != iabPackerBedDefinition_);

			// Create IAB bed definition element (parser) to de-serialize stream buffer
			iabParserBedDefinition_ = new IABBedDefinition(frameRate_);
            ASSERT_TRUE(NULL != iabParserBedDefinition_);

            // Set up IAB packer bed definition element
            SetupPackerBedDefinition();
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serialize bed definition into stream
            ASSERT_EQ(iabPackerBedDefinition_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            auto pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerBedDefinition_->GetElementSize(elementSize);
            
            bytesInStream -= 2;     // Deduct 2 bytes for element ID and element size code
            
            if (elementSize >= 255)
            {
                // deduct 2 more bytes for plex coding if > 8-bit range (0xFF + 16-bit code)
                bytesInStream -= 2;
                
                if (elementSize >= 65535)
                {
                    // deduct 4 more bytes for plex coding if > 16-bit range. Assume that the size field does not exceed 32-bit range (0xFFFFFF + 32-bit code)
                    bytesInStream -= 4;
                }
            }
            
            EXPECT_EQ(elementSize, bytesInStream);

            // Reset  stream to beginning
            elementBuffer.seekg(0, std::ios::beg);

            // stream reader for parsing
            StreamReader elementReader(elementBuffer);

            // DeSerialize bed definition from stream
            ASSERT_EQ(iabParserBedDefinition_->DeSerialize(elementReader), kIABNoError);
            
            // Verify deserialized bed definition
            VerifyDeSerializedBedDefinition();
            
            // Verify element size
            
            auto readerPos = elementReader.streamPosition();
            
            bytesInStream = static_cast<IABElementSizeType>(readerPos);
            
            iabParserBedDefinition_->GetElementSize(elementSize);
            
            bytesInStream -= 2;     // Deduct 2 bytes for element ID and element size code
            
            if (elementSize >= 255)
            {
                // deduct 2 more bytes for plex coding if > 8-bit range (0xFF + 16-bit code)
                bytesInStream -= 2;
                
                if (elementSize >= 65535)
                {
                    // deduct 4 more bytes for plex coding if > 16-bit range. Assume that the size field does not exceed 32-bit range (0xFFFFFF + 32-bit code)
                    bytesInStream -= 4;
                }
            }
            
            EXPECT_EQ(elementSize, bytesInStream);
            
			delete iabPackerBedDefinition_;
			delete iabParserBedDefinition_;
		}

		void TestDynamicPacking()
		{
			// Create IAB bed definition element (packer) to Serialize stream buffer
			iabPackerBedDefinition_ = new IABBedDefinition(frameRate_);
			ASSERT_TRUE(NULL != iabPackerBedDefinition_);

			// Set up IAB packer bed definition element
			SetupPackerBedDefinition();

			// stream buffer to hold serialzed stream
			std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

			// Default, packing enabled
			// Serialize bed definition into stream
			ASSERT_EQ(iabPackerBedDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			auto pos = elementBuffer.tellg();
			IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect bytes present in packed buffer
			EXPECT_GT(bytesInStream, 0);

			// Disable packing 
			iabPackerBedDefinition_->DisablePacking();
			// Clear elementBuffer content
			elementBuffer.str("");
			// Serialize bed definition into stream
			ASSERT_EQ(iabPackerBedDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			pos = elementBuffer.tellg();
			bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect 0 bytes as packing is disabled
			EXPECT_EQ(bytesInStream, 0);

			// re-enable packing 
			iabPackerBedDefinition_->EnablePacking();
			// Clear elementBuffer content
			elementBuffer.str("");
			// Serialize bed definition into stream
			ASSERT_EQ(iabPackerBedDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			pos = elementBuffer.tellg();
			bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect some bytes in packed buffer as packing is enabled
			EXPECT_GT(bytesInStream, 0);

			delete iabPackerBedDefinition_;
		}

    private:
        IABBedDefinition*   iabPackerBedDefinition_;
        IABBedDefinition*   iabParserBedDefinition_;
        
        // Bed definition data members
        IABMetadataIDType           bedDefinitionMetaID_;
        uint1_t                     conditionalBed_;     // no bed use case in bitstream
        IABUseCaseType              bedUseCase_;
        IABAudioDataIDType          bedChannelAudioDataID_;  // use same ID for all bed channels to simplify testing
        IABAudioDescription         audioDescription_;

		IABFrameRateType frameRate_;
		IABSampleRateType sampleRate_;
		IABBitDepthType bitDepth_;
};
    
    
    // ********************
    // Run tests
    // ********************
    
    // Run frame element setters and getters API tests
    TEST_F(IABBedDefiniton_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial frame, then deSerialize frame tests
    TEST_F(IABBedDefiniton_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
	// Test dynamic packing
	TEST_F(IABBedDefiniton_Test, Test_Dynamic_Packing)
	{
		TestDynamicPacking();
	}
}
