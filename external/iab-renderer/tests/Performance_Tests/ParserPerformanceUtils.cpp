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

#include <iostream>
#include <sstream>

#include "ParserPerformanceUtils.h"
#include "TestConstants.h"
#include "CPUTime.h"
#include "TestUtils.h"
#include <stdexcept>

IABPerformanceParser::IABPerformanceParser()
{
    iabParser_ = NULL;
}

IABPerformanceParser::~IABPerformanceParser()
{
	CleanUp();
}

void IABPerformanceParser::MeasureMaxComplexity()
{
    double frameParsingRate = 0;
    
    // Measure frame parsing rate
    std::cout << "\nMeasuring frame parsing rate ......" << std::endl << std::flush;
    frameParsingRate = MeasureFramesParsingRate();
    
    std::cout << "\tFrame parsing rate on Max Complecity Frame: " << frameParsingRate << " frames/sec" << std::endl;
}

double IABPerformanceParser::MeasureFramesParsingRate()
{
    double frameParsingRate = 0;
    
	// Construct and then serialize Max Complexity Frame, for parsing performance measurment
	maxComplexityFrame_.Construct();
	maxComplexityFrame_.PackToStream();

    // Get serialsed frame data
    std::vector<char> packedStream = maxComplexityFrame_.GetSerializedFrame();
    uint32_t streamLength = maxComplexityFrame_.GetSerializedDataSize();

	// Convert to stream buffer for creating IAB parser on.
    std::stringstream streamForParsing(std::string(&packedStream[0], streamLength));

    // Create a parser
    if (iabParser_)
    {
        IABParserInterface::Delete(iabParser_);
    }
    
    iabParser_ = IABParserInterface::Create(&streamForParsing);
    
    if (!iabParser_)
    {
        CleanUp();
        throw std::runtime_error("Unable to create an IAB parser to run measurement.");
    }
    
    // Initialise test time
    _cpu_time starttime;
    _cpu_time endtime;
    double actualTestTime = 0;
    int32_t totalFramesParsed = 0;

    // Use current time as start time
    getCPUTime(&starttime);

    // ************************
    // Measure
    // ************************
    // Run parsing on the same packed frame and measure total time taken
    for (totalFramesParsed = 0; actualTestTime < kIABTargetTestTime; totalFramesParsed++)
    {
        // Reset stream before parsing
        streamForParsing.seekg(0);

        if (kIABNoError != iabParser_->ParseIABFrame())
        {
            CleanUp();
            throw std::runtime_error("Unable to measure parsing rate due to error when parsing.");
        }

        getCPUTime(&endtime);
        actualTestTime = diffCPUTime(&endtime, &starttime);
    }

    CleanUp();

    // ************************
    // Return result
    // ************************
    frameParsingRate = ((double)totalFramesParsed) * 1e9 / actualTestTime ; // 1e9: CPUTime class internally measures in nano seconds
    
    return frameParsingRate;
}

void IABPerformanceParser::CleanUp()
{
    IABParserInterface::Delete(iabParser_);
    iabParser_ = NULL;
}



