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
//  MD5Generator_Test.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/hash/MD5Generator.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"
#include "commonstream/bytestream/ByteStreamReaderT.h"
#include "commonstream/bytestream/ByteStreamWriterT.h"
#include "commonstream/bytestream/ByteStreamIOHelpers.h"

using namespace CommonStream;

#define TEST_STR "123"
#define HASH_VAL "202cb962ac59075b964b07152d234b70"


class MD5GeneratorTest : public testing::Test {
    
protected:
    
    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
    }
    
    virtual void TearDown()
    {
    }
    
    template<typename T>
    void TestWrite()
    {
        std::ofstream outputStream("testmd5.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            MD5Generator *hash = new MD5Generator();
            
            T *bsw = new T(outputStream);
            bsw->setHashGenerator(hash);
            
            hash->start();
            
            std::string test1 = TEST_STR;
            uint64_t count = 0;
            uint64_t len = test1.length();
            
            while (count < len)
            {
                uint8_t testChar = test1[count];
                bsw->write(testChar);
                count++;
            }
            
            hash->stop();
            
            uint64_t hashSize = hash->getHashSize();
            uint8_t *hashVal = new uint8_t[hashSize];
            
            hash->getHash(hashVal);
            
            std::string hashTestResult;
            std::stringstream sstream;
            
            for (int32_t i = 0; i < hashSize; i++)
            {
                uint32_t testChar = hashVal[i];
                sstream << std::hex << std::setw(2) << std::setfill('0') << testChar;
            }
            
            hashTestResult = sstream.str();
            
            std::string hashTestVal = HASH_VAL;
            ASSERT_EQ(hashTestVal, hashTestResult);
            
            bsw->setHashGenerator(nullptr);
            
            delete bsw;
            delete hash;
            delete [] hashVal;
            
            outputStream.flush();
            outputStream.close();
        }
    }
    
    template<typename T>
    void TestRead()
    {
        std::ifstream inputStream("testmd5.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            MD5Generator *hash = new MD5Generator();
            
            T *bsr = new T(inputStream);
            
            bsr->setHashGenerator(hash);
            
            hash->start();
            
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
            
            ASSERT_EQ(testStr, stringRead);
            
            hash->stop();
            
            uint64_t hashSize = hash->getHashSize();
            uint8_t *hashVal = new uint8_t[hashSize];
            
            hash->getHash(hashVal);
            
            std::string hashTestResult;
            std::stringstream sstream;
            
            for (int32_t i = 0; i < hashSize; i++)
            {
                uint32_t testChar = hashVal[i];
                sstream << std::hex << std::setw(2) << std::setfill('0') << testChar;
            }
            
            hashTestResult = sstream.str();
            
            std::string hashTestVal = HASH_VAL;
            
            ASSERT_EQ(hashTestVal, hashTestResult);
            
            bsr->setHashGenerator(nullptr);
            
            delete bsr;
            delete hash;
            delete [] hashVal;
            
            inputStream.close();
        }
    }
    
    template<typename T>
    void TestStateChanges()
    {
        std::ofstream outputStream("testmd5_state.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            MD5Generator *hash = new MD5Generator();
            
            T *bsw = new T(outputStream);
            bsw->setHashGenerator(hash);
            
            // Test initial state
            // When we create a MD5Generator class
            // it automatically starts hashing
            //
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Stopped);
            
            // Test all valid state transitions
            //
            // 1) Stopped -> Stopped
            ASSERT_EQ(hash->stop(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Stopped);
            
            // 2) Stopped -> Started
            ASSERT_EQ(hash->start(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Started);
            
            // 3) Started -> Started
            ASSERT_EQ(hash->start(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Started);
            
            // 4) Started -> Paused
            ASSERT_EQ(hash->pause(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Paused);
            
            // 7) Paused -> Started
            ASSERT_EQ(hash->start(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Started);
            
            // 5) Started -> Stopped
            ASSERT_EQ(hash->stop(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Stopped);
            
            // Reset to test more cases
            // 2) Stopped -> Started
            ASSERT_EQ(hash->start(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Started);
            
            // 6) Paused -> Paused
            ASSERT_EQ(hash->pause(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Paused);
            
            // 8) Paused -> Stopped
            ASSERT_EQ(hash->stop(), true);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Stopped);
            
            // Test invalid state transitions
            //
            
            // Stopped -> Paused
            ASSERT_EQ(hash->pause(), false);
            ASSERT_EQ(hash->getState(), HashGenerator::eHashState_Stopped);
            
            bsw->setHashGenerator(nullptr);
            
            delete bsw;
            delete hash;
            
            outputStream.flush();
            outputStream.close();
        }
    }
};

TEST_F(MD5GeneratorTest, MD5GeneratorTest_Case1)
{
    TestWrite<BitStreamWriterT<std::ofstream> >();
    TestRead<BitStreamReaderT<std::ifstream> >();
    TestStateChanges<BitStreamWriterT<std::ofstream> >();

    TestWrite<ByteStreamWriterT<std::ofstream> >();
    TestRead<ByteStreamReaderT<std::ifstream> >();
    TestStateChanges<ByteStreamWriterT<std::ofstream> >();
}
