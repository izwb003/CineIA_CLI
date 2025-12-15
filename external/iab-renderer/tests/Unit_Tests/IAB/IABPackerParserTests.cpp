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
#include "packer/IABPacker.h"
#include "parser/IABParser.h"

#include <vector>
#include <memory>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    
    // IABPacker and IABParser API tests:
    // 1. Test setters and getter APIs
    // 2. Test Packing into a stream (packed buffer)
    // 3. Test Parsing from the stream (packed buffer).
    // 4. Do high level checks on IABPacker and IABParser APIs. Note that detailed tests are done by the respective IAB elements, i.e. IABBedDefinition, etc..
    
    class IABPackerParser_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabPacker_ = NULL;
            iabParser_ = NULL;
            
            bedID_ = 0;
            audioIDStart_ = 1;  // audio ID starts from 1
            audioIDIndex_ = audioIDStart_;
            objectMetaDataIDStart_ = 1;
            sampleRate_ = kIABSampleRate_48000Hz;
            frameRateCode_ = kIABFrameRate_24FPS;
            frameSampleCount_ = 2000;   // 2000 samples for 24FPS
            numObjects_ = 1;
            numBedChannels_ = 0;
            numBedDefinitions_ = 0;
            numElementsInPackerFrame_ = 0;
            expectedElementsInParsedFrame_ = 0;
            
            panParameters_.panInfoExists_ = 1;
            panParameters_.position_.setIABObjectPosition(0.0f, 0.0f, 0.0f);
            panParameters_.spread_.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 0.0f, 0.0f, 0.0f);
            panParameters_.objectGain_.setIABGain(1.0f);
            bedLayout_ = kIABUseCase_NoUseCase;
            
            authoringToolInfo_.clear();
            userDataBytes_.clear();

			uint8_t* userID = userID_;

			for (uint8_t i = 0; i < 16; i++)
			{
				*userID++ = 0;
			}
		}
        
        // void TearDown() to do any clean-up
        void TearDown()
        {
            // Nothing to clean up
        }
        
        // **********************************************
        // Functions to set up for IABFrame for packing and test IABPacker APIs
        // **********************************************

        void AddElementsToFrame()
        {
           // Add each element type to IAB frame
            
            // Add single object first. Metadata ID = 1, audio ID = 1
            if (numObjects_ > 0)
            {
                AddObjectDefinition();
                numElementsInPackerFrame_ = numObjects_;
                expectedElementsInParsedFrame_ = numObjects_;
            }
            
            numBedChannels_ = 0;

            if (kIABUseCase_5_1 == bedLayout_)
            {
                numBedChannels_ = 6;
                AddBedDefinition51();
                numElementsInPackerFrame_++;
                expectedElementsInParsedFrame_++;
            }
            else if (kIABUseCase_7_1_DS == bedLayout_)
            {
                numBedChannels_ = 8;
                AddBedDefinition71DS();
                numElementsInPackerFrame_++;
                expectedElementsInParsedFrame_++;
            }
            else if (kIABUseCase_9_1_OH == bedLayout_)
            {
                numBedChannels_ = 10;
                AddBedDefinition91OH();
                numElementsInPackerFrame_++;
                expectedElementsInParsedFrame_++;
            }

            numDLCElements_ = numObjects_ + numBedChannels_;
            AddDLCElements();
            numElementsInPackerFrame_ += numDLCElements_;
            expectedElementsInParsedFrame_ += numDLCElements_;
            
            if (authoringToolInfo_.size() > 0)
            {
                ASSERT_EQ(iabPacker_->AddAuthoringToolInfo(authoringToolInfo_), kIABNoError);
                numElementsInPackerFrame_++;
				expectedElementsInParsedFrame_++;
			}
            
			if (userDataBytes_.size() > 0)
			{
				ASSERT_EQ(iabPacker_->AddUserData(userID_, userDataBytes_), kIABNoError);
				numElementsInPackerFrame_++;
				expectedElementsInParsedFrame_++;
			}

            // Set up object meta data and panning information
            UpdateObjectMetaData();
            
            // Update audio samples in DLC elements
            UpdateAudioSamples();
        }
        
        void AddObjectDefinition()
        {
            // Start of object meta data ID. It uniquely identifies each object
            IABMetadataIDType objectMetaDataIndex = objectMetaDataIDStart_;
            
			IABObjectDefinitionInterface* outputPointer = NULL;

			for (uint32_t i = 0; i < numObjects_; i++)
            {
                // Both meta data ID and audio ID increment sequentially
                // Test IABPacker AddObjectDefinition API
                ASSERT_EQ(iabPacker_->AddObjectDefinition(objectMetaDataIndex++, audioIDIndex_++, outputPointer), kIABNoError);
            }
        }
        
        void AddBedDefinition51()
        {
            // Create a channel ID, audio ID map
            IABBedMappingInfo bedInfo(kIABUseCase_5_1,bedID_);
            
            // Assoicate bed channel ID with a audio ID. Audio ID increments sequentially for each channel
            bedInfo.lookupMap_[kIABChannelID_Left] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Center] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Right] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LFE] = audioIDIndex_++;
            
            // Test IABPacker AddBedDefinition API
            // Calls with bed definition meta data ID, bed use case and ID map
			IABBedDefinitionInterface* outputPointer = NULL;
            ASSERT_EQ(iabPacker_->AddBedDefinition(bedInfo.getMetadataID(),bedInfo.getUseCase(), bedInfo, outputPointer), kIABNoError);
        }
        
        void AddBedDefinition71DS()
        {
            // Create a channel ID, audio ID map
            IABBedMappingInfo bedInfo(kIABUseCase_7_1_DS,bedID_);
            
            // Assoicate bed channel ID with a audio ID. Audio ID increments sequentially for each channel
            // Note that std::map sort according to key in ascending value
            bedInfo.lookupMap_[kIABChannelID_Left] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Center] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Right] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftSideSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftRearSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightRearSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightSideSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LFE] = audioIDIndex_++;
            
            // Test IABPacker AddBedDefinition API
            // Calls with bed definition meta data ID, bed use case and ID map
			IABBedDefinitionInterface* outputPointer = NULL;
			ASSERT_EQ(iabPacker_->AddBedDefinition(bedInfo.getMetadataID(),bedInfo.getUseCase(),bedInfo, outputPointer), kIABNoError);
        }
        
        void AddBedDefinition91OH()
        {
            // Create a channel ID, audio ID map
            IABBedMappingInfo bedInfo(kIABUseCase_9_1_OH,bedID_);
            
            // Assoicate bed channel ID with a audio ID. Audio ID increments sequentially for each channel
            // Note that std::map sort according to key in ascending value
            bedInfo.lookupMap_[kIABChannelID_Left] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Center] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_Right] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftSideSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftRearSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightRearSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightSideSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LeftTopSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_RightTopSurround] = audioIDIndex_++;
            bedInfo.lookupMap_[kIABChannelID_LFE] = audioIDIndex_++;
            
            // Test IABPacker AddBedDefinition API
            // Calls with bed definition meta data ID, bed use case and ID map
			IABBedDefinitionInterface* outputPointer = NULL;
			ASSERT_EQ(iabPacker_->AddBedDefinition(bedInfo.getMetadataID(), bedInfo.getUseCase(),bedInfo, outputPointer), kIABNoError);
        }

        void AddDLCElements()
        {
            // Create a list of uniuqe audio IDs for each DLC element
            
            IABAudioDataIDType audioID = audioIDStart_;
            
            for (uint32_t i = 0; i < numDLCElements_; i++)
            {
                audioDataIDList_.push_back(audioID++);
            }
            
            // Test IABPacker AddDLCElements API
            // Create and add DLC elements to IAB frame, one for each audio source
            // Each bed channel or object is linked to the DLC element through the audio ID
            ASSERT_EQ(iabPacker_->AddDLCElements(audioDataIDList_), kIABNoError);
        }
        
        void UpdateAudioSamples()
        {
            // map linking object audio ID and its corresponding audio samples for updating DLC elements
            std::map<IABAudioDataIDType, int32_t*> audioSamplesMap;
            std::map<IABAudioDataIDType, int32_t*>::iterator iterASM;

            std::vector<IABAudioDataIDType>::const_iterator iterADL;
            for (iterADL = audioDataIDList_.begin(); iterADL != audioDataIDList_.end(); iterADL++)
            {
                // Use silent audio samples
                int32_t* pAudioSamples = new int32_t[frameSampleCount_];
				memset(pAudioSamples, 0, sizeof(int32_t)*frameSampleCount_);
                
                if (pAudioSamples)
                {
                    audioSamplesMap[*iterADL] = pAudioSamples;
                }
            }
            
            // Test IABPacker UpdateAudioSamples API
            // Update samples and run DLC encoding
            iabError returnCode = iabPacker_->UpdateAudioSamples(audioSamplesMap);
            
            // Delete temporary sample buffers
            for (iterASM = audioSamplesMap.begin(); iterASM != audioSamplesMap.end(); iterASM++)
            {
                delete[] iterASM->second;
            }

            // Ensure all DLC elements updated
            ASSERT_EQ(audioSamplesMap.size(), numDLCElements_);

            // Ensure UpdateAudioSamples returns no error
            ASSERT_EQ(returnCode, kIABNoError);
        }
        
        void UpdateObjectMetaData()
        {
            // Vector of object subblock pan information
            std::vector<IABObjectPanningParameters> subblockPanParameters;

            // Use the same settings for all objects
            subblockPanParameters.push_back(panParameters_); // subblock 0
            subblockPanParameters.push_back(panParameters_); // subblock 2
            subblockPanParameters.push_back(panParameters_); // subblock 3
            subblockPanParameters.push_back(panParameters_); // subblock 4
            subblockPanParameters.push_back(panParameters_); // subblock 5
            subblockPanParameters.push_back(panParameters_); // subblock 6
            subblockPanParameters.push_back(panParameters_); // subblock 7
            subblockPanParameters.push_back(panParameters_); // subblock 8
            
            IABMetadataIDType metaDataID = objectMetaDataIDStart_;
            
            for (uint32_t i = 0; i < numObjects_; i++)
            {
                // Test IABPacker UpdateObjectMetaData API
                // Update object meta data
                ASSERT_EQ(iabPacker_->UpdateObjectMetaData(metaDataID++, subblockPanParameters), kIABNoError);
            }
        }

        // **********************************************
        // Functions to IABParser APIs and check parsed IAB frame contents
        // **********************************************

        void CheckParsedFrame()
        {
            // check frame header
            
            IABSampleRateType sampleRate;
            IABFrameRateType frameRateCode;
            uint32_t frameSampleCount;
            IABElementCountType frameSubElementCount;
            IABMaxRenderedRangeType maxRendered;
            
            // Test IABParser GetSampleRate API
            sampleRate = iabParser_->GetSampleRate();
            
            // Test IABParser GetFrameRate API
            frameRateCode = iabParser_->GetFrameRate();
            
            // Test IABParser GetFrameSampleCount API
            frameSampleCount = iabParser_->GetFrameSampleCount();

            // Test IABParser GetFrameSubElementCount API
            frameSubElementCount = iabParser_->GetFrameSubElementCount();
            
            // Test IABParser GetMaximumAssetsToBeRendered API
            maxRendered = iabParser_->GetMaximumAssetsToBeRendered();
            
            // Check values matching setup config
            EXPECT_EQ(sampleRate, sampleRate_);
            EXPECT_EQ(frameRateCode, frameRateCode_);
            EXPECT_EQ(frameSampleCount, frameSampleCount_);
            EXPECT_EQ(frameSubElementCount, static_cast<IABElementCountType>(expectedElementsInParsedFrame_));
            EXPECT_EQ(maxRendered, static_cast<IABMaxRenderedRangeType>(numBedChannels_ + numObjects_));
            
            // Not doing full content check since detailed serialize/seserialize of the elements are tested by their own unit tests
            // Check that the packer frame elements are present in the parser frame, but the skipped elements (authoring tool info
            // and userData elements should not be present
            IABFrameInterface *iabPackerFrame = NULL;
            ASSERT_EQ(kIABNoError, iabPacker_->GetIABFrame(iabPackerFrame));
            ASSERT_TRUE(iabPackerFrame != NULL);

            const IABFrameInterface *iabParserFrame = NULL;
            ASSERT_EQ(kIABNoError, iabParser_->GetIABFrame(iabParserFrame));
            ASSERT_TRUE(iabPackerFrame != NULL);
            
            std::vector<IABElement*> packerSubElements;
            std::vector<IABElement*> parserSubElements;
            std::vector<IABElement*>::const_iterator iterPackerFrameSubElements;
            std::vector<IABElement*>::const_iterator iterParserFrameSubElements;
            
            iabPackerFrame->GetSubElements(packerSubElements);
            iabParserFrame->GetSubElements(parserSubElements);
            
            IABMetadataIDType packerElementID;
            IABMetadataIDType parserElementID;
            IABAudioDataIDType packerDLCAudioID;
            IABAudioDataIDType parserDLCAudioID;
            
            uint32_t numBedsFound = 0;
            uint32_t numObjectsFound = 0;
            uint32_t numDLCElementsFound = 0;
            
            for (iterPackerFrameSubElements = packerSubElements.begin(); iterPackerFrameSubElements != packerSubElements.end(); iterPackerFrameSubElements++)
            {
                bool foundTheElement = false;
                
                // Check bed definition type
                const IABBedDefinitionInterface* packerBedElement = dynamic_cast<const IABBedDefinitionInterface*>(*iterPackerFrameSubElements);
                if (packerBedElement)
                {
                    packerBedElement->GetMetadataID(packerElementID);
                              
                    // element is a bed definition, look for a bed definition in the parser frame with same ID
                    for (iterParserFrameSubElements = parserSubElements.begin(); iterParserFrameSubElements != parserSubElements.end(); iterParserFrameSubElements++)
                    {
                        const IABBedDefinitionInterface* parserBedElement = dynamic_cast<const IABBedDefinitionInterface*>(*iterParserFrameSubElements);
                        if (parserBedElement)
                        {
                            // Parser element is also a bed definition, get and compare IDs
                            parserBedElement->GetMetadataID(parserElementID);
                            if (packerElementID == parserElementID)
                            {
                                // Found it, update count
                                numBedsFound++;
                                foundTheElement = true;
                                break;
                            }
                        }
                    }
                    
                    EXPECT_TRUE(foundTheElement);
                    continue;
                }

                // Check object definition type
                const IABObjectDefinitionInterface* packerObjectElement = dynamic_cast<const IABObjectDefinitionInterface*>(*iterPackerFrameSubElements);
                if (packerObjectElement)
                {
                    packerObjectElement->GetMetadataID(packerElementID);
                    
                    // element is an object definition, look for an object definition in the parser frame with same ID
                    for (iterParserFrameSubElements = parserSubElements.begin(); iterParserFrameSubElements != parserSubElements.end(); iterParserFrameSubElements++)
                    {
                        const IABObjectDefinitionInterface* parserObjectElement = dynamic_cast<const IABObjectDefinitionInterface*>(*iterParserFrameSubElements);
                        if (parserObjectElement)
                        {
                            // Parser element is also a bed definition, get and compare IDs
                            parserObjectElement->GetMetadataID(parserElementID);
                            if (packerElementID == parserElementID)
                            {
                                // Found it, update count
                                numObjectsFound++;
                                foundTheElement = true;
                                break;
                            }
                        }
                    }
                    
                    EXPECT_TRUE(foundTheElement);
                    continue;
                }

                // Check authoring tool info type
                const IABAuthoringToolInfoInterface* packerAuthoringToolElement = dynamic_cast<const IABAuthoringToolInfoInterface*>(*iterPackerFrameSubElements);
                if (packerAuthoringToolElement)
                {
                    // element is an authoring tool info element, they are parsed by parser for v1.1 onwards, and should be present in parser frame
                    for (iterParserFrameSubElements = parserSubElements.begin(); iterParserFrameSubElements != parserSubElements.end(); iterParserFrameSubElements++)
                    {
                        const IABAuthoringToolInfoInterface* parserAuthoringToolElement = dynamic_cast<const IABAuthoringToolInfoInterface*>(*iterParserFrameSubElements);
                        if (parserAuthoringToolElement)
                        {
                            foundTheElement = true;
                            break;
                        }
                    }
                    
                    EXPECT_TRUE(foundTheElement);
                    continue;
                }

                // Check user data type
                const IABUserDataInterface* packerUserDataElement = dynamic_cast<const IABUserDataInterface*>(*iterPackerFrameSubElements);
                if (packerUserDataElement)
                {
                    // element is an user data element, they are parsed by parser for v1.1 onwards, and should be present in parser frame
                    for (iterParserFrameSubElements = parserSubElements.begin(); iterParserFrameSubElements != parserSubElements.end(); iterParserFrameSubElements++)
                    {
                        const IABUserDataInterface* parserUserDataElement = dynamic_cast<const IABUserDataInterface*>(*iterParserFrameSubElements);
                        if (parserUserDataElement)
                        {
                            foundTheElement = true;
                            break;
                        }
                    }
                    
                    EXPECT_TRUE(foundTheElement);
                    continue;
                }

                // Check AudioDataDLC type
                const IABAudioDataDLCInterface* packerDLCElement = dynamic_cast<const IABAudioDataDLCInterface*>(*iterPackerFrameSubElements);
                if (packerDLCElement)
                {
                    packerDLCElement->GetAudioDataID(packerDLCAudioID);
                    
                    // element is an AudioDataDLC, look for an AudioDataDLC in the parser frame with same ID
                    for (iterParserFrameSubElements = parserSubElements.begin(); iterParserFrameSubElements != parserSubElements.end(); iterParserFrameSubElements++)
                    {
                        const IABAudioDataDLCInterface* parserDLCElement = dynamic_cast<const IABAudioDataDLCInterface*>(*iterParserFrameSubElements);
                        if (parserDLCElement)
                        {
                            // Parser element is also a DLC element, get and compare IDs
                            parserDLCElement->GetAudioDataID(parserDLCAudioID);
                            if (packerDLCAudioID == parserDLCAudioID)
                            {
                                // Found it, update count
                                numDLCElementsFound++;
                                foundTheElement = true;
                                break;
                            }
                        }
                    }
                    
                    EXPECT_TRUE(foundTheElement);
                    continue;
                }
                
                // If it gets here, we haven't found the packer element and it's not a skipped item either
                // Error
                ASSERT_TRUE(foundTheElement);
                
            }
            
            EXPECT_EQ(numBedsFound, numBedDefinitions_);
            EXPECT_EQ(numObjectsFound, numObjects_);
            EXPECT_EQ(numDLCElementsFound, numDLCElements_);
        }
        
        // **********************************************
        // Function to test IABPacker and IABParser APIs
        // **********************************************
        
        void TestIABFramePackerParserAPIs()
        {
            // Test case: one object, no bed
            bedLayout_ = kIABUseCase_NoUseCase;
            // Reset audio ID and list
            audioIDIndex_ = audioIDStart_;
            audioDataIDList_.clear();
            RunIABFramePackParseTestCase();
            
            // Test case: 7.1DS with one object
            numBedDefinitions_ = 1;
            bedLayout_ = kIABUseCase_7_1_DS;
            // Reset audio ID and list
            audioIDIndex_ = audioIDStart_;
            audioDataIDList_.clear();
            RunIABFramePackParseTestCase();

            // Test case: 9.1OH with one object
            bedLayout_ = kIABUseCase_9_1_OH;
            // Reset audio ID and list
            audioIDIndex_ = audioIDStart_;
            audioDataIDList_.clear();
            RunIABFramePackParseTestCase();

        }
        
        void RunIABFramePackParseTestCase()
        {
            // Create a packer instance
            iabPacker_ = IABPackerInterface::Create();
            
            // Test IABPacker GetAPIVersion API
            IABAPIVersionType version;
            iabPacker_->GetAPIVersion(version);
            ASSERT_EQ(version.fHigh_, 0);
            ASSERT_EQ(version.fLow_, 0);
            
            // Test IABPacker SetFrameRate API
            ASSERT_EQ(iabPacker_->SetFrameRate(kIABFrameRate_24FPS), kIABNoError);
            
            // Test IABPacker SetSampleRate API
            ASSERT_EQ(iabPacker_->SetSampleRate(kIABSampleRate_48000Hz), kIABNoError);
            
            // Add sub-element to IAB frame, according to bedUseCase_, i.e. 5.1, 7.1 or 9.1
            AddElementsToFrame();
            
            IABFrameInterface *iabPackerFrame = NULL;
            ASSERT_EQ(kIABNoError, iabPacker_->GetIABFrame(iabPackerFrame));
            ASSERT_TRUE(iabPackerFrame != NULL);
            IABElementCountType numSubElements = 0;
            iabPackerFrame->GetSubElementCount(numSubElements);
            ASSERT_EQ(numElementsInPackerFrame_, numSubElements);
            
            // Test PackIABFrame API
            iabPacker_->PackIABFrame();
            
            // Get packed frame from IABPacker
            std::vector<char> programBuffer;
            uint32_t progBufferLen = 0;;
            
            // Test GetPackedBuffer API
            ASSERT_EQ(iabPacker_->GetPackedBuffer(programBuffer, progBufferLen), kIABNoError);
            
            // Check stream size
            ASSERT_NE(progBufferLen, 0);
            
            // Create a stream from returned vector<char>
            std::stringstream packedStream(std::string(&programBuffer[0], progBufferLen));
            
            // Test IABParser CreateIABParser API
            iabParser_ = IABParserInterface::Create(&packedStream);

            // Test ParseIABFrame API
            ASSERT_EQ(iabParser_->ParseIABFrame(), kIABNoError);
            
            // Check frame contents against expected reference values
            CheckParsedFrame();

            IABPackerInterface::Delete(iabPacker_);
            IABParserInterface::Delete(iabParser_);
        }
        
        void TestIABParserSkipUnsupportedElements()
        {
            // Test case: one object, no bed
            bedLayout_ = kIABUseCase_NoUseCase;
            // Reset audio ID and list
            audioIDIndex_ = audioIDStart_;
            audioDataIDList_.clear();
            
            // Set authoring tool info string
            authoringToolInfo_ = "Authoring tool info for IAB unit test";
            
            // Set up user ID
			uint8_t userID[16] = {0x06,0x0E,0x2B,0x34,0x04,0x01,0x01,0x0D,0x0F,0x01,0x02,0x03,0x04,0x05,0x06,0x07};

			uint8_t* srcUserID = userID;
			uint8_t* destUserID = userID_;
			for (uint8_t i = 0; i < 16; i++)
			{
				*destUserID++ = *srcUserID++;
			}
            
            // Add some data bytes to user data
            for (uint32_t i = 0; i < 32; i++)
            {
                userDataBytes_.push_back(i);
            }

            RunIABFramePackParseTestCase();
        }
        
    private:

        // IAB packer to test frame packing into stream
        IABPackerInterface* iabPacker_;
        
        
        // ****************************************************
        // Frame packing configuration parameters and variables

        // IAB audio sample rate code
        IABSampleRateType       sampleRate_;
        
        // IAB frame rate code
        IABFrameRateType        frameRateCode_;
        
        // Per mono essence frame sample count
        uint32_t                frameSampleCount_;
        
        // Bed parameters
        // Number of bed channels in packer frame
        uint32_t                numBedChannels_;
        
        // Meta data ID for bed definition
        IABMetadataIDType       bedID_;
        
        // Bed layout for packer frame
        IABUseCaseType          bedLayout_;
        
        // Number of bed definition elements in packer frame
        uint32_t                numBedDefinitions_;
        
        // Audio and DLC parameters
        
        // Number of DLC elements (one per object or bed channel)
        uint32_t                numDLCElements_;
        
        // Audio ID list for DLC element
        std::vector<IABAudioDataIDType> audioDataIDList_;
        
        // Object audio ID starting value
        IABAudioDataIDType      audioIDStart_;
        
        // Audio data ID starting value
        IABAudioDataIDType      audioIDIndex_;

        // Object parameters
        // Number of IAB objects to add to IAB frame
        uint32_t                numObjects_;
        
        // Object metadata ID starting value
        IABMetadataIDType       objectMetaDataIDStart_;

        // Panning parameters, common to all objects
        IABObjectPanningParameters panParameters_;
        
        // Authoring tool info string
        std::string             authoringToolInfo_;
        
        // UserID string
		uint8_t                 userID_[16];
        std::vector<uint8_t>    userDataBytes_;
        
        // Number of elements in packer frame
        uint32_t                numElementsInPackerFrame_;
        
        // ****************************************************
        // Frame parsing variables
        
        // IAB parser to test stream parsing into IAB frame
        IABParserInterface*     iabParser_;
        
        // Expected number of elements in the parsed frame
        uint32_t                expectedElementsInParsedFrame_;
        
    };

    // ********************
    // Run tests
    // ********************
    
    // Run IAB frame packing tests
    TEST_F(IABPackerParser_Test, Test_IABPackerParserAPIs)
    {
        TestIABFramePackerParserAPIs();
    }
    
    TEST_F(IABPackerParser_Test, Test_IABParserSkipUnsupportedElements)
    {
        TestIABParserSkipUnsupportedElements();
    }
    
}
