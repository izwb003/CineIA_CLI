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
#include <fstream>

#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

class BitStreamReader_ReadTest : public testing::Test {
    
protected:
    
    void MakeReadStream()
    {
        std::ofstream outputStream("readstream.bin",
                                   std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            // First bit of file is 1
            //
            uint8_t tmp = 0x80;
            
            for (int32_t i = 0; i < 4; i++)
                outputStream.write((char *)&tmp, 1);
            
            outputStream.flush();
            outputStream.close();
        }
    }
    
    void TestRead()
    {
        std::ifstream inputStream("readstream.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);

            uint8_t bit;
            read(bsr,bit, 1);
            
            // first bit of file is 1
            //
            ASSERT_TRUE(bit);
            
            bsr.reset();
            
            uint32_t bits = 0;
            read(bsr,bits, 32);
            
            // First 32 bits of file is 0x80808080
            //
            ASSERT_EQ(0x80808080,bits);
            
            bsr.reset();
            bits = 0;
            read(bsr,bits, 31);
            
            // First 31 bits of file is 0x80808080
            //
            ASSERT_EQ(0x40404040,bits);
            
            bsr.reset();
            skip(bsr,1);
            
            read(bsr,bits,31);
            
            ASSERT_EQ(0x00808080,bits);
            
            bsr.reset();
            skip(bsr,1);
            bsr.align();
            
            read(bsr,bits,24);
            
            ASSERT_EQ(0x00808080,bits);

            bsr.reset();
            skip(bsr,1);
            bsr.align();
            skip(bsr,1);
            bsr.align();
            read(bsr,bits,24);
            
            inputStream.close();
        }
    }
};

TEST_F(BitStreamReader_ReadTest,BitStreamReader_Case1)
{
     MakeReadStream();
     TestRead();
}



