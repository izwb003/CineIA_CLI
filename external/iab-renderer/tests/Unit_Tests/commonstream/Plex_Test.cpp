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
//  Plex_Test.cpp
//  Created by Bob Brown on 12/29/2016.
//
//

#include "gtest/gtest.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

class Plex_Test : public testing::Test {
    
protected:
    
    // virtual void SetUp() to init variables
    virtual void SetUp()
    {
        aligned_ = false;
    
        value0_ = 0x08;
        value1_ = 0x10;
        value2_ = 0x20;
        value3_ = 0x40;
        value4_ = 0x80;
        value5_ = 0x100;
        value6_ = 0x200;
        value7_ = 0x400;
        value8_ = 0x0001257C;
        value9_ = 0x00012675;
        value10_ = 0x12345678;
    
        value11_ = 0x0; // (4 bits)
        value12_ = 0x1; // (4 bits)
        value13_ = 0xE; // (4 bits)
        value14_ = 0xF;
        value15_ = 0xFE;
        value16_ = 0xFF;
        value17_ = 0xFFFE;
        value18_ = 0xFFFF;
        value19_ = 0xFFFFFFFE;
    
        // Properly fails to write
        //
        value20_ = 0xFFFFFFFF;
    }
    
    virtual void TearDown()
    {
    }
    
    void setByteAligned() { aligned_ = true; }
    void setByteUnaligned() { aligned_ = false; }
    
    template<uint32_t N>
    void TestWrite(BitStreamWriterT<std::ofstream> &bsw)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        Plex<N>test0(value0_);
        rc = write(bsw, test0);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test1(value1_);
        rc = write(bsw, test1);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test2(value2_);
        rc = write(bsw, test2);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test3(value3_);
        rc = write(bsw, test3);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test4(value4_);
        rc = write(bsw, test4);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test5(value5_);
        rc = write(bsw, test5);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test6(value6_);
        rc = write(bsw, test6);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test7(value7_);
        rc = write(bsw, test7);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();
        
        Plex<N>test8(value8_);
        rc = write(bsw, test8);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test9(value9_);
        rc = write(bsw, test9);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();
        
        Plex<N>test10(value10_);
        rc = write(bsw, test10);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test11(value11_);
        rc = write(bsw, test11);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test12(value12_);
        rc = write(bsw, test12);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test13(value13_);
        rc = write(bsw, test13);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test14(value14_);
        rc = write(bsw, test14);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test15(value15_);
        rc = write(bsw, test15);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test16(value16_);
        rc = write(bsw, test16);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test17(value17_);
        rc = write(bsw, test17);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test18(value18_);
        rc = write(bsw, test18);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test19(value19_);
        rc = write(bsw, test19);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsw.align();

        Plex<N>test20(value20_);
        rc = write(bsw, test20);
        
        ASSERT_TRUE(rc == CMNSTRM_PARAMS_BAD);

        if (aligned_)
            bsw.align();

