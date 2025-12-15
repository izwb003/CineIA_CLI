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
#include "common/IABConstants.h"
#include <vector>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    
    // IABObjectSubBlock tests:
    // 1. Test setters and getter APIs
    // 2. Test Serialize() into a stream (packed buffer)
    // 3. Test DeSerialize() from the stream (packed buffer)
    // 4. Verify de-serialized object subBlock contents
    
    class IABObjectSubBlock_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            // These members will be changed per test case
            panInfoExists_ = 1;
            objectGain_.setIABGain(0.5f);
            objectPosition_.setIABObjectPosition(0.1f, 0.2f, 0.3f);
            
            // objectSnapPresent_ = 1, objectSnapTolExists_ = 1, objectSnapTolerance_ is unsigned 12-bit, set to maximum value
            objectSnap_.objectSnapPresent_ = 1;
            objectSnap_.objectSnapTolExists_ = 0;
            objectSnap_.objectSnapTolerance_ =  0xFFF;

            zoneGain_.objectZoneControl_ = 1;
            
            // Set prefix to gain in stream. Set gain to different values
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                zoneGain_.zoneGains_[i].setIABZoneGain(static_cast<float>(0.1f * i));
            }

            objectSpread_.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.1f, 0.2f, 0.3f);
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_DecorCoeffInStream;
            objectDecorCoef_.decorCoef_ = 0x40;     // A random non-zero test value

        }
        
        // **********************************************
        
        // Setters and getters API tests
        
        // **********************************************
        
        
        void TestSetterGetterAPIs()
        {
            IABObjectSubBlockInterface* objectSubBlockInterface;
            objectSubBlockInterface = IABObjectSubBlockInterface::Create();
            ASSERT_TRUE(NULL != objectSubBlockInterface);
            
            // Test pan info exists
            uint1_t panInfoExists;
            objectSubBlockInterface->GetPanInfoExists(panInfoExists);
            EXPECT_EQ(panInfoExists, 0);
            
            EXPECT_EQ(objectSubBlockInterface->SetPanInfoExists(1), kIABNoError);
            objectSubBlockInterface->GetPanInfoExists(panInfoExists);
            EXPECT_EQ(panInfoExists, 1);
            
            // Test object gain, gain range [0.0f, 1.0f] map to [0x3FF, 0]
            IABGain objectGain, objectGainGet;
            
            // Default
            objectSubBlockInterface->GetObjectGain(objectGainGet);
            EXPECT_EQ(objectGainGet.getIABGain(), 1.0f);
            EXPECT_EQ(objectGainGet.getIABGainPrefix(), kIABGainPrefix_Unity);
            EXPECT_EQ(objectGainGet.getIABGainInStreamValue(), 0);
            
            objectGain.setIABGain(0.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectGain(objectGain), kIABNoError);
            objectSubBlockInterface->GetObjectGain(objectGainGet);
            EXPECT_EQ(objectGainGet.getIABGain(), 0.0f);
            EXPECT_EQ(objectGainGet.getIABGainPrefix(), kIABGainPrefix_Silence);
            EXPECT_EQ(objectGainGet.getIABGainInStreamValue(), 0x3FF);
            
            objectGain.setIABGain(1.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectGain(objectGain), kIABNoError);
            objectSubBlockInterface->GetObjectGain(objectGainGet);
            EXPECT_EQ(objectGainGet.getIABGain(), 1.0f);
            EXPECT_EQ(objectGainGet.getIABGainPrefix(), kIABGainPrefix_Unity);
            EXPECT_EQ(objectGainGet.getIABGainInStreamValue(), 0);
            
            objectGain.setIABGain(0.5f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectGain(objectGain), kIABNoError);
            objectSubBlockInterface->GetObjectGain(objectGainGet);
            EXPECT_EQ(objectGainGet.getIABGain(), 0.5f);
            EXPECT_EQ(objectGainGet.getIABGainPrefix(), kIABGainPrefix_InStream);
            EXPECT_EQ(objectGainGet.getIABGainInStreamValue(), 64); // gain in stream code for 0.5f
            
            EXPECT_EQ(objectGain.setIABGain(1.1f), kIABBadArgumentsError);
            EXPECT_EQ(objectGain.setIABGain(-0.1f), kIABBadArgumentsError);
            
            // Test object position
            CartesianPosInUnitCube objectPositionUC;
            float posX, posY, posZ;
            
            // Invalid range, < 0.0f
            EXPECT_EQ(objectPositionUC.setIABObjectPosition(-0.1f, -0.1f, -0.1f), kIABBadArgumentsError);
            
            // Invalid range, > 1.0f
            EXPECT_EQ(objectPositionUC.setIABObjectPosition(1.1f, 1.1f, 1.1f), kIABBadArgumentsError);
            
            // Valid range check, different value
            EXPECT_EQ(objectPositionUC.setIABObjectPosition(0.1f, 0.2f, 0.3f), kIABNoError);
            EXPECT_EQ(objectSubBlockInterface->SetObjectPositionFromUnitCube(objectPositionUC), kIABNoError);
            objectSubBlockInterface->GetObjectPositionToUnitCube(objectPositionUC);
            objectPositionUC.getIABObjectPosition(posX, posY, posZ);
            EXPECT_EQ(posX, 0.1f);
            EXPECT_EQ(posY, 0.2f);
            EXPECT_EQ(posZ, 0.3f);
            
            EXPECT_EQ(objectPositionUC.getStreamPosX(), 36044);  // code value for 0.1f
            EXPECT_EQ(objectPositionUC.getStreamPosY(), 39321);  // code value for 0.2f
            EXPECT_EQ(objectPositionUC.getStreamPosZ(), 19661);  // code value for 0.3f
            
            EXPECT_EQ(objectPositionUC.setIABObjectPosition(1.0f, 1.0f, 1.0f), kIABNoError);
            EXPECT_EQ(objectSubBlockInterface->SetObjectPositionFromUnitCube(objectPositionUC), kIABNoError);
            objectSubBlockInterface->GetObjectPositionToUnitCube(objectPositionUC);
            objectPositionUC.getIABObjectPosition(posX, posY, posZ);
            EXPECT_EQ(objectPositionUC.getStreamPosX(), 65535);
            EXPECT_EQ(objectPositionUC.getStreamPosY(), 65535);
            EXPECT_EQ(objectPositionUC.getStreamPosZ(), 65535);
            
            // Test object snap
            IABObjectSnap objectSnap;
            objectSubBlockInterface->GetObjectSnap(objectSnap);
            EXPECT_EQ(objectSnap.objectSnapPresent_, 0);
            EXPECT_EQ(objectSnap.objectSnapTolExists_, 0);
            EXPECT_EQ(objectSnap.objectSnapTolerance_, 0);
            EXPECT_EQ(objectSnap.reservedBit_, 0);
            
            // objectSnapPresent_ = 1, objectSnapTolExists_ = 0, expect objectSnapTolerance_ to reset to IAB DEFAULT_OBJ_SNAP_TOL, which is zero
            // reserved bit should always be zero
            objectSnap.objectSnapPresent_ = 1;
            objectSnap.objectSnapTolExists_ = 0;
            objectSnap.objectSnapTolerance_ =  1;
            objectSnap.reservedBit_ = 1;
            EXPECT_EQ(objectSubBlockInterface->SetObjectSnap(objectSnap), kIABNoError);
            objectSubBlockInterface->GetObjectSnap(objectSnap);
            EXPECT_EQ(objectSnap.objectSnapPresent_, 1);
            EXPECT_EQ(objectSnap.objectSnapTolExists_, 0);
            EXPECT_EQ(objectSnap.objectSnapTolerance_, 0);
            EXPECT_EQ(objectSnap.reservedBit_, 0);
            
            // objectSnapPresent_ = 1, objectSnapTolExists_ = 1, objectSnapTolerance_ is unsigned 12-bit, set to maximum value
            // reserved bit should always be zero
            objectSnap.objectSnapPresent_ = 1;
            objectSnap.objectSnapTolExists_ = 1;
            objectSnap.objectSnapTolerance_ =  0xFFF;
            objectSnap.reservedBit_ = 1;
            EXPECT_EQ(objectSubBlockInterface->SetObjectSnap(objectSnap), kIABNoError);
            objectSubBlockInterface->GetObjectSnap(objectSnap);
            EXPECT_EQ(objectSnap.objectSnapPresent_, 1);
            EXPECT_EQ(objectSnap.objectSnapTolExists_, 1);
            EXPECT_EQ(objectSnap.objectSnapTolerance_, 0xFFF);
            EXPECT_EQ(objectSnap.reservedBit_, 0);
            
            // objectSnapPresent_ = 0, objectSnapTolExists_ and objectSnapTolerance_ not zero (objectSnapTolExists_ = 1, objectSnapTolerance_ = maximum value)
            // Expect objectSnapTolExists_ and objectSnapTolerance_ to remain at 0
            //
            objectSnap.objectSnapPresent_ = 0;
            objectSnap.objectSnapTolExists_ = 1;
            objectSnap.objectSnapTolerance_ =  0xFFF;
            objectSnap.reservedBit_ = 1;
            EXPECT_EQ(objectSubBlockInterface->SetObjectSnap(objectSnap), kIABNoError);
            objectSubBlockInterface->GetObjectSnap(objectSnap);
            EXPECT_EQ(objectSnap.objectSnapPresent_, 0);
            EXPECT_EQ(objectSnap.objectSnapTolExists_, 0);
            EXPECT_EQ(objectSnap.objectSnapTolerance_, 0);
            EXPECT_EQ(objectSnap.reservedBit_, 0);
            
            // Test object 9-zone gains
            IABObjectZoneGain9 objectZoneGains9, objectZoneGains9Get;
            float zoneGain;
            
            // Default
            objectSubBlockInterface->GetObjectZoneGains9(objectZoneGains9Get);
            EXPECT_EQ(objectZoneGains9Get.objectZoneControl_, 0);
            
            // Set prefix to unity gain
            objectZoneGains9.objectZoneControl_ = 1;
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                objectZoneGains9.zoneGains_[i].setIABZoneGain(1.0f);
            }
            
            EXPECT_EQ(objectSubBlockInterface->SetObjectZoneGains9(objectZoneGains9), kIABNoError);
            objectSubBlockInterface->GetObjectZoneGains9(objectZoneGains9Get);
            EXPECT_EQ(objectZoneGains9Get.objectZoneControl_, 1);
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                zoneGain = objectZoneGains9Get.zoneGains_[i].getIABZoneGain();
                EXPECT_EQ(zoneGain, 1.0f);
                EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Unity);
                EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainInStreamValue(), 0x3FF);
                
            }
            
            // Set prefix to zero gain
            objectZoneGains9.objectZoneControl_ = 1;
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                objectZoneGains9.zoneGains_[i].setIABZoneGain(0.0f);
            }
            
            EXPECT_EQ(objectSubBlockInterface->SetObjectZoneGains9(objectZoneGains9), kIABNoError);
            objectSubBlockInterface->GetObjectZoneGains9(objectZoneGains9Get);
            EXPECT_EQ(objectZoneGains9Get.objectZoneControl_, 1);
            for (uint32_t i = 0; i < 9; i++)
            {
                zoneGain = objectZoneGains9Get.zoneGains_[i].getIABZoneGain();
                EXPECT_EQ(zoneGain, 0.0f);
                EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Silence);
                EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainInStreamValue(), 0);
            }
            
            // Set prefix to gain in stream. Set gain to different values
            objectZoneGains9.objectZoneControl_ = 1;
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                objectZoneGains9.zoneGains_[i].setIABZoneGain(static_cast<float>(0.1f * i));
            }
            
            EXPECT_EQ(objectSubBlockInterface->SetObjectZoneGains9(objectZoneGains9), kIABNoError);
            objectSubBlockInterface->GetObjectZoneGains9(objectZoneGains9Get);
            EXPECT_EQ(objectZoneGains9Get.objectZoneControl_, 1);
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
                zoneGain = objectZoneGains9Get.zoneGains_[i].getIABZoneGain();
				EXPECT_LT(std::fabs(zoneGain - 0.1f * i), 0.001f);									// Quatization to be less than 0.2%
                if (i == 0)
                {
                    EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_Silence);
                    EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainInStreamValue(), 0);
                }
                else
                {
                    EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainPrefix(), kIABZoneGainPrefix_InStream);
                    EXPECT_EQ(objectZoneGains9Get.zoneGains_[i].getIABZoneGainInStreamValue(), static_cast<uint16_t>(floor(zoneGain * 1023.0f + 0.5f)));
                }
            }
            
            IABObjectZoneGain iabZoneGain;
            EXPECT_EQ(iabZoneGain.setIABZoneGain(1.1f), kIABBadArgumentsError);
            EXPECT_EQ(iabZoneGain.setIABZoneGain(-0.1f), kIABBadArgumentsError);
            
            // Test object spread
            IABObjectSpread objectSpread, objectSpreadGet;
            float spreadXYZ, spreadY, spreadZ;
            
            // below bottom limit (negative value)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, -0.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            // exceed top limit (> 1.0f)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, 1.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            // below bottom limit (negative value)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, -0.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            // exceed top limit (> 1.0f)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 1.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            // below bottom limit (negative value)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, -0.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.0f, -0.1f, 0.0f), kIABBadArgumentsError);
            
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.0f, 0.0f, -0.1f), kIABBadArgumentsError);
            
            // exceed top limit (> 1.0f)
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 1.1f, 0.0f, 0.0f), kIABBadArgumentsError);
            
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.0f, 1.1f, 0.0f), kIABBadArgumentsError);
            EXPECT_EQ(objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.0f, 0.0f, 1.1f), kIABBadArgumentsError);
            
            // Low res, spread @ unsigned 8-bit maximum value (0xFF)
            objectSpread.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, 1.0f, 1.0f, 1.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectSpread(objectSpread), kIABNoError);
            objectSubBlockInterface->GetObjectSpread(objectSpreadGet);
            objectSpreadGet.getIABObjectSpread(spreadXYZ, spreadY, spreadZ);
            EXPECT_EQ(spreadXYZ, 1.0f);
            EXPECT_EQ(spreadY, 1.0f);
            EXPECT_EQ(spreadZ, 1.0f);
            EXPECT_EQ(objectSpreadGet.getIABObjectSpreadMode(), kIABSpreadMode_LowResolution_1D);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadXYZ(), 255);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadY(), 255);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadZ(), 255);
            
            // Hi res 1D, spread @ unsigned 12-bit minimum value
            objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 0.0f, 0.0f, 0.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectSpread(objectSpread), kIABNoError);
            objectSubBlockInterface->GetObjectSpread(objectSpreadGet);
            objectSpreadGet.getIABObjectSpread(spreadXYZ, spreadY, spreadZ);
            EXPECT_EQ(spreadXYZ, 0.0f);
            EXPECT_EQ(spreadY, 0.0f);
            EXPECT_EQ(spreadZ, 0.0f);
            EXPECT_EQ(objectSpreadGet.getIABObjectSpreadMode(), kIABSpreadMode_HighResolution_1D);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadXYZ(), 0);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadY(), 0);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadZ(), 0);
            
            // Hi res 1D, spread @ unsigned 12-bit maximum value
            objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 1.0f, 1.0f, 1.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectSpread(objectSpread), kIABNoError);
            objectSubBlockInterface->GetObjectSpread(objectSpreadGet);
            objectSpreadGet.getIABObjectSpread(spreadXYZ, spreadY, spreadZ);
            EXPECT_EQ(spreadXYZ, 1.0f);
            EXPECT_EQ(spreadY, 1.0f);
            EXPECT_EQ(spreadZ, 1.0f);
            EXPECT_EQ(objectSpreadGet.getIABObjectSpreadMode(), kIABSpreadMode_HighResolution_1D);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadXYZ(), 4095);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadY(), 4095);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadZ(), 4095);
            
            // Hi res 1D, spread @ unsigned different values
            objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.0f, 0.5f, 1.0f);
            EXPECT_EQ(objectSubBlockInterface->SetObjectSpread(objectSpread), kIABNoError);
           objectSubBlockInterface->GetObjectSpread(objectSpreadGet);
            objectSpreadGet.getIABObjectSpread(spreadXYZ, spreadY, spreadZ);
            EXPECT_EQ(spreadXYZ, 0.0f);
            EXPECT_EQ(spreadY, 0.5f);
            EXPECT_EQ(spreadZ, 1.0f);
            EXPECT_EQ(objectSpreadGet.getIABObjectSpreadMode(), kIABSpreadMode_HighResolution_3D);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadXYZ(), 0);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadY(), 2048);
            EXPECT_EQ(objectSpreadGet.getStreamSpreadZ(), 4095);
            
            // Test object decor coef
            IABDecorCoeff decorCoef, decorCoefGet;
            
            // Default
            objectSubBlockInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_NoDecor);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0);
            
            // Set to maximum decor
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            decorCoef.decorCoef_ = 0;    // value not relevant/used in this prefix
            EXPECT_EQ(objectSubBlockInterface->SetDecorCoef(decorCoef), kIABNoError);
            objectSubBlockInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_MaxDecor);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0);
            
            // Set to coeff in stream. Coefficient is 8-bit unsigned. Set to maximum value.
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_DecorCoeffInStream;
            decorCoef.decorCoef_ = 0xFF;
            EXPECT_EQ(objectSubBlockInterface->SetDecorCoef(decorCoef), kIABNoError);
            objectSubBlockInterface->GetDecorCoef(decorCoefGet);
            EXPECT_EQ(decorCoefGet.decorCoefPrefix_, kIABDecorCoeffPrefix_DecorCoeffInStream);
            EXPECT_EQ(decorCoefGet.decorCoef_, 0xFF);
            
            IABObjectSubBlockInterface::Delete(objectSubBlockInterface);
        }
        
        // **********************************************
        
        // Functions to set up for Serialize tests
        
        // **********************************************

        void SetupPackerObjectSubBlock()
        {
            ASSERT_EQ(iabPackerObjectSubBlock_->SetPanInfoExists(panInfoExists_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetObjectGain(objectGain_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetObjectPositionFromUnitCube(objectPosition_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetObjectSnap(objectSnap_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetObjectZoneGains9(zoneGain_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetObjectSpread(objectSpread_), kIABNoError);
            ASSERT_EQ(iabPackerObjectSubBlock_->SetDecorCoef(objectDecorCoef_), kIABNoError);
            
        }

        void SetZoneGain(uint8_t iObjectZoneControl_, IABZoneGainPrefixType iGainPrefix)
        {
            zoneGain_.objectZoneControl_ = iObjectZoneControl_;
            
            if (1 == iObjectZoneControl_)
            {
                // Set prefix to gain in stream. Set gain to different values
                for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
                {
                    if (kIABZoneGainPrefix_Silence == iGainPrefix)
                    {
                        ASSERT_EQ(zoneGain_.zoneGains_[i].setIABZoneGain(static_cast<float>(0.0f)), kIABNoError);
                    }
                    else if (kIABZoneGainPrefix_Unity == iGainPrefix)
                    {
                        ASSERT_EQ(zoneGain_.zoneGains_[i].setIABZoneGain(static_cast<float>(1.0f)), kIABNoError);
                    }
                    else
                    {
                        ASSERT_EQ(zoneGain_.zoneGains_[i].setIABZoneGain(static_cast<float>(0.1f * i)), kIABNoError);
                    }
                        
                }
            }
        }

        // **********************************************
        
        // Functions for DeSerialize tests
        
        // **********************************************

        void VerifyDeSerializedObjectSubBlock()
        {
            uint1_t panInfoExists;
            IABGain objectGain;
            CartesianPosInUnitCube objectPosition;
            IABObjectSnap objectSnap;
            IABObjectZoneGain9 zoneGain;
            IABObjectSpread objectSpread;
            IABDecorCoeff objectDecorCoef;
            
            iabParserObjectSubBlock_->GetPanInfoExists(panInfoExists);
            
            ASSERT_EQ(panInfoExists, panInfoExists_);
            
            if (panInfoExists_)
            {
                iabParserObjectSubBlock_->GetObjectGain(objectGain);
                iabParserObjectSubBlock_->GetObjectPositionToUnitCube(objectPosition);
                iabParserObjectSubBlock_->GetObjectSnap(objectSnap);
                iabParserObjectSubBlock_->GetObjectZoneGains9(zoneGain);
                iabParserObjectSubBlock_->GetObjectSpread(objectSpread);
                iabParserObjectSubBlock_->GetDecorCoef(objectDecorCoef);
                
                EXPECT_EQ((objectGain == objectGain_), true);
                EXPECT_EQ((objectPosition == objectPosition_), true);
                EXPECT_EQ((objectSpread == objectSpread_), true);
                
                EXPECT_EQ(objectSnap.objectSnapPresent_, objectSnap_.objectSnapPresent_);
                
                if (1 == objectSnap.objectSnapPresent_)
                {
                    EXPECT_EQ(objectSnap.objectSnapTolExists_, objectSnap_.objectSnapTolExists_);
                    
                    if (1 == objectSnap.objectSnapTolExists_)
                    {
                        EXPECT_EQ(objectSnap.objectSnapTolerance_, objectSnap_.objectSnapTolerance_);
                    }
                }
                
                ASSERT_EQ(zoneGain.objectZoneControl_, zoneGain_.objectZoneControl_);
                
                if (1 == zoneGain.objectZoneControl_)
                {
                    for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
                    {
                        EXPECT_EQ((zoneGain.zoneGains_[i] == zoneGain_.zoneGains_[i]), true);
                    }
                }
            }
        }
        
        // **********************************************
        
        // Function to test Serialize() and DeSerialize()
        
        // **********************************************

        void TestSerializeDeSerialize()
        {

            // Test case: panInfoExists_ = 0
            panInfoExists_ = 0;
            RunSerializeDeSerializeTestCase();
            
            // Set panInfoExists_ = 1 for remaining tests
            
            panInfoExists_ = 1;
            
            objectPosition_.setIABObjectPosition(0.0f, 0.0f, 0.0f);

            // objectSnapPresent_ = 0
            objectSnap_.objectSnapPresent_ = 0;
            objectSnap_.objectSnapTolExists_ = 0;
            objectSnap_.objectSnapTolerance_ =  0;

            zoneGain_.objectZoneControl_ = 0;

            ASSERT_EQ(objectSpread_.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, 0.0f, 0.0f, 0.0f), kIABNoError);
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
            objectDecorCoef_.decorCoef_ = 0;     // A random test value

            // Test case: object gain = silence
            ASSERT_EQ(objectGain_.setIABGain(0.0f), kIABNoError);
            RunSerializeDeSerializeTestCase();
            
            // Test case: object gain = 0.1
            ASSERT_EQ(objectGain_.setIABGain(0.1f), kIABNoError);
            RunSerializeDeSerializeTestCase();
            
            // Test case: object gain = 0.5
            ASSERT_EQ(objectGain_.setIABGain(0.5f), kIABNoError);
            RunSerializeDeSerializeTestCase();
            
            // Test case: object gain = unity
            ASSERT_EQ(objectGain_.setIABGain(1.0f), kIABNoError);
            RunSerializeDeSerializeTestCase();
            
            // Test object position [0,0,0]
            ASSERT_EQ(objectPosition_.setIABObjectPosition(0.0f, 0.0f, 0.0f), kIABNoError);
            RunSerializeDeSerializeTestCase();
           
            // Test object position [0,0,0]
            ASSERT_EQ(objectPosition_.setIABObjectPosition(0.1f, 0.5f, 1.0f), kIABNoError);     // non zero position, different values
            RunSerializeDeSerializeTestCase();

            // Test object position [1,1,1]
            ASSERT_EQ(objectPosition_.setIABObjectPosition(1.0f, 1.0f, 1.0f), kIABNoError);     // At max position
            RunSerializeDeSerializeTestCase();
            
            // Test case: snap present, snap tolerance does not exist
            objectSnap_.objectSnapPresent_ = 1;
            objectSnap_.objectSnapTolExists_ = 0;
            objectSnap_.objectSnapTolerance_ =  0;
            RunSerializeDeSerializeTestCase();

            // Test case: snap present, snap tolerance exists, tolerance = at minimum
            objectSnap_.objectSnapPresent_ = 1;
            objectSnap_.objectSnapTolExists_ = 1;
            objectSnap_.objectSnapTolerance_ =  0;
            RunSerializeDeSerializeTestCase();

            // Test case: snap present, snap tolerance exists, tolerance = at maximum (unsigned 12-bit = 0xFFF)
            objectSnap_.objectSnapPresent_ = 1;
            objectSnap_.objectSnapTolExists_ = 1;
            objectSnap_.objectSnapTolerance_ =  0xFFF;
            RunSerializeDeSerializeTestCase();

            // Test case: Low res 1-D mode, non-zero spread, note that in 1-D mode, 2nd and 3rd parameters are ignored
            objectSnap_.objectSnapPresent_ = 0;     // set snap present to 0
            ASSERT_EQ(objectSpread_.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, 0.5f, 0.2f, 0.3f), kIABNoError);
            RunSerializeDeSerializeTestCase();

            // Test case: High res 1-D mode, non-zero spread, note that in 1-D mode, 2nd and 3rd parameters are ignored
            ASSERT_EQ(objectSpread_.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 0.5f, 0.2f, 0.3f), kIABNoError);
            RunSerializeDeSerializeTestCase();

            // Test case: High res 3-D mode, non-zero spread
            ASSERT_EQ(objectSpread_.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, 0.5f, 0.2f, 0.3f), kIABNoError);
            RunSerializeDeSerializeTestCase();
            
            // Test case: decor prefix = maximum decor
            ASSERT_EQ(objectSpread_.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, 0.0f, 0.0f, 0.0f), kIABNoError);    // change spread mode to low res 1-D
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            RunSerializeDeSerializeTestCase();
            
            // Test case: decor prefix = decor coefficient in stream, coefficient = 0
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            objectDecorCoef_.decorCoef_ = 0;
            RunSerializeDeSerializeTestCase();

            // Test case: decor prefix = decor coefficient in stream, coefficient = 0x40, a random non-zero test value
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            objectDecorCoef_.decorCoef_ = 0x40;
            RunSerializeDeSerializeTestCase();

            // Test case: decor prefix = decor coefficient in stream, coefficient = coefficient = 0xFF (maximum value for 8-bit range)
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_MaxDecor;
            objectDecorCoef_.decorCoef_ = 0xFF;
            RunSerializeDeSerializeTestCase();
            
            // Test case: zoneGain control = 1, prefix = unity
            objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;   // change to no decor
            SetZoneGain(1, kIABZoneGainPrefix_Unity);
            RunSerializeDeSerializeTestCase();
            
            // Test case: zoneGain control = 1, prefix = silence
            SetZoneGain(1, kIABZoneGainPrefix_Silence);
            RunSerializeDeSerializeTestCase();
           
            // Test case: zoneGain control = 1, prefix = gain in stream, using fixed non-zero gains
            SetZoneGain(1, kIABZoneGainPrefix_InStream);
            RunSerializeDeSerializeTestCase();

        }
        
        void RunSerializeDeSerializeTestCase()
        {
            // Create IAB object subBlock (packer) to Serialize stream buffer
            iabPackerObjectSubBlock_ = new IABObjectSubBlock();
            ASSERT_TRUE(NULL != iabPackerObjectSubBlock_);
            
            // Create IAB object subBlock (parser) to de-serialize stream buffer
            iabParserObjectSubBlock_ = new IABObjectSubBlock();
            ASSERT_TRUE(NULL != iabParserObjectSubBlock_);

            // Set up IAB packer object subBlock
            SetupPackerObjectSubBlock();
            
            // stream to hold serialized bitstream
            std::stringstream  objectSubBlockStream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
            
            // stream writer for serialing
            StreamWriter objectSubBlockWriter(objectSubBlockStream);
            
            // Serialize object subBlock into stream
            ASSERT_EQ(iabPackerObjectSubBlock_->Serialize(objectSubBlockWriter), kIABNoError);
            
            // If panInfoExists_ = 0, there is only 1 bit to write and current streamwritter will only write a minimum of 8 bits to buffer.
            // Use align() to force a write so panInfoExists_ = 0 can be tested.
            // Align() would not affect the panInfoExists_ = 1 test case
            objectSubBlockWriter.align();
            
            // stream reader for de-serializing object subBlock from stream
            StreamReader objectSubBlockReader(objectSubBlockStream);
            
            // DeSerialize object subBlock from stream
            ASSERT_EQ(iabParserObjectSubBlock_->DeSerialize(objectSubBlockReader), kIABNoError);
            
            // Verify deserialized bed channel
            VerifyDeSerializedObjectSubBlock();
            
			delete iabPackerObjectSubBlock_;
			delete iabParserObjectSubBlock_;
        }

        
    private:
        
        IABObjectSubBlock*   iabPackerObjectSubBlock_;
        IABObjectSubBlock*   iabParserObjectSubBlock_;

        uint1_t panInfoExists_;
        IABGain objectGain_;
        CartesianPosInUnitCube objectPosition_;
        IABObjectSnap objectSnap_;
        IABObjectZoneGain9 zoneGain_;
        IABObjectSpread objectSpread_;
        IABDecorCoeff objectDecorCoef_;
        
    };
    
    // ********************
    // Run tests
    // ********************
    
    // Run IABChannel element setters and getters API tests
    TEST_F(IABObjectSubBlock_Test, Test_Setters_Getters_APIs)
    {
        TestSetterGetterAPIs();
    }
    
    // Run serial, then deSerialize tests
    TEST_F(IABObjectSubBlock_Test, Test_Serialize_DeSerialize)
    {
        TestSerializeDeSerialize();
    }
    
    
}
