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
//  BitStreamReader_Test.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include <fstream>

#include "gtest/gtest.h"

#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"

using namespace CommonStream;

// BitStream_SyncTest tests two separate sync functions
// (1) sync(BitStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes, const uint8_t *iMask)
// (2) sync(BitStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes)

class BitStream_SyncTest : public testing::Test {
    
protected:
    uint8_t * syncword;
    uint8_t * mask;
    
    // virtual void SetUp() to init variables
    virtual void SetUp() {
        
        // Set up a 4-byte sync sequence/pattern
        syncword = new uint8_t[4];
        syncword[0]=0x00;
        syncword[1]=0x01;
        syncword[2]=0x02;
        syncword[3]=0x03;
        
        // Set up a bitmask pattern for sync detection with mask pattern
        // Checking lower 7 bits of the sync bytes only
        mask = new uint8_t[4];
        mask[0]=0x7F;
        mask[1]=0x7F;
        mask[2]=0x7F;
        mask[3]=0x7F;
        
    }
    
    virtual void TearDown()
    {
        if (syncword)
        {
            delete [] syncword;
            syncword = nullptr;
        }
        
        if (mask)
        {
            delete [] mask;
            mask = nullptr;
        }
    }
    
    void MakeSyncStream()
    {
        srand ((unsigned int)time(NULL));
        
        std::ofstream outputStream("BitStream_SyncTest.bin", std::ofstream::out | std::ifstream::binary);
        
        if (outputStream.good())
        {
            BitStreamWriterT<std::ofstream> *bsw = new BitStreamWriterT<std::ofstream>(outputStream);
            
            // Generate 10 blocks (1000000/1000) of data, each starts with the 4-byte sync pattern, remaining bytes (1000-4) contain
            // randomly generated characters
            
            for (int i=0; i<100000; i++)
            {
                if (i%1000 == 0) {
                    
                    outputStream.write((char *)syncword, 4);
                    i++;
                    i++;
                    i++;
                    
                } else {
                    
                    unsigned char randval;
                    randval = rand();
                    randval = randval % 256;
                    outputStream.write((char *) &randval, 1);
                }
            }
            
            delete bsw;
            
            outputStream.flush();
            outputStream.close();
        }
    }
    
    void TestSync()
    {
        std::ifstream inputStream("BitStream_SyncTest.bin", std::ifstream::in | std::ifstream::binary);
        
        if (inputStream.good())
        {
            BitStreamReaderT<std::ifstream> bsr(inputStream);
            
            int foundsyncwords = 0;
            
            // sync() scans through the stream and returns when the sync sequence is found or at end of stream
            // If found, stream position is at first of the sync bytes
            
            while (sync(bsr,syncword, 4) == CMNSTRM_OK)
            {
                int64_t pos = bsr.getBitCount();
                
                // Sync are expected at start of each 1000-byte block
                // Bypass alias sync if generated by randval()
                
                if (pos % 1000 == 0) {
                    
                    foundsyncwords++;
                    
                }
                
                // Move forward for next detection
                skip(bsr,7);
            }
            
            // Confirm number of genuine sync found
            ASSERT_EQ(foundsyncwords,100);
            
            ASSERT_TRUE((bool) bsr);
            
            // Reset the bitstream reader
            
            bsr.reset();
            
            foundsyncwords = 0;
            
            // sync() scans through the stream and returns when the sync sequence is found or at end of stream
            // If found, stream position is at first of the sync bytes
            // In this case, sync() is called with "mask" to apply bitmask on the sync pattern
            
            while (sync(bsr,syncword, 4, mask) == CMNSTRM_OK)
            {
                int64_t pos = bsr.getBitCount();
                
                // Sync are expected at start of each 1000-byte block
                // Bypass alias sync if generated by randval()
                
                if (pos % 1000 == 0) {
                    
                    foundsyncwords++;
                    
                }
                
                // Move forward for next detection
                skip(bsr,7);
            }
            
            // Confirm number of genuine sync found
            ASSERT_EQ(foundsyncwords, 100);
            
            ASSERT_TRUE((bool) bsr);
            
            inputStream.close();
        }
    }
};


TEST_F(BitStream_SyncTest, BitStream_SyncTest_Case1)
{
    for (int i = 0; i < 100; i++) {
        MakeSyncStream();
        TestSync();
    }
}


