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
#include <memory>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    
    // IABFrame element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    // 4. Do high level checks on de-serialized frame. Note that detailed tests are done by the respective IAB elements, i.e. IABBedDefinition, etc..
    
    class IABFrameElement_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            // Initialize frame variables, these will be changed per test case to test different settings
            sampleRate_ = kIABSampleRate_48000Hz;
            bitDepth_ = kIABBitDepth_24Bit;
            frameRateCode_ = kIABFrameRate_24FPS;
            frameSampleCount_ = 2000;   // 2000 samples for  24 FPS

            maxRendered_ = 1;   // No bed channel, 1 object

            // Initialize bed defintion variables, no sub element in bed definition
            bedDefinitionMetaID_ = 0;
            
            // Initialize object varaibles, no sub element in object definition
            objectMetaID_ = 1;
            objectAudioDataID_ = 30;
            numPanSubblocks_ = 8;   // use 8 panning subblocks for 24 FPS
            
            // Initialize DLC variables
            dlcAudioDataID_ = 30;
            dlcSize_ = 0;

			// Initialize PCM variables
			pcmAudioDataID_ = 31;
		}
        
        // void TearDown() to do any clean-up
        void TearDown()
        {
            // Nothing to clean up
        }
        
        // **********************************************
        // IABFrame element setters and getters API tests
        // **********************************************
        
        void TestSetterGetterAPIs()
        {
            IABFrameInterface* iabFrameInterface = NULL;
            iabFrameInterface = IABFrameInterface::Create(NULL);
            ASSERT_TRUE(NULL != iabFrameInterface);
            
            // Test version
            uint8_t version = 0;
            iabFrameInterface->GetVersion(version);
            EXPECT_EQ(version, kIABDefaultFrameVersion);    // check default
            
            version = 0xFF;
            EXPECT_EQ(iabFrameInterface->SetVersion(version), kIABNoError);    // Set version to 0xFF
            iabFrameInterface->GetVersion(version);    // Get and verify version
            EXPECT_EQ(version, 0xFF);
            
            // Test audio sampling rate
            IABSampleRateType sampleRate = kIABSampleRate_96000Hz;
            
            // default value
            iabFrameInterface->GetSampleRate(sampleRate);
            EXPECT_EQ(sampleRate, kIABSampleRate_48000Hz);
            
            // Set to 96KHz and verify
            EXPECT_EQ(iabFrameInterface->SetSampleRate(kIABSampleRate_96000Hz), kIABNoError);
            iabFrameInterface->GetSampleRate(sampleRate);
            EXPECT_EQ(sampleRate, kIABSampleRate_96000Hz);
            
            // Set to 48KHz and verify
            EXPECT_EQ(iabFrameInterface->SetSampleRate(kIABSampleRate_48000Hz), kIABNoError);
            iabFrameInterface->GetSampleRate(sampleRate);
            EXPECT_EQ(sampleRate, kIABSampleRate_48000Hz);
            
            // Test audio bit depth
            IABBitDepthType bitDepth = kIABBitDepth_16Bit;
            
            // default value
            iabFrameInterface->GetBitDepth(bitDepth);
            EXPECT_EQ(bitDepth, kIABBitDepth_24Bit);
            
            // Set to 16-bit and verify
            EXPECT_EQ(iabFrameInterface->SetBitDepth(kIABBitDepth_16Bit), kIABNoError);
            iabFrameInterface->GetBitDepth(bitDepth);
            EXPECT_EQ(bitDepth, kIABBitDepth_16Bit);
            
            // Set to 24-bit and verify
            EXPECT_EQ(iabFrameInterface->SetBitDepth(kIABBitDepth_24Bit), kIABNoError);
            iabFrameInterface->GetBitDepth(bitDepth);
            EXPECT_EQ(bitDepth, kIABBitDepth_24Bit);
            
            // Test frame rate
            IABFrameRateType frameRateCode = kIABFrameRate_120FPS;
            
            // default value
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_24FPS);
            
            // Set to 24 frames and verify
            frameRateCode = kIABFrameRate_120FPS;
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_24FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_24FPS);
            
            // Set to 25 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_25FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_25FPS);
            
            // Set to 30 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_30FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_30FPS);
            
            // Set to 48 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_48FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_48FPS);
            
            // Set to 50 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_50FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_50FPS);
            
            // Set to 60 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_60FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(kIABFrameRate_60FPS, frameRateCode);
            
            // Set to 96 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_96FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_96FPS);
            
            // Set to 100 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_100FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_100FPS);
            
            // Set to 120 frames and verify
            EXPECT_EQ(iabFrameInterface->SetFrameRate(kIABFrameRate_120FPS), kIABNoError);
            iabFrameInterface->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, kIABFrameRate_120FPS);
            
            // Test frame complexity MaxRendered
            IABMaxRenderedRangeType maxRendered;
            
            // Default, expect to initialize to 0
            iabFrameInterface->GetMaxRendered(maxRendered);
            EXPECT_EQ(maxRendered, 0);
            
            // Set to 1 and verify
            EXPECT_EQ(iabFrameInterface->SetMaxRendered(1), kIABNoError);
            iabFrameInterface->GetMaxRendered(maxRendered);
            EXPECT_EQ(maxRendered, 1);
            
            // Set to uint32_t max value and verify
            EXPECT_EQ(iabFrameInterface->SetMaxRendered(0xFFFFFFFF), kIABNoError);
            iabFrameInterface->GetMaxRendered(maxRendered);
            EXPECT_EQ(maxRendered, 0xFFFFFFFF);
            
            // Get/set frame sub element and count
            
            // Valid sub element for frame, valid elements will be deleted by library, no need to delete them at end of test
            IABBedDefinition *bedDefinitionElement = new IABBedDefinition(frameRateCode_);
            IABObjectDefinition *objectDefinitionElement = new IABObjectDefinition(frameRateCode_);
			IABAudioDataDLC *dlcAudioElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));
            IABAuthoringToolInfo *authoringToolElement = new IABAuthoringToolInfo();
            IABUserData *userDataElement = new IABUserData();
            
            // Invalid sub-element for frame
            IABFrame *frameElement = new IABFrame();
            IABBedRemap *remapElement = new IABBedRemap(5, 7, frameRateCode_);
            IABObjectZoneDefinition19 *objectZoneElement = new IABObjectZoneDefinition19(frameRateCode_);
            
            std::vector<IABElement*> invalidSubElements;
            std::vector<IABElement*> frameSubElements;
            std::vector<IABElement*> frameSubElementsGet;
            IABElementCountType count = 1;
            
            // Default
            iabFrameInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 0);
            
            // empty sub element vector
            iabFrameInterface->GetSubElements(frameSubElementsGet);
            EXPECT_EQ(frameSubElementsGet.size(), 0);
            
            // Invalid sub element test
            invalidSubElements.push_back(frameElement);
            EXPECT_EQ(iabFrameInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(remapElement);
            EXPECT_EQ(iabFrameInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            invalidSubElements.clear();
            invalidSubElements.push_back(objectZoneElement);
            EXPECT_EQ(iabFrameInterface->SetSubElements(invalidSubElements), kIABBadArgumentsError);
            
            frameSubElements.push_back(bedDefinitionElement);
            frameSubElements.push_back(objectDefinitionElement);
            frameSubElements.push_back(dlcAudioElement);
            frameSubElements.push_back(authoringToolElement);
            frameSubElements.push_back(userDataElement);
            
            // Set frame sub elements
            EXPECT_EQ(iabFrameInterface->SetSubElements(frameSubElements), kIABNoError);
            
            // Get and verify count
            iabFrameInterface->GetSubElementCount(count);
            EXPECT_EQ(count, 5);
            
            iabFrameInterface->GetSubElements(frameSubElementsGet);
            EXPECT_EQ(frameSubElementsGet.size(), 5);
            
            IABElementIDType id;
            IABElementSizeType elementSize;
            
            frameSubElementsGet[0]->GetElementID(id);
            EXPECT_EQ(id, kIABElementID_BedDefinition);
            frameSubElementsGet[0]->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, 0);
            
            frameSubElementsGet[1]->GetElementID(id);
            EXPECT_EQ(id, kIABElementID_ObjectDefinition);
            frameSubElementsGet[1]->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, 0);
            
            frameSubElementsGet[2]->GetElementID(id);
            EXPECT_EQ(id, kIABElementID_AudioDataDLC);
            frameSubElementsGet[2]->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, 0);
            
            frameSubElementsGet[3]->GetElementID(id);
            EXPECT_EQ(id, kIABElementID_AuthoringToolInfo);
            frameSubElementsGet[3]->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, 0);

            frameSubElementsGet[4]->GetElementID(id);
            EXPECT_EQ(id, kIABElementID_UserData);
            frameSubElementsGet[4]->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, 0);
            
            //SetSubElements method test
            // Set 2 sub-elements first.
            std::vector<IABElement*> validsubelements_1;
            validsubelements_1.push_back(bedDefinitionElement);
            validsubelements_1.push_back(objectDefinitionElement);
            validsubelements_1.push_back(dlcAudioElement);
            
            EXPECT_EQ(kIABNoError, iabFrameInterface->SetSubElements(validsubelements_1));
            
            // Set 1 sub-elements next.
            std::vector<IABElement*> validsubelements_2;
            validsubelements_2.push_back(bedDefinitionElement);
            validsubelements_2.push_back(dlcAudioElement);
            
            EXPECT_EQ(kIABNoError, iabFrameInterface->SetSubElements(validsubelements_2));
            
            // verify that only sub-element is present.
            std::vector<IABElement*> ret_subelments;
            iabFrameInterface->GetSubElements(ret_subelments);
            EXPECT_EQ(bedDefinitionElement, ret_subelments.at(0));
            EXPECT_EQ(dlcAudioElement, ret_subelments.at(1));
            EXPECT_EQ(2, ret_subelments.size());
            
            // Duplicate test
            std::vector<IABElement*> duplicate_subelements;
            duplicate_subelements.push_back(bedDefinitionElement);
            duplicate_subelements.push_back(dlcAudioElement);
            duplicate_subelements.push_back(bedDefinitionElement);
            EXPECT_EQ(kIABDuplicateSubElementsError, iabFrameInterface->SetSubElements(duplicate_subelements));
            
            
            // clean up
            
			// Test sub element packing enable/disable flag
			// Default = true
			EXPECT_EQ(iabFrameInterface->AreSubElementsEnabledForPacking(), true);

			iabFrameInterface->DisablePackingSubElements();
			EXPECT_EQ(iabFrameInterface->AreSubElementsEnabledForPacking(), false);

			iabFrameInterface->EnablePackingSubElements();
			EXPECT_EQ(iabFrameInterface->AreSubElementsEnabledForPacking(), true);

			// clean up
            // Delete invalid sub elements that have not been added to the frame
            delete frameElement;
            frameElement = NULL;
            
            delete remapElement;
            remapElement = NULL;
            
            delete objectZoneElement;
            objectZoneElement = NULL;
            
            IABFrameInterface::Delete(iabFrameInterface);
        }

        // **********************************************
        
        // Functions to set up for IABFrame Serialize tests
        
        // **********************************************

        void SetupPackerFrame()
        {
            SetupPackerFrameHeader();
            SetupPackerBedDefinition();
            SetupPackerObjectDefinition();
            SetupPackerDLCElement();
            
            std::vector<IABElement*> frameSubElements;
            
            // Push to frameSubElements and add to IAB packer frame instance
            frameSubElements.push_back(bedDefinitionElement_);
            frameSubElements.push_back(objectDefinitionElement_);
            frameSubElements.push_back(dlcAudioElement_);
            
            ASSERT_EQ(iabPackerFrame_->SetSubElements(frameSubElements), kIABNoError);
            iabPackerFrame_->GetSubElementCount(frameSubElementCount_);
            ASSERT_EQ(frameSubElementCount_, 3);
        }
        
        void SetupPackerFrameHeader()
        {
            // Set sample rate, bit depth, frame rate and maxrendered
            ASSERT_EQ(iabPackerFrame_->SetSampleRate(sampleRate_), kIABNoError);
            ASSERT_EQ(iabPackerFrame_->SetBitDepth(bitDepth_), kIABNoError);
            ASSERT_EQ(iabPackerFrame_->SetFrameRate(frameRateCode_), kIABNoError);
            ASSERT_EQ(iabPackerFrame_->SetMaxRendered(maxRendered_), kIABNoError);
        }
        
        void SetupPackerBedDefinition()
        {
            ASSERT_EQ(bedDefinitionElement_->SetMetadataID(bedDefinitionMetaID_), kIABNoError);
            // Leave the rest at default settings
        }
        
        void SetupPackerObjectDefinition()
        {
            ASSERT_EQ(objectDefinitionElement_->SetMetadataID(objectMetaID_), kIABNoError);
            ASSERT_EQ(objectDefinitionElement_->SetAudioDataID(objectAudioDataID_), kIABNoError);
            
            // Leave the rest at default settings
            
            // After added to objectDefinition with SetPanSubBlocks call, these IABObjectSubBlocks will be deleted by IABObjectDefinition destructor
            // No need to explicitly delete them at end of test
            
            std::vector<IABObjectSubBlock*> panSubBlocks;
            
            for (uint8_t i = 0; i < numPanSubblocks_; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
                
                // Leave the rest at default settings
                
                // Add to object
                panSubBlocks.push_back(subBlock);
            }
            
            uint8_t numPanSubBlocks = 0;
            
            // Set subBlocks and check block count
            ASSERT_EQ(objectDefinitionElement_->SetPanSubBlocks(panSubBlocks), kIABNoError);
            objectDefinitionElement_->GetNumPanSubBlocks(numPanSubBlocks);
            ASSERT_EQ(numPanSubBlocks, numPanSubblocks_);
        }

        void SetupPackerDLCElement()
        {
            ASSERT_EQ(dlcAudioElement_->SetAudioDataID(dlcAudioDataID_), kIABNoError);
            ASSERT_EQ(dlcAudioElement_->SetDLCSampleRate(sampleRate_), kIABNoError);
            ASSERT_EQ(dlcAudioElement_->SetDLCSize(dlcSize_), kIABNoError);
            
            // Use silent audio samples
            int32_t* pAudioSamples = new int32_t[frameSampleCount_];
			memset(pAudioSamples, 0, sizeof(int32_t)*frameSampleCount_);

            ASSERT_EQ(dlcAudioElement_->EncodeMonoPCMToDLC(pAudioSamples, frameSampleCount_), kIABNoError);
            
            delete [] pAudioSamples;
        }

        // **********************************************
        // Functions for IABFrame DeSerialize tests. High level integrity checks
        // **********************************************

        void VerifyDeSerializedFrame()
        {
            // Get and verify sample rate, bit depth, frame rate and maxrendered from de-serialized frame
            uint8_t version = 0;
            IABSampleRateType           sampleRate;
            IABBitDepthType             bitDepth;
            IABFrameRateType            frameRateCode;
            IABMaxRenderedRangeType     maxRendered;
            
            iabParserFrame_->GetVersion(version);
            EXPECT_EQ(version, kIABDefaultFrameVersion);
            
            iabParserFrame_->GetSampleRate(sampleRate);
            EXPECT_EQ(sampleRate, sampleRate_);
            
            iabParserFrame_->GetBitDepth(bitDepth);
            EXPECT_EQ(bitDepth, bitDepth_);
            
            iabParserFrame_->GetFrameRate(frameRateCode);
            EXPECT_EQ(frameRateCode, frameRateCode_);
            
            iabParserFrame_->GetMaxRendered(maxRendered);
            EXPECT_EQ(maxRendered, maxRendered_);

            // Check sub-elements from de-serialized frame
            std::vector<IABElement*> frameSubElements;
            IABElementCountType count = 0;

            iabParserFrame_->GetSubElements(frameSubElements);
            iabParserFrame_->GetSubElementCount(count);
            EXPECT_EQ(count, frameSubElementCount_);
            EXPECT_EQ(frameSubElements.size(), frameSubElementCount_);
            
            IABBedDefinition        *parsedBedDefinitionElement;
            IABObjectDefinition     *parsedObjectDefinitionElement;
            IABAudioDataDLC         *parsedDLCAudioElement;
            IABMetadataIDType       bedDefinitionMetaID;
            IABMetadataIDType       objectMetaID;
            IABAudioDataIDType      objectAudioDataID;

            // Check that first sub-element is a IABBedDefinition
            parsedBedDefinitionElement = dynamic_cast<IABBedDefinition*>(frameSubElements[0]);
            ASSERT_TRUE(parsedBedDefinitionElement != NULL);
            parsedBedDefinitionElement->GetMetadataID(bedDefinitionMetaID);
            EXPECT_EQ(bedDefinitionMetaID, bedDefinitionMetaID_);

            // Check that second sub-element is a IABObjectDefinition element
            parsedObjectDefinitionElement = dynamic_cast<IABObjectDefinition*>(frameSubElements[1]);
            ASSERT_TRUE(parsedObjectDefinitionElement != NULL);
            parsedObjectDefinitionElement->GetMetadataID(objectMetaID);
            parsedObjectDefinitionElement->GetAudioDataID(objectAudioDataID);
            EXPECT_EQ(objectMetaID, objectMetaID_);
            EXPECT_EQ(objectAudioDataID, objectAudioDataID_);
            
            // Check that third sub_element is a IABAudioDataDLC element
            parsedDLCAudioElement = dynamic_cast<IABAudioDataDLC*>(frameSubElements[2]);
            ASSERT_TRUE(parsedDLCAudioElement != NULL);
        }
        
        // **********************************************
        // Function to test IABFrame Serialize() and DeSerialize()
        // **********************************************
        
        void TestSerializeDeSerialize()
        {
			// Out-of-range frame rate returns 0
            ASSERT_EQ(GetIABNumSubBlocks(static_cast<IABFrameRateType>(10)), 0);
            
            // Test default setting: 24 Frames per second (FPS), 24-bit, 48KHz sample rate
            // Note Current DLC implmentation does not support 96KHz sample rate, so no 96KHz test
            sampleRate_ = kIABSampleRate_48000Hz;
            bitDepth_ = kIABBitDepth_24Bit;
            frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
            ASSERT_EQ(frameSampleCount_, 2000);
            ASSERT_EQ(numPanSubblocks_, 8);
            RunSerializeDeSerializeTestCase();
            
            // Test 25 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_25FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 1920);
            ASSERT_EQ(numPanSubblocks_, 8);
            RunSerializeDeSerializeTestCase();

            // Test 30 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_30FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 1600);
            ASSERT_EQ(numPanSubblocks_, 8);
            RunSerializeDeSerializeTestCase();
            
            // Test 48 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_48FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 1000);
            ASSERT_EQ(numPanSubblocks_, 4);
            RunSerializeDeSerializeTestCase();
            
            // Test 50 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_50FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 960);
            ASSERT_EQ(numPanSubblocks_, 4);
            RunSerializeDeSerializeTestCase();
            
            // Test 60 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_60FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 800);
            ASSERT_EQ(numPanSubblocks_, 4);
            RunSerializeDeSerializeTestCase();
            
            // Test 96 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_96FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 500);
            ASSERT_EQ(numPanSubblocks_, 2);
            RunSerializeDeSerializeTestCase();
            
            // Test 100 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_100FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 480);
            ASSERT_EQ(numPanSubblocks_, 2);
            RunSerializeDeSerializeTestCase();
            
            // Test 120 Frames per second (FPS), bit depth and sample rate unchanged
            frameRateCode_ = kIABFrameRate_120FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 400);
            ASSERT_EQ(numPanSubblocks_, 2);
            RunSerializeDeSerializeTestCase();
            
            // Test 16-bit sample coding
            sampleRate_ = kIABSampleRate_48000Hz;
            bitDepth_ = kIABBitDepth_16Bit;
            frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 2000);
            ASSERT_EQ(numPanSubblocks_, 8);
            RunSerializeDeSerializeTestCase();
        }
        
        void RunSerializeDeSerializeTestCase()
        {
            // Create IAB packer frame
            iabPackerFrame_ = new IABFrame();
            ASSERT_TRUE(NULL != iabPackerFrame_);
            
            // Sub elements to be used in IAB frame, 3 in total
            // One empty bed definition (no bed channels), one object, one DLC element
            // After added to IABFrame, these sub-elements will be deleted by IABFrame destructor, no need to delete them at end of test
            bedDefinitionElement_ = new IABBedDefinition(frameRateCode_);
            objectDefinitionElement_ = new IABObjectDefinition(frameRateCode_);
			dlcAudioElement_ = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));

            ASSERT_TRUE(NULL != bedDefinitionElement_);
            ASSERT_TRUE(NULL != objectDefinitionElement_);
            ASSERT_TRUE(NULL != dlcAudioElement_);
            
            // Set up IAB packer frame
            SetupPackerFrame();

            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serial frame into stream
            ASSERT_EQ(iabPackerFrame_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            std::ios_base::streampos pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerFrame_->GetElementSize(elementSize);
            
            bytesInStream -= 12;		// Deduct 1). 5 bytes for IA preamble (default state, 0-length)
										// 2). 5 bytes for IA subframe, and 
										// 3). 2 bytes for IAB frame ID and element size code
            
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
            
            ASSERT_EQ(elementSize, bytesInStream);

            // Reset  stream to beginning
            elementBuffer.seekg(0, std::ios::beg);

            // Create IAB frame to operate on stream buffer
            iabParserFrame_ = new IABFrame(&elementBuffer);
            ASSERT_TRUE(NULL != iabParserFrame_);
            
            // DeSerialize frame from stream
            ASSERT_EQ(iabParserFrame_->DeSerialize(), kIABNoError);
            
            // Verify deserialized frame
            VerifyDeSerializedFrame();
            
            // Verify de-serialized frame element size
            
            iabParserFrame_->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, bytesInStream);

			delete iabPackerFrame_;
			delete iabParserFrame_;
        }
        
        void TestSubElementManagement()
        {
            IABFrameInterface* iabFrameInterface = NULL;
            iabFrameInterface = IABFrameInterface::Create(NULL);
            ASSERT_TRUE(NULL != iabFrameInterface);
            
            // Sub elements to be used in IAB frame, 3 in total
            // One empty bed definition (no bed channels), one object, one DLC element
            // After added to IABFrame, these sub-elements will be deleted by IABFrame destructor, no need to delete them at end of test
            bedDefinitionElement_ = new IABBedDefinition(frameRateCode_);
            objectDefinitionElement_ = new IABObjectDefinition(frameRateCode_);
			dlcAudioElement_ = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));

            ASSERT_TRUE(NULL != bedDefinitionElement_);
            ASSERT_TRUE(NULL != objectDefinitionElement_);
            ASSERT_TRUE(NULL != dlcAudioElement_);
            
            // Null element test
            EXPECT_EQ(kIABBadArgumentsError, iabFrameInterface->AddSubElement(NULL));
            EXPECT_EQ(false, iabFrameInterface->IsSubElement(NULL));

            // Add subelements
            EXPECT_EQ(kIABNoError, iabFrameInterface->AddSubElement(bedDefinitionElement_));
            EXPECT_EQ(kIABNoError, iabFrameInterface->AddSubElement(objectDefinitionElement_));
            EXPECT_EQ(kIABNoError, iabFrameInterface->AddSubElement(dlcAudioElement_));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(bedDefinitionElement_));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(objectDefinitionElement_));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(dlcAudioElement_));

            // Remove a sub-element
            EXPECT_EQ(kIABNoError, iabFrameInterface->RemoveSubElement(dlcAudioElement_));
            EXPECT_EQ(false, iabFrameInterface->IsSubElement(dlcAudioElement_));
            
            //Add already existing one
            EXPECT_EQ(kIABDuplicateSubElementsError, iabFrameInterface->AddSubElement(bedDefinitionElement_));
            
            // Remove all subelments
            EXPECT_EQ(kIABNoError, iabFrameInterface->RemoveSubElement(bedDefinitionElement_));
            EXPECT_EQ(kIABNoError, iabFrameInterface->RemoveSubElement(objectDefinitionElement_));
            
            EXPECT_EQ(false, iabFrameInterface->IsSubElement(bedDefinitionElement_));
            EXPECT_EQ(false, iabFrameInterface->IsSubElement(objectDefinitionElement_));
            
            // Object Definition interface test
            IABObjectDefinition *objectDefinitionElement = new IABObjectDefinition(frameRateCode_);
            IABObjectZoneDefinition19 *objectZoneElement = new IABObjectZoneDefinition19(frameRateCode_);
            IABBedRemap *remapElement = new IABBedRemap(5, 7, frameRateCode_);
            EXPECT_EQ(kIABNoError, objectDefinitionElement->AddSubElement(objectZoneElement));
            EXPECT_EQ(kIABNoError, objectDefinitionElement->AddSubElement(objectDefinitionElement_));
            
            // Null element test
            EXPECT_EQ(kIABBadArgumentsError, objectDefinitionElement->AddSubElement(NULL));
            EXPECT_EQ(false, objectDefinitionElement->IsSubElement(NULL));
            
            // Invalid element insert test into ObjectDefintion
            EXPECT_EQ(kIABBadArgumentsError, objectDefinitionElement->AddSubElement(remapElement));
            
            //Add already existing one
            EXPECT_EQ(kIABDuplicateSubElementsError, objectDefinitionElement->AddSubElement(objectDefinitionElement_));
            
            EXPECT_EQ(kIABNoError, iabFrameInterface->AddSubElement(objectDefinitionElement));
            EXPECT_EQ(true, objectDefinitionElement->IsSubElement(objectZoneElement));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(objectDefinitionElement));
            
            // Remove tests
            EXPECT_EQ(kIABNoError, objectDefinitionElement->RemoveSubElement(objectDefinitionElement_));
            EXPECT_EQ(kIABNoError, objectDefinitionElement->RemoveSubElement(objectZoneElement));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(objectDefinitionElement));
            EXPECT_EQ(false, objectDefinitionElement->IsSubElement(objectZoneElement));
            EXPECT_EQ(false, objectDefinitionElement->IsSubElement(objectDefinitionElement_));

            // Bed Definition interface tests
            IABBedDefinition *bedDefinitionElement = new IABBedDefinition(frameRateCode_);
            EXPECT_EQ(kIABBadArgumentsError, bedDefinitionElement->AddSubElement(objectZoneElement));  // invalid element insert
            EXPECT_EQ(kIABNoError, bedDefinitionElement->AddSubElement(remapElement));                 // valid element insert
            EXPECT_EQ(kIABNoError, bedDefinitionElement->AddSubElement(bedDefinitionElement_));        // valid element insert
            
            // Null element test
            EXPECT_EQ(kIABBadArgumentsError, bedDefinitionElement->AddSubElement(NULL));
            EXPECT_EQ(false, bedDefinitionElement->IsSubElement(NULL));
            
            //Add already existing one
            EXPECT_EQ(kIABDuplicateSubElementsError, bedDefinitionElement->AddSubElement(remapElement));
            
            EXPECT_EQ(kIABNoError, iabFrameInterface->AddSubElement(bedDefinitionElement));
            EXPECT_EQ(true, bedDefinitionElement->IsSubElement(remapElement));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(bedDefinitionElement));
            EXPECT_EQ(false, iabFrameInterface->IsSubElement(objectZoneElement));
            
            // Remove tests
            EXPECT_EQ(kIABNoError, bedDefinitionElement->RemoveSubElement(remapElement));
            EXPECT_EQ(kIABNoError, bedDefinitionElement->RemoveSubElement(bedDefinitionElement_));
            EXPECT_EQ(true, iabFrameInterface->IsSubElement(bedDefinitionElement));
            EXPECT_EQ(false, bedDefinitionElement->IsSubElement(remapElement));
            EXPECT_EQ(false, bedDefinitionElement->IsSubElement(bedDefinitionElement_));
            
            delete objectDefinitionElement_;
            delete remapElement;
            delete objectZoneElement;
            delete bedDefinitionElement_;
            delete dlcAudioElement_;

            IABFrameInterface::Delete(iabFrameInterface);
        }
        
		void TestSubElementPackingInclusion()
		{
			// Create IAB packer frame
			iabPackerFrame_ = new IABFrame();
			ASSERT_TRUE(NULL != iabPackerFrame_);

			// Sub elements to be used in IAB frame, 3 in total
			// One empty bed definition (no bed channels), one object, one DLC element
			// After added to IABFrame, these sub-elements will be deleted by IABFrame destructor, no need to delete them at end of test
			bedDefinitionElement_ = new IABBedDefinition(frameRateCode_);
			objectDefinitionElement_ = new IABObjectDefinition(frameRateCode_);
			dlcAudioElement_ = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));

			ASSERT_TRUE(NULL != bedDefinitionElement_);
			ASSERT_TRUE(NULL != objectDefinitionElement_);
			ASSERT_TRUE(NULL != dlcAudioElement_);

			// Set up IAB packer frame
			SetupPackerFrame();

			// stream buffer to hold serialzed stream
			std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

			// Case 1: Default sub-element packing behavior, enabled
			// Sub element packing is enabled by default. All 3 sub-elements are packed.
			// Serial frame into stream
			ASSERT_EQ(iabPackerFrame_->Serialize(elementBuffer), kIABNoError);
			// Reset  stream to beginning
			elementBuffer.seekg(0, std::ios::beg);

			// Create IAB frame to operate on stream buffer
			iabParserFrame_ = new IABFrame(&elementBuffer);
			ASSERT_TRUE(NULL != iabParserFrame_);

			// DeSerialize frame from stream
			ASSERT_EQ(iabParserFrame_->DeSerialize(), kIABNoError);

			IABElementCountType parsedSubElementCount = 0;
			iabParserFrame_->GetSubElementCount(parsedSubElementCount);
			EXPECT_EQ(parsedSubElementCount, 3);						// 3 sub elements as constructed above

			// Clean up parsed frame
			delete iabParserFrame_;
			elementBuffer.str("");

			// Case 2: Disable packing sub-elements at IABFrame level: none packed (almost an "empty" IABFrame)
			//
			iabPackerFrame_->DisablePackingSubElements();

			// Serial frame into stream
			ASSERT_EQ(iabPackerFrame_->Serialize(elementBuffer), kIABNoError);
			// Reset  stream to beginning
			elementBuffer.seekg(0, std::ios::beg);

			// Create IAB frame to operate on stream buffer
			iabParserFrame_ = new IABFrame(&elementBuffer);
			ASSERT_TRUE(NULL != iabParserFrame_);

			// DeSerialize frame from stream
			ASSERT_EQ(iabParserFrame_->DeSerialize(), kIABNoError);

			iabParserFrame_->GetSubElementCount(parsedSubElementCount);
			EXPECT_EQ(parsedSubElementCount, 0);						// none packed

			// Clean up parsed frame
			delete iabParserFrame_;
			elementBuffer.str("");

			// Case 3: Re-enable packing sub-elements at IABFrame level
			// However, disable packing of objectDefinitionElement_
			//
			iabPackerFrame_->EnablePackingSubElements();

			// Disable packing of objectDefinitionElement_ only
			objectDefinitionElement_->DisablePacking();

			// Serial frame into stream
			ASSERT_EQ(iabPackerFrame_->Serialize(elementBuffer), kIABNoError);
			// Reset  stream to beginning
			elementBuffer.seekg(0, std::ios::beg);

			// Create IAB frame to operate on stream buffer
			iabParserFrame_ = new IABFrame(&elementBuffer);
			ASSERT_TRUE(NULL != iabParserFrame_);

			// DeSerialize frame from stream
			ASSERT_EQ(iabParserFrame_->DeSerialize(), kIABNoError);

			iabParserFrame_->GetSubElementCount(parsedSubElementCount);
			EXPECT_EQ(parsedSubElementCount, 2);						// only 2 are in ctream, with objectDefinitionElement_ not packed

			delete iabPackerFrame_;
			delete iabParserFrame_;
		}

		// Jan. 20, 2020 - Add Fractional Frame Rate (FFR) serialize-deserialize test
		//
		void SetupPackerPCMElement()
		{
			ASSERT_EQ(pcmAudioElement_->SetAudioDataID(pcmAudioDataID_), kIABNoError);

			// Use silent audio samples
			int32_t* pAudioSamples = new int32_t[frameSampleCount_];
			memset(pAudioSamples, 0, sizeof(int32_t)*frameSampleCount_);

			ASSERT_EQ(pcmAudioElement_->PackMonoSamplesToPCM(pAudioSamples, frameSampleCount_), kIABNoError);

			delete[] pAudioSamples;
		}

		void TestFFRSerializeDeSerialize()
		{
			// Test fractiona frame rate of 23_976FPS Frames per second (FPS), 24-bit, 48KHz sample rate
			// Note PCM elements only is allowed. No DLC.
			sampleRate_ = kIABSampleRate_48000Hz;
			bitDepth_ = kIABBitDepth_24Bit;
			frameRateCode_ = kIABFrameRate_23_976FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			numPanSubblocks_ = GetIABNumSubBlocks(frameRateCode_);
			ASSERT_EQ(frameSampleCount_, 2002);
			ASSERT_EQ(numPanSubblocks_, 8);
			RunFFRSerializeDeSerializeTestCase();

			// More FFR case can be added below
		}

        void RunFFRSerializeDeSerializeTestCase()
        {
            // Create IAB packer frame
            iabPackerFrame_ = new IABFrame();
            ASSERT_TRUE(NULL != iabPackerFrame_);
            
            // Sub elements to be used in IAB frame, 3 in total
            // One empty bed definition (no bed channels), one object, one PCM element
            // After added to IABFrame, these sub-elements will be deleted by IABFrame destructor, no need to delete them at end of test
            bedDefinitionElement_ = new IABBedDefinition(frameRateCode_);
            objectDefinitionElement_ = new IABObjectDefinition(frameRateCode_);
			pcmAudioElement_ = dynamic_cast<IABAudioDataPCM*>(IABAudioDataPCMInterface::Create(frameRateCode_, sampleRate_, bitDepth_));

            ASSERT_TRUE(NULL != bedDefinitionElement_);
            ASSERT_TRUE(NULL != objectDefinitionElement_);
            ASSERT_TRUE(NULL != pcmAudioElement_);
            
            // Set up IAB packer frame
			SetupPackerFrameHeader();
			SetupPackerBedDefinition();
			SetupPackerPCMElement();

			// Switch to link object to use PCM element audio data ID before setting up object for FFR test
			objectAudioDataID_ = 31;
			SetupPackerObjectDefinition();

			std::vector<IABElement*> frameSubElements;

			// Push to frameSubElements and add to IAB packer frame instance
			frameSubElements.push_back(bedDefinitionElement_);
			frameSubElements.push_back(objectDefinitionElement_);
			frameSubElements.push_back(pcmAudioElement_);

			ASSERT_EQ(iabPackerFrame_->SetSubElements(frameSubElements), kIABNoError);
			iabPackerFrame_->GetSubElementCount(frameSubElementCount_);
			ASSERT_EQ(frameSubElementCount_, 3);

			// Set up for serialize-deserialize test
			//
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serial frame into stream
            ASSERT_EQ(iabPackerFrame_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            std::ios_base::streampos pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerFrame_->GetElementSize(elementSize);
            
            bytesInStream -= 12;		// Deduct 1). 5 bytes for IA preamble (default state, 0-length)
										// 2). 5 bytes for IA subframe, and 
										// 3). 2 bytes for IAB frame ID and element size code
            
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
            
            ASSERT_EQ(elementSize, bytesInStream);

            // Reset  stream to beginning
            elementBuffer.seekg(0, std::ios::beg);

            // Create IAB frame to operate on stream buffer
            iabParserFrame_ = new IABFrame(&elementBuffer);
            ASSERT_TRUE(NULL != iabParserFrame_);
            
            // DeSerialize frame from stream
            ASSERT_EQ(iabParserFrame_->DeSerialize(), kIABNoError);
            
            // Verify de-serialized frame element size
            iabParserFrame_->GetElementSize(elementSize);
            EXPECT_EQ(elementSize, bytesInStream);

			// Verify deserialized frame
			// Get and verify sample rate, bit depth, frame rate and maxrendered from de-serialized frame
			uint8_t version = 0;
			IABSampleRateType           sampleRate;
			IABBitDepthType             bitDepth;
			IABFrameRateType            frameRateCode;
			IABMaxRenderedRangeType     maxRendered;

			iabParserFrame_->GetVersion(version);
			EXPECT_EQ(version, kIABDefaultFrameVersion);

			iabParserFrame_->GetSampleRate(sampleRate);
			EXPECT_EQ(sampleRate, sampleRate_);

			iabParserFrame_->GetBitDepth(bitDepth);
			EXPECT_EQ(bitDepth, bitDepth_);

			iabParserFrame_->GetFrameRate(frameRateCode);
			EXPECT_EQ(frameRateCode, frameRateCode_);

			iabParserFrame_->GetMaxRendered(maxRendered);
			EXPECT_EQ(maxRendered, maxRendered_);

			// Check sub-elements from de-serialized frame
			frameSubElements.clear();
			IABElementCountType count = 0;

			iabParserFrame_->GetSubElements(frameSubElements);
			iabParserFrame_->GetSubElementCount(count);
			EXPECT_EQ(count, frameSubElementCount_);
			EXPECT_EQ(frameSubElements.size(), frameSubElementCount_);

			IABBedDefinition        *parsedBedDefinitionElement;
			IABObjectDefinition     *parsedObjectDefinitionElement;
			IABAudioDataPCM         *parsedPCMAudioElement;
			IABMetadataIDType       bedDefinitionMetaID;
			IABMetadataIDType       objectMetaID;
			IABAudioDataIDType      objectAudioDataID;

			// Check that first sub-element is a IABBedDefinition
			parsedBedDefinitionElement = dynamic_cast<IABBedDefinition*>(frameSubElements[0]);
			ASSERT_TRUE(parsedBedDefinitionElement != NULL);
			parsedBedDefinitionElement->GetMetadataID(bedDefinitionMetaID);
			EXPECT_EQ(bedDefinitionMetaID, bedDefinitionMetaID_);

			// Check that second sub-element is a IABObjectDefinition element
			parsedObjectDefinitionElement = dynamic_cast<IABObjectDefinition*>(frameSubElements[1]);
			ASSERT_TRUE(parsedObjectDefinitionElement != NULL);
			parsedObjectDefinitionElement->GetMetadataID(objectMetaID);
			parsedObjectDefinitionElement->GetAudioDataID(objectAudioDataID);
			EXPECT_EQ(objectMetaID, objectMetaID_);
			EXPECT_EQ(objectAudioDataID, objectAudioDataID_);

			// Check that third sub_element is an IABAudioDataPCM element
			parsedPCMAudioElement = dynamic_cast<IABAudioDataPCM*>(frameSubElements[2]);
			ASSERT_TRUE(parsedPCMAudioElement != NULL);

			// Restore default object's linked audio data ID
			objectAudioDataID_ = 30;

			delete iabPackerFrame_;
			delete iabParserFrame_;
        }

    private:
        IABFrame*   iabPackerFrame_;
        IABFrame*   iabParserFrame_;
        IABBedDefinition            *bedDefinitionElement_;
        IABObjectDefinition         *objectDefinitionElement_;
        IABAudioDataDLC             *dlcAudioElement_;
		IABAudioDataPCM             *pcmAudioElement_;

        // Frame data members
        IABSampleRateType           sampleRate_;
        IABBitDepthType             bitDepth_;
        IABFrameRateType            frameRateCode_;
        IABMaxRenderedRangeType     maxRendered_;
        uint32_t                    frameSampleCount_;
        uint32_t                    frameSubElementCount_;
        
        // Bed definition data members
        IABMetadataIDType           bedDefinitionMetaID_;
        
        // Object defintition data members
        IABMetadataIDType           objectMetaID_;
        IABAudioDataIDType          objectAudioDataID_;
        uint8_t                     numPanSubblocks_;
        
        // DLC data members
        IABAudioDataIDType          dlcAudioDataID_;
        uint16_t                    dlcSize_;
        
		// PCM data members
		IABAudioDataIDType          pcmAudioDataID_;
    };

    // ********************
    // Run tests
    // ********************
    
    // Run frame element setters and getters API tests
    TEST_F(IABFrameElement_Test, Test_Setters_Getters_APIS)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial frame, then deSerialize frame tests
    TEST_F(IABFrameElement_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
    // Test SubElement addition/remove
    TEST_F(IABFrameElement_Test, Test_SubElementManagement)
    {
        TestSubElementManagement();
    }

	// Test dynamic packing
	TEST_F(IABFrameElement_Test, Test_SubElementPacking_Inclusion)
	{
		TestSubElementPackingInclusion();
	}

	// Test Fractional Frame rate (FFR) serialize-deserialize
	TEST_F(IABFrameElement_Test, Test_FractionalFrameRate_Serialize_DeSerialize)
	{
		TestFFRSerializeDeSerialize();
	}
}
