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
//  BitByte_ReadWriteTest.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"

#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

#include "commonstream/bytestream/ByteStreamWriterT.h"
#include "commonstream/bytestream/ByteStreamReaderT.h"
#include "commonstream/bytestream/ByteStreamIOHelpers.h"

using namespace CommonStream;

#define BITS_PER_BYTE 8

class BitByte_ReadWriteTest : public testing::Test {
    
protected:
    
    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
    }
    
    virtual void TearDown()
    {
    }
    
    void BitWrite1()
    {
        std::ofstream outputStream("BitByte_ReadWriteTest_Case1.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            
            int64_t bitCount = 0;
            
            bsw->write((uint8_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(1 * BITS_PER_BYTE, bitCount);

            bsw->write((uint32_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(5 * BITS_PER_BYTE, bitCount);
            
            bsw->write((uint64_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(13 * BITS_PER_BYTE, bitCount);

            bsw->write((int8_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(14 * BITS_PER_BYTE, bitCount);
            
            bsw->write((int32_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(18 * BITS_PER_BYTE, bitCount);
            
            bsw->write((int64_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(26 * BITS_PER_BYTE, bitCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void BitRead1()
    {
        std::ifstream inputStream("BitByte_ReadWriteTest_Case1.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t bitCount = 0;

            uint8_t ui8;
            bsr.read(ui8);
            ASSERT_EQ(1, ui8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(1 * BITS_PER_BYTE, bitCount);

            uint32_t ui32;
            bsr.read(ui32);
            ASSERT_EQ(1, ui32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(5 * BITS_PER_BYTE, bitCount);

            uint64_t ui64;
            bsr.read(ui64);
            ASSERT_EQ(1, ui64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(13 * BITS_PER_BYTE, bitCount);

            int8_t i8;
            bsr.read(i8);
            ASSERT_EQ(1, i8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(14 * BITS_PER_BYTE, bitCount);
            
            int32_t i32;
            bsr.read(i32);
            ASSERT_EQ(1, i32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(18 * BITS_PER_BYTE, bitCount);
            
            int64_t i64;
            bsr.read(i64);
            ASSERT_EQ(1, i64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(26 * BITS_PER_BYTE, bitCount);

            inputStream.close();
        }
    }
    
    void ByteRead1()
    {
        std::ifstream inputStream("BitByte_ReadWriteTest_Case1.bin", std::ifstream::in | std::ifstream::binary);
        
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

            int8_t i8;
            bsr.read(i8);
            ASSERT_EQ(1, i8);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(14, byteCount);
            
            int32_t i32;
            bsr.read(i32);
            ASSERT_EQ(1, i32);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(18, byteCount);
            
            int64_t i64;
            bsr.read(i64);
            ASSERT_EQ(1, i64);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(26, byteCount);

            inputStream.close();
        }
    }

    void ByteWrite2()
    {
        std::ofstream outputStream("BitByte_ReadWriteTest_Case2.bin", std::ofstream::out | std::ifstream::binary);
        
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

            bsw->write((int8_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(14, byteCount);
            
            bsw->write((int32_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(18, byteCount);
            
            bsw->write((int64_t)1);
            
            byteCount = bsw->getByteCount();
            ASSERT_EQ(26, byteCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void ByteRead2()
    {
        std::ifstream inputStream("BitByte_ReadWriteTest_Case2.bin", std::ifstream::in | std::ifstream::binary);
        
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
            
            int8_t i8;
            bsr.read(i8);
            ASSERT_EQ(1, i8);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(14, byteCount);
            
            int32_t i32;
            bsr.read(i32);
            ASSERT_EQ(1, i32);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(18, byteCount);
            
            int64_t i64;
            bsr.read(i64);
            ASSERT_EQ(1, i64);
            
            byteCount = bsr.getByteCount();
            ASSERT_EQ(26, byteCount);
            
            inputStream.close();
        }
    }

    void BitRead2()
    {
        std::ifstream inputStream("BitByte_ReadWriteTest_Case2.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t bitCount = 0;
            
            uint8_t ui8;
            bsr.read(ui8);
            ASSERT_EQ(1, ui8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(1 * BITS_PER_BYTE, bitCount);
            
            uint32_t ui32;
            bsr.read(ui32);
            ASSERT_EQ(1, ui32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(5 * BITS_PER_BYTE, bitCount);
            
            uint64_t ui64;
            bsr.read(ui64);
            ASSERT_EQ(1, ui64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(13 * BITS_PER_BYTE, bitCount);
            
            int8_t i8;
            bsr.read(i8);
            ASSERT_EQ(1, i8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(14 * BITS_PER_BYTE, bitCount);
            
            int32_t i32;
            bsr.read(i32);
            ASSERT_EQ(1, i32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(18 * BITS_PER_BYTE, bitCount);
            
            int64_t i64;
            bsr.read(i64);
            ASSERT_EQ(1, i64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(26 * BITS_PER_BYTE, bitCount);

            inputStream.close();
        }
    }

};


TEST_F(BitByte_ReadWriteTest, BitByte_ReadWriteTest_Case1)
{
    BitWrite1();
    BitRead1();
    ByteRead1();

    ByteWrite2();
    ByteRead2();
    BitRead2();
}