        // Align before we do our final write
        //
        bsw.align();
    }
    
    template<uint32_t N>
    void TestRead(BitStreamReaderT<std::ifstream> &bsr)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        Plex<N>test0;
        rc = read(bsr, test0);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test0 == value0_);

        Plex<N>test1;
        rc = read(bsr, test1);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test1 == value1_);

        Plex<N>test2;
        rc = read(bsr, test2);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test2 == value2_);

        Plex<N>test3;
        rc = read(bsr, test3);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test3 == value3_);

        Plex<N>test4;
        rc = read(bsr, test4);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test4 == value4_);

        Plex<N>test5;
        rc = read(bsr, test5);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test5 == value5_);

        Plex<N>test6;
        rc = read(bsr, test6);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test6 == value6_);

        Plex<N>test7;
        rc = read(bsr, test7);

        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();

        ASSERT_TRUE(test7 == value7_);

        Plex<N>test8;
        rc = read(bsr, test8);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test8 == value8_);

        Plex<N>test9;
        rc = read(bsr, test9);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test9 == value9_);
        
        Plex<N>test10;
        rc = read(bsr, test10);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test10 == value10_);

        Plex<N>test11;
        rc = read(bsr, test11);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test11 == value11_);

        Plex<N>test12;
        rc = read(bsr, test12);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test12 == value12_);

        Plex<N>test13;
        rc = read(bsr, test13);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test13 == value13_);

        Plex<N>test14;
        rc = read(bsr, test14);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test14 == value14_);

        Plex<N>test15;
        rc = read(bsr, test15);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test15 == value15_);

        Plex<N>test16;
        rc = read(bsr, test16);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test16 == value16_);

        Plex<N>test17;
        rc = read(bsr, test17);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test17 == value17_);

        Plex<N>test18;
        rc = read(bsr, test18);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test18 == value18_);

        Plex<N>test19;
        rc = read(bsr, test19);
        
        ASSERT_TRUE(rc == CMNSTRM_OK);

        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test19 == value19_);

        /*
         // We don't read test20 since it properly fails to write in the write test
         //
        Plex<N>test20;
        rc = read(bsr, test20);
        
        ASSERT_TRUE(rc == CMNSTRM_PARAMS_BAD);
        if (aligned_)
            bsr.align();
        
        ASSERT_TRUE(test20 == value20_);
         */
    }
    
private:
    bool    aligned_;;
    
    uint32_t value0_;
    uint32_t value1_;
    uint32_t value2_;
    uint32_t value3_;
    uint32_t value4_;
    uint32_t value5_;
    uint32_t value6_;
    uint32_t value7_;
    uint32_t value8_;
    uint32_t value9_;
    uint32_t value10_;
    
    uint32_t value11_;
    uint32_t value12_;
    uint32_t value13_;
    uint32_t value14_;
    uint32_t value15_;
    uint32_t value16_;
    uint32_t value17_;
    uint32_t value18_;
    uint32_t value19_;
    
    // Properly fails to write
    //
    uint32_t value20_;
    
    // To large for 32 bits
    //
    //uint32_t value21_ = 0xFFFFFFFFFFFFFFFE;
    //uint32_t value122_ = 0xFFFFFFFFFFFFFFFF;

#if 0
    // TODO: Add more tests that validate the encoded results
    //
    uint32_t plex4Result0_ = 0x8;
    uint32_t plex4Result1_ = 0xf10;
    uint32_t plex4Result2_ = 0xf20;
    uint32_t plex4Result3_ = 0xf40;
    uint32_t plex4Result4_ = 0xf80;
    uint32_t plex4Result5_ = 0xfff0100;
    uint32_t plex4Result6_ = 0xfff0200;
    uint32_t plex4Result7_ = 0xfff0400;
    
    uint32_t plex4Resultvalue11_ = 0x0;
    uint32_t plex4Resultvalue12_ = 0x1;
    uint32_t plex4Resultvalue13_ = 0xE;
    uint32_t plex4Resultvalue14_ = 0xF0F;
    uint32_t plex4Resultvalue15_ = 0xFFE;
    uint32_t plex4Resultvalue16_ = 0xFFF00FF;
    uint32_t plex4Resultvalue17_ = 0xFFFFFFE;
    //uint32_t plex4Resultvalue18_ = 0xFFFFFFF0000FFFF;
    //uint32_t plex4Resultvalue19_ = 0xFFFFFFFFFFFFFFE;
    //uint32_t plex4Resultvalue20_ = 0xFFFFFFFFFFFFFFF00000000FFFFFFFF;
    //uint32_t plex4Resultvalue21_ = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE;
    //uint32_t plex4Resultvalue22_ = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000000000FFFFFFFFFFFFFFFF;

    uint32_t plex8Result0_ = 0x08;
    uint32_t plex8Result1_ = 0x10;
    uint32_t plex8Result2_ = 0x20;
    uint32_t plex8Result3_ = 0x40;
    uint32_t plex8Result4_ = 0x80;
    uint32_t plex8Result5_ = 0xff0100;
    uint32_t plex8Result6_ = 0xff0200;
    uint32_t plex8Result7_ = 0xff0400;

    uint32_t plex8Resultvalue11_ = 0x0;
    uint32_t plex8Resultvalue12_ = 0x1;
    uint32_t plex8Resultvalue13_ = 0xE;
    uint32_t plex8Resultvalue14_ = 0xF;
    uint32_t plex8Resultvalue15_ = 0xFE;
    uint32_t plex8Resultvalue16_ = 0xFF00FF;
    uint32_t plex8Resultvalue17_ = 0xFFFFFE;
    //uint32_t plex8Resultvalue18_ = 0xFFFFFF0000FFFF;
    //uint32_t plex8Resultvalue19_ = 0xFFFFFFFFFFFFFE;
    //uint32_t plex8Resultvalue20_ = 0xFFFFFFFFFFFFFF00000000FFFFFFFF;
    //uint32_t plex8Resultvalue21_ = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE;
    //uint32_t plex8Resultvalue22_ = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000000000FFFFFFFFFFFFFFFF;
