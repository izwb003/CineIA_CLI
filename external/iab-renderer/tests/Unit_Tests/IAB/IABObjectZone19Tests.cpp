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
    // IABObjectZone19 element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABObjectZone19_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabPackerZoneDefinition19_ = NULL;
            iabParserZoneDefinition19_ = NULL;
            
			frameRate_ = kIABFrameRate_24FPS;		// 24 frames/second

			// initial value and will be updated for each Serialize/Deserialize test case
            numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);
        }
        
        // **********************************************
        
        // IABObjectZone19 element setters and getters API tests
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
            // zone definition with 8 pan subBlocks. Test IABObjectZoneDefinition19 creation API
            IABObjectZoneDefinition19Interface* objectZoneDefinition19Interface8 = NULL;
            objectZoneDefinition19Interface8 = IABObjectZoneDefinition19Interface::Create(kIABFrameRate_24FPS);   // subblock = 8 for 24, 25, 30 FPS
            ASSERT_TRUE(NULL != objectZoneDefinition19Interface8);

            // zone definition with 4 pan subBlocks
            IABObjectZoneDefinition19Interface* objectZoneDefinition19Interface4 = NULL;
            objectZoneDefinition19Interface4 = IABObjectZoneDefinition19Interface::Create(kIABFrameRate_48FPS);   // subblock = 4 for 48, 50, 60 FPS
            ASSERT_TRUE(NULL != objectZoneDefinition19Interface4);

            // zone definition with 2 pan subBlocks
            IABObjectZoneDefinition19Interface* objectZoneDefinition19Interface2 = NULL;
            objectZoneDefinition19Interface2 = IABObjectZoneDefinition19Interface::Create(kIABFrameRate_96FPS);   // subblock = 2 for 96, 100, 120 FPS
            ASSERT_TRUE(NULL != objectZoneDefinition19Interface2);

            uint8_t numPanSubBlocks;

            // Test Getter for number of pan subBocks
            objectZoneDefinition19Interface8->GetNumZone19SubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, GetIABNumSubBlocks(kIABFrameRate_24FPS));

            objectZoneDefinition19Interface4->GetNumZone19SubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, GetIABNumSubBlocks(kIABFrameRate_48FPS));

            objectZoneDefinition19Interface2->GetNumZone19SubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, GetIABNumSubBlocks(kIABFrameRate_96FPS));
            
            // Test object 19-zone gains
            IABObjectZoneGain19 objectZoneGains19;
            float zoneGain;
            
            // Set gain to 1.0 for all zones
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                objectZoneGains19.zoneGains_[i].setIABZoneGain(1.0f);
            }
            
            // Check zone gains
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
                EXPECT_EQ(zoneGain, 1.0f);
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Unity);
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 0x3FF);
            }

            // Set gain to 0.0 for all zones
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                objectZoneGains19.zoneGains_[i].setIABZoneGain(0.0f);
            }
            
            // Check zone gains
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
                EXPECT_EQ(zoneGain, 0.0f);
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Silence);
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 0);
            }

            // Set gains to different values between 0.0 and 1.0, in 0.05 increment
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                objectZoneGains19.zoneGains_[i].setIABZoneGain(i * 0.05f);
            }
            
            // Check zone gains
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
				EXPECT_LT(std::fabs(zoneGain - (i * 0.05f)), 0.001f);									// Quatization to be less than 0.2%
                if (0 == i)
                {
                    EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Silence);
                    EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 0);
                }
                else
                {
                    EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_InStream);
                    EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), static_cast<uint16_t>(floor(zoneGain * 1023.0f + 0.5f)));
                }
            }

            // Create IABZone19SubBlock instance
            IABZone19SubBlockInterface* zone19SubBlockInterface1 = IABZone19SubBlockInterface::Create();
            IABZone19SubBlockInterface* zone19SubBlockInterface2 = IABZone19SubBlockInterface::Create();
            
            ASSERT_TRUE(NULL != zone19SubBlockInterface1);
            ASSERT_TRUE(NULL != zone19SubBlockInterface2);
            
            // Test object zones
            IABObjectZoneGain19 objectZoneGains19_1, objectZoneGains19_2;
            objectZoneGains19_1.objectZone19InfoExists_ = 1;
            objectZoneGains19_2.objectZone19InfoExists_ = 0;    // No zone gain information for this subBlock
            
            // Set gain to 0.5 (an arbitrary non-zero value between 0.0 and 1.0) for all zones
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                objectZoneGains19_1.zoneGains_[i].setIABZoneGain(0.5f); // set to
            }

            // Test zone 19 subBlock setters
            EXPECT_EQ(zone19SubBlockInterface1->SetObjectZoneGains19(objectZoneGains19_1), kIABNoError);
            EXPECT_EQ(zone19SubBlockInterface2->SetObjectZoneGains19(objectZoneGains19_2), kIABNoError);
            
            // Test zone definition 19 setters
            // Note that when these subBlocks are passed to objectZoneDefinition19Interface2. They willl be deleted by its destructor and
            // no need ot delete them explicitly at end of test
            std::vector<IABZone19SubBlock*> zone19SubBlocks;
            zone19SubBlocks.push_back(dynamic_cast<IABZone19SubBlock*>(zone19SubBlockInterface1));
            zone19SubBlocks.push_back(dynamic_cast<IABZone19SubBlock*>(zone19SubBlockInterface2));
            EXPECT_EQ(objectZoneDefinition19Interface2->SetZone19SubBlocks(zone19SubBlocks), kIABNoError);

            // Test zone definition 19 getters
            std::vector<IABZone19SubBlock*> zone19SubBlocksGet;
            objectZoneDefinition19Interface2->GetZone19SubBlocks(zone19SubBlocksGet);
            
            // Check contents
            objectZoneDefinition19Interface2->GetNumZone19SubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, 2);
            EXPECT_EQ(zone19SubBlocksGet.size(), 2);
            
            // Check first subBlock
            zone19SubBlocksGet[0]->GetObjectZoneGains19(objectZoneGains19);
            EXPECT_EQ(objectZoneGains19.objectZone19InfoExists_, 1);
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
				EXPECT_LT(std::fabs(zoneGain - 0.5f), 0.001f);									// Quatization to be less than 0.2%
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_InStream);
                EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 512);   // coded value for gain = 0.5
            }

            // Check second subBlock
            zone19SubBlocksGet[1]->GetObjectZoneGains19(objectZoneGains19);
            EXPECT_EQ(objectZoneGains19.objectZone19InfoExists_, 0);

            IABObjectZoneDefinition19Interface::Delete(objectZoneDefinition19Interface8);
            IABObjectZoneDefinition19Interface::Delete(objectZoneDefinition19Interface4);
            IABObjectZoneDefinition19Interface::Delete(objectZoneDefinition19Interface2);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************
        
        void SetupPackerZoneDefinition19()
        {
            // Set up an object zone structure and use it for all subBlocks
            IABObjectZoneGain19 objectZoneGains19;
            
            // Set exist flag
            objectZoneGains19.objectZone19InfoExists_ = 1;
            
            // Set first zone gain to 0.0
            EXPECT_EQ(objectZoneGains19.zoneGains_[0].setIABZoneGain(0.0f), kIABNoError);

            // Set second zone gain to 1.0
            EXPECT_EQ(objectZoneGains19.zoneGains_[1].setIABZoneGain(1.0f), kIABNoError);
            
            // Set gain to 0.5 (an arbitrary non-zero value between 0.0 and 1.0) for remaining zones
            for (uint32_t i = 2; i < kIABObjectZoneCount19; i++)
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
            
            EXPECT_EQ(iabPackerZoneDefinition19_->SetZone19SubBlocks(zone19SubBlocks), kIABNoError);
            
        }

        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        void VerifyDeSerializedZoneDefinition19()
        {
            // Zone gain SubBlock vector
            std::vector<IABZone19SubBlock*> zone19SubBlocks;
            std::vector<IABZone19SubBlock*>::iterator iterz19;
            
            iabParserZoneDefinition19_->GetZone19SubBlocks(zone19SubBlocks);
            
            uint8_t numPanSubBlocks = 0;
            float zoneGain;
            
            // Confirm number of subBlocks
            iabParserZoneDefinition19_->GetNumZone19SubBlocks(numPanSubBlocks);
            EXPECT_EQ(numPanSubBlocks, numPanSubBlocks_);
            
            // Confirm actual vector size
            EXPECT_EQ(zone19SubBlocks.size(), numPanSubBlocks_);
 
            
            for (iterz19 = zone19SubBlocks.begin(); iterz19 != zone19SubBlocks.end(); iterz19++)
            {
                IABObjectZoneGain19 objectZoneGains19;

                (*iterz19)->GetObjectZoneGains19(objectZoneGains19);
                EXPECT_EQ(objectZoneGains19.objectZone19InfoExists_, 1);
                for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
                {
                    zoneGain = objectZoneGains19.zoneGains_[i].getIABZoneGain();
                    
                    if (i > 1)
                    {
						EXPECT_LT(std::fabs(zoneGain - 0.5f), 0.001f);									// Quatization to be less than 0.2%
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_InStream);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 512);   // coded value for gain = 0.5
                    }
                    else if (0 == i)
                    {
                        EXPECT_EQ(zoneGain, 0.0f);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Silence);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 0);   // coded value for gain = 0.0
                    }
                    else    // i = 1
                    {
                        EXPECT_EQ(zoneGain, 1.0f);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Unity);
                        EXPECT_EQ(objectZoneGains19.zoneGains_[i].getIABZoneGainInStreamValue(), 0x3FF);    // // coded value for gain = 1.0
                    }
                }
            }
        }
        
        
        // **********************************************
        
        // Function to test Serialize() and DeSerialize()
        
        // **********************************************
        
 
        void TestSerializeDeSerialize()
        {
            // Test case: Number of subBlocks = 8 for 24, 25, 30 FPS
			frameRate_ = kIABFrameRate_24FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 8 subblokcs for 24FPS (25FPS, 30FPS also)
			RunSerializeDeSerializeTestCase();
            
            // Test case: Number of subBlocks = 4 for 48, 50, 60 FPS
			frameRate_ = kIABFrameRate_48FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 4 subblokcs for 48FPS (50FPS, 60FPS also)
			RunSerializeDeSerializeTestCase();
            
            // Test case: Number of subBlocks = 2 for 96, 100, 120 FPS
			frameRate_ = kIABFrameRate_96FPS;
			numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);    // 2 subblokcs for 96FPS (100FPS, 120FPS also)
			RunSerializeDeSerializeTestCase();
            
        }

        void RunSerializeDeSerializeTestCase()
        {
            // Create IAB object zone 19 (packer) to Serialize stream buffer
            iabPackerZoneDefinition19_ = new IABObjectZoneDefinition19(frameRate_);
            ASSERT_TRUE(NULL != iabPackerZoneDefinition19_);
            
            // Create IAB object zone 19 (parser) to de-serialize stream buffer
            iabParserZoneDefinition19_ = new IABObjectZoneDefinition19(frameRate_);
            ASSERT_TRUE(NULL != iabParserZoneDefinition19_);
            
            // Set up IAB packer zone definition 19
            SetupPackerZoneDefinition19();
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serialize zone definition 19 into stream
            ASSERT_EQ(iabPackerZoneDefinition19_->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            std::ios_base::streampos pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerZoneDefinition19_->GetElementSize(elementSize);
            
            bytesInStream -= 2;     // Deduct 2 bytes for element ID = 0x80 and element size code
            
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
            
            // DeSerialize zone definition 19 from stream
            ASSERT_EQ(iabParserZoneDefinition19_->DeSerialize(elementReader), kIABNoError);
            
            // Verify deserialized zone definition 19
            VerifyDeSerializedZoneDefinition19();
            
            // Verify element size
            
            std::ios_base::streampos readerPos = elementReader.streamPosition();
            
            bytesInStream = static_cast<IABElementSizeType>(readerPos);
            
            iabParserZoneDefinition19_->GetElementSize(elementSize);
            
            bytesInStream -= 2;     // Deduct 2 bytes for element ID (= 0x80) and element size code
            
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

			delete iabPackerZoneDefinition19_;
			delete iabParserZoneDefinition19_;
        }

        
    private:
        
        IABObjectZoneDefinition19*   iabPackerZoneDefinition19_;
        IABObjectZoneDefinition19*   iabParserZoneDefinition19_;
		IABFrameRateType frameRate_;
		uint8_t numPanSubBlocks_;

    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run zone definition 19 element setters and getters API tests
    TEST_F(IABObjectZone19_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serialize zone definition 19, then deSerialize zone definition 19 tests
    TEST_F(IABObjectZone19_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
}
