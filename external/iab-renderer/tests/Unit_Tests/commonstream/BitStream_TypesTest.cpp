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
//  BitStream_TypesTest.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

class BitStream_TypesTest : public testing::Test {
    
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
        std::ofstream outputStream("BitStream_TypesTest_Case1.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> bsw(outputStream);
            
            std::vector<uint8_t> foo2;
            foo2.push_back(0x00);
            foo2.push_back(0x01);
            foo2.push_back(0x02);
            foo2.push_back(0x03);
            
            write(bsw, foo2);
            
            std::vector<int8_t> foo3;
            foo3.push_back(0x00);
            foo3.push_back(0x01);
            foo3.push_back(0x02);
            foo3.push_back(0x03);
            
            write(bsw, foo3);

            std::wstring foo5 = L"Hello World!";
            
            write(bsw, foo5);
            
            std::vector<uint64_t> foo15;
            foo15.push_back(123);
            foo15.push_back(456);
            write(bsw, foo15);
            
            outputStream.flush();
            outputStream.close();
        }
    }
    
    void TestRead()
    {
        std::ifstream inputStream("BitStream_TypesTest_Case1.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            std::vector<uint8_t> foo2;
            read(bsr, foo2);
            
            std::vector<uint8_t>::iterator iter1 = foo2.begin();
            
            ASSERT_TRUE((*iter1) == 0x00);
            
            iter1++;
            ASSERT_TRUE((*iter1) == 0x01);
            
            iter1++;
            ASSERT_TRUE((*iter1) == 0x02);
            
            iter1++;
            ASSERT_TRUE((*iter1) == 0x03);
            
            std::vector<int8_t> foo3;
            read(bsr, foo3);
            
            std::vector<int8_t>::iterator iter2 = foo3.begin();
            
            ASSERT_TRUE((*iter2) == 0x00);
            
            iter2++;
            ASSERT_TRUE((*iter2) == 0x01);
            
            iter2++;
            ASSERT_TRUE((*iter2) == 0x02);
            
            iter2++;
            ASSERT_TRUE((*iter2) == 0x03);
            
            std::wstring foo5;
            
            read(bsr, foo5);
            
            std::wstring foo5_result = L"Hello World!";
            
            ASSERT_TRUE(foo5.compare(foo5_result) == 0);
            
            std::vector<uint64_t> foo15;
            read(bsr, foo15);
            
            std::vector<uint64_t>::iterator iter3 = foo15.begin();
            
            ASSERT_TRUE((*iter3) == 123);
            
            iter3++;
            ASSERT_TRUE((*iter3) == 456);

            inputStream.close();
        }
    }
};

TEST_F(BitStream_TypesTest, BitStream_TypesTest_Case1)
{
    TestWrite();
    TestRead();
}