#endif
};

TEST_F(Plex_Test, Plex_Test_Case1)
{
    setByteUnaligned();

    std::ofstream outputStream0("Plex_Test_Case0.bin", std::ofstream::out | std::ifstream::binary);

    if (outputStream0.good())
    {
        BitStreamWriterT<std::ofstream> bsw(outputStream0);
        TestWrite<4>(bsw);
        
        outputStream0.flush();
        outputStream0.close();
    }

    std::ifstream inputStream0("Plex_Test_Case0.bin", std::ifstream::in | std::ifstream::binary);
    
    if (inputStream0.good())
    {
        BitStreamReaderT<std::ifstream> bsr(inputStream0);

        TestRead<4>(bsr);

        inputStream0.close();
    }

    setByteAligned();

    std::ofstream outputStream1("Plex_Test_Case1.bin", std::ofstream::out | std::ifstream::binary);
    
    if (outputStream1.good())
    {
        BitStreamWriterT<std::ofstream> bsw(outputStream1);
        TestWrite<4>(bsw);
        
        outputStream1.flush();
        outputStream1.close();
    }

    std::ifstream inputStream1("Plex_Test_Case1.bin", std::ifstream::in | std::ifstream::binary);
    
    if (inputStream1.good())
    {
        BitStreamReaderT<std::ifstream> bsr(inputStream1);
        
        TestRead<4>(bsr);
        
        inputStream1.close();
    }

    setByteUnaligned();
    
    std::ofstream outputStream2("Plex_Test_Case2.bin", std::ofstream::out | std::ifstream::binary);
    
    if (outputStream2.good())
    {
        BitStreamWriterT<std::ofstream> bsw(outputStream2);
        TestWrite<8>(bsw);
        
        outputStream2.flush();
        outputStream2.close();
    }
    
    std::ifstream inputStream2("Plex_Test_Case2.bin", std::ifstream::in | std::ifstream::binary);
    
    if (inputStream2.good())
    {
        BitStreamReaderT<std::ifstream> bsr(inputStream2);
        
        TestRead<8>(bsr);
        
        inputStream2.close();
    }
    
    setByteAligned();
    
    std::ofstream outputStream3("Plex_Test_Case3.bin", std::ofstream::out | std::ifstream::binary);
    
    if (outputStream3.good())
    {
        BitStreamWriterT<std::ofstream> bsw(outputStream3);
        TestWrite<8>(bsw);
        
        outputStream3.flush();
        outputStream3.close();
    }
    
    std::ifstream inputStream3("Plex_Test_Case3.bin", std::ifstream::in | std::ifstream::binary);
    
    if (inputStream3.good())
    {
        BitStreamReaderT<std::ifstream> bsr(inputStream3);
        
        TestRead<8>(bsr);
        
        inputStream3.close();
    }
}
