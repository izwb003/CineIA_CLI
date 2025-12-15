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
//  ByteStream_ReadWriteTest.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/bytestream/ByteStreamWriterT.h"
#include "commonstream/bytestream/ByteStreamIOHelpers.h"

using namespace CommonStream;

class ByteStream_ReadWriteTest : public testing::Test {
    
protected:
    
    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
    }
    
    virtual void TearDown()
    {
    }
    
    void TestWrite1()
    {
        std::ofstream outputStream("ByteStream_ReadWriteTest_Case1.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            ByteStreamWriterT<std::ofstream> *bsw = new ByteStreamWriterT<std::ofstream>(outputStream);
            
            int64_t byteCount = 0;
            
            bsw->write((uint8_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(1, byteCount);

            bsw->write((uint32_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(5, byteCount);
            
            bsw->write((uint64_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(13, byteCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void TestRead1()
    {
        std::ifstream inputStream("ByteStream_ReadWriteTest_Case1.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            ByteStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t byteCount = 0;

            uint8_t ui8;
            bsr.read(ui8);
            ASSERT_EQ(1, ui8);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(1, byteCount);

            uint32_t ui32;
            bsr.read(ui32);
            ASSERT_EQ(1, ui32);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(5, byteCount);

            uint64_t ui64;
            bsr.read(ui64);
            ASSERT_EQ(1, ui64);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(13, byteCount);

            inputStream.close();
        }
    }

    void TestWrite2()
    {
        std::ofstream outputStream("ByteStream_ReadWriteTest_Case2.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            ByteStreamWriterT<std::ofstream> *bsw = new ByteStreamWriterT<std::ofstream>(outputStream);
            
            int64_t byteCount = 0;
            
            bsw->write(true);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(1, byteCount);
            
            bsw->write(false);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(2, byteCount);

            bsw->write((uint8_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(3, byteCount);

            bsw->write((uint8_t)0xFF);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(4, byteCount);

            bsw->write((uint32_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(8, byteCount);
            
            bsw->write((uint32_t)0xFFFFFFFF);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(12, byteCount);
            
            bsw->write((uint64_t)0);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(20, byteCount);
            
            bsw->write((uint64_t)0xFFFFFFFFFFFFFFFF);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(28, byteCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void TestRead2()
    {
        std::ifstream inputStream("ByteStream_ReadWriteTest_Case2.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            ByteStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t byteCount = 0;
            bool readBool = false;
            uint8_t read8 = 0;
            uint32_t read32 = 0;
            uint64_t read64 = 0;
            
            bsr.read(readBool);
            ASSERT_EQ(true, readBool);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(1, byteCount);
            
            bsr.read(readBool);
            ASSERT_EQ(false, readBool);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(2, byteCount);

            bsr.read(read8);
            ASSERT_EQ(1, read8);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(3, byteCount);

            bsr.read(read8);
            ASSERT_EQ(0xFF, read8);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(4, byteCount);

            bsr.read(read32);
            ASSERT_EQ(1, read32);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(8, byteCount);

            bsr.read(read32);
            ASSERT_EQ(0xFFFFFFFF, read32);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(12, byteCount);

            bsr.read(read64);
            ASSERT_EQ(0, read64);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(20, byteCount);

            bsr.read(read64);
            ASSERT_EQ(0xFFFFFFFFFFFFFFFF, read64);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(28, byteCount);

            inputStream.close();
        }
    }

};


TEST_F(ByteStream_ReadWriteTest, ByteStream_ReadWriteTest_Case1)
{
    TestWrite1();
    TestRead1();

    TestWrite2();
    TestRead2();
}
