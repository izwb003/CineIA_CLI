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

    // IABAudioDataDLC element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABDLCElement_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            // Initialize variables
            dlcAudioDataID_ = 1;
            dlcSize_ = 0;
            sampleRate_ = kIABSampleRate_48000Hz;
            frameRateCode_ = kIABFrameRate_24FPS;
        }
        
        // **********************************************
        
        // IABAudioDataDLC element setters and getters API tests
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
            IABAudioDataDLCInterface* dlcElementInterface = NULL;
            dlcElementInterface = IABAudioDataDLCInterface::Create(kIABFrameRate_24FPS, kIABSampleRate_48000Hz);
            ASSERT_TRUE(NULL != dlcElementInterface);
            
            // Test audio data ID
            IABAudioDataIDType audioDataID = 1;
            EXPECT_EQ(dlcElementInterface->SetAudioDataID(0), kIABNoError);
            dlcElementInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0);
            
            // Set audiodata ID and verify
            EXPECT_EQ(dlcElementInterface->SetAudioDataID(0xFFFFFFFF), kIABNoError);
            dlcElementInterface->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, 0xFFFFFFFF);
            
            // Test sample rate
            IABSampleRateType dlcSampleRate;
            EXPECT_EQ(dlcElementInterface->SetDLCSampleRate(kIABSampleRate_48000Hz), kIABNoError);
            EXPECT_EQ(dlcElementInterface->GetDLCSampleRate(dlcSampleRate), kIABNoError);
            EXPECT_EQ(dlcSampleRate, kIABSampleRate_48000Hz);

            // 96KHz
            EXPECT_EQ(dlcElementInterface->SetDLCSampleRate(kIABSampleRate_96000Hz), kIABNoError);
            EXPECT_EQ(dlcElementInterface->GetDLCSampleRate(dlcSampleRate), kIABNoError);
            EXPECT_EQ(dlcSampleRate, kIABSampleRate_96000Hz);
            
            // Test DLC data size
            uint16_t dlcSize;
            
            // Default
            dlcElementInterface->GetDLCSize(dlcSize);
            EXPECT_EQ(dlcSize, 0);
            
            // Set to uint32_t maximum value
            dlcSize = 0xFFFF;
            EXPECT_EQ(dlcElementInterface->SetDLCSize(dlcSize), kIABNoError);
            dlcElementInterface->GetDLCSize(dlcSize);
            EXPECT_EQ(dlcSize, 0xFFFF);
            
            IABAudioDataDLCInterface::Delete(dlcElementInterface);
        }
        
        
        // **********************************************
        // Function to test Serialize() and DeSerialize()
        // **********************************************
        
        void TestSerializeDeSerialize()
        {
			// Test 24 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
            RunSerializeDeSerializeTestCase();
            
            // Test 25 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_25FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 30 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_30FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 48 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_48FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 50 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_50FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 60 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_60FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 96 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_96FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 100 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_100FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
            
            // Test 120 Frames per second (FPS)
            frameRateCode_ = kIABFrameRate_120FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();
        }
        
		void TestSerializeDeSerialize96k()
		{
			sampleRate_ = kIABSampleRate_96000Hz;

			// Test 24 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 25 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_25FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 30 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_30FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 48 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_48FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 50 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_50FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 60 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_60FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 96 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_96FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 100 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_100FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			// Test 120 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_120FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRate_);
			RunSerializeDeSerializeTestCase();

			sampleRate_ = kIABSampleRate_48000Hz;
		}

		void RunSerializeDeSerializeTestCase()
        {
			IABAudioDataDLC *iabPackerDLCElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));
            ASSERT_TRUE(NULL != iabPackerDLCElement);
            
            ASSERT_EQ(iabPackerDLCElement->SetAudioDataID(dlcAudioDataID_), kIABNoError);
            ASSERT_EQ(iabPackerDLCElement->SetDLCSampleRate(sampleRate_), kIABNoError);
            
            // Use silent audio samples
            int32_t* pAudioSamples = new int32_t[frameSampleCount_];
			memset(pAudioSamples, 0, sizeof(int32_t)*frameSampleCount_);

            iabError returnCode = kIABNoError;
            
            // **********************
            // Test encode API
            // **********************
            
            returnCode = iabPackerDLCElement->EncodeMonoPCMToDLC(pAudioSamples, frameSampleCount_);
            
            if (kIABNoError == returnCode)
            {
                // **********************
                // Test decode API
                // **********************
                
                returnCode = iabPackerDLCElement->DecodeDLCToMonoPCM(pAudioSamples, frameSampleCount_, sampleRate_);
            }
            
            delete [] pAudioSamples;
            
            ASSERT_EQ(returnCode, kIABNoError);
            
            // **********************
            // Serialize
            // **********************
            
            // stream buffer to hold serialzed stream
            std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // Serialize AudioDataDLC into stream
            ASSERT_EQ(iabPackerDLCElement->Serialize(elementBuffer), kIABNoError);
            
            elementBuffer.seekg(0, std::ios::end);
            
            std::ios_base::streampos pos = elementBuffer.tellg();
            
            IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
            
            IABElementSizeType elementSize = 0;
            
            iabPackerDLCElement->GetElementSize(elementSize);
            
            bytesInStream -= 4;     // Deduct 3 bytes for element ID (0xFF0200) and 1 byte element size code
            
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
            
            // **********************
            // De-Serialize
            // **********************

			IABAudioDataDLC *iabParserDLCElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRateCode_, sampleRate_));
            ASSERT_TRUE(NULL != iabParserDLCElement);

            // stream reader for parsing
            StreamReader elementReader(elementBuffer);
            
            // DeSerialize bed definition from stream
            ASSERT_EQ(iabParserDLCElement->DeSerialize(elementReader), kIABNoError);
            
            // Verify element size
            
            std::ios_base::streampos readerPos = elementReader.streamPosition();
            
            bytesInStream = static_cast<IABElementSizeType>(readerPos);
            
            iabParserDLCElement->GetElementSize(elementSize);
            
            bytesInStream -= 4;     // Deduct 3 bytes for element ID (0xFF0200) and 1 byte element size code
            
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
            
            // Test audio data ID
            IABAudioDataIDType audioDataID;
            iabParserDLCElement->GetAudioDataID(audioDataID);
            EXPECT_EQ(audioDataID, dlcAudioDataID_);
            
            // Test sample rate
            IABSampleRateType dlcSampleRate;
            EXPECT_EQ(iabParserDLCElement->GetDLCSampleRate(dlcSampleRate), kIABNoError);
            EXPECT_EQ(dlcSampleRate, sampleRate_);
            
			delete iabPackerDLCElement;
			delete iabParserDLCElement;
        }


    private:
        
        IABSampleRateType           sampleRate_;
        IABFrameRateType            frameRateCode_;
        IABAudioDataIDType          dlcAudioDataID_;
        uint16_t                    dlcSize_;
        uint32_t                    frameSampleCount_;

        
    };

    // ********************
    // Run tests
    // ********************
    
    // Run frame element setters and getters API tests
    TEST_F(IABDLCElement_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial frame, then deSerialize frame tests
    TEST_F(IABDLCElement_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
	// Run serial frame, then deSerialize frame tests (96k)
	TEST_F(IABDLCElement_Test, Test_Serialize_DeSerialize_96k)
	{
		TestSerializeDeSerialize96k();
	}
}
