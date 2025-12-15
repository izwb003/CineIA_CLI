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

/**
 * Header file for common IAB utility functions.
 *
 * @file
 */


#ifndef __IABUTILITIES_H__
#define	__IABUTILITIES_H__

#include <string.h>
#include <math.h>
#include <string>

#include "IABDataTypes.h"
#include "common/IABElements.h"

/**************************************************************************/
/**
 *
 * Utilities for IAB parameter values that are defined, or preset, 
 * or calculated using methods as specified by IAB specification.
 *
 *****************************************************************************/

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
     /**
     * Function to return "number of sub blocks".
     *
     * @param[in] iFrameRate Frame rate, input.
     * @returns uint8_t, Number of sub blocks.
     */
    uint8_t GetIABNumSubBlocks(IABFrameRateType iFrameRate);
    
    /**
     * Function to return "number of samples in a frame".
     *
     * @param[in] iFrameRate Frame rate, input.
     * @param[in] iSampleRate Sample rate, input.
     * @returns uint32_t, Number of sample in an IAB frame.
     */
    uint32_t GetIABNumFrameSamples(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate);

	/**
	* Calculate MaxRendered parameter value from a list of frame sub-elements, per ST2098-2 spec
	* definition of MaxRendered.
	*
	* @param[in] iFrameSubElements Frame sub-element list.
	* @param[out] oMaxRendered Calculated MaxRendered value.
	* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
	*/
	iabError CalculateFrameMaxRendered(const std::vector<IABElement*> iFrameSubElements, IABMaxRenderedRangeType &oMaxRendered);

}
}

#endif // __IABUTILITIES_H__
