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

//
//  CRC16Generator_Test.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/hash/CRC16.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

#define TEST_STR "123"
#define CRC_VAL 0x865e

class CRC16GeneratorTest : public testing::Test {
    
protected:
    
    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
    }
    
    virtual void TearDown()
    {
    }
    
    void TestWrite()
    {
        std::ofstream outputStream("testCRC16.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            CRC16Generator *crc = new CRC16Generator();
            
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            bsw->setCRCGenerator(crc);
            
            crc->start();
            
            std::string test1 = TEST_STR;
            uint64_t count = 0;
            uint64_t len = test1.length();
            
            while (count < len)
            {
                uint8_t testChar = test1[count];
                bsw->write(testChar, 8);
                
                count++;
            }
            
            crc->stop();
            
            uint32_t crcRegSize = crc->getCRCRegisterSize();
            uint8_t *crcReg = new uint8_t[crcRegSize];
            
            bool success = crc->getCRCRegister(crcReg);
            
            ASSERT_TRUE(success);
            
            uint16_t crc_val = CRC_VAL;
            
            int32_t result = memcmp(&crc_val, crcReg, crcRegSize);
            ASSERT_TRUE(result == 0);
            
            bsw->setCRCGenerator(nullptr);
            
            delete bsw;
            delete crc;
            delete [] crcReg;
            
            outputStream.flush();
            outputStream.close();
        }
    }
    
    void TestRead()
    {
        std::ifstream inputStream("testCRC16.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            CRC16Generator *crc = new CRC16Generator();
            
            BitStreamReaderT<std::ifstream> *bsr = new BitStreamReaderT<std::ifstream>(inputStream);
            
            bsr->setCRCGenerator(crc);
            
            crc->start();
            
            std::string testStr = TEST_STR;
            std::string stringRead;
            
            uint64_t count = 0;
            uint64_t len = testStr.length();
            
            while (count < len)
            {
                uint8_t testChar;
                bsr->read(testChar);
                stringRead += testChar;
                count++;
            }
            
            ASSERT_TRUE(testStr == stringRead);
            
            crc->stop();
            
            uint32_t crcRegSize = crc->getCRCRegisterSize();
            uint8_t *crcReg = new uint8_t[crcRegSize];
            
            bool success = crc->getCRCRegister(crcReg);
            
            ASSERT_TRUE(success);
            
            uint16_t crc_val = CRC_VAL;
            
            int32_t result = memcmp(&crc_val, crcReg, crcRegSize);
            ASSERT_TRUE(result == 0);
            
            bsr->setCRCGenerator(nullptr);
            
            delete bsr;
            delete crc;
            delete [] crcReg;
            
            inputStream.close();
        }
    }
    
    void TestStateChanges()
    {
        std::ofstream outputStream("testmd5_state.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            CRC16Generator *crc = new CRC16Generator();
            
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            bsw->setCRCGenerator(crc);
            
            // Test initial state
            //
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Stopped);
            
            // Test all valid state transitions
            //
            // 1) Stopped -> Stopped
            ASSERT_EQ(crc->stop(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Stopped);
            
            // 2) Stopped -> Started
            ASSERT_EQ(crc->start(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Started);
            
            // 3) Started -> Started
            ASSERT_EQ(crc->start(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Started);
            
            // 4) Started -> Paused
            ASSERT_EQ(crc->pause(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Paused);
            
            // 7) Paused -> Started
            ASSERT_EQ(crc->start(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Started);
            
            // 5) Started -> Stopped
            ASSERT_EQ(crc->stop(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Stopped);
            
            // Reset to test more cases
            // 2) Stopped -> Started
            ASSERT_EQ(crc->start(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Started);
            
            // 6) Paused -> Paused
            ASSERT_EQ(crc->pause(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Paused);
            
            // 8) Paused -> Stopped
            ASSERT_EQ(crc->stop(), true);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Stopped);
            
            // Test invalid state transitions
            //
            
            // Stopped -> Paused
            ASSERT_EQ(crc->pause(), false);
            ASSERT_EQ(crc->getState(), CRCGenerator::eCRCState_Stopped);
            
            bsw->setCRCGenerator(nullptr);
            
            delete bsw;
            delete crc;
            
            outputStream.flush();
            outputStream.close();
        }
    }
};

TEST_F(CRC16GeneratorTest, CRC16GeneratorTest_Case1)
{
    TestWrite();
    TestRead();
    TestStateChanges();
}
