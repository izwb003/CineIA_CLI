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
//  BitStream_CheckParameters.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include <fstream>
#include "gtest/gtest.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"

using namespace CommonStream;

#define BITS_PER_BYTE 8

template<typename StreamType, typename N>
void TestWriteReturnParameters(BitStreamWriterT<StreamType> *iBSW)
{
    N test = 0;
    ReturnCode rc = 0;
    int numBits = sizeof(N) * BITS_PER_BYTE;
    
    rc = iBSW->write(test, 0);
    ASSERT_EQ(rc, CMNSTRM_OK);
    
    for (int i = 1; i <= numBits; i++)
    {
        rc = iBSW->write(test, i);
        ASSERT_EQ(rc, CMNSTRM_OK);
    }
    
    rc = iBSW->write(test, numBits + 1);
    ASSERT_EQ(rc, CMNSTRM_PARAMS_BAD);
}

template<typename StreamType, typename N>
void TestReadReturnParameters(BitStreamReaderT<StreamType> *iBSR)
{
    N test;

    ReturnCode rc = 0;
    int numBits = sizeof(N) * BITS_PER_BYTE;
    
    rc = iBSR->read(test, 0);
    ASSERT_EQ(rc, CMNSTRM_OK);

    for (int i = 1; i <= numBits; i++)
    {
        rc = iBSR->read(test, i);
        ASSERT_EQ(rc, CMNSTRM_OK);
    }
    
    rc = iBSR->read(test, numBits + 1);
    ASSERT_EQ(rc, CMNSTRM_PARAMS_BAD);
}

template<typename StreamType, typename N>
void TestPeekReturnParameters(BitStreamReaderT<StreamType> *iBSR)
{
    N test;
    ReturnCode rc = 0;
    int numBits = sizeof(N) * BITS_PER_BYTE;
    
    rc = iBSR->peek(test, 0);
    ASSERT_EQ(rc, CMNSTRM_OK);

    for (int i = 1; i <= numBits; i++)
    {
        rc = iBSR->peek(test, i);
        ASSERT_EQ(rc, CMNSTRM_OK);
    }
    
    rc = iBSR->peek(test, numBits + 1);
    ASSERT_EQ(rc, CMNSTRM_PARAMS_BAD);
}

template<typename StreamType, typename N>
void TestPeekAndRead(BitStreamReaderT<StreamType> *iBSR)
{
    N peekVal;
    N readVal;
    ReturnCode rc = 0;
    int numBits = sizeof(N) * BITS_PER_BYTE;
    
    for (int i = 1; i <= numBits; i++)
    {
        rc = iBSR->peek(peekVal, i);
        rc = iBSR->read(readVal, i);
        ASSERT_EQ(peekVal, readVal);
    }
}

TEST(BitStream_CheckParameters, BitStream_CheckParameters_Case1)
{
    std::ofstream outputStream("CheckParameters.bin", std::ofstream::out | std::ofstream::binary);
    
    ASSERT_TRUE(outputStream.good());
    if (outputStream.good())
    {
        BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
        
        TestWriteReturnParameters<std::ofstream, uint8_t>(bsw);
        TestWriteReturnParameters<std::ofstream, uint32_t>(bsw);
        TestWriteReturnParameters<std::ofstream, uint64_t>(bsw);
        
        delete bsw;
        outputStream.close();
    }

    std::ifstream inputStream("CheckParameters.bin", std::ifstream::in | std::ifstream::binary);
    
    ASSERT_TRUE(inputStream.good());
    if (inputStream.good())
    {
        BitStreamReaderT<std::ifstream> *bsr = new BitStreamReaderT<std::ifstream>(inputStream);

        TestPeekReturnParameters<std::ifstream, uint8_t>(bsr);
        TestPeekReturnParameters<std::ifstream, uint32_t>(bsr);
        TestPeekReturnParameters<std::ifstream, uint64_t>(bsr);

        TestReadReturnParameters<std::ifstream, uint8_t>(bsr);
        TestReadReturnParameters<std::ifstream, uint32_t>(bsr);
        TestReadReturnParameters<std::ifstream, uint64_t>(bsr);
        
        delete bsr;
    }

    inputStream.seekg(0, std::ios::beg);

    ASSERT_TRUE(inputStream.good());
    if (inputStream.good())
    {
        BitStreamReaderT<std::ifstream> *bsr = new BitStreamReaderT<std::ifstream>(inputStream);
        
        TestPeekAndRead<std::ifstream, uint8_t>(bsr);
        TestPeekAndRead<std::ifstream, uint32_t>(bsr);
        TestPeekAndRead<std::ifstream, uint64_t>(bsr);
        
        delete bsr;
    }

    inputStream.close();
}


