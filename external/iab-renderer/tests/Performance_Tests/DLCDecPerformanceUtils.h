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

#ifndef __DLCDECPERFORMANCEUTILS_H__
#define __DLCDECPERFORMANCEUTILS_H__

#include "DLC/DLCAudioData.h"
#include "DLC/DLCSimpleEncoder.h"
#include "DLC/DLCFullDecoder.h"

/**
 * Class to test DLC decoding performance on a target platform by measuring time taken
 * to run specific decoding use cases.
 *
 * Procedure per test case:
 * 1. Generate random samples for test frame
 * 2. Encode test frame with dlc::SimpleEncoder
 * 3. Run dlc::FullDecoder on the encoded frame in a loop for the target test period, one dlc::FullDecoder.decode() call per loop
 * 4. Calculate and report frames decoded per second
 */

class IABPerformanceDLCDecoder
{
public:
    IABPerformanceDLCDecoder();
    ~IABPerformanceDLCDecoder();
    
    /**
     * Runs all DLC decoder performance measurements
     *
     */
    void RunMeasurements();
    
private:
    
    // Measures decoding rate of the DLC encoded frame set up by MeasureRandomDataDecodingRate()
    double MeasureDLCFrameDecodingRate(const dlc::AudioData &iDLCAudioData, dlc::SampleRate iSampleRate, uint32_t iFrameSampleCount);
    
    // Encodes a frame of random audio samples and calls MeasureDLCFrameDecodingRate() to measure DLC decoding rate on the frame.
    double MeasureRandomDataDecodingRate(dlc::SampleRate iSampleRate, uint32_t iFrameSampleCount);
        
};
#endif // __DLCDECPERFORMANCEUTILS_H__
