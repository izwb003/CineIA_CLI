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
	// IABAudioDataPCM element tests:
	// 1. Test setters and getter APIs
	// 2. Test Serialize() into a stream (packed buffer)
	// 3. Test DeSerialize() from the stream (packed buffer).

	class IABPCMElement_Test : public testing::Test
	{
	protected:

		// Set up test variables and configuration
		void SetUp()
		{
			// Initialize variables
			pcmAudioDataID_ = 1;
			frameRateCode_ = kIABFrameRate_24FPS;
			sampleRateCode_ = kIABSampleRate_48000Hz;
			bitDepthCode_ = kIABBitDepth_24Bit;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
		}

		// **********************************************
		// IABAudioDataPCM element API tests
		// **********************************************
		void TestSetterGetterAPIs()
		{
			IABAudioDataPCMInterface* pcmElementInterface = NULL;
			pcmElementInterface = IABAudioDataPCMInterface::Create(frameRateCode_, sampleRateCode_, bitDepthCode_);
			ASSERT_TRUE(NULL != pcmElementInterface);

			// Test audio data ID
			IABAudioDataIDType audioDataID = 0;
			EXPECT_EQ(pcmElementInterface->SetAudioDataID(pcmAudioDataID_), kIABNoError);
			pcmElementInterface->GetAudioDataID(audioDataID);
			EXPECT_EQ(audioDataID, pcmAudioDataID_);

			// Set audiodata ID and verify
			EXPECT_EQ(pcmElementInterface->SetAudioDataID(0xFFFFFFFF), kIABNoError);
			pcmElementInterface->GetAudioDataID(audioDataID);
			EXPECT_EQ(audioDataID, 0xFFFFFFFF);

			// Test frame rate, sample rate and bit depth associated with the PCM element.
			// They are expected to be equal to what have been set at instance creation
			EXPECT_EQ(pcmElementInterface->GetPCMFrameRate(), frameRateCode_);
			EXPECT_EQ(pcmElementInterface->GetPCMSampleRate(), sampleRateCode_);
			EXPECT_EQ(pcmElementInterface->GetPCMBitDepth(), bitDepthCode_);
			EXPECT_EQ(pcmElementInterface->GetPCMSampleCount(), frameSampleCount_);

			IABAudioDataPCMInterface::Delete(pcmElementInterface);
		}

		// **********************************************
		// Function to test Serialize() and DeSerialize()
		// **********************************************
		void TestSerializeDeSerialize()
		{
			// ************* 48kHz tests section **********************
			//

			// These are the same values as coming off SetUp(), but included for clarity.
			sampleRateCode_ = kIABSampleRate_48000Hz;

			// Test 24 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 25 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_25FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 30 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_30FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 48 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_48FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 50 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_50FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 60 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_60FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 96 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_96FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 100 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_100FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 120 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_120FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// ************* 96kHz tests section **********************
			//
			sampleRateCode_ = kIABSampleRate_96000Hz;

			// Test 24 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_24FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 25 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_25FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 30 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_30FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 48 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_48FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 50 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_50FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 60 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_60FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 96 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_96FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 100 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_100FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();

			// Test 120 Frames per second (FPS)
			frameRateCode_ = kIABFrameRate_120FPS;
			frameSampleCount_ = GetIABNumFrameSamples(frameRateCode_, sampleRateCode_);
			bitDepthCode_ = kIABBitDepth_24Bit;					// 24-bit test
			RunSerializeDeSerializeTestCase();
			bitDepthCode_ = kIABBitDepth_16Bit;					// 16-bit test
			RunSerializeDeSerializeTestCase();
		}

		void RunSerializeDeSerializeTestCase()
		{
			IABAudioDataPCM* iabPackerPCMElement = new IABAudioDataPCM(frameRateCode_, sampleRateCode_, bitDepthCode_);
			ASSERT_TRUE(NULL != iabPackerPCMElement);

			ASSERT_EQ(iabPackerPCMElement->SetAudioDataID(pcmAudioDataID_), kIABNoError);

			// Test samples
			int32_t* sourceAudioSamples = new int32_t[frameSampleCount_];
			int32_t *inputSample = sourceAudioSamples;

			// Generate random int32_t samples between [-2147483648, 2147483647]
			// Rand seed init
			srand(0);
			for (uint32_t i = 0; i < frameSampleCount_; i++)
			{
				*inputSample++ = int32_t((float(rand()) / float(RAND_MAX) - 0.5f) * 4294967295.0f);
			}

			// Pack PCM sample
			ASSERT_EQ(iabPackerPCMElement->PackMonoSamplesToPCM(sourceAudioSamples, frameSampleCount_), kIABNoError);

			// **********************
			// Serialize
			// **********************

			// stream buffer to hold serialzed stream
			std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

			// Serialize AudioDataPCM into stream
			ASSERT_EQ(iabPackerPCMElement->Serialize(elementBuffer), kIABNoError);

			elementBuffer.seekg(0, std::ios::end);

			std::ios_base::streampos pos = elementBuffer.tellg();

			IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);

			IABElementSizeType elementSize = 0;

			iabPackerPCMElement->GetElementSize(elementSize);

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
			IABAudioDataPCM* iabParserPCMElement = new IABAudioDataPCM(frameRateCode_, sampleRateCode_, bitDepthCode_);
			ASSERT_TRUE(NULL != iabParserPCMElement);

			// stream reader for parsing
			StreamReader elementReader(elementBuffer);

			// DeSerialize bed definition from stream
			ASSERT_EQ(iabParserPCMElement->DeSerialize(elementReader), kIABNoError);

			// Verify element size
			std::ios_base::streampos readerPos = elementReader.streamPosition();

			bytesInStream = static_cast<IABElementSizeType>(readerPos);

			iabParserPCMElement->GetElementSize(elementSize);

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
			iabParserPCMElement->GetAudioDataID(audioDataID);
			EXPECT_EQ(audioDataID, pcmAudioDataID_);

			// Buffer for unpacked samples
			int32_t* unpackedAudioSamples = new int32_t[frameSampleCount_];
			int32_t *outputSample = unpackedAudioSamples;
			inputSample = sourceAudioSamples;

			// Unpack PCM samples
			ASSERT_EQ(iabPackerPCMElement->UnpackPCMToMonoSamples(unpackedAudioSamples, frameSampleCount_), kIABNoError);

			// Prep to compare unpacked samples to source samples
			uint32_t byteMask = 0xFFFFFF00;						// Init to 24-bit mask
			if (bitDepthCode_ == kIABBitDepth_16Bit)
			{
				byteMask = 0xFFFF0000;							// 16-bit mask
			}

			// Compare PCM sample values
			for (uint32_t i = 0; i < frameSampleCount_; i++)
			{
				EXPECT_EQ((*inputSample++ & byteMask), (*outputSample++ & byteMask));
			}

			delete[] sourceAudioSamples;
			delete[] unpackedAudioSamples;
			delete iabPackerPCMElement;
			delete iabParserPCMElement;
		}

	private:

		IABFrameRateType            frameRateCode_;
		IABSampleRateType           sampleRateCode_;
		IABBitDepthType             bitDepthCode_;
		uint32_t                    frameSampleCount_;
		IABAudioDataIDType          pcmAudioDataID_;
	};

	// ********************
	// Run tests
	// ********************

	// Run frame element setters and getters API tests
	TEST_F(IABPCMElement_Test, Test_Setters_Getters_APIs)
	{
		TestSetterGetterAPIs();
	}

	// Run serial frame, then deSerialize frame tests
	TEST_F(IABPCMElement_Test, Test_Serialize_DeSerialize)
	{
		TestSerializeDeSerialize();
	}
}
