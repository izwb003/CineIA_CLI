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
//  BitStreamWriter_Test.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"

#include <deque>

#include "commonstream/utils/iitersbuf.h"
#include "commonstream/utils/opbacksbuf.h"
#include "commonstream/utils/rawbuf.h"

#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

TEST(ReadWriteBuffer, ReadWriteBuffer_Case1)
{
    int32_t bufferSize = 256;
    int8_t *dataBuffer = new int8_t[bufferSize];
    memset(dataBuffer, 0, bufferSize);
    
    std::vector<char> writeBuf;

    opbacksbuf<std::vector<char> > sbuf(writeBuf);
    
    std::ostream os(&sbuf);

    BitStreamWriterT<std::ostream> bsw(os);
    int32_t writeVal = 123;
    write(bsw, writeVal);

    memcpy(dataBuffer, writeBuf.data(), writeBuf.size());
    
    std::deque<int8_t> bufferDeque;
    
    bufferDeque.insert(bufferDeque.end(), dataBuffer, dataBuffer + bufferSize);
    
    iitersbuf<std::deque<int8_t>::iterator> iter(bufferDeque.begin(), bufferDeque.end());

    std::istream is(&iter);

    BitStreamReaderT<std::istream> bsr(is);
    int32_t readVal;
    read(bsr, readVal);

    ASSERT_EQ(writeVal, readVal);
    
    delete [] dataBuffer;
}

TEST(ReadWriteBuffer, ReadWriteBuffer_Case2)
{
    int bufsize = 10;
    char *buf = new char[bufsize];
    
    OStreamBufWrapper<char> ost(buf, bufsize);
    std::ostream sb(&ost);
    BitStreamWriterT<std::ostream> bwriter(sb);
    
    unsigned i = 0;
    int rc = 0;
    while ( 1 )
    {
        ASSERT_EQ(bwriter.getBitCount(), 8*i);
        
        rc = bwriter.write(i, 8);
        if ( rc )
        {
            break;
        }
        
        i++;
    }

    printf("At i = %d got rc: %d eof: %d good: %d fail: %d bitcount: %d\n",
           i, rc, bwriter.eof(), bwriter.good(), bwriter.fail(), (int)bwriter.getBitCount());

    delete [] buf;
}


TEST(ReadWriteBuffer, ReadWriteBuffer_Case3)
{
    int32_t bufferSize = 256;
    char *dataBuffer = new char[bufferSize];
    memset(dataBuffer, 0, bufferSize);
    
    ReturnCode rc = CMNSTRM_OK;

    OStreamBufWrapper<char> sbwForWrite(dataBuffer, bufferSize);
    
    std::ostream os(&sbwForWrite);

    BitStreamWriterT<std::ostream> bsw(os);
    int32_t writeVal = 123;
    rc = write(bsw, writeVal);
    ASSERT_EQ(rc, CMNSTRM_OK);

    IStreamBufWrapper<char> sbwForRead(dataBuffer, bufferSize);

    std::istream is(&sbwForRead);
    
    BitStreamReaderT<std::istream> bsr(is);
    int32_t readVal;
    rc = read(bsr, readVal);
    
    ASSERT_EQ(rc, CMNSTRM_OK);
    ASSERT_EQ(writeVal, readVal);
    
    delete [] dataBuffer;
}
