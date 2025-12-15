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
    // IABBedRemap element tests:
    // 1. Test RemapCoeff class
    // 2. Test setters and getter APIs
    // 3. Test Serialize() into a stream (packed buffer)
    // 4. Test DeSerialize() from the stream (packed buffer).
    
    class IABBedRemap_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabPackerBedRemap_ = NULL;
            iabParserBedRemap_ = NULL;
            
			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second
			numRemapSubBlocks_ = GetIABNumSubBlocks(frameRate_);

			// These members will be changed per test case
            sourceConfig_ = kIABUseCase_7_1_DS;
            targetConfig_ = kIABUseCase_5_1;
            numberDestination_ = 6;
			numberSource_ = 8;
        }
        
        // **********************************************
        // RemapCoeff tests
        // **********************************************
        
        void TestRemapCoeff()
        {
            // *********************************
            // Test constructor that takes a channel ID and number of sources
            // *********************************
            
            std::vector<IABGain> remapGainCoeffs;   // One vector per source
            
            IABRemapCoeff remapCoeff(kIABChannelID_Left, 2);
            EXPECT_EQ(remapCoeff.getDestinationChannelID(), kIABChannelID_Left);
            EXPECT_EQ(remapCoeff.getRemapSourceNumber(), 2);
            
            // Test getRemapCoeffVector API and default state
            remapGainCoeffs = remapCoeff.getRemapCoeffVector();
            EXPECT_EQ(remapGainCoeffs.size(), 2);
            EXPECT_EQ(remapGainCoeffs[0].getIABGain(), 0.0f);
            EXPECT_EQ(remapGainCoeffs[1].getIABGain(), 0.0f);
            
            // Test setRemapCoeff API with two different gain values
            IABGain gain;
            
            // Set first gain to 0.1f
            gain.setIABGain(0.1f);
            EXPECT_EQ(remapCoeff.setRemapCoeff(gain, 0), kIABNoError);
            
            // Set second gain to 0.5f
            gain.setIABGain(0.5f);
            EXPECT_EQ(remapCoeff.setRemapCoeff(gain, 1), kIABNoError);
            
            // Test return code for invalid index
            EXPECT_EQ(remapCoeff.setRemapCoeff(gain, 2), kIABBadArgumentsError);
            
            // Test getRemapCoeff API with two different gain values
            // Test first gain
            EXPECT_EQ(remapCoeff.getRemapCoeff(gain, 0), kIABNoError);
            EXPECT_EQ(gain.getIABGain(), 0.1f);
            
            // Test second gain
            EXPECT_EQ(remapCoeff.getRemapCoeff(gain, 1), kIABNoError);
            EXPECT_EQ(gain.getIABGain(), 0.5f);
            
            // Test return code for invalid index
            EXPECT_EQ(remapCoeff.getRemapCoeff(gain, 2), kIABBadArgumentsError);
           
            // *********************************
            // Test constructor without parameters
            // *********************************

            IABRemapCoeff remapCoeff2;
            
           // Test DestinationChannelID setter/getter using 3 different arbitrary channel IDs
            EXPECT_EQ(remapCoeff.setDestinationChannelID(kIABChannelID_Left), kIABNoError);
            EXPECT_EQ(remapCoeff.getDestinationChannelID(), kIABChannelID_Left);
            
            EXPECT_EQ(remapCoeff.setDestinationChannelID(kIABChannelID_LFE), kIABNoError);
            EXPECT_EQ(remapCoeff.getDestinationChannelID(), kIABChannelID_LFE);
            
            EXPECT_EQ(remapCoeff.setDestinationChannelID(kIABChannelID_TopSurround), kIABNoError);
            EXPECT_EQ(remapCoeff.getDestinationChannelID(), kIABChannelID_TopSurround);
            
            remapGainCoeffs.clear();
            
            gain.setIABGain(0.1f);
            remapGainCoeffs.push_back(gain);
            
            gain.setIABGain(0.2f);
            remapGainCoeffs.push_back(gain);
            
            // Test setRemapCoeffVector API
            EXPECT_EQ(remapCoeff2.setRemapCoeffVector(remapGainCoeffs), kIABNoError);
            
            // Test getRemapSourceNumber API. Should be set vector size in setRemapCoeffVector call above
            EXPECT_EQ(remapCoeff2.getRemapSourceNumber(), 2);
            
            // Test getRemapCoeffVector API for this instance
            remapGainCoeffs = remapCoeff2.getRemapCoeffVector();
            EXPECT_EQ(remapGainCoeffs.size(), 2);
            EXPECT_EQ(remapGainCoeffs[0].getIABGain(), 0.1f);
            EXPECT_EQ(remapGainCoeffs[1].getIABGain(), 0.2f);
            
        }

        
        // **********************************************
        
        // IABBedRemap element setters and getters API tests
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
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
            
            // Gain array
            std::vector<IABGain> remapGainCoeffs;

            // For creation of IABBedRemapInterface and IABBedRemapSubBlockInterface
            uint8_t numRemapSubBlocks;
            uint16_t numberDestination = 8;
            uint16_t numberSource = 8;

			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second
			numRemapSubBlocks_ = GetIABNumSubBlocks(frameRate_);

			numRemapSubBlocks = numRemapSubBlocks_;

            // Test IABBedRemapInterface creation API
            IABBedRemapInterface *bedRemapInterface = NULL;
            bedRemapInterface = IABBedRemapInterface::Create(numberDestination, numberSource, frameRate_);
            ASSERT_TRUE(NULL != bedRemapInterface);
            
            // Test IABBedRemapInterface creation API
            IABBedRemapSubBlockInterface* bedRemapSubBlockInterface = NULL;
            bedRemapSubBlockInterface = IABBedRemapSubBlockInterface::Create(numberDestination, numberSource);
            ASSERT_TRUE(NULL != bedRemapSubBlockInterface);

            // *********************
            // IABBedRemapSubBlock Setter and Getter API tests
            // *********************
            
            uint1_t remapInfoExists;
            std::vector<IABRemapCoeff*> remapCoeffArray;
            uint16_t sourceNumber;
            uint16_t destinationNumber;
            
            // Test remap info exist flag setter getter API
            remapInfoExists = 1;
            EXPECT_EQ(bedRemapSubBlockInterface->SetRemapInfoExists(remapInfoExists), kIABNoError);
            bedRemapSubBlockInterface->GetRemapInfoExists(remapInfoExists);
            EXPECT_EQ(remapInfoExists, 1);
            
            remapInfoExists = 0;
            EXPECT_EQ(bedRemapSubBlockInterface->SetRemapInfoExists(remapInfoExists), kIABNoError);
            bedRemapSubBlockInterface->GetRemapInfoExists(remapInfoExists);
            EXPECT_EQ(remapInfoExists, 0);
            
            // Test GetRemapSourceNumber API
            bedRemapSubBlockInterface->GetRemapSourceNumber(sourceNumber);
            EXPECT_EQ(sourceNumber, numberSource);

            // Test GetRemapDestinationNumber API
            bedRemapSubBlockInterface->GetRemapDestinationNumber(destinationNumber);
            EXPECT_EQ(destinationNumber, numberDestination);
            
            
            // *********************
            // IABBedRemap Setter and Getter API tests
            // Not testing actual remapping from one channel configuration to another
            // *********************
            
            IABMetadataIDType metaID;
            IABUseCaseType remapUseCase;
            
            // Test metadata ID setter and getter APIs
            
            // Set to minimum value for uint32_t
            EXPECT_EQ(bedRemapInterface->SetMetadataID(0), kIABNoError);
            bedRemapInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0);
            
            // Set to maximum value for uint32_t
            EXPECT_EQ(bedRemapInterface->SetMetadataID(0xFFFFFFFF), kIABNoError);
            bedRemapInterface->GetMetadataID(metaID);
            EXPECT_EQ(metaID, 0xFFFFFFFF);

            // Test RemapUseCase setter and getter APIs
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_5_1), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_5_1);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_7_1_DS), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_7_1_DS);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_7_1_SDS), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_7_1_SDS);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_11_1_HT), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_11_1_HT);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_13_1_HT), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_13_1_HT);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_9_1_OH), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_9_1_OH);
            
            EXPECT_EQ(bedRemapInterface->SetRemapUseCase(kIABUseCase_Always), kIABNoError);
            bedRemapInterface->GetRemapUseCase(remapUseCase);
            EXPECT_EQ(remapUseCase, kIABUseCase_Always);
            
            // *********************
            // Test setting IABBedRemap
            // *********************
            
            std::vector<IABBedRemapSubBlock*> bedRemapSubBlocksSet;

            // Test setters and setting up a bed remap
            for (uint16_t i = 0; i < numRemapSubBlocks; i++)
            {
                // NOTE: These will be deleted by IABBedRemap destructor, no need to delete them explictly at end of test
                IABBedRemapSubBlock* bedRemapSubBlock = new IABBedRemapSubBlock(numberDestination, numberSource);
                
                // Set exist flag
                EXPECT_EQ(bedRemapSubBlock->SetRemapInfoExists(1), kIABNoError);

                remapCoeffArray.clear();
                
                // Iterator for destination channel list
                std::vector<IABChannelIDType>::iterator iterDest = destinationList.begin();
                
                for (uint8_t j = 0; j < numberDestination; j++)
                {
                    // Set gain coeffiecent array for each destination channel
                    IABRemapCoeff *destChannelRemapCoeff = new IABRemapCoeff();
                    
                   // Clear remap gain list for current channel
                    remapGainCoeffs.clear();
                    IABGain gainCoeff;
                    float gain = 0.1f;
                    
                    // Set up some non-zero gains at 0.1f increments
                    // Use same values for every subBlock
                    for (uint8_t k = 0; k < numberSource; k++)
                    {
                        gainCoeff.setIABGain(gain);
                        remapGainCoeffs.push_back(gainCoeff);
                        gain += 0.1f;
                    }
                    
                    // Set destination channel ID
                    EXPECT_EQ(destChannelRemapCoeff->setDestinationChannelID(*iterDest++), kIABNoError);
                    
                    // Add gain array to current destination channel
                    EXPECT_EQ(destChannelRemapCoeff->setRemapCoeffVector(remapGainCoeffs), kIABNoError);
                    
                    // Add destination channel to remap coeff array
                    remapCoeffArray.push_back(destChannelRemapCoeff);
                }
                
                // Test SetRemapCoeffArray API, add gain array of all destination channels to current subBock
                EXPECT_EQ(bedRemapSubBlock->SetRemapCoeffArray(remapCoeffArray), kIABNoError);

                // Test checkRemapCoeffArray API
                EXPECT_EQ(bedRemapSubBlock->checkRemapCoeffArray(), kIABNoError);
                
                // Add subBlock the remap vector
                bedRemapSubBlocksSet.push_back(bedRemapSubBlock);
                
            }
            
            // Add remap subBlocks to bed remap. Remap data structure is now complete
            bedRemapInterface->SetRemapSubBlocks(bedRemapSubBlocksSet);
            
            // *************************************
            // Test getting IABBedRemap
            // *************************************
            
            std::vector<IABBedRemapSubBlock*> bedRemapSubBlocksGet;
            std::vector<IABRemapCoeff*> remapCoeffArrayGet;
            std::vector<IABGain> remapGainCoeffsGet;
            
            // Test GetRemapSubBlocks API
            bedRemapInterface->GetRemapSubBlocks(bedRemapSubBlocksGet);
            ASSERT_EQ(bedRemapSubBlocksGet.size(), numRemapSubBlocks);
            
            // Check each subBlock
            for (uint16_t i = 0; i < numRemapSubBlocks; i++)
            {
                remapCoeffArrayGet.clear();

                // Test GetRemapCoeffArray API
                bedRemapSubBlocksGet[i]->GetRemapCoeffArray(remapCoeffArrayGet);
                ASSERT_EQ(remapCoeffArrayGet.size(), numberDestination);
                
                // Iterator for destination channel list
                std::vector<IABChannelIDType>::iterator iterDest = destinationList.begin();
                
                // Check remap coeff array in this subBlock
                for (uint8_t j = 0; j < numberDestination; j++)
                {
					ASSERT_TRUE(remapCoeffArrayGet[j] != NULL);
                    EXPECT_EQ(remapCoeffArrayGet[j]->getDestinationChannelID(), *iterDest++); // Check against the expected channel ID list
                    
                    // TestgetRemapCoeffVector API
                    remapGainCoeffsGet = remapCoeffArrayGet[j]->getRemapCoeffVector();
                    ASSERT_EQ(remapGainCoeffsGet.size(), numberSource);
                    
                    // Check each gain
                    float gain = 0.1f;
                    for (uint8_t k = 0; k < numberSource; k++)
                    {
                        EXPECT_EQ(remapGainCoeffsGet[k].getIABGain(), gain);
                        gain += 0.1f;
                    }
                }
            }

            IABBedRemapInterface::Delete(bedRemapInterface);
            IABBedRemapSubBlockInterface::Delete(bedRemapSubBlockInterface);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************

        bool UpdateNumberOfMappingChannels()
        {
            destinationList_.clear();

            // Set number of source channels
            switch (sourceConfig_)
            {
                case kIABUseCase_5_1:
                    numberSource_ = 6;
                    break;
                    
                case kIABUseCase_7_1_DS:
                    numberSource_ = 8;
                    break;
                    
                case kIABUseCase_9_1_OH:
                    numberSource_ = 10;
                    break;

                // Not supported
                case kIABUseCase_7_1_SDS:
                case kIABUseCase_11_1_HT:
                case kIABUseCase_13_1_HT:
                default:
                    return false;
                    
            }
            
            // Set number of destination channels and create destination channel list
            switch (targetConfig_)
            {
                case kIABUseCase_5_1:
                    destinationList_.push_back(kIABChannelID_Left);
                    destinationList_.push_back(kIABChannelID_Center);
                    destinationList_.push_back(kIABChannelID_Right);
                    destinationList_.push_back(kIABChannelID_LeftSurround);
                    destinationList_.push_back(kIABChannelID_RightSurround);
                    destinationList_.push_back(kIABChannelID_LFE);
                    numberDestination_ = 6;
                    break;
                    
                case kIABUseCase_7_1_DS:
                    numberDestination_ = 8;
                    destinationList_.push_back(kIABChannelID_Left);
                    destinationList_.push_back(kIABChannelID_Center);
                    destinationList_.push_back(kIABChannelID_Right);
                    destinationList_.push_back(kIABChannelID_LeftSideSurround);
                    destinationList_.push_back(kIABChannelID_RightSideSurround);
                    destinationList_.push_back(kIABChannelID_LeftRearSurround);
                    destinationList_.push_back(kIABChannelID_RightRearSurround);
                    destinationList_.push_back(kIABChannelID_LFE);
                    break;
                    
                case kIABUseCase_9_1_OH:
                    numberDestination_ = 10;
                    destinationList_.push_back(kIABChannelID_Left);
                    destinationList_.push_back(kIABChannelID_Center);
                    destinationList_.push_back(kIABChannelID_Right);
                    destinationList_.push_back(kIABChannelID_LeftSideSurround);
                    destinationList_.push_back(kIABChannelID_RightSideSurround);
                    destinationList_.push_back(kIABChannelID_LeftRearSurround);
                    destinationList_.push_back(kIABChannelID_RightRearSurround);
                    destinationList_.push_back(kIABChannelID_LeftTopSurround);
                    destinationList_.push_back(kIABChannelID_RightTopSurround);
                    destinationList_.push_back(kIABChannelID_LFE);
                    break;
                    
                    // Not supported
                case kIABUseCase_7_1_SDS:
                case kIABUseCase_11_1_HT:
                case kIABUseCase_13_1_HT:
                default:
                    return false;
                    
            }
            
            return true;

        }

        void SetupPackerBedRemap()
        {
            ASSERT_TRUE(NULL != iabPackerBedRemap_);
            ASSERT_TRUE(destinationList_.size() > 0);
            
            // Set meta data ID to 123, an arbitrary test value
            EXPECT_EQ(iabPackerBedRemap_->SetMetadataID(123), kIABNoError);
            
            // Set use case to target config
            EXPECT_EQ(iabPackerBedRemap_->SetRemapUseCase(targetConfig_), kIABNoError);

            std::vector<IABBedRemapSubBlock*> bedRemapSubBlocksSet;
            std::vector<IABRemapCoeff*> remapCoeffArray;
            std::vector<IABGain> remapGainCoeffs;
            
            // Test setters and set up a bed remap for serialisation
            for (uint16_t i = 0; i < numRemapSubBlocks_; i++)
            {
                // NOTE: These will be deleted by IABBedRemap destructor, no need to delete them explictly at end of test
                IABBedRemapSubBlock* bedRemapSubBlock = new IABBedRemapSubBlock(numberDestination_, numberSource_);
                
                // Set exist flag
                EXPECT_EQ(bedRemapSubBlock->SetRemapInfoExists(1), kIABNoError);
                
                remapCoeffArray.clear();
                
                // Iterator for destination channel list
                std::vector<IABChannelIDType>::iterator iterDest = destinationList_.begin();
                
                for (uint8_t j = 0; j < numberDestination_; j++)
                {
                    // Gain coeffiecent array for each destination channel
                    IABRemapCoeff *destChannelRemapCoeff = new IABRemapCoeff();
                    remapGainCoeffs.clear();
                    IABGain gainCoeff;
                    float gain = 0.1f;
                    
                    // Set up some non-zero gains at 0.1f increments
                    for (uint8_t k = 0; k < numberSource_; k++)
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
            iabPackerBedRemap_->SetRemapSubBlocks(bedRemapSubBlocksSet);
            
        }

        
        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        // Check contents of the de-serialsed bed remap element
        void VerifyDeSerializedBedRemap()
        {
            ASSERT_TRUE(NULL != iabParserBedRemap_);
            ASSERT_TRUE(NULL != iabPackerBedRemap_);
            ASSERT_TRUE(destinationList_.size() > 0);

            std::vector<IABBedRemapSubBlock*> bedRemapSubBlocksParser;
            std::vector<IABRemapCoeff*> remapCoeffArrayParser;
            std::vector<IABGain> remapGainCoeffsParser;
            
            iabParserBedRemap_->GetRemapSubBlocks(bedRemapSubBlocksParser);
            ASSERT_EQ(bedRemapSubBlocksParser.size(), numRemapSubBlocks_);
            
            for (uint16_t i = 0; i < numRemapSubBlocks_; i++)
            {
                remapCoeffArrayParser.clear();
                
                // Get RemapCoeffArray and verify size
                bedRemapSubBlocksParser[i]->GetRemapCoeffArray(remapCoeffArrayParser);
                ASSERT_EQ(remapCoeffArrayParser.size(), numberDestination_);
                
                // Iterator for destination channel list
                std::vector<IABChannelIDType>::iterator iterDest = destinationList_.begin();
                
                // Get remap coeff array in this subBlock and verify
                for (uint8_t j = 0; j < numberDestination_; j++)
                {
                    ASSERT_TRUE(remapCoeffArrayParser[j] != NULL);

                    // Check against the expected channel ID list
                    EXPECT_EQ(remapCoeffArrayParser[j]->getDestinationChannelID(), *iterDest++);
                    
                    remapGainCoeffsParser = remapCoeffArrayParser[j]->getRemapCoeffVector();
                    ASSERT_EQ(remapGainCoeffsParser.size(), numberSource_);
                    
                    // Check gain. There may be small difference due to quantisation through serialze/de-serialize.
                    // Use the IABGain == operator to compare quantized values instead of comparing float directly
                    float gain = 0.1f;
                    IABGain iabGain;
                    for (uint8_t k = 0; k < numberSource_; k++)
                    {
                        iabGain.setIABGain(gain);
                        EXPECT_EQ((remapGainCoeffsParser[k] == iabGain), true);
                        gain += 0.1f;
                    }
                }
            }
            
        }
        
        // **********************************************
        
        // Function to test Serialize() and DeSerialize()
        
        // **********************************************

        void TestSerializeDeSerialize()
        {
            // NOTE: We are testing the APIs only and will only verify that the same coefficents go into and come out the bitstream.
            // The coefficients will be chosen for convenience and would be different from coefficient sets used in normal/standard downmix.
            
            // Test 7.1 to 51 serialize, DeSerialize.
            sourceConfig_ = kIABUseCase_7_1_DS;
            targetConfig_ = kIABUseCase_5_1;
            RunSerializeDeSerializeTestCase();

        }
        
        void RunSerializeDeSerializeTestCase()
        {
            // Set up correct number of source and destination channels for current test case
            ASSERT_EQ(UpdateNumberOfMappingChannels(), true);

            // Create IABBedRemap element (packer) to Serialize stream buffer
            iabPackerBedRemap_ = new IABBedRemap(numberDestination_, numberSource_, frameRate_);
            ASSERT_TRUE(NULL != iabPackerBedRemap_);
            
            // Create IABBedRemap element (parser) to de-serialize stream buffer
            iabParserBedRemap_ = new IABBedRemap(numberDestination_, numberSource_, frameRate_);
            ASSERT_TRUE(NULL != iabParserBedRemap_);
           
            // Set up IAB packer bed remap element
            SetupPackerBedRemap();
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serialize bed remap element into stream
            ASSERT_EQ(iabPackerBedRemap_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);

            auto pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerBedRemap_->GetElementSize(elementSize);
            
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
            
            // DeSerialize bed remap from stream
            ASSERT_EQ(iabParserBedRemap_->DeSerialize(elementReader), kIABNoError);
            
            // Verify deserialized bed remap element
            VerifyDeSerializedBedRemap();
            
            // Verify element size
            
            auto readerPos = elementReader.streamPosition();
            
            bytesInStream = static_cast<IABElementSizeType>(readerPos);
            
            iabParserBedRemap_->GetElementSize(elementSize);
            
            bytesInStream -= 2;     // Deduct 2 bytes for element ID and element size code
            
            if (elementSize >= 255)
            {
                // deduct 2 more bytes for plex coding if > 8-bit range
                bytesInStream -= 2;
                
                if (elementSize >= 65535)
                {
                    // deduct 4 more bytes for plex coding if > 16-bit range. Assume that the size field does not exceed 32-bit range
                    bytesInStream -= 4;
                }
            }
            
            EXPECT_EQ(elementSize, bytesInStream);
            
			delete iabPackerBedRemap_;
			delete iabParserBedRemap_;
		}

        
    private:
        
        IABBedRemap*   iabPackerBedRemap_;
        IABBedRemap*   iabParserBedRemap_;

		IABFrameRateType    frameRate_;
		uint8_t             numRemapSubBlocks_;
        uint16_t            numberDestination_;
        uint16_t            numberSource_;
        
        IABUseCaseType      sourceConfig_;
        IABUseCaseType      targetConfig_;
        
        std::vector<IABChannelIDType> destinationList_;

        
    };

    // ********************
    // Run tests
    // ********************
    
    // Run RemapCoeff tests
    TEST_F(IABBedRemap_Test, Test_TestRemapCoeff)
    {
        TestRemapCoeff();
    }

    // Run frame element setters and getters API tests
    TEST_F(IABBedRemap_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial frame, then deSerialize frame tests
    TEST_F(IABBedRemap_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }

    
}
