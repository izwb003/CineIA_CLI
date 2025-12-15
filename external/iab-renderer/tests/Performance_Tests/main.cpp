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

#include "ParserPerformanceUtils.h"
#include "RendererPerformanceUtils.h"
#include "DecodePerformanceUtils.h"
#include "DLCDecPerformanceUtils.h"
#include "IABMaxComplexityFrame.h"


//========================================================================
void DisplayProgramID()
{
    printf("\n\t-----   Xperi ProAudio IAB Library performance measurement utility   %s, %s ------\n\n", __DATE__, __TIME__);
    fflush(stdout);
};

void ShowToolInformation()
{
    // Display Tool description
    printf("Tool description:\n"
           "This application is designed to measure IAB library performance of an implementation on a target platform.\n"
           "The measurement is done by running various test cases and calculating a performance rating for each test case.\n"
           "The following rating will be measured:\n"
           "     1. DLC frame decoding rate,\n"
	       "     2. IAB frame (MCF) parsing rate,\n"
		   "     3. IAB frame (MCF) rendering rate,\n"
		   "     4. IAB frame (MCF) decoding (parsing + rendering) rate.\n\n"
           );
}

int main(int argc, const char * argv[])
{
    DisplayProgramID();
    
    ShowToolInformation();

    // Run DLC decoder measurements
    IABPerformanceDLCDecoder testDLCDecoder;
    testDLCDecoder.RunMeasurements();
    
    // Run parser measurements
    IABPerformanceParser testParser;
    testParser.MeasureMaxComplexity();
    
    // Run renderer measurements
    IABPerformanceRenderer testRenderer;
    testRenderer.MeasureMaxComplexity();
    
	// Run full decode measurements
	IABPerformanceDecode testDecoder;
	testDecoder.MeasureMaxComplexity();

	return 0;
}
