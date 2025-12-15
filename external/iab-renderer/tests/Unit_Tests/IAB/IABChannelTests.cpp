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
    // IABChannel tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABChannel_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            channelID_ = kIABChannelID_Left;
            audioDataID_ = 123;
            
            // These members will be changed per test case
            channelGain_.setIABGain(0.5f);
            decorInfoExists_ = 1;
            decorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_DecorCoeffInStream;
            decorCoef_.decorCoef_ = 0x40;   // initialize to a random non-zero decorrelation value
            
			iabPackerChannel_ = NULL;
			iabParserChannel_ = NULL;
		}

        // **********************************************
        
        // IABChannel setters and getters API tests624
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
            IABChannelInterface* iabChannelInterface = NULL;
            iabChannelInterface = IABChannelInterface::Create();
            ASSERT_TRUE(NULL != iabChannelInterface);
            
            // Test channel ID
            IABChannelIDType channelID, channelIDGet;
            
            // DEfault
            iabChannelInterface->GetChannelID(channelID);
            EXPECT_EQ(channelID, kIABChannelID_Center);
            
            // Test set and get
            for (uint16_t idCode = kIABChannelID_Left; idCode < (kIABChannelID_TopSurround + 1); idCode++)
            {
                channelID = static_cast<IABChannelIDType>(idCode);
                EXPECT_EQ(iabChannelInterface->SetChannelID(channelID), kIABNoError);
                iabChannelInterface->GetChannelID(channelIDGet);
                EXPECT_EQ(channelID, channelIDGet);
            }
            
            // Test audio data ID
            IABAudioDataIDType audioDataID = 1;
            iabChannelInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0);
            
            // Set audiodata ID and verify
            EXPECT_EQ(iabChannelInterface->SetAudioDataID(0xFFFFFFFF), kIABNoError);
            iabChannelInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0xFFFFFFFF);
            
            // Test channel gain, prefix code 3 is reserved, gain range [0.0f, 1.0f] map to [0x3FF, 0]
            IABGain channelGain, channelGainGet;
            
            // Default
            iabChannelInterface->GetChannelGain(channelGainGet);
            EXPECT_EQ(channelGainGet.getIABGain(), 1.0f);
            EXPECT_EQ(channelGainGet.getIABGainPrefix(), kIABGainPrefix_Unity);
            EXPECT_EQ(channelGainGet.getIABGainInStreamValue(), 0);
            
            channelGain.setIABGain(0.0f);
            EXPECT_EQ(iabChannelInterface->SetChannelGain(channelGain), kIABNoError);
            iabChannelInterface->GetChannelGain(channelGainGet);
            EXPECT_EQ(channelGainGet.getIABGain(), 0.0f);
            EXPECT_EQ(channelGainGet.getIABGainPrefix(), kIABGainPrefix_Silence);
            EXPECT_EQ(channelGainGet.getIABGainInStreamValue(), 0x3FF);
            
            channelGain.setIABGain(1.0f);
            EXPECT_EQ(iabChannelInterface->SetChannelGain(channelGain), kIABNoError);
            iabChannelInterface->GetChannelGain(channelGainGet);
            EXPECT_EQ(channelGainGet.getIABGain(), 1.0f);
            EXPECT_EQ(channelGainGet.getIABGainPrefix(), kIABGainPrefix_Unity);
            EXPECT_EQ(channelGainGet.getIABGainInStreamValue(), 0);
            
            channelGain.setIABGain(0.5f);
            EXPECT_EQ(iabChannelInterface->SetChannelGain(channelGain), kIABNoError);
            iabChannelInterface->GetChannelGain(channelGainGet);
            EXPECT_EQ(channelGainGet.getIABGain(), 0.5f);
            EXPECT_EQ(channelGainGet.getIABGainPrefix(), kIABGainPrefix_InStream);
            EXPECT_EQ(channelGainGet.getIABGainInStreamValue(), 64);    // gain in stream code for 0.5f
            
            EXPECT_EQ(channelGain.setIABGain(1.1f), kIABBadArgumentsError);
            EXPECT_EQ(channelGain.setIABGain(-0.1f), kIABBadArgumentsError);
            
            // Test channel decor info exists
            uint1_t decorInfoExists;
            iabChannelInterface->GetDecorInfoExists(decorInfoExists);
            EXPECT_EQ(decorInfoExists, 0);
            
            EXPECT_EQ(iabChannelInterface->SetDecorInfoExists(1), kIABNoError);
            iabChannelInterface->GetDecorInfoExists(decorInfoExists);
            EXPECT_EQ(decorInfoExists, 1);
            
            // Test channel decor coefficient
            IABDecorCoeff decorCoef, decorCoefGet;
            
            // Default
            iabChannelInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_NoDecor);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0);
            
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
            decorCoef.decorCoef_ = 0;
            EXPECT_EQ(iabChannelInterface->SetDecorCoef(decorCoef), kIABNoError);
            iabChannelInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_NoDecor);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0);
            
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            decorCoef.decorCoef_ = 0;
            EXPECT_EQ(iabChannelInterface->SetDecorCoef(decorCoef), kIABNoError);
            iabChannelInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_MaxDecor);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0);
            
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_DecorCoeffInStream;
            decorCoef.decorCoef_ = 0xFF;    // unsigned 8-bit maximum value
            EXPECT_EQ(iabChannelInterface->SetDecorCoef(decorCoef), kIABNoError);
            iabChannelInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_DecorCoeffInStream);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0xFF);
            
            IABChannelInterface::Delete(iabChannelInterface);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************

        void SetupPackerChannel()
        {
            ASSERT_EQ(iabPackerChannel_->SetChannelID(channelID_), kIABNoError);
            ASSERT_EQ(iabPackerChannel_->SetAudioDataID(audioDataID_), kIABNoError);
            ASSERT_EQ(iabPackerChannel_->SetChannelGain(channelGain_), kIABNoError);
            ASSERT_EQ(iabPackerChannel_->SetDecorInfoExists(decorInfoExists_), kIABNoError);
            ASSERT_EQ(iabPackerChannel_->SetDecorCoef(decorCoef_), kIABNoError);
            
        }
        
        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        void VerifyDeSerializedChannel()
        {
            IABChannelIDType channelID;
            IABAudioDataIDType audioDataID;
            IABGain channelGain;
            uint1_t decorInfoExists;
            IABDecorCoeff decorCoef;
            
            iabParserChannel_->GetChannelID(channelID);
            iabParserChannel_->GetAudioDataID(audioDataID);
            iabParserChannel_->GetChannelGain(channelGain);

            EXPECT_EQ(channelID, channelID_);
            EXPECT_EQ(audioDataID, audioDataID_);
            EXPECT_EQ((channelGain == channelGain_), true);

            iabParserChannel_->GetDecorInfoExists(decorInfoExists);
            EXPECT_EQ(decorInfoExists, decorInfoExists_);
            
            if (decorInfoExists)
            {
                iabParserChannel_->GetDecorCoef(decorCoef);
                EXPECT_EQ(decorCoef.decorCoefPrefix_, decorCoef_.decorCoefPrefix_);
                EXPECT_EQ(decorCoef.decorCoef_, decorCoef_.decorCoef_);
            }
        }

        // **********************************************
        
        // Function to test Serialize() and DeSerialize()
        
        // **********************************************

        void TestSerializeDeSerialize()
        {
            // Test case:
            // Unity gain, decorInfoExists_ = 0, no decor coeff
            channelGain_.setIABGain(1.0f);
            decorInfoExists_ = 0;
            decorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
            decorCoef_.decorCoef_ = 0;
            RunSerializeDeSerializeTestCase();
            
            // Test case:
            // Silence gain, decorInfoExists_ = 0, no decor coeff
            channelGain_.setIABGain(0.0f);
            RunSerializeDeSerializeTestCase();

            // Test case:
            // Gain in bitstream = 0.1, decorInfoExists_ = 0, no decor coeff
            channelGain_.setIABGain(0.1f);
            RunSerializeDeSerializeTestCase();
            
            // Test case:
            // Gain in bitstream = 0.5, decorInfoExists_ = 0, no decor coeff
            channelGain_.setIABGain(0.5f);
            RunSerializeDeSerializeTestCase();

            // Test case:
            // Unity gain, decorInfoExists_ = 1, prefix = max decor (decor coefficient will be ignored)
            decorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            RunSerializeDeSerializeTestCase();

            // Test case:
            // Unity gain, decorInfoExists_ = 1, prefix = decor coefficient in stream, coefficient = 0
            decorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_DecorCoeffInStream;
            decorCoef_.decorCoef_ = 0;
            RunSerializeDeSerializeTestCase();

            // Test case:
            // Unity gain, decorInfoExists_ = 1, prefix = decor coefficient in stream, coefficient = 0x40, a non-zero random test value
            decorCoef_.decorCoef_ = 0x40;
            RunSerializeDeSerializeTestCase();

            // Test case:
            // Unity gain, decorInfoExists_ = 1, prefix = decor coefficient in stream, coefficient = 0xFF (maximum value for 8-bit range)
            decorCoef_.decorCoef_ = 0xFF;
            RunSerializeDeSerializeTestCase();

        }

        void RunSerializeDeSerializeTestCase()
        {
            // Create IAB bed channel (packer) to Serialize stream buffer
            iabPackerChannel_ = new IABChannel();
            ASSERT_TRUE(NULL != iabPackerChannel_);
            
            // Create IAB bed channel (parser) to de-serialize stream buffer
            iabParserChannel_= new IABChannel();
            ASSERT_TRUE(NULL != iabParserChannel_);
            
            // Set up IAB packer channel
            SetupPackerChannel();
            
            // stream to hold serialized bitstream
            std::stringstream  channelStream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // stream writer for serialing
            StreamWriter channelWriter(channelStream);

            // Serialize bed channel into stream
            ASSERT_EQ(iabPackerChannel_->Serialize(channelWriter), kIABNoError);
            channelWriter.align();      // ensure last byte (decor coefficient) is properly serilaized into stream
            
            // stream reader for de-serializing channel from stream
            StreamReader channelReader(channelStream);
            
            // DeSerialize bed channel from stream
            ASSERT_EQ(iabParserChannel_->DeSerialize(channelReader), kIABNoError);
            
            // Verify deserialized bed channel
            VerifyDeSerializedChannel();

			delete iabPackerChannel_;
			delete iabParserChannel_;
        }
        
        
    private:
        IABChannel*   iabPackerChannel_;
        IABChannel*   iabParserChannel_;
        
        // Channel element data members
        IABChannelIDType channelID_;
        IABAudioDataIDType audioDataID_;
        IABGain channelGain_;
        uint1_t decorInfoExists_;
        IABDecorCoeff decorCoef_;

    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run IABChannel setters and getters API tests
    TEST_F(IABChannel_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial, then deSerialize tests
    TEST_F(IABChannel_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
}
