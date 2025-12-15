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

#ifndef __IABPERFORMANCERENDERER_H__
#define __IABPERFORMANCERENDERER_H__

#include <stdio.h>
#include <string>

#include "IABMaxComplexityFrame.h"
#include "common/IABElements.h"
#include "IABRenderer.h"

/**
 * Class to test IAB Renderer performance on a target platform by measuring the time taken
 * to run specific rendering use cases.
 *
 * Procedure per test case:
 * 1. Construct IABFrame per test case requirement. Note all current test cases have one object in frame.
 * 2. Instantiate IABRenderer per test case configuration
 * 3. Run IABRender on the test frame in a loop for the target test period, one RenderIABFrame() call per loop
 * 4. Calculate and report frames rendered per second.
 */

using namespace SMPTE::ImmersiveAudioBitstream;

class IABPerformanceRenderer
{
public:
    IABPerformanceRenderer();
    ~IABPerformanceRenderer();
    
    /// Runs complexity measurements
    void MeasureMaxComplexity();
    
private:

    // Measures frame rendering rate on MCF, using the specified renderer configuration
    double MeasureFramesRenderingRate(std::string iConfigString);
    
    // Clean up test and delete allocated memory
    void CleanUp();    

	// Maximum Complexity Frame for measurement, per MCF spec
	IABMaxComplexityFrame       maxComplexityFrame_;

    // IABRenderer for the measurement
    IABRenderer                 *iabRenderer_;

    // Renderer configuration
    RenderUtils::IRendererConfiguration *rendererConfig_;
};

#endif // __IABPERFORMANCERENDERER_H__
