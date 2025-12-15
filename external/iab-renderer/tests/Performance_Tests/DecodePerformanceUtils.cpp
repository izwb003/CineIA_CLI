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

#include "DecodePerformanceUtils.h"
#include "TestConstants.h"
#include "CPUTime.h"
#include "testcfg.h"
#include "TestUtils.h"
#include <stdexcept>

IABPerformanceDecode::IABPerformanceDecode()
{
    iabParser_ = NULL;
	iabRenderer_ = NULL;
	rendererConfig_ = NULL;
}

IABPerformanceDecode::~IABPerformanceDecode()
{
	CleanUp();
}

void IABPerformanceDecode::MeasureMaxComplexity()
{
    double frameParsingRate = 0;
    
    // Measure frame decoding (parsing + rendering) rate
    std::cout << "\nMeasuring frame decoding (parsing + rendering) rate ......" << std::endl << std::flush;
    frameParsingRate = MeasureFramesDecodingRate(IAB91OHcfg);
    
    std::cout << "\tFrame Decoding rate on Max Complecity Frame: " << frameParsingRate << " frames/sec" << std::endl;
}

double IABPerformanceDecode::MeasureFramesDecodingRate(std::string iConfigString)
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
    
	// ************************
	// Create renderer configuration from config string
	// ************************
	RenderUtils::IRendererConfiguration *rendererConfig = RenderUtils::IRendererConfigurationFile::FromBuffer((char*)iConfigString.c_str());

	if (!rendererConfig)
	{
		CleanUp();
		throw std::runtime_error("Unable to create renderer configuration to run measurement.");
	}

	// ************************
	// Create and set up IABRenderer
	// ************************
	IABRenderer *iabRenderer;

	// Instantiate an IABRenderer with iFrameGainsCacheEnable set to false. This prevents the renderer from re-using
	// gains calculated in previous frame and to ensure that all objects are rendered in every RenderIABFrame() call.
	iabRenderer = new IABRenderer(*rendererConfig, false);

	if (!iabRenderer)
	{
		CleanUp();
		throw std::runtime_error("Unable to create IAB Renderer to run measurement.");
	}

	IABRenderedOutputChannelCountType outputChannelCount = iabRenderer->GetOutputChannelCount();
	uint32_t frameSampleCount = maxComplexityFrame_.GetFrameSampleCount();

	if (outputChannelCount == 0)
	{
		CleanUp();
		throw std::runtime_error("Renderer is not initialised properly.");
	}

	// Pasred IABFrame to be further rendered
	const IABFrameInterface* parsedIABFrame = NULL;

	// Create renderer output buffer
	float *outBuffer = new float[outputChannelCount * frameSampleCount];
	float **outPointers = new float*[outputChannelCount];
	for (uint32_t i = 0; i < outputChannelCount; i++)
	{
		outPointers[i] = outBuffer + i * frameSampleCount;
	}

	// Initialise test time
    _cpu_time starttime;
    _cpu_time endtime;
    double actualTestTime = 0;

    int32_t totalFramesDecoded = 0;
	IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;

    // Use current time as start time
    getCPUTime(&starttime);

    // ************************
    // Measure
    // ************************
    // Run parsing on the same packed frame and measure total time taken
    for (totalFramesDecoded = 0; actualTestTime < kIABTargetTestTime; totalFramesDecoded++)
    {
        // Reset stream before parsing
        streamForParsing.seekg(0);

        if (kIABNoError != iabParser_->ParseIABFrame())
        {
            CleanUp();
            throw std::runtime_error("Unable to measure parsing rate due to error when parsing.");
        }

		iabParser_->GetIABFrame(parsedIABFrame);

		if ( kIABNoError != iabRenderer->RenderIABFrame(*parsedIABFrame, outPointers, outputChannelCount, frameSampleCount, renderedOutputSampleCount) )
		{
			delete[] outBuffer;
			delete[] outPointers;

			CleanUp();
			throw std::runtime_error("Unable to successfully render the IAB frame.");
		}

		getCPUTime(&endtime);
        actualTestTime = diffCPUTime(&endtime, &starttime);
    }

    CleanUp();

    // ************************
    // Return result
    // ************************
    frameParsingRate = ((double)totalFramesDecoded) * 1e9 / actualTestTime ; // 1e9: CPUTime class internally measures in nano seconds
    
    return frameParsingRate;
}

void IABPerformanceDecode::CleanUp()
{
    IABParserInterface::Delete(iabParser_);
    iabParser_ = NULL;

	if (iabRenderer_)
	{
		delete iabRenderer_;
		iabRenderer_ = NULL;
	}

	if (rendererConfig_)
	{
		delete rendererConfig_;
		rendererConfig_ = NULL;
	}
}



