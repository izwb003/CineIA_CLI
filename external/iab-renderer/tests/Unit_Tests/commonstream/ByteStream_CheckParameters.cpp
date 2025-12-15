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
//  ByteStream_CheckParamters.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include <fstream>
#include "gtest/gtest.h"
#include "commonstream/bytestream/ByteStreamReaderT.h"
#include "commonstream/bytestream/ByteStreamWriterT.h"

using namespace CommonStream;

template<typename N>
void TestPeekAndRead(ByteStreamReaderT<std::ifstream> *iBSR)
{
    N peekVal;
    N readVal;
    ReturnCode rc = 0;
    
    rc = iBSR->peek(peekVal);
    rc = iBSR->read(readVal);

    ASSERT_EQ(peekVal, readVal);
}

TEST(ByteStream_CheckParameters, ByteStream_CheckParameters_Case1)
{
    std::ofstream outputStream("CheckParameters.bin", std::ofstream::out | std::ofstream::binary);
    
    ASSERT_TRUE(outputStream.good());
    if (outputStream.good())
    {
        ByteStreamWriterT<std::ofstream> *bsw = new ByteStreamWriterT<std::ofstream>(outputStream);
        ReturnCode rc = 0;

        uint8_t ui8 = 1;
        rc = bsw->write(ui8);
        ASSERT_EQ(rc, CMNSTRM_OK);

        uint32_t ui32 = 1;
        rc = bsw->write(ui32);
        ASSERT_EQ(rc, CMNSTRM_OK);

        uint64_t ui64 = 1;
        rc = bsw->write(ui64);
        ASSERT_EQ(rc, CMNSTRM_OK);

        delete bsw;
        outputStream.close();
    }

    std::ifstream inputStream("CheckParameters.bin", std::ifstream::in | std::ifstream::binary);
    
    ASSERT_TRUE(inputStream.good());
    if (inputStream.good())
    {
        ByteStreamReaderT<std::ifstream> *bsr = new ByteStreamReaderT<std::ifstream>(inputStream);
        
        TestPeekAndRead<uint8_t>(bsr);
        TestPeekAndRead<uint32_t>(bsr);
        TestPeekAndRead<uint64_t>(bsr);
        
        delete bsr;
    }

    inputStream.close();
}


