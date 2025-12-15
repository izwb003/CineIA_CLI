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

#ifndef __IABPERFORMANCEDECODE_H__
#define __IABPERFORMANCEDECODE_H__

#include "IABMaxComplexityFrame.h"
#include "IABParserAPI.h"
#include "IABRenderer.h"

using namespace SMPTE::ImmersiveAudioBitstream;

/**
 * Class to measure frame decoding rate on a target platform by measuring the time taken
 * to run specific use cases.
 *
 * Decodeing = Parsing + Rendering.
 */

class IABPerformanceDecode
{
public:
    IABPerformanceDecode();
    ~IABPerformanceDecode();
    
    /// Runs complexity measurements
    void MeasureMaxComplexity();
    
private:
    
    // Measures frame parsing rate for the bed type and object setting
    double MeasureFramesDecodingRate(std::string iConfigString);

    // Clean up test and delete allocated memory
    void CleanUp();
    
	// Maximum Complexity Frame for measurement, per MCF spec
	IABMaxComplexityFrame   maxComplexityFrame_;

    // IAB parser instance for measurement
    IABParserInterface*     iabParser_;

	// IABRenderer for the measurement
	IABRenderer                 *iabRenderer_;

	// Renderer configuration
	RenderUtils::IRendererConfiguration *rendererConfig_;
};
#endif // __IABPERFORMANCEDECODE_H__
