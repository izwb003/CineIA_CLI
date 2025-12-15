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
 * IABDecorrelation.h
 *
 * @file
 */

#ifndef __IABDECORRELATION_H__
#define __IABDECORRELATION_H__

#include "IABDataTypes.h"
#include "renderutils/IRendererConfiguration.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	/**
	* struct containing parameters for a 2-level allpass filter.
	*/
	typedef struct L2APFFilterParamers
	{
		float    outerGain_;
		int32_t  outerDelayLength_;
		float    innerGain_;
		int32_t  innerDelayLength_;
	} L2APFFilterParamers;

	/// Maximum delay length in sample for designed nested, allpass filters
	static const uint32_t kNestedAllpassL2LengthMax = 155;

	/**
	* Class for a nested allpass filter structure (based on nested comb filters)
	*
	* (Defined from struct code as from DTS pps-common-dsp lib.)
	*
	*/
	class NestedAllPassFilter
	{
	public:

		/// Default constructor
		NestedAllPassFilter();

		/// Paramerized constructor
		NestedAllPassFilter(float iGainCoeff
			, int32_t iDelayLength
			, NestedAllPassFilter* iChildAPF);

		/// Destructor
		~NestedAllPassFilter();

		/**
		* Clears delayBuffer_ and readWritePointer_, for itself and any child APFs
		* It is recommended that caller to call Reset() once on the outer most APF once a nested
		* APF instance is created.
		*
		* Note that Reset() does NOT modify gains, delay length and enable flag throughout
		* the nest hierarchy.
		*/
		void Reset();

		/// Enable()
		void Enable() {	isEnabled_ = true; }

		/// Disable()
		void Disable() { isEnabled_ = false; }

		/// IsEnabled()
		bool IsEnabled() { return isEnabled_; }

		/**
		* Decorrelate PCM samples. In-place sample buffer processing.
		*
		* @param[in, out] ioPCMSample pointer to input (also decorr'ed output) samples 
		* @param[in] iSampleCount number of PCM samples to process in ioPCMSample buffer
		*
		*/
		void Decorrelate(IABSampleType* ioPCMSample, IABRenderedOutputSampleCountType iSampleCount);

		// ******* APF member variables
		// (This is a lib internal function.
		//  All variables in public to facilitate simplified, nested decorrelation processing at outer level.)
		//
		float    gainCoeff_;								/**< Allpass filter coefficient */
		int32_t  delayLength_;								/**< Actual delay length in samples */
		float    delayBuffer_[kNestedAllpassL2LengthMax];   /**< Delay buffer */
		int32_t  readWritePointer_;							/**< Read/write pointer */
		NestedAllPassFilter* innerAPF_;						/**< pointer to the inner allpass if it exists, otherwise this should be set to NULL */

		// APF active flag
		bool     isEnabled_;								/**< Active flag. Non-VBAP speakers/channels in output list are disabled. */
	};

	/**
     * IABDecorrelation class
	 * 
	 * (IAB SDK v1.1 onward: Currently supporting object decorrelation in binary ON/OFF mode only.)
	 *
     */
    class IABDecorrelation
    {
    public:
        
		/// Constructor
		IABDecorrelation();
	
        /// Destructor
        ~IABDecorrelation();
        
		/**
		* Set up IABDecorrelation instance based on iConfig.
		* 
		* For each of the VBAP outputs, a NestedAllPassFilter instance is created for decorrelating
		* the rendered output.
		*
		* @memberof IABDecorrelation
		*
		* @param[in] iConfig target configuration
		*/
        iabError Setup(const RenderUtils::IRendererConfiguration &iConfig);

		/**
		* Clears delayBuffer_ and readWritePointer_, for all decorrelators.
		*
		* Note that Reset() does NOT modify gains, delay length and enable flag of any decorrelator.
		*/
		void Reset();

		/**
		* Process decorrelation to all D-Output, each using its own L2 nested APF decorrelator filters.
		* 
		* Input and output samples are stored in the sample buffers (in-place processing), for all 
		* channels/speakers.
		*
		* Memory backing ioOutputChannels is allocated and owned by the caller, and not used by the 
		* IABDecorrelation instance internally, except accepting input and sending output to caller.
		*
		* @memberof IABDecorrelation
		*
		* @param[in,out] ioOutputChannels Pointer to an array of iOutputChannelCount pointers, each 
		*                corresponding to an audio channel and each pointing to an array of 
		*                iOutputSampleBufferCount samples.
		* @param[in]     iOutputChannelCount Number of output channels. Must matching config that is used
		*                set up this IABDecorrelation instance.
		* @param[in]     iOutputSampleBufferCount Number of input/output samples per channel.
		* @return \link iabKNoError \endlink if no errors occurred.
		*/
		iabError DecorrelateDecorOutputs(IABSampleType **ioOutputChannels
                                        , IABRenderedOutputChannelCountType iOutputChannelCount
                                        , IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:
		
		// A bank (vector) of decorrelators
		std::vector<NestedAllPassFilter*> decorrelators_;

		// Number of output channels/speakers in target config
		uint32_t        numOutputChannels_;
	};
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABDECORRELATION_H__
