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

#include <time.h>
#include <iostream>

#include "DLCDecPerformanceUtils.h"
#include "TestConstants.h"
#include "CPUTime.h"
#include "TestUtils.h"


IABPerformanceDLCDecoder::IABPerformanceDLCDecoder()
{
}

IABPerformanceDLCDecoder::~IABPerformanceDLCDecoder()
{
}

void IABPerformanceDLCDecoder::RunMeasurements()
{
    double frameDecodingRate = 0;
    
    // Measurement #1: Decode 2000 sample frame with random data
    std::cout << "\nMeasuring DLC decoding rate of 48KHz encoded frame with random data ......" << std::endl << std::flush;
    frameDecodingRate = MeasureRandomDataDecodingRate(dlc::eSampleRate_48000, 2000);
    std::cout << "\tFrame decoding rate (2000 samples per frame): " << frameDecodingRate << " frames/sec" << std::endl;

}

// Measure DLC decoding on random data
double IABPerformanceDLCDecoder::MeasureRandomDataDecodingRate(dlc::SampleRate iSampleRate, uint32_t iFrameSampleCount)
{
    double frameDecodingRate = 0;
    int32_t* audioSamples = new int32_t[iFrameSampleCount];
    IABTestUtils::GenerateRandomSamples(audioSamples, iFrameSampleCount, 0);    // Generate random audio samples with a seed value of 0

    dlc::SimpleEncoder encoder;
    dlc::AudioData encodedAudioData;
    
    // Encode the frame for testing
    encoder.encode(audioSamples, iFrameSampleCount, iSampleRate, encodedAudioData);
    
    frameDecodingRate = MeasureDLCFrameDecodingRate(encodedAudioData, iSampleRate, iFrameSampleCount);
    
    delete [] audioSamples;
    
    return frameDecodingRate;
    
}

// Measure decoding rate of a DLC encoded frame
double IABPerformanceDLCDecoder::MeasureDLCFrameDecodingRate(const dlc::AudioData &iDLCAudioData, dlc::SampleRate iSampleRate, uint32_t iFrameSampleCount)
{
    dlc::FullDecoder decoder;
    int32_t* decodedSamples = new int32_t[iFrameSampleCount];
    
    double frameDecodingRate = 0;
    _cpu_time starttime;
    _cpu_time endtime;
    double actualTestTime = 0;
    int32_t totalFramesDecoded = 0;
    
    // Use current time as start time
    getCPUTime(&starttime);
    
    // Run parsing for the target test period on the same IAB frame and measure total time taken
    for (totalFramesDecoded = 0; actualTestTime < kIABTargetTestTime; totalFramesDecoded++)
    {
        // Run decoder on the encode frame
        decoder.decode(decodedSamples, iFrameSampleCount, iSampleRate, iDLCAudioData);
        getCPUTime(&endtime);
        actualTestTime = diffCPUTime(&endtime, &starttime);
    }
    
    frameDecodingRate = ((double)totalFramesDecoded) * 1e9 / actualTestTime ; // 1e9: CPUTime class internally measures in nano seconds
    
    delete [] decodedSamples;
    return frameDecodingRate;
    
}






