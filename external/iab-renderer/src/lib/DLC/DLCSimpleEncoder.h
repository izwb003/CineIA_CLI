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

#ifndef DLC_SIMPLEENCODER_H
#define DLC_SIMPLEENCODER_H

#include "DLCCodec.h"

namespace dlc
{

    /**
    * Implements a simple DLC encoder that supports only 48 kHz samples, uses no predictors, and uses only PCM residuals.
    * Supports the frame rates specified in ST 2098-2.
    */
    class SimpleEncoder : public Encoder
    {
    public:

		// Constructor
		SimpleEncoder();

		// Destructor
		virtual ~SimpleEncoder();

		enum StatusCode
        {
            StatusCode_OK,
            StatusCode_FATAL_ERROR	/**< Unrecoverable error occurred. */
        };

		// Set up per sample rate & frame rate
		StatusCode setup(SampleRate iSampleRate, FrameRate iFrameRate);

		/**
		* Note that this DLC encoder implementation minimally compress PCM data using a method as
		* "minimum width signed magnitude PCM" as specified in Annex B.11 of ST2098-2 specification.
		*
		* @throws std::invalid_argument if sampleRateCode is not supported
        *                               if sampleCount is not supported
        * @throws std::runtime_error if there is any inconsistency in dlcData
        */
        void encode(const int32_t * iSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, AudioData& oDLCData);

        /**
        * Identical semantics to encode(), but catches all exceptions and returns an error code instead.
        *
        * @return Status following the encoding operation
        */
        StatusCode encode_noexcept(const int32_t * iSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, AudioData& oDLCData);

		/**
		* Low pass filtering 96kHz input to band limit to 0~24kHz. This is called before down sampling
		* 96kHz input to base 48kHz, by 2.
		*
		* Input and output sample both in int32_t form. Unity gain for pass band. Stop band attenuation
		* approximately -70dB. LPF -3dB cutoff freq = ~23kHz.
		*
		* @param[in] iSamples Pointer to a sequence of iSampleCount PCM sample. 
		*            Must not be nullptr.
		* @param[in] iSampleCount Number of samples to encode. Must be larger than 0.
		* @param[in] iSampleRateCode Sample rate of the samples
		* @param[in/out] oSamples pointer to buffer for storing LPF-filtered output samples.
		* @throws std::invalid_argument If iSamples is NULL, or iSampleCount is 0, or iSampleRateCode is not 96k.
		*/
		StatusCode lpfFilter96k(int32_t * iSamples, uint32_t iSampleCount, SampleRate iSampleRateCode, int32_t * oSamples);

	private:

		// Downsampling to 48k integer
		StatusCode downSampling96kTo48k(int32_t * iSamples, uint32_t iSampleCount, int32_t * oSamples);

		// Upsampling back to 96k
		// This is a fixed-point processing to ensure lossless when decoding
		StatusCode upSampling48kTo96k(int32_t * iSamples, int32_t * oSamples);

		// Delay 96k sample by "LowPassFilterCoeffs + upsampling" filter group delay of 80 samples
		StatusCode delayPCM96k(const int32_t * iSamples, uint32_t iSampleCount, int32_t * oSamples);

		// *** Variables

		// Frame size in PCM samples
		uint32_t frameSize_;

		// Number of DLC sub-blocks
		uint8_t numDLCSubBlocks_;

		// DLC sub-blocks size, in PCM samples
		uint32_t subBlockSize_;

		// Delay buffer (interger) for accounting delays from LowPassFilterCoeffs and upsampling
		// 64 + 16 = 80.
		uint32_t delayBuffer96k_[kDLCTotal96kFiltGrpDelay];

		// 96k anti-aliasing filtering
		//

		// Delay line for running filter, with buffer size = (kDLCMaxFrameSize96k + kDLCLPF96kFiltOrder)
		int32_t lpfDelayline_[kDLCMaxFrameSize96k+ kDLCLPF96kFiltOrder];
	};

} // namespace dlc

#endif // #ifndef DLC_SIMPLEENCODER_H
