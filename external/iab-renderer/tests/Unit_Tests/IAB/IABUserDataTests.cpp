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
    // IABUserData element tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer).
    
    class IABUserData_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            iabPackerUserData_ = NULL;
            iabParserUserData_ = NULL;
        }
        
        // **********************************************
        // IABUserData element setters and getters API tests
        // **********************************************
        
        void TestSetterGetterAPIs()
        {
            // Create an instance. Test IABUserDataInterface creation API
            IABUserDataInterface* iabUserDataInterface = NULL;
			iabUserDataInterface = IABUserDataInterface::Create();
            ASSERT_TRUE(NULL != iabUserDataInterface);

			// Set up user ID
			uint8_t userID[16] = { 0x06,0x0E,0x2B,0x34,0x04,0x01,0x01,0x0D,0x0F,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
			uint8_t getUserID[16];

            // Test Getter / Setter for user ID
            iabUserDataInterface->GetUserID(getUserID);
            EXPECT_EQ(0, getUserID[0]);														// default bytes = 0, testing on 1st byte

			EXPECT_EQ(iabUserDataInterface->SetUserID(userID), kIABNoError);
			iabUserDataInterface->GetUserID(getUserID);
			// Sample check selected bytes
			EXPECT_EQ(0x6, getUserID[0]);
			EXPECT_EQ(0xF, getUserID[8]);
			EXPECT_EQ(0x5, getUserID[13]);

			std::vector<uint8_t> testUserData(rawUserData, rawUserData + sizeof(rawUserData) / sizeof(rawUserData[0]));
			std::vector<uint8_t> getUserData;

			// Test Getter / Setter for user data
			iabUserDataInterface->GetUserDataBytes(getUserData);
			EXPECT_EQ(0, getUserData.size());													// default to 0-length (cleared)

			EXPECT_EQ(iabUserDataInterface->SetUserDataBytes(testUserData), kIABNoError);
			iabUserDataInterface->GetUserDataBytes(getUserData);

			EXPECT_EQ(getUserData.size(), testUserData.size());									// Sizes are expected to be equal

			for (uint32_t i = 0; i < testUserData.size(); i++)
			{
				EXPECT_EQ(getUserData[i], testUserData[i]);										// All pair-wise values are expected to be equal
			}
            
            IABUserDataInterface::Delete(iabUserDataInterface);
		}
        
        // **********************************************
        // Function to test Serialize() and DeSerialize()
        // **********************************************

        void TestSerializeDeSerialize()
        {
			// Create IAB user data element (packer) to Serialize stream buffer
			iabPackerUserData_ = new IABUserData();
			ASSERT_TRUE(NULL != iabPackerUserData_);

			// Create IAB user data element (parser) to de-serialize stream buffer
			iabParserUserData_ = new IABUserData();
			ASSERT_TRUE(NULL != iabParserUserData_);

			// Set up user ID and data
			uint8_t userID[16] = { 0x06,0x0E,0x2B,0x34,0x04,0x01,0x01,0x0D,0x0F,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
			std::vector<uint8_t> testUserData(rawUserData, rawUserData + sizeof(rawUserData) / sizeof(rawUserData[0]));

			// Set up for packing/serialize
			EXPECT_EQ(iabPackerUserData_->SetUserID(userID), kIABNoError);
			EXPECT_EQ(iabPackerUserData_->SetUserDataBytes(testUserData), kIABNoError);

			// stream buffer to hold serialzed stream
			std::stringstream  elementBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

			// Serialize iabPackerUserData_ into stream
			ASSERT_EQ(iabPackerUserData_->Serialize(elementBuffer), kIABNoError);

			elementBuffer.seekg(0, std::ios::end);							// Do we need this? Would it not be already at this position?
			std::ios_base::streampos pos = elementBuffer.tellg();
			IABElementSizeType bytesInStream = static_cast<IABElementSizeType>(pos);
			IABElementSizeType elementSize = 0;

			iabPackerUserData_->GetElementSize(elementSize);

			// Element ID for IAB user data is "kIABElementID_UserData = 0x101".
			// With Plex coding, it is known to take 3 bytes to code 0x101.
			bytesInStream -= 4;					// Deduct 3 bytes for element ID (0xFF0101) and 1 byte for element size code

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
			ASSERT_EQ(iabParserUserData_->DeSerialize(elementReader), kIABNoError);

			// Verify deserialized data for user data element
			uint8_t retrievedUserID[16];
			std::vector<uint8_t> retrievedUserData;

			iabParserUserData_->GetUserID(retrievedUserID);
			iabParserUserData_->GetUserDataBytes(retrievedUserData);

			// Verify desrialized data against data that was packed in
			for (uint32_t i = 0; i < 16; i++)
			{
				EXPECT_EQ(retrievedUserID[i], userID[i]);
			}

			EXPECT_EQ(retrievedUserData.size(), testUserData.size());									// Sizes are expected to be equal

			for (uint32_t i = 0; i < testUserData.size(); i++)
			{
				EXPECT_EQ(retrievedUserData[i], testUserData[i]);										// All pair-wise values are expected to be equal
			}

			// Verify element size
			std::ios_base::streampos readerPos = elementReader.streamPosition();
			bytesInStream = static_cast<IABElementSizeType>(readerPos);
			iabParserUserData_->GetElementSize(elementSize);

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

			delete iabPackerUserData_;
			delete iabParserUserData_;
		}

    private:
        
        IABUserData*   iabPackerUserData_;
        IABUserData*   iabParserUserData_;

		// Random uint8_t as user data, taken from MDACreator code base, JUCER_RESOURCE: ui_toplogo_png, 2565, "../Images/ui_toplogo.png"
		static const uint8_t rawUserData[2567];
	};

	// Random uint8_t as user data, taken from MDACreator code base, JUCER_RESOURCE: ui_toplogo_png, 2565, "../Images/ui_toplogo.png"
	const uint8_t IABUserData_Test::rawUserData[2567] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,85,0,0,0,25,8,6,0,0,0,64,111,98,77,0,0,0,25,116,69,88,116,83,111,
		102,116,119,97,114,101,0,65,100,111,98,101,32,73,109,97,103,101,82,101,97,100,121,113,201,101,60,0,0,3,36,105,84,88,116,88,77,76,58,99,111,109,46,97,100,111,98,101,46,120,109,112,0,0,0,0,0,60,63,120,112,
		97,99,107,101,116,32,98,101,103,105,110,61,34,239,187,191,34,32,105,100,61,34,87,53,77,48,77,112,67,101,104,105,72,122,114,101,83,122,78,84,99,122,107,99,57,100,34,63,62,32,60,120,58,120,109,112,109,101,
		116,97,32,120,109,108,110,115,58,120,61,34,97,100,111,98,101,58,110,115,58,109,101,116,97,47,34,32,120,58,120,109,112,116,107,61,34,65,100,111,98,101,32,88,77,80,32,67,111,114,101,32,53,46,51,45,99,48,
		49,49,32,54,54,46,49,52,53,54,54,49,44,32,50,48,49,50,47,48,50,47,48,54,45,49,52,58,53,54,58,50,55,32,32,32,32,32,32,32,32,34,62,32,60,114,100,102,58,82,68,70,32,120,109,108,110,115,58,114,100,102,61,
		34,104,116,116,112,58,47,47,119,119,119,46,119,51,46,111,114,103,47,49,57,57,57,47,48,50,47,50,50,45,114,100,102,45,115,121,110,116,97,120,45,110,115,35,34,62,32,60,114,100,102,58,68,101,115,99,114,105,
		112,116,105,111,110,32,114,100,102,58,97,98,111,117,116,61,34,34,32,120,109,108,110,115,58,120,109,112,61,34,104,116,116,112,58,47,47,110,115,46,97,100,111,98,101,46,99,111,109,47,120,97,112,47,49,46,
		48,47,34,32,120,109,108,110,115,58,120,109,112,77,77,61,34,104,116,116,112,58,47,47,110,115,46,97,100,111,98,101,46,99,111,109,47,120,97,112,47,49,46,48,47,109,109,47,34,32,120,109,108,110,115,58,115,
		116,82,101,102,61,34,104,116,116,112,58,47,47,110,115,46,97,100,111,98,101,46,99,111,109,47,120,97,112,47,49,46,48,47,115,84,121,112,101,47,82,101,115,111,117,114,99,101,82,101,102,35,34,32,120,109,112,
		58,67,114,101,97,116,111,114,84,111,111,108,61,34,65,100,111,98,101,32,80,104,111,116,111,115,104,111,112,32,67,83,54,32,40,77,97,99,105,110,116,111,115,104,41,34,32,120,109,112,77,77,58,73,110,115,116,
		97,110,99,101,73,68,61,34,120,109,112,46,105,105,100,58,65,49,52,52,56,48,65,50,70,56,51,66,49,49,69,51,65,57,69,55,65,67,49,68,48,55,57,55,52,52,52,69,34,32,120,109,112,77,77,58,68,111,99,117,109,101,
		110,116,73,68,61,34,120,109,112,46,100,105,100,58,65,49,52,52,56,48,65,51,70,56,51,66,49,49,69,51,65,57,69,55,65,67,49,68,48,55,57,55,52,52,52,69,34,62,32,60,120,109,112,77,77,58,68,101,114,105,118,101,
		100,70,114,111,109,32,115,116,82,101,102,58,105,110,115,116,97,110,99,101,73,68,61,34,120,109,112,46,105,105,100,58,65,49,52,52,56,48,65,48,70,56,51,66,49,49,69,51,65,57,69,55,65,67,49,68,48,55,57,55,
		52,52,52,69,34,32,115,116,82,101,102,58,100,111,99,117,109,101,110,116,73,68,61,34,120,109,112,46,100,105,100,58,65,49,52,52,56,48,65,49,70,56,51,66,49,49,69,51,65,57,69,55,65,67,49,68,48,55,57,55,52,
		52,52,69,34,47,62,32,60,47,114,100,102,58,68,101,115,99,114,105,112,116,105,111,110,62,32,60,47,114,100,102,58,82,68,70,62,32,60,47,120,58,120,109,112,109,101,116,97,62,32,60,63,120,112,97,99,107,101,
		116,32,101,110,100,61,34,114,34,63,62,106,111,219,129,0,0,6,119,73,68,65,84,120,218,236,153,123,76,149,101,28,199,207,123,56,192,225,16,30,36,2,163,16,114,134,168,89,24,173,49,192,2,212,144,90,137,88,
		171,85,150,221,111,43,43,139,108,53,250,163,105,235,178,106,66,235,170,149,93,192,106,41,89,116,91,148,41,54,155,164,89,116,37,28,102,1,37,151,0,185,28,56,188,125,127,236,123,214,187,151,247,125,207,1,
		78,176,149,207,246,145,215,247,125,222,203,243,125,126,183,231,57,138,170,170,182,163,45,184,205,33,255,120,63,152,54,105,31,16,178,164,249,191,41,234,40,91,44,56,129,127,93,192,201,243,189,224,8,104,
		1,77,160,253,127,96,148,83,192,108,176,15,244,7,42,106,60,88,2,50,64,58,56,14,12,129,46,48,200,251,163,64,52,136,209,221,123,24,236,7,187,65,53,216,14,6,130,56,160,10,112,142,238,92,40,56,134,199,202,
		4,136,186,156,198,35,227,255,208,74,212,48,118,22,33,251,192,30,176,30,52,104,103,195,160,133,131,4,48,11,156,194,73,56,27,228,129,123,65,43,216,4,30,165,37,143,183,137,120,83,131,240,156,52,112,136,70,
		48,22,75,13,167,54,166,238,127,6,95,242,41,40,215,156,63,22,100,129,57,32,153,226,137,213,70,114,18,108,180,94,17,93,130,228,65,240,49,5,148,76,184,20,92,14,238,0,215,131,27,193,171,147,232,182,118,112,
		62,88,5,114,193,169,99,20,181,138,222,124,80,123,82,233,188,238,164,225,3,87,81,207,12,249,219,243,182,171,1,199,226,58,153,224,34,144,74,97,190,7,191,131,78,240,23,195,64,56,227,170,204,152,100,187,68,
		90,234,108,141,216,34,116,13,120,7,116,131,187,65,138,8,141,119,109,139,122,174,97,172,194,188,11,206,179,184,110,228,254,18,170,86,130,219,192,76,205,121,17,245,155,127,35,81,53,98,144,94,8,186,140,22,
		249,35,120,16,231,90,181,55,224,186,147,150,235,102,44,113,241,82,15,147,85,11,103,125,14,221,127,17,88,204,48,32,137,108,3,216,197,193,109,155,32,203,148,48,113,31,184,150,223,61,49,217,191,183,42,162,
		0,130,73,70,175,134,144,91,40,160,27,44,213,184,254,201,20,180,143,22,104,231,71,70,233,38,104,128,46,177,151,86,90,6,196,37,110,167,149,252,0,222,152,64,119,159,11,86,79,120,73,165,246,43,101,61,91,93,
		249,174,194,158,67,16,242,2,77,102,253,10,84,130,39,37,193,64,240,17,171,5,134,139,56,112,60,221,127,30,56,29,228,131,11,217,237,55,240,2,67,201,157,20,217,95,83,24,82,146,25,78,188,244,130,22,62,39,88,
		205,202,122,67,248,173,241,244,74,9,123,29,224,79,38,184,33,115,247,87,241,177,170,173,182,191,38,162,34,44,221,83,174,56,189,183,250,4,132,104,17,156,237,60,28,39,153,36,42,159,251,55,243,101,27,105,
		153,177,20,246,18,240,0,99,242,211,224,52,63,181,112,49,184,146,147,53,150,246,139,230,216,233,167,239,102,122,159,228,140,5,60,151,3,238,98,34,115,153,220,39,57,226,59,222,255,184,81,76,61,32,55,123,
		91,148,107,122,171,194,11,96,35,101,206,172,65,175,61,222,147,75,247,174,167,88,82,151,125,173,201,162,110,38,170,4,90,170,148,98,73,154,231,182,179,154,40,225,4,220,67,214,154,124,168,132,154,183,152,
		248,140,90,43,159,57,77,83,147,26,181,25,163,152,128,4,157,30,226,165,239,115,124,254,202,186,51,169,139,161,168,185,28,76,229,112,64,87,109,235,250,106,28,176,84,71,173,166,120,151,154,245,15,127,95,
		8,107,142,162,251,231,168,30,123,190,18,170,22,218,20,181,136,174,187,145,3,206,54,184,85,220,236,61,19,119,244,128,171,88,234,137,7,189,6,46,181,248,12,237,98,36,131,229,143,89,203,166,197,249,92,249,
		150,0,4,149,111,121,147,11,14,213,44,251,75,70,190,129,110,227,115,211,133,204,220,197,196,183,82,106,160,155,55,49,190,120,232,250,195,73,11,97,67,220,101,58,195,67,189,61,70,45,117,230,244,38,178,46,
		44,102,2,219,105,240,161,107,45,226,91,41,120,93,87,120,91,53,237,50,185,203,79,223,78,93,255,193,0,172,187,134,213,140,101,73,149,194,65,73,118,126,138,22,41,150,187,2,252,204,122,85,220,250,68,186,158,
		157,214,16,67,97,37,166,182,177,158,109,228,18,85,254,111,27,106,83,164,254,117,219,66,148,206,176,249,131,243,28,211,251,175,230,117,253,138,172,208,98,16,47,5,80,135,6,171,149,114,193,18,98,209,103,
		11,141,174,197,74,212,78,174,118,74,104,5,82,71,222,76,36,136,127,73,11,19,55,249,12,252,74,225,187,13,214,223,83,153,181,37,164,204,103,204,106,178,121,213,77,158,61,33,207,216,35,195,54,219,221,158,
		18,221,125,146,225,35,76,6,48,196,247,78,84,147,241,221,4,158,181,152,188,185,236,183,144,9,206,80,212,47,152,209,159,0,223,130,71,64,29,151,173,62,46,99,102,214,183,94,10,31,205,240,80,199,90,116,63,
		45,172,94,211,119,65,223,118,71,148,171,200,179,70,247,140,142,73,92,182,70,25,156,123,158,97,109,131,133,197,166,50,9,103,179,196,26,33,106,1,216,202,115,43,89,246,12,112,54,170,249,240,125,12,202,241,
		236,231,214,148,22,71,248,224,126,131,58,47,141,43,171,48,46,87,119,32,28,68,234,250,181,112,198,19,76,214,234,169,227,176,86,127,49,210,44,41,189,76,75,125,209,226,222,20,102,254,21,70,162,174,103,44,
		217,75,211,143,164,208,121,76,86,54,22,223,7,52,137,170,153,113,115,144,201,96,10,133,139,101,121,21,203,18,104,7,55,80,124,59,210,153,180,94,125,160,151,149,215,58,147,143,191,2,172,25,163,168,63,249,
		185,158,106,146,56,125,177,252,33,139,18,207,198,69,142,161,251,151,113,86,30,6,175,80,184,10,38,175,46,174,146,18,153,213,227,25,255,210,56,203,237,116,255,195,92,158,126,206,16,210,160,41,83,196,98,
		207,165,37,86,153,148,102,143,113,147,36,203,224,218,42,134,168,202,49,136,218,198,93,179,197,38,215,239,7,159,208,96,244,45,51,128,5,72,163,225,46,149,174,16,94,205,173,186,56,77,121,178,147,117,106,
		29,203,174,102,10,51,100,240,146,8,90,234,76,238,0,205,226,51,202,233,9,255,4,179,145,187,84,110,134,154,229,38,3,104,226,46,89,146,69,98,51,170,14,146,88,107,39,153,244,31,224,162,166,67,19,18,226,184,
		227,102,85,5,136,247,46,211,110,14,25,137,170,141,133,25,204,224,233,180,212,100,221,11,188,172,26,6,232,202,209,60,238,224,86,90,45,39,99,23,251,142,204,16,230,91,127,139,88,55,159,21,224,82,181,71,23,
		207,99,76,150,191,18,66,46,102,105,56,158,38,158,185,155,225,234,35,127,150,234,47,160,199,115,144,161,20,209,151,168,186,233,254,163,250,37,47,192,253,212,48,198,120,159,69,117,7,41,227,59,12,118,216,
		252,181,126,78,96,123,32,251,169,129,180,33,186,95,48,126,14,25,77,243,144,96,182,46,131,149,87,80,154,114,244,119,255,224,183,191,5,24,0,17,129,202,113,100,235,237,22,0,0,0,0,73,69,78,68,174,66,96,130,
		0,0 };

    // ********************
    // Run tests
    // ********************
    
    // Run IAB user data element setters and getters API tests
    TEST_F(IABUserData_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serialize IAB user data, then deSerialize IAB user data tests
    TEST_F(IABUserData_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
}
