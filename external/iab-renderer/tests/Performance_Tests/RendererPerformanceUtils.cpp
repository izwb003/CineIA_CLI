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

#include "RendererPerformanceUtils.h"
#include "TestConstants.h"
#include "CPUTime.h"
#include "testcfg.h"
#include "TestUtils.h"
#include <stdexcept>

IABPerformanceRenderer::IABPerformanceRenderer()
{
    iabRenderer_ = NULL;
    rendererConfig_ = NULL;
}

IABPerformanceRenderer::~IABPerformanceRenderer()
{
    CleanUp();
}

void IABPerformanceRenderer::MeasureMaxComplexity()
{
    double frameRenderingRate = 0;
    
    // Measuring frame rendering rate
    std::cout << "\nMeasuring frame rendering rate with selected configuration ......" << std::endl << std::flush;

    frameRenderingRate = MeasureFramesRenderingRate(IAB91OHcfg);
    std::cout << "\tFrame rendering rate: " << frameRenderingRate << " frames/sec" << std::endl;
    
}

double IABPerformanceRenderer::MeasureFramesRenderingRate(std::string iConfigString)
{
    double frameRenderingRate = 0;
    
	// Construct Max Complexity Frame, for rendering performance measurment
	maxComplexityFrame_.Construct();
	// IABFrame to be rendered
	IABFrameInterface* testIABFrame = maxComplexityFrame_.GetMaxComplexityFrame();

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
    
    // Create renderer output buffer
    float *outBuffer = new float[outputChannelCount * frameSampleCount];
    float **outPointers = new float*[outputChannelCount];
    for(uint32_t i = 0; i < outputChannelCount; i++)
    {
        outPointers[i] = outBuffer + i * frameSampleCount;
    }

    // Initialise test time
    _cpu_time starttime;
    _cpu_time endtime;
    double actualTestTime = 0;
    int32_t totalFramesRendered = 0;
    iabError returnCode = kIABNoError;
    IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
    
    // Use current time as start time
    getCPUTime(&starttime);    
    
    // ************************
    // Measure
    // ************************

    // Run rendering on the same IAB frame and measure total time taken
    for (totalFramesRendered = 0; actualTestTime < kIABTargetTestTime; totalFramesRendered++)
    {
        returnCode = iabRenderer->RenderIABFrame(*testIABFrame, outPointers, outputChannelCount, frameSampleCount, renderedOutputSampleCount);
        
        if (returnCode != kIABNoError)
        {
            delete [] outBuffer;
            delete [] outPointers;

            CleanUp();
            throw std::runtime_error("Unable to successfully render the IAB frame.");
        }
        
        getCPUTime(&endtime);
        actualTestTime = diffCPUTime(&endtime, &starttime);
    }
    
    // Clean up
    delete [] outBuffer;
    delete [] outPointers;
    
    delete iabRenderer;
    iabRenderer = NULL;
    
    delete rendererConfig;
    rendererConfig = NULL;
    
    frameRenderingRate = ((double)totalFramesRendered) * 1e9 / actualTestTime;  // 1e9: CPUTime class internally measures in nano seconds
    
    return frameRenderingRate;
}

void IABPerformanceRenderer::CleanUp()
{
    if (iabRenderer_)
    {
        delete iabRenderer_;
    }
    
    if (rendererConfig_)
    {
        delete rendererConfig_;
    }
}


