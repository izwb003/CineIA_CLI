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
    // IABAuthoringToolInfo element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABAuthoringToolInfo_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabPackerAuthoringToolInfo_ = NULL;
            iabParserAuthoringToolInfo_ = NULL;

			// Create IAB authoring tool info (packer) to Serialize stream buffer
			iabPackerAuthoringToolInfo_ = new IABAuthoringToolInfo();

			// Create IAB authoring tool info (parser) to de-serialize stream buffer
			iabParserAuthoringToolInfo_ = new IABAuthoringToolInfo();
		}

		// TearDown() to do any clean-up
		void TearDown()
		{
			delete iabPackerAuthoringToolInfo_;
			delete iabParserAuthoringToolInfo_;
		}

        // **********************************************
        // IABAuthoringToolInfo element setters and getters API tests
        // **********************************************
        
        void TestSetterGetterAPIs()
        {
            // Create an instance. Test IABAuthoringToolInfoInterface creation API
            IABAuthoringToolInfoInterface* iabAuthoringToolInfoInterface = NULL;
			iabAuthoringToolInfoInterface = IABAuthoringToolInfoInterface::Create();
            ASSERT_TRUE(NULL != iabAuthoringToolInfoInterface);

			const char* authoringToolURI = NULL;

            // Test Getter for authoring tool URI, a null-terminated, strictly ASCII string
            iabAuthoringToolInfoInterface->GetAuthoringToolInfo(authoringToolURI);
            EXPECT_EQ(0, strlen(authoringToolURI));								// default to 0-length

			const char testAuthoringToolURI[100] = "This is a test string for IAB authoring tool info URI. ";

			// Test Setter and Getter for authoring tool URI, a null-terminated, strictly ASCII string
			EXPECT_EQ(iabAuthoringToolInfoInterface->SetAuthoringToolInfo(testAuthoringToolURI), kIABNoError);
			iabAuthoringToolInfoInterface->GetAuthoringToolInfo(authoringToolURI);
			EXPECT_EQ(strlen(testAuthoringToolURI), strlen(authoringToolURI));
			EXPECT_EQ(0, strcmp(authoringToolURI, testAuthoringToolURI));

            IABAuthoringToolInfoInterface::Delete(iabAuthoringToolInfoInterface);
        }
        
        // **********************************************
        // Function to test Serialize() and DeSerialize()
        // **********************************************

        void TestSerializeDeSerialize()
        {
			// Check the 2 class variable pointers, allocated as part of test class set-up.
			ASSERT_TRUE(NULL != iabPackerAuthoringToolInfo_);
			ASSERT_TRUE(NULL != iabParserAuthoringToolInfo_);

			const char testAuthoringToolURI[100] = "Serialize-DeSerialize test: This is a test string for IAB authoring tool info URI. ";

			// Set up for packing/serialize
			EXPECT_EQ(iabPackerAuthoringToolInfo_->SetAuthoringToolInfo(testAuthoringToolURI), kIABNoError);

			// stream buffer to hold serialzed stream
			std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

			// Serialize iabPackerAuthoringToolInfo_ into stream
			ASSERT_EQ(iabPackerAuthoringToolInfo_->Serialize(elementBuffer), kIABNoError);

			elementBuffer.seekg(0, std::ios::end);							// Do we need this? Would it not be already at this position?
			auto pos = elementBuffer.tellg();
			IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
			IABElementSizeType elementSize = 0;

			iabPackerAuthoringToolInfo_->GetElementSize(elementSize);

			// Element ID for authoring tool info is "kIABElementID_AuthoringToolInfo = 0x100".
			// With Plex coding, it is known to take 3 bytes to code 0x100.
			bytesInStream -= 4;					// Deduct 3 bytes for element ID (0xFF0100) and 1 byte element size code

			// elementSize is also Plex coded. The following is to resolve number fo bytes used for coding "size"
			// It depends on its value range: 8-bit? 16-bit? or greater (32-bit is the current MAX).
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

			// Reset stream to beginning
			elementBuffer.seekg(0, std::ios::beg);

			// stream reader for parsing
			StreamReader elementReader(elementBuffer);

			// DeSerialize from packed buffer
			ASSERT_EQ(iabParserAuthoringToolInfo_->DeSerialize(elementReader), kIABNoError);

			// Verify deserialized authroing tool info URI
			const char* retrievedAuthoringToolURI = NULL;

			iabParserAuthoringToolInfo_->GetAuthoringToolInfo(retrievedAuthoringToolURI);

			EXPECT_EQ(strlen(testAuthoringToolURI), strlen(retrievedAuthoringToolURI));
			EXPECT_EQ(0, strcmp(retrievedAuthoringToolURI, testAuthoringToolURI));

			// Verify element size
			auto readerPos = elementReader.streamPosition();
			bytesInStream = static_cast<IABElementSizeType>(readerPos);
			iabParserAuthoringToolInfo_->GetElementSize(elementSize);

			bytesInStream -= 4;					// Deduct 3 bytes for element ID (0xFF0100) and 1 byte element size code

			// See comment above during packing
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
		}

    private:
        
        IABAuthoringToolInfo*   iabPackerAuthoringToolInfo_;
        IABAuthoringToolInfo*   iabParserAuthoringToolInfo_;
    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run IABAuthoringToolInfo element setters and getters API tests
    TEST_F(IABAuthoringToolInfo_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serialize IABAuthoringToolInfo, then deSerialize IABAuthoringToolInfo tests
    TEST_F(IABAuthoringToolInfo_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
}
