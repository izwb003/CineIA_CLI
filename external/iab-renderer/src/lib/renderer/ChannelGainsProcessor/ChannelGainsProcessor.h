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
 * Header file for Channel Gains Processor module.
 *
 * @file
 */

#ifndef __CHANNELGAINSPROCESSOR_H__
#define __CHANNELGAINSPROCESSOR_H__

#include <map>

// Header files from "CoreUtils" library
#include "coreutils/VectDSPInterface.h"

// Header files from "RenderUtils" library
#include "renderutils/VirtualSources.h"
#include "renderutils/IRendererConfiguration.h"

// Header files of this library
#include "renderer/VBAPRenderer/VBAPRendererDataStructures.h"


// Constants used in gain smoothing allgorithm
#define MAX_RAMP_SAMPLES        4800							// maximum interpolation ramp size
#define MAX_SLOPE               0.00208333333333f				// maximum interpolation slope (corresponds to full range in 480 samples)
#define RAMP_SAMPLE_MAX_SLOPE   480								// Number of samples to rample over when MAX_SLOPE is used

namespace IABGAINSPROC
{

    /**
     * Data type for error codes returned by gains processor.
     *
     */
    typedef uint32_t gainsProcError;
    
	/**
	* Represents a set of channel gains from smoothing processing history.
	* Used to save/restore gains history.
	*
	*/
	struct EntityPastChannelGains
	{
		std::vector<float>	channelGains_;
		bool				touched_;      // Has any gain been set in the current frame
		bool                gainsValid_;   // Has any gain been set at all
										   // (only used in the multi-threaded renderer)

		EntityPastChannelGains() : 
			channelGains_(32 /* poor man's optimization */, 0.0f),
			touched_(false),
			gainsValid_(false)
		{
		}
	};

    enum gainsProcErrorCodes {
        
		kGainsProcNoError                           = 0,              /**< No error. */
		kGainsProcGeneralError                      = 1,              /**< Error: General error. */
		kGainsProcBadArgumentsError                 = 2,              /**< Error: Bad arguments. */
		kGainsProcMemoryError                       = 3,              /**< Error: Memory allocation failed. */
		kGainsProcDivisionByZeroError               = 4               /**< Error: Division by zero error.. */
		
	};
	
	/**
	*
	* Channel gains processor class, for applying channel gains.
	*
	*/
	class ChannelGainsProcessor
	{
	public:

		/// Constructor
		ChannelGainsProcessor();

		/// Destructor
		~ChannelGainsProcessor();

		/**
		* Update gains history for smoothing.
		* It clears out unused entries in gains history and retains used entries.
		*
		*/
		virtual void UpdateGainsHistory();

		/**
		* Reset gains history entityGainHistory_ to empty.
		*
		*/
		virtual void ResetGainsHistory(void);

		/**
		* ApplyChannelGains() is a gain processing function. It does not own any PCM sample memory
		* internally. Instead, it simply applies gains in (iTargetChannelGains)
		* to (*iInputSamples), and store output samples to (**oOutputsamples).
		* 
		* For smoothing processing, ApplyChannelGains() save a copy of iObjectID-mapped channel
		* gains internally, as history, to support ObjectID-based smoothing processing. 
		*
		* Caller must ensure all buffers are properlly allocated and set up before calling the API
		* for required gain processing. Caller has the ownership to all buffers prior to, and after
		* calling ApplyChannelGains().
		*
		* - (iObjectID) is the ID of the object for which ApplyChannelGains() is used. For smoothing
		*   processing, previous saved channel gains are retrieved as the start gains to ramp up (or down)
		*   to what is in (iTargetChannelGains).
		*
		* - (iInputSamples) points to the start of input/source PCM samples, for 1 object or channel.
		*   The number of samples is (iSampleCount).
		*
		* - (oOutputsamples) contains an array of (iOutputChannelCount) pointers. Each points to the
		*   start of channel output buffer. There are (iOutputChannelCount) output channels. For each
		*   of output channels, (iSampleCount) output samples are generated.
		*
		* - (iTargetChannelGains) contains a set of (iOutputChannelCount) channel gains that are applied
		*   to (*iInputSamples) to generate (iOutputChannelCount) channels of PCM output, each containing
		*   (iSampleCount) output samples.
		*
		* - (iInitializeOutputBuffers), when true, buffers pointed from (oOutputsamples) are initilized to
		*   0.0f before gain processing. If false, no initialization is carried out. Gain-processed output
		*   samples are accummulated/aggregated/added into existent values in (oOutputsamples) buffers.
		*
		* - (iEnableSmoothing), when true, ApplyChannelGains() applys gains using a smoothing algorithm,
		*   from current/start gains (retrieved from internal held history per iObjectID) to target/end gains
		*   (iTargetChannelGains). Gains at the end of (iSampleCount) is saved back to internal history per
		*   iObjectID, for smoothing processing in next time segment.
		*   When (iEnableSmoothing is false, smoothing is disabled. Channel gains in (iTargetChannelGains)
		*   are applied uniformly. A copy of (iTargetChannelGains) is saved to internaly history, as
		*   "current gains" at the end of (iSampleCount) samples.
		*
		* @param[in] iObjectID - object ID for which channel gains processing is carried out
		* @param[in] iInputSamples - pointer to input PCM samples
		* @param[in] iSampleCount - number of PCM samples, either input or each of output channel buffers
		* @param[out] oOutputSamples - pointer to an array of pointers, each corresponding to a channel output buffer
		* @param[in] iOutputChannelCount - number of output channels
		* @param[in] iInitializeOutputBuffers - when true, init to "0" on all channel output buffers
		* @param[in] iTargetChannelGains - channels gains to be applied, or target channel gains when smoothing is enabled
		* @param[in] iEnableSmoothing - when true, smoothing is enabled. When flase, disabled and iTargetChannelGains is applied uniformly.
		* @return \link gainsProcError \endlink if no errors. Otherwise error condition.
		*/
		virtual gainsProcError ApplyChannelGains(
			int32_t iObjectID
			, const float *iInputSamples
			, uint32_t iSampleCount
			, float **oOutputSamples
			, uint32_t iOutputChannelCount
			, bool iInitializeOutputBuffers
			, const std::vector<float>& iTargetChannelGains
			, bool iEnableSmoothing
			);

	private:

		// Object VBAP gain history. Used to support preceding object channel gains, per object ID,
		// to support smoothing processing (when enabled)
		//
		std::map<int32_t, EntityPastChannelGains>   entityGainHistory_;
	};

} // namespace IABGAINSPROC

#endif // __CHANNELGAINSPROCESSOR_H__
