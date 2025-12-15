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
#include "IABUtilities.h"
#include <vector>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    // IABObjectDefinition element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABObjectDefiniton_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            metadataID_ = 123;
            audioDataID_ = 1;
            
            // These members will be changed per test case
            audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
            audioDescription_.audioDescriptionText_.clear();
            conditionalObject_ = 1;
            objectUseCase_ = kIABUseCase_Always;
			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);
        }
        
        // **********************************************
        
        // IABObjectDefinition element setters and getters API tests
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
            IABObjectDefinitionInterface* objectDefinitionInterface = NULL;

			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second
			numPanSubBlocks_ = GetIABNumSubBlocks(kIABFrameRate_24FPS);

			objectDefinitionInterface = IABObjectDefinitionInterface::Create(frameRate_);
            ASSERT_TRUE(NULL != objectDefinitionInterface);

            // Test metadata ID
            IABMetadataIDType metaID = 1;
            objectDefinitionInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0);   // expect to default to zero
            
            // Set metadata ID and verify
            EXPECT_EQ(objectDefinitionInterface->SetMetadataID(0xFFFFFFFF), kIABNoError);
            objectDefinitionInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0xFFFFFFFF);
            
            // Test audio data ID
            IABAudioDataIDType audioDataID = 1;
            objectDefinitionInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0);   // expect to default to zero
            
            // Set audiodata ID and verify
            EXPECT_EQ(objectDefinitionInterface->SetAudioDataID(0xFFFFFFFF), kIABNoError);
            objectDefinitionInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0xFFFFFFFF);
            
            // Test conditional object
            // Check default
            uint1_t conditionalObject = 0;
            objectDefinitionInterface->GetConditionalObject(conditionalObject);
            EXPECT_EQ(conditionalObject, 0);   // Default to 1
            
            // Set to zero and verify
            EXPECT_EQ(objectDefinitionInterface->SetConditionalObject(1), kIABNoError);
            objectDefinitionInterface->GetConditionalObject(conditionalObject);
            EXPECT_EQ(conditionalObject, 1);
            
            // Test Object use case
            // Check default value
            IABUseCaseType objectUseCase;
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_7_1_DS);
            
            // Set to kIABUseCase_5_1 and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_5_1), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_5_1);
            
            // Set to kIABUseCase_7_1_DS and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_7_1_DS), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_7_1_DS);
            
            // Set to kIABUseCase_7_1_SDS and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_7_1_SDS), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_7_1_SDS);
            
            // Set to kIABUseCase_11_1_HT and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_11_1_HT), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_11_1_HT);
            
            // Set to kIABUseCase_13_1_HT and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_13_1_HT), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_13_1_HT);
            
            // Set to kIABUseCase_9_1_OH and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_9_1_OH), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_9_1_OH);
            
            // Set to kIABUseCase_Always and verify
            EXPECT_EQ(objectDefinitionInterface->SetObjectUseCase(kIABUseCase_Always), kIABNoError);
            objectDefinitionInterface->GetObjectUseCase(objectUseCase);
            EXPECT_EQ(objectUseCase, kIABUseCase_Always);
            
            // Test pan subBlock setters and getters, number of sub-blokcs is frame rate dependent, maximum is 8
            // After added to objectDefinition with SetPanSubBlocks call, these IABObjectSubBlocks will be deleted by IABObjectDefinition destructor
            // No need to explicitly delete them at end of test
            
            std::vector<IABObjectSubBlock*> panSubBlocks;
            std::vector<IABObjectSubBlock*> panSubBlocksGet;
            
            for (uint8_t i = 0; i < numPanSubBlocks_; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
                
                // Leave the rest at default settings
                
                // Add to object
                panSubBlocks.push_back(subBlock);
            }
            
            uint8_t numPanSubBlocks = 0;
            
            // Set and get subBlocks
            EXPECT_EQ(objectDefinitionInterface->SetPanSubBlocks(panSubBlocks), kIABNoError);
            objectDefinitionInterface->GetNumPanSubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, numPanSubBlocks_);
            objectDefinitionInterface->GetPanSubBlocks(panSubBlocksGet);
            EXPECT_EQ(panSubBlocksGet.size(), numPanSubBlocks);
            
            // Valid sub-element type for object definition, these will be deleted by IABObjectDefinition destructor
            IABElement *objectDefinitionElement = new IABElement(kIABElementID_ObjectDefinition);
            IABElement *objectZoneElement = new IABElement(kIABElementID_ObjectZoneDefinition19);
            
            // Invalid sub-element type for bed definition
            IABElement *frameElement = new IABElement(kIABElementID_IAFrame);
            IABElement *bedDefinitionElement = new IABElement(kIABElementID_BedDefinition);
            IABElement *remapElement = new IABElement(kIABElementID_BedRemap);
            IABElement *dlcAudioElement = new IABElement(kIABElementID_AudioDataDLC);
            IABElement *pcmAudioElement = new IABElement(kIABElementID_AudioDataPCM);
            
            // Only object definition and object zone definition allowed as sub-elements
            
            IABElementCountType count;
            std::vector<IABElement*> subElements;
            std::vector<IABElement*> invalidSubElements;
            std::vector<IABElement*> subElementsGet;
            
            // check default
            objectDefinitionInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 0);
            
            objectDefinitionInterface->GetSubElements(subElementsGet);
            EXPECT_EQ(subElementsGet.size(), 0);
            
            // Invalid sub element tests
            invalidSubElements.push_back(frameElement);
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(bedDefinitionElement);
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(remapElement);
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(dlcAudioElement);
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(pcmAudioElement);
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            // Valid sub element tests
            
            subElements.push_back(objectDefinitionElement);
            subElements.push_back(objectZoneElement);
            
            EXPECT_EQ(objectDefinitionInterface->SetSubElements(subElements), kIABNoError);
            objectDefinitionInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 2);
            
            objectDefinitionInterface->GetSubElements(subElementsGet);
            EXPECT_EQ(subElementsGet.size(), 2);
            
            IABElementIDType elementID;
            (subElementsGet[0])->GetElementID(elementID);
            EXPECT_EQ(elementID, kIABElementID_ObjectDefinition);
            (subElementsGet[1])->GetElementID(elementID);
            EXPECT_EQ(elementID, kIABElementID_ObjectZoneDefinition19);
            
            //SetSubElements method test
            // Set 2 valid sub-elements first
            std::vector<IABElement*> validsubelements_1;
            validsubelements_1.push_back(objectDefinitionElement);
            validsubelements_1.push_back(objectZoneElement);
            
            EXPECT_EQ(kIABNoError, objectDefinitionInterface->SetSubElements(validsubelements_1));
            
            // set 1 valid sub-element
            std::vector<IABElement*> validsubelements_2;
            validsubelements_2.push_back(objectZoneElement);
            
            EXPECT_EQ(kIABNoError, objectDefinitionInterface->SetSubElements(validsubelements_2));
            
            // verify that only second set valid sub-elements present.
            std::vector<IABElement*> ret_subelments;
            objectDefinitionInterface->GetSubElements(ret_subelments);
            EXPECT_EQ(objectZoneElement, ret_subelments.at(0));
            EXPECT_EQ(1, ret_subelments.size());
            
            // Duplicate test
            std::vector<IABElement*> duplicate_subelements;
            duplicate_subelements.push_back(objectZoneElement);
            duplicate_subelements.push_back(objectZoneElement);
            EXPECT_EQ(kIABDuplicateSubElementsError, objectDefinitionInterface->SetSubElements(duplicate_subelements));

            
            // Delete invalid subelements that have not been added, valid subelements will be deleted by the destructor
            delete bedDefinitionElement;
            bedDefinitionElement = NULL;
            
            delete remapElement;
            remapElement = NULL;
            
            delete frameElement;
            frameElement = NULL;
            
            delete dlcAudioElement;
            dlcAudioElement = NULL;
            
            delete pcmAudioElement;
            pcmAudioElement = NULL;
            
            subElements.clear();
            subElementsGet.clear();
            invalidSubElements.clear();
            
            // Test audio description
            
            IABAudioDescription audioDescription;
            IABAudioDescription audioDescriptionGet;
            
            audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
            audioDescription.audioDescriptionText_.clear();
            
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_NotIndicated, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Dialog;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Dialog, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Music;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Music, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Effects;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Effects, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Foley;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Foley, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_Ambience;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_Ambience, audioDescriptionGet.audioDescription_);
            EXPECT_EQ(audioDescriptionGet.audioDescriptionText_.size(), 0);
            
            audioDescription.audioDescription_ = kIABAudioDescription_TextInStream;
            std::string desc("audio description contents");
            std::string desc2;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(kIABAudioDescription_TextInStream, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));
            
            IABAudioDescriptionType combinedCode1 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Dialog + kIABAudioDescription_TextInStream);
            audioDescription.audioDescription_ = combinedCode1;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(combinedCode1, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));
            
            IABAudioDescriptionType combinedCode2 = static_cast<IABAudioDescriptionType>(kIABAudioDescription_Music + kIABAudioDescription_TextInStream);
            audioDescription.audioDescription_ = combinedCode2;
            audioDescription.audioDescriptionText_ = desc;
            EXPECT_EQ(objectDefinitionInterface->SetAudioDescription(audioDescription), kIABNoError);
            objectDefinitionInterface->GetAudioDescription(audioDescriptionGet);
            EXPECT_EQ(combinedCode2, audioDescriptionGet.audioDescription_);
            desc2 = audioDescription.audioDescriptionText_;
            EXPECT_EQ(0, desc.compare(desc2));

			// Test dynamic packing flag
			// Default = true
			EXPECT_EQ(objectDefinitionInterface->IsIncludedForPacking(), true);

			objectDefinitionInterface->DisablePacking();
			EXPECT_EQ(objectDefinitionInterface->IsIncludedForPacking(), false);

			objectDefinitionInterface->EnablePacking();
			EXPECT_EQ(objectDefinitionInterface->IsIncludedForPacking(), true);
			
            IABObjectDefinitionInterface::Delete(objectDefinitionInterface);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************
        
        void SetupPackerObjectDefinition()
        {
            ASSERT_EQ(iabPackerObjectDefinition_->SetMetadataID(metadataID_), kIABNoError);
            ASSERT_EQ(iabPackerObjectDefinition_->SetAudioDataID(audioDataID_), kIABNoError);
            ASSERT_EQ(iabPackerObjectDefinition_->SetConditionalObject(conditionalObject_), kIABNoError);
            ASSERT_EQ(iabPackerObjectDefinition_->SetObjectUseCase(objectUseCase_), kIABNoError);
            
            // After added to objectDefinition with SetPanSubBlocks call, these IABObjectSubBlocks will be deleted by IABObjectDefinition destructor
            // No need to explicitly delete them at end of test
            
            std::vector<IABObjectSubBlock*> panSubBlocks;
            
            for (uint8_t i = 0; i < numPanSubBlocks_; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
                
                // Leave the rest at default settings
                
                // Add to object
                panSubBlocks.push_back(subBlock);
            }
            
            uint8_t numPanSubBlocks = 0;
            
            // Set subBlocks and check block count
            // Note that the subblocks will be deleted by IABObjectSubBlock destructor after added to iabPackerObjectDefinition_
            ASSERT_EQ(iabPackerObjectDefinition_->SetPanSubBlocks(panSubBlocks), kIABNoError);
            iabPackerObjectDefinition_->GetNumPanSubBlocks(numPanSubBlocks);
            ASSERT_EQ(numPanSubBlocks, numPanSubBlocks_);
            
            // Add audio description: no description in bitstream
            ASSERT_EQ(iabPackerObjectDefinition_->SetAudioDescription(audioDescription_), kIABNoError);
            
            // ***************
            // Set up and add sub-elements: One object definition and one object zone definition 19
            // ***************
            
            std::vector<IABElement*> subElements;

            // Set up a dummy, empty object definition sub-element
            IABObjectDefinition *subElement_object = new IABObjectDefinition(frameRate_);      // sub element has the same frame rate
            ASSERT_TRUE(NULL != subElement_object);
            
            // Set metadata ID and verify
            EXPECT_EQ(subElement_object->SetMetadataID(1234), kIABNoError);     // 1234 is an arbitrary value
            
            // Test audio data ID
            EXPECT_EQ(subElement_object->SetAudioDataID(5678), kIABNoError);    // 5678 is an arbitrary value
            
            std::vector<IABObjectSubBlock*> panSubBlocks2;
            
            for (uint8_t i = 0; i < numPanSubBlocks_; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
                
                // Leave the rest at default settings
                
                // Add to object
                panSubBlocks2.push_back(subBlock);
            }
            
            // Note that the subblocks will be deleted by IABObjectSubBlock destructor after added to iabPackerObjectDefinition_
            ASSERT_EQ(subElement_object->SetPanSubBlocks(panSubBlocks2), kIABNoError);
            
            // Add to object sub-element vector and add vector to object definition
            subElements.push_back(subElement_object);
            
            // Set up a zone definition 19 sub-element
            IABObjectZoneDefinition19 *subElement_zoneDefinition19 = new IABObjectZoneDefinition19(frameRate_);
            ASSERT_TRUE(NULL != subElement_zoneDefinition19);
            
            // Set up an object zone structure and use it for all subBlocks
            IABObjectZoneGain19 objectZoneGains19;
            
            // Set exist flag
            objectZoneGains19.objectZone19InfoExists_ = 1;
            
            // Set gain to 0.5 (an arbitrary non-zero value between 0.0 and 1.0) for remaining zones
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].setIABZoneGain(0.5f), kIABNoError);
            }
            
            // Zone gain SubBlock vector
            std::vector<IABZone19SubBlock*> zone19SubBlocks;
            
            for (uint8_t j = 0; j < numPanSubBlocks_; j++)
            {
                // Create IABZone19SubBlock instance
                IABZone19SubBlock* iabZone19SubBlock = new IABZone19SubBlock();
                ASSERT_TRUE(NULL != iabZone19SubBlock);
                
                // Add zone gain to the subBock
                EXPECT_EQ(iabZone19SubBlock->SetObjectZoneGains19(objectZoneGains19), kIABNoError);
                
                // Add to vector
                zone19SubBlocks.push_back(dynamic_cast<IABZone19SubBlock*>(iabZone19SubBlock));
            }
            
            // Note that IABObjectZoneDefinition19 destructor will delete the added subblock, so no need to explicitly delete it at end of test
            EXPECT_EQ(subElement_zoneDefinition19->SetZone19SubBlocks(zone19SubBlocks), kIABNoError);
            
            // Add to object sub-element vector and add vector to object definition
            subElements.push_back(subElement_zoneDefinition19);
            
            EXPECT_EQ(iabPackerObjectDefinition_->SetSubElements(subElements), kIABNoError);

            IABElementCountType count = 0;
            iabPackerObjectDefinition_->GetSubElementCount(count);
            EXPECT_EQ(count, 2);
        }

        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        void VerifyDeSerializedObjectDefinition()
        {
            IABMetadataIDType metadataID;
            IABAudioDataIDType audioDataID;
            uint1_t	conditionalObject;
            IABUseCaseType	objectUseCase;
            uint8_t numPanSubBlocks;
            IABAudioDescription audioDescription;
            std::vector<IABObjectSubBlock*> panSubBlocks;

            iabParserObjectDefinition_->GetMetadataID(metadataID);
            iabParserObjectDefinition_->GetAudioDataID(audioDataID);
            iabParserObjectDefinition_->GetConditionalObject(conditionalObject);
            iabParserObjectDefinition_->GetObjectUseCase(objectUseCase);
            
            EXPECT_EQ(metadataID, metadataID_);
            EXPECT_EQ(audioDataID, audioDataID_);
            EXPECT_EQ(conditionalObject, conditionalObject_);
            
            if (1 == conditionalObject)
            {
                EXPECT_EQ(objectUseCase, objectUseCase_);
            }
            
            iabParserObjectDefinition_->GetPanSubBlocks(panSubBlocks);
            iabParserObjectDefinition_->GetNumPanSubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, numPanSubBlocks_);
            EXPECT_EQ(panSubBlocks.size(), numPanSubBlocks_);

            // Leave detailed test to IABObjectSubBlock unit test
            
            iabPackerObjectDefinition_->GetAudioDescription(audioDescription);
            
            EXPECT_EQ(audioDescription.audioDescription_, audioDescription_.audioDescription_);
            
            if ((kIABAudioDescription_TextInStream & audioDescription.audioDescription_) == 0)
            {
                EXPECT_EQ(audioDescription.audioDescriptionText_.size(), 0);
            }
            else
            {
                EXPECT_EQ(0, audioDescription.audioDescriptionText_.compare(audioDescription_.audioDescriptionText_));
            }

            
            // ********************
            // Check sub-elements present and do very basic checks
            // ********************
            
            std::vector<IABElement*> subElements;
            IABElementCountType count = 0;

            iabParserObjectDefinition_->GetSubElements(subElements);
            iabParserObjectDefinition_->GetSubElementCount(count);
            ASSERT_EQ(count, 2);    // skip remaining tests if number of elements is not the expected value

            IABObjectDefinition *subElement_object = dynamic_cast<IABObjectDefinition*>(subElements[0]);
            EXPECT_TRUE(NULL != subElement_object);
            if (subElement_object)
            {
                // Check metadata ID
                IABMetadataIDType metaID = 1;
                subElement_object->GetMetadataID(metaID);
                EXPECT_EQ(metaID, 1234);
                
                // Check audio data ID
                IABAudioDataIDType audioDataID = 1;
                subElement_object->GetAudioDataID(audioDataID);
                EXPECT_EQ(audioDataID, 5678);
             }

            IABObjectZoneDefinition19 *subElement_zoneDefinition19 = dynamic_cast<IABObjectZoneDefinition19*>(subElements[1]);
            EXPECT_TRUE(NULL != subElement_zoneDefinition19);
            if (subElement_zoneDefinition19)
            {
                // Check number of pan blocks
                subElement_zoneDefinition19->GetNumZone19SubBlocks(numPanSubBlocks);
                EXPECT_EQ(numPanSubBlocks, numPanSubBlocks_);
                
                // Test object 19-zone gains
                std::vector<IABZone19SubBlock*> zone19SubBlocks;
                std::vector<IABZone19SubBlock*>::iterator iterz19;
                IABObjectZoneGain19 objectZoneGains19;
                float zoneGain;
                
                subElement_zoneDefinition19->GetZone19SubBlocks(zone19SubBlocks);
                         
                for (iterz19 = zone19SubBlocks.begin(); iterz19 != zone19SubBlocks.end(); iterz19++)
                {
                    IABObjectZoneGain19 objectZoneGains19;
                    
                    (*iterz19)->GetObjectZoneGains19(objectZoneGains19);
                    EXPECT_EQ(objectZoneGains19.objectZone19InfoExists_, 1);
                    for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
                    {
                        zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
                        EXPECT_LT(std::fabs(zoneGain - 0.5f), 0.001f);									// Quatization to be less than 0.2%
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_InStream);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 512);   // coded value for gain = 0.5
                    }
                }
            }
        }
        
        // **********************************************
        
        // Function to test Serialize() and DeSerialize()
        
        // **********************************************
        
 
        void TestSerializeDeSerialize()
        {
            // When conditionalObject_ is set to 0, bedUseCase is not written to stream
            // When conditionalObject_ is set to 1, a valid use case code should be written to stream.
            // This is rendering information and does not necessarily reflect what bed configuration or objects are in the bitstream

            conditionalObject_ = 1;
			frameRate_ = kIABFrameRate_24FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 8 subblokcs for 24FPS (25FPS, 30FPS also)
            
            // Note that description text in stream (i.e. kIABAudioDescription_TextInStream) is not yet supported by the library
            audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
			audioDescription_.audioDescriptionText_.clear();

            // Test case: 5.1, number of subblocks = 8
            objectUseCase_ = kIABUseCase_5_1;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 7.1
            objectUseCase_ = kIABUseCase_7_1_DS;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 7_1_SDS
            objectUseCase_ = kIABUseCase_7_1_SDS;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 9.1
            objectUseCase_ = kIABUseCase_9_1_OH;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 11_1_HT
            objectUseCase_ = kIABUseCase_11_1_HT;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 13_1_HT
            objectUseCase_ = kIABUseCase_13_1_HT;
            RunSerializeDeSerializeTestCase();
            
            // Test case: Always
            objectUseCase_ = kIABUseCase_Always;
            RunSerializeDeSerializeTestCase();
            
            // Test case: conditionalObject_ = 0
            conditionalObject_ = 0;
            objectUseCase_ = kIABUseCase_7_1_DS;
            RunSerializeDeSerializeTestCase();
            
            // Test case: 4 subblocks
            conditionalObject_ = 1;
            objectUseCase_ = kIABUseCase_7_1_DS;
			frameRate_ = kIABFrameRate_48FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 4 subblokcs for 48FPS (50FPS, 60FPS also)
            RunSerializeDeSerializeTestCase();

            // Test case: 2 subblocks
			frameRate_ = kIABFrameRate_96FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 2 subblokcs for 96FPS (100FPS, 120FPS also)
            RunSerializeDeSerializeTestCase();
            
            // Test case: description = kIABAudioDescription_Dialog, conditionalObject_ = 0, numPanSubBlocks_ = 8
            conditionalObject_ = 0;
			frameRate_ = kIABFrameRate_24FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 8 subblokcs for 24FPS (25FPS, 30FPS also)
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
            audioDescription_.audioDescriptionText_ = "Object definition audio description test string";
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
            // Create IAB object definition element (packer) to Serialize stream buffer
            iabPackerObjectDefinition_ = new IABObjectDefinition(frameRate_);      // use 8 subblocks for 24FPS
            ASSERT_TRUE(NULL != iabPackerObjectDefinition_);
            
            // Create IAB object definition element (parser) to De-serialize stream buffer
            iabParserObjectDefinition_ = new IABObjectDefinition(frameRate_);      // use 8 subblocks for 24FPS
            ASSERT_TRUE(NULL != iabParserObjectDefinition_);

            // Set up IAB packer frame
            SetupPackerObjectDefinition();
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serialize object definition into stream
            ASSERT_EQ(iabPackerObjectDefinition_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            std::ios_base::streampos pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerObjectDefinition_->GetElementSize(elementSize);
            
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
            
            // To do: re-enable this. There is a mismatch of 1 byte
            EXPECT_EQ(elementSize, bytesInStream);
            
            // Reset  stream to beginning
            elementBuffer.seekg(0, std::ios::beg);
            
            // stream reader for parsing
            StreamReader elementReader(elementBuffer);
            
            // DeSerialize bed definition from stream
            ASSERT_EQ(iabParserObjectDefinition_->DeSerialize(elementReader), kIABNoError);
            
            // Verify deserialized bed definition
            VerifyDeSerializedObjectDefinition();
            
            // Verify element size
            
            std::ios_base::streampos readerPos = elementReader.streamPosition();
            
            bytesInStream = static_cast<IABElementSizeType>(readerPos);
            
            iabParserObjectDefinition_->GetElementSize(elementSize);
            
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
            
            // To do: re-enable this. There is a mismatch of 1 byte
            EXPECT_EQ(elementSize, bytesInStream);

			delete iabPackerObjectDefinition_;
			delete iabParserObjectDefinition_;
        }

		void TestDynamicPacking()
		{
            // Create IAB object definition element (packer) to Serialize stream buffer
            iabPackerObjectDefinition_ = new IABObjectDefinition(frameRate_);      // use 8 subblocks for 24FPS
            ASSERT_TRUE(NULL != iabPackerObjectDefinition_);
            
            // Set up IAB packer frame
            SetupPackerObjectDefinition();
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
			// Default, packing enabled
			// Serialize object definition into stream
			ASSERT_EQ(iabPackerObjectDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			std::ios_base::streampos pos = elementBuffer.tellg();
			IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect bytes present in packed buffer
			EXPECT_GT(bytesInStream, 0);

			// Disable packing 
			iabPackerObjectDefinition_->DisablePacking();
			// Clear elementBuffer content
			elementBuffer.str("");
			// Serialize object definition into stream
			ASSERT_EQ(iabPackerObjectDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			pos = elementBuffer.tellg();
			bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect 0 bytes as packing is disabled
			EXPECT_EQ(bytesInStream, 0);

			// re-enable packing 
			iabPackerObjectDefinition_->EnablePacking();
			// Clear elementBuffer content
			elementBuffer.str("");
			// Serialize object definition into stream
			ASSERT_EQ(iabPackerObjectDefinition_->Serialize(elementBuffer), kIABNoError);
			elementBuffer.seekg(0, std::ios::end);
			pos = elementBuffer.tellg();
			bytesInStream = static_cast<IABElementSizeType>(pos);
			// Expect some bytes in packed buffer as packing is enabled
			EXPECT_GT(bytesInStream, 0);

			delete iabPackerObjectDefinition_;
		}
        
    private:
        
        IABObjectDefinition*   iabPackerObjectDefinition_;
        IABObjectDefinition*   iabParserObjectDefinition_;

        IABMetadataIDType metadataID_;
        IABAudioDataIDType audioDataID_;
        uint1_t	conditionalObject_;
        IABUseCaseType	objectUseCase_;
        uint8_t numPanSubBlocks_;
		IABFrameRateType frameRate_;
        IABAudioDescription audioDescription_;

    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run frame element setters and getters API tests
    TEST_F(IABObjectDefiniton_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial frame, then deSerialize frame tests
    TEST_F(IABObjectDefiniton_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
	// Test dynamic packing
	TEST_F(IABObjectDefiniton_Test, Test_Dynamic_Packing)
	{
		TestDynamicPacking();
	}
}
