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

#include <fstream>

#include "gtest/gtest.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

class BitStreamReader_PeekTest : public testing::Test {
    
protected:
    
    void MakePeekStream()
    {
        srand ((unsigned int)time(NULL));
        
        std::ofstream outputStream("peekstream.bin", std::ofstream::out | std::ifstream::binary);
        
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
    
    void TestPeek()
    {
        std::ifstream inputStream("peekstream.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            bool bit;
            peek(bsr,bit);

            // first bit of file is 1
            //
            ASSERT_TRUE(bit);
            
            uint32_t bits = 0;
            peek(bsr,bits);

            // First 32 bits of file is 0x80808080
            //
            ASSERT_EQ(0x80808080,bits);

            bits = 0;
            bsr.peek(bits, 31);
            
            // First 31 bits of file is 0x80808080
            //
            ASSERT_EQ(0x40404040,bits);
            
            bits = 0;
            skip(bsr,1);
            peek(bsr,bits,31);
            ASSERT_EQ(0x00808080,bits);

            uint8_t byte = 0;
            bsr.reset();
            peek(bsr,byte);

            // First 8 bits of file is 0x80
            //
            ASSERT_EQ(0x80,byte);
            
            bits = 0;
            skip(bsr,8);
            ReturnCode rc = peek(bsr,bits);
            
            ASSERT_EQ(CMNSTRM_IO_EOF,rc);
            
            bits = 0;
            rc = peek(bsr,bits,24);
            
            ASSERT_EQ(CMNSTRM_OK,rc);
            
            ASSERT_EQ(0x00808080,bits);
            
            inputStream.close();
        }
    }

};


TEST_F(BitStreamReader_PeekTest,BitStreamReader_Case1)
{

    MakePeekStream();
    TestPeek();

}



