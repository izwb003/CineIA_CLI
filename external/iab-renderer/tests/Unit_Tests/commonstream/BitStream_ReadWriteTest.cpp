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
//  BitStream_ReadWriteTest.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

class BitStream_ReadWriteTest : public testing::Test {
    
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
        std::ofstream outputStream("BitStream_ReadWriteTest_Case1.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            
            int64_t bitCount = 0;
            
            bsw->write((uint32_t)1, 8);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(8, bitCount);
            
            bsw->write((uint32_t)0x11, 9);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(17, bitCount);
            
            bsw->write((uint32_t)0x1, 7);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(24, bitCount);
            
            bsw->write((uint32_t)0x11, 16);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(40, bitCount);
            
            bsw->write((uint32_t)0x11, 24);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(64, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(65, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(66, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(67, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(68, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(69, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(70, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(71, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(72, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(73, bitCount);
            
            bsw->write((uint32_t)0x1, 7);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(80, bitCount);
            
            bsw->write((uint32_t)0x1, 8);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(88, bitCount);
            
            bsw->write((uint32_t)0x1, 1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(89, bitCount);
            
            bsw->write((uint32_t)0x1, 2);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(91, bitCount);
            
            bsw->write((uint32_t)0x1, 3);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(94, bitCount);
            
            bsw->write((uint32_t)0x1, 4);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(98, bitCount);
            
            bsw->write((uint32_t)0x1, 5);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(103, bitCount);
            
            bsw->write((uint32_t)0x1, 6);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(109, bitCount);
            
            bsw->write((uint32_t)0x1, 7);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(116, bitCount);
            
            bsw->write((uint32_t)0x1, 8);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(124, bitCount);
            
            bsw->write((uint32_t)0x1, 9);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(133, bitCount);
            
            bsw->write((uint32_t)0x1, 10);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(143, bitCount);
            
            bsw->write((uint32_t)0x1, 11);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(154, bitCount);
            
            bsw->write((uint32_t)0x1, 12);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(166, bitCount);
            
            bsw->write((uint32_t)0x1, 13);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(179, bitCount);
            
            bsw->write((uint32_t)0x1, 14);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(193, bitCount);
            
            bsw->write((uint32_t)0x1, 15);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(208, bitCount);
            
            bsw->write((uint32_t)0x1, 16);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(224, bitCount);
            
            bsw->write((uint32_t)0x1, 17);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(241, bitCount);
            
            bsw->write((uint32_t)0x1, 18);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(259, bitCount);
            
            bsw->write((uint32_t)0x1, 19);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(278, bitCount);
            
            bsw->write((uint32_t)0x1, 20);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(298, bitCount);
            
            bsw->write((uint32_t)0x1, 21);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(319, bitCount);
            
            bsw->write((uint32_t)0x1, 22);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(341, bitCount);
            
            bsw->write((uint32_t)0x1, 23);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(364, bitCount);
            
            bsw->write((uint32_t)0x1, 24);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(388, bitCount);
            
            bsw->write((uint32_t)0x1, 25);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(413, bitCount);
            
            bsw->write((uint32_t)0x1, 26);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(439, bitCount);
            
            bsw->write((uint32_t)0x1, 27);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(466, bitCount);
            
            bsw->write((uint32_t)0x1, 28);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(494, bitCount);
            
            bsw->write((uint32_t)0x1, 29);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(523, bitCount);
            
            bsw->write((uint32_t)0x1, 30);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(553, bitCount);
            
            bsw->write((uint32_t)0x1, 31);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(584, bitCount);
            
            bsw->write((uint32_t)0x1, 32);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(616, bitCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void TestRead1()
    {
        std::ifstream inputStream("BitStream_ReadWriteTest_Case1.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t bitCount = 0;
            uint32_t readBits = 0;

            bsr.read(readBits, 8);
            ASSERT_EQ(1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(8, bitCount);
            
            bsr.read(readBits, 9);
            ASSERT_EQ(0x11, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(17, bitCount);
            
            bsr.read(readBits, 7);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(24, bitCount);
            
            bsr.read(readBits, 16);
            ASSERT_EQ(0x11, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(40, bitCount);
            
            bsr.read(readBits, 24);
            ASSERT_EQ(0x11, readBits);

            bitCount = bsr.getBitCount();
            ASSERT_EQ(64, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);

            bitCount = bsr.getBitCount();
            ASSERT_EQ(65, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(66, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(67, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(68, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(69, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(70, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(71, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(72, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(73, bitCount);
            
            bsr.read(readBits, 7);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(80, bitCount);
            
            bsr.read(readBits, 8);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(88, bitCount);
            
            bsr.read(readBits, 1);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(89, bitCount);
            
            bsr.read(readBits, 2);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(91, bitCount);
            
            bsr.read(readBits, 3);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(94, bitCount);
            
            bsr.read(readBits, 4);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(98, bitCount);
            
            bsr.read(readBits, 5);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(103, bitCount);
            
            bsr.read(readBits, 6);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(109, bitCount);
            
            bsr.read(readBits, 7);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(116, bitCount);
            
            bsr.read(readBits, 8);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(124, bitCount);
            
            bsr.read(readBits, 9);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(133, bitCount);
            
            bsr.read(readBits, 10);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(143, bitCount);
            
            bsr.read(readBits, 11);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(154, bitCount);
            
            bsr.read(readBits, 12);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(166, bitCount);
            
            bsr.read(readBits, 13);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(179, bitCount);
            
            bsr.read(readBits, 14);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(193, bitCount);
            
            bsr.read(readBits, 15);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(208, bitCount);
            
            bsr.read(readBits, 16);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(224, bitCount);
            
            bsr.read(readBits, 17);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(241, bitCount);
            
            bsr.read(readBits, 18);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(259, bitCount);
            
            bsr.read(readBits, 19);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(278, bitCount);
            
            bsr.read(readBits, 20);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(298, bitCount);
            
            bsr.read(readBits, 21);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(319, bitCount);
            
            bsr.read(readBits, 22);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(341, bitCount);
            
            bsr.read(readBits, 23);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(364, bitCount);
            
            bsr.read(readBits, 24);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(388, bitCount);
            
            bsr.read(readBits, 25);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(413, bitCount);
            
            bsr.read(readBits, 26);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(439, bitCount);
            
            bsr.read(readBits, 27);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(466, bitCount);
            
            bsr.read(readBits, 28);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(494, bitCount);
            
            bsr.read(readBits, 29);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(523, bitCount);
            
            bsr.read(readBits, 30);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(553, bitCount);
            
            bsr.read(readBits, 31);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(584, bitCount);
            
            bsr.read(readBits, 32);
            ASSERT_EQ(0x1, readBits);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(616, bitCount);

            inputStream.close();
        }
    }

    void TestWrite2()
    {
        std::ofstream outputStream("BitStream_ReadWriteTest_Case2.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            
            int64_t bitCount = 0;
            
            bsw->write(true);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(1, bitCount);
            
            bsw->write(false);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(2, bitCount);

            bsw->write((uint8_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(10, bitCount);

            bsw->write((uint8_t)0xFF);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(18, bitCount);

            bsw->write((uint32_t)1);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(50, bitCount);
            
            bsw->write((uint32_t)0xFFFFFFFF);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(82, bitCount);
            
            bsw->write((uint64_t)0);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(146, bitCount);
            
            bsw->write((uint64_t)0xFFFFFFFFFFFFFFFF);
            
            bitCount = bsw->getBitCount();
            ASSERT_EQ(210, bitCount);

            bsw->align();
            bitCount = bsw->getBitCount();
            ASSERT_EQ(216, bitCount);

            outputStream.close();
            delete  bsw;
        }
    }
    
    void TestRead2()
    {
        std::ifstream inputStream("BitStream_ReadWriteTest_Case2.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t bitCount = 0;
            bool readBool = false;
            uint8_t read8 = 0;
            uint32_t read32 = 0;
            uint64_t read64 = 0;
            
            bsr.read(readBool);
            ASSERT_EQ(true, readBool);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(1, bitCount);
            
            bsr.read(readBool);
            ASSERT_EQ(false, readBool);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(2, bitCount);

            bsr.read(read8);
            ASSERT_EQ(1, read8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(10, bitCount);

            bsr.read(read8);
            ASSERT_EQ(0xFF, read8);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(18, bitCount);

            bsr.read(read32);
            ASSERT_EQ(1, read32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(50, bitCount);

            bsr.read(read32);
            ASSERT_EQ(0xFFFFFFFF, read32);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(82, bitCount);

            bsr.read(read64);
            ASSERT_EQ(0, read64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(146, bitCount);

            bsr.read(read64);
            ASSERT_EQ(0xFFFFFFFFFFFFFFFF, read64);
            
            bitCount = bsr.getBitCount();
            ASSERT_EQ(210, bitCount);
            
            bsr.align();
            bitCount = bsr.getBitCount();
            ASSERT_EQ(216, bitCount);

            inputStream.close();
        }
    }

    void TestWrite3()
    {
        std::ofstream outputStream("BitStream_ReadWriteTest_Case3.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);

            int32_t count = 100;
            uint64_t min = 0x0;
            uint64_t max = 0xFFFFFFFFFFFFFFFF;
            for (int32_t i = 0; i < count; i++)
            {
                bsw->writeAligned((uint8_t*)&min, 8);
                bsw->writeAligned((uint8_t*)&max, 8);
            }

            int64_t bitCount = 0;
            bitCount = bsw->getBitCount();
            ASSERT_EQ(count*128, bitCount);

            bsw->align();
            bitCount = bsw->getBitCount();
            ASSERT_EQ(count*128, bitCount);
            
            outputStream.close();
            delete  bsw;
        }
    }
    
    void TestRead3()
    {
        std::ifstream inputStream("BitStream_ReadWriteTest_Case3.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int64_t bitCount = 0;
            uint64_t read64 = 0;
            
            int32_t count = 100;

            uint64_t min = 0x0;
            uint64_t max = 0xFFFFFFFFFFFFFFFF;

            for (int32_t i = 0; i < count; i++)
            {
                bsr.read(read64);
                ASSERT_EQ(min, read64);

                bsr.read(read64);
                ASSERT_EQ(max, read64);
            }

            bitCount = bsr.getBitCount();
            ASSERT_EQ(count*128, bitCount);
            
            bsr.align();
            bitCount = bsr.getBitCount();
            ASSERT_EQ(count*128, bitCount);

            inputStream.close();
        }
    }
};


TEST_F(BitStream_ReadWriteTest, BitStream_ReadWriteTest_Case1)
{
    TestWrite1();
    TestRead1();

    TestWrite2();
    TestRead2();

    TestWrite3();
    TestRead3();
}


