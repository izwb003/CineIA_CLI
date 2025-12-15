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

#include "DLC/DLCSimpleEncoder.h"
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

namespace dlc
{
	// Constructor
	SimpleEncoder::SimpleEncoder()
	{
		// Default to 48kHz, 24 fps
		frameSize_ = 2000;
		numDLCSubBlocks_ = 10;
		subBlockSize_ = 200;

		// Initialize lpfDelayline_ to 0
		std::fill(lpfDelayline_, (lpfDelayline_ + kDLCMaxFrameSize96k + kDLCLPF96kFiltOrder), 0);

		// Init delayBuffer96k_ to 0
		std::fill(delayBuffer96k_, (delayBuffer96k_ + kDLCTotal96kFiltGrpDelay), 0);
	}

	// Destructor
	SimpleEncoder::~SimpleEncoder()
	{
	}

	SimpleEncoder::StatusCode SimpleEncoder::setup(SampleRate iSampleRate, FrameRate iFrameRate)
	{
		// Reset/clear delay buffers
		std::fill(lpfDelayline_, (lpfDelayline_ + kDLCMaxFrameSize96k + kDLCLPF96kFiltOrder), 0);
		std::fill(delayBuffer96k_, (delayBuffer96k_ + kDLCTotal96kFiltGrpDelay), 0);

		/* The following computes the 48 kHz sub block length based on the permitted frame size (in samples),
		* according to ST 2098-2.
		*/
		if (iSampleRate == eSampleRate_48000)
		{
			switch (iFrameRate)
			{
			case eFrameRate_24:
				frameSize_ = 2000;
				numDLCSubBlocks_ = 10;
				subBlockSize_ = 200;
				break;

			case eFrameRate_25:
				frameSize_ = 1920;
				numDLCSubBlocks_ = 10;
				subBlockSize_ = 192;
				break;

			case eFrameRate_30:
				frameSize_ = 1600;
				numDLCSubBlocks_ = 8;
				subBlockSize_ = 200;
				break;

			case eFrameRate_48:
				frameSize_ = 1000;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 200;
				break;

			case eFrameRate_50:
				frameSize_ = 960;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 192;
				break;

			case eFrameRate_60:
				frameSize_ = 800;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 200;
				break;

			case eFrameRate_96:
				frameSize_ = 500;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 100;
				break;

			case eFrameRate_100:
				frameSize_ = 480;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 120;
				break;

			case eFrameRate_120:
				frameSize_ = 400;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 100;
				break;

			default:
				return StatusCode_FATAL_ERROR;
			}
		}
		else if (iSampleRate == eSampleRate_96000)
		{
			switch (iFrameRate)
			{
			case eFrameRate_24:
				frameSize_ = 4000;
				numDLCSubBlocks_ = 10;
				subBlockSize_ = 400;
				break;

			case eFrameRate_25:
				frameSize_ = 3840;
				numDLCSubBlocks_ = 10;
				subBlockSize_ = 384;
				break;

			case eFrameRate_30:
				frameSize_ = 3200;
				numDLCSubBlocks_ = 8;
				subBlockSize_ = 400;
				break;

			case eFrameRate_48:
				frameSize_ = 2000;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 400;
				break;

			case eFrameRate_50:
				frameSize_ = 1920;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 384;
				break;

			case eFrameRate_60:
				frameSize_ = 1600;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 400;
				break;

			case eFrameRate_96:
				frameSize_ = 1000;
				numDLCSubBlocks_ = 5;
				subBlockSize_ = 200;
				break;

			case eFrameRate_100:
				frameSize_ = 960;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 240;
				break;

			case eFrameRate_120:
				frameSize_ = 800;
				numDLCSubBlocks_ = 4;
				subBlockSize_ = 200;
				break;

			default:
				return StatusCode_FATAL_ERROR;
			}
		}
		else
		{
			return StatusCode_FATAL_ERROR;
		}

		return StatusCode_OK;
	}


	/**
	* See Minimal DLC Encoder at ST 2098-2 for a description of the algorithm.
	*/
	void SimpleEncoder::encode(const int32_t * iSamples
		, uint32_t iSampleCount
		, SampleRate iSampleRateCode
		, AudioData& oDLCData)
	{
		if (iSamples == NULL || iSampleCount == 0)
		{
			throw std::invalid_argument("No samples provided.");
		}

		if ( ((iSampleRateCode != eSampleRate_48000) && (iSampleRateCode != eSampleRate_96000))
			|| (iSampleCount != frameSize_) )
		{
			throw std::invalid_argument("Invalid \"sampling rate & sample count (frame size)\" combination.");
		}

		oDLCData.setShiftBits(8);			// Change ShiftBits to 8 to match current IABFrame.bitDepth_ (default) setting
											// Note: "DLC AudioData.ShiftBits = 32 - IABFrame.bitDepth_". For 24-bit IAFrame, AudioData.ShiftBits is set to 8.

		// Shift input samples according to set value of DLC AudioData.ShiftBits
		// This should be applied here to discard unnecesary bits in corresponding LSB positions
		int32_t shiftedInputSamplesInt[kDLCMaxFrameSize96k];

		uint5_t shiftBits = oDLCData.getShiftBits();
		for (uint32_t i = 0; i < frameSize_; i++)
		{
			shiftedInputSamplesInt[i] = iSamples[i] >> shiftBits;
		}

		oDLCData.setSampleRate(iSampleRateCode);

		// predictors are not used 
		oDLCData.setNumPredRegions48(0);

		oDLCData.setNumPredRegions96(0);

		// set the number of sub blocks
		oDLCData.setNumDLCSubBlocks(numDLCSubBlocks_);

		// init pointer to 48k sample input
		const int32_t* inputSample48k = shiftedInputSamplesInt;

		// DLC sub-blocks size for 48k or 48k base band of 96k, in PCM samples
		// Init to subBlockSize_, ie for 48k encoding
		// In the case of 96k, it is half of subBlockSize_
		uint32_t subBlockSize48k = subBlockSize_;

		// For 96k encoding, split 96k into base band and extension (upper) band. This processing 
		// involves data delay, frame alignment
		//
		int32_t delayed96k[kDLCMaxFrameSize96k];

		int32_t lpfOutput96kInt[kDLCMaxFrameSize96k];						// int32_t output buffer for LPF filter
		int32_t lpfOutput48kInt[kDLCMaxFrameSize48k];

		int32_t upSampled96kInt[kDLCMaxFrameSize96k];

		// init 96k sample pointer to delayed96k
		const int32_t* inputSample96k = &delayed96k[0];

		// Run 96k pre-processing only when iSampleRateCode is 96k
		if (iSampleRateCode == eSampleRate_96000)
		{
			// 48k block size is half of subBlockSize_ for 96k encoding
			subBlockSize48k = subBlockSize_ >> 1;

			// LPF filtering 96k sample to band limit to below ~23kHz (anti-aliasing)
			lpfFilter96k(shiftedInputSamplesInt, frameSize_, eSampleRate_96000, lpfOutput96kInt);

			// Down sample by 2 to 48k, output in integer format
			downSampling96kTo48k(lpfOutput96kInt, frameSize_, lpfOutput48kInt);

			// Set pointer to beginning of baseband 48k samples
			inputSample48k = lpfOutput48kInt;

			// For extension band, upsampling by 2, baseband samples back to 96k
			upSampling48kTo96k(lpfOutput48kInt, upSampled96kInt);

			// Delay 96k integer sample by corresponding delay = (LPF + UpSampling) processing delay
			delayPCM96k(shiftedInputSamplesInt, frameSize_, delayed96k);

			// Calculate difference between delayed 96k and upsampled base band
			// Samples already time-aligned to minimize sample difference values
			for (uint32_t i = 0; i < frameSize_; i++)
			{
				delayed96k[i] -= upSampled96kInt[i];
			}
		}

		// fille the PCM residual sub blocks with the input PCM samples
		int32_t subBlockSample = 0;

		for (uint32_t i = 0; i < numDLCSubBlocks_; i++)
		{
			// DLC sub block residual coding for 48k (or base band of 96k)
			oDLCData.initDLCSubBlock48(i, eCodeType_PCM_CODE_TYPE, subBlockSize48k);

			PCMResidualsSubBlock *b = static_cast<PCMResidualsSubBlock*>(oDLCData.getDLCSubBlock48(i));

			if (b == NULL)
			{
				throw std::runtime_error("At least one 48k DLC Subblock is undefined.");
			}

			int32_t max = 0; ///< Keeps track of the maximum sample value in the subblock
			uint5_t shiftBits = oDLCData.getShiftBits();

			for (uint32_t j = 0; j < subBlockSize48k; j++)
			{
				subBlockSample = inputSample48k[i * subBlockSize48k + j];
				b->getResiduals()[j] = subBlockSample;
				max = std::max(abs(subBlockSample), max);
			}

			// computes the bit depth required to represent the maximum sample value
			uint5_t requiredBitDepth = 0;

			while (max > 0)
			{
				max >>= 1;
				requiredBitDepth++;
			}

			b->setBitDepth(requiredBitDepth);
			// ** End of 48k or baseband 48k coding

			// DLC sub block residual coding for 96k extension band 
			if (iSampleRateCode == eSampleRate_96000)
			{
				oDLCData.initDLCSubBlock96(i, eCodeType_PCM_CODE_TYPE, subBlockSize_);

				b = static_cast<PCMResidualsSubBlock*>(oDLCData.getDLCSubBlock96(i));

				if (b == NULL)
				{
					throw std::runtime_error("At least one 96k DLC Subblock is undefined.");
				}

				max = 0; ///< Keeps track of the maximum sample value in the subblock
				shiftBits = oDLCData.getShiftBits();

				for (uint32_t j = 0; j < subBlockSize_; j++)
				{
					subBlockSample = inputSample96k[i * subBlockSize_ + j];
					b->getResiduals()[j] = subBlockSample;
					max = std::max(abs(subBlockSample), max);
				}

				// computes the bit depth required to represent the maximum sample value
				requiredBitDepth = 0;

				while (max > 0)
				{
					max >>= 1;
					requiredBitDepth++;
				}

				b->setBitDepth(requiredBitDepth);
				// ** End of 96k extension band coding
			}
		}
	}

	SimpleEncoder::StatusCode SimpleEncoder::encode_noexcept(const int32_t * iSamples
		, uint32_t iSampleCount
		, SampleRate iSampleRateCode
		, AudioData& oDLCData)
	{
		try
		{
			this->encode(iSamples, iSampleCount, iSampleRateCode, oDLCData);
		}
		catch (...)
		{

			return StatusCode_FATAL_ERROR;

		}

		return StatusCode_OK;
	}

	SimpleEncoder::StatusCode SimpleEncoder::lpfFilter96k(int32_t * iSamples, uint32_t iSampleCount, SampleRate iSampleRateCode, int32_t * oSamples)
	{
		int32_t i, k;

		if (iSamples == NULL || iSampleCount == 0 || oSamples == NULL)
		{
			return StatusCode_FATAL_ERROR;
		}

		// Check if called iSampleCount agrees with frame size that is set up for this encoder
		// If not, exit.
		if (iSampleCount != frameSize_)
		{
			return StatusCode_FATAL_ERROR;
		}

		// Copy input into delay line for convolution/filtering
		// Note the sequence of samples is reversed in lpfDelayline_
		for (i = 0; i < static_cast<int32_t>(frameSize_); i++)
		{
			lpfDelayline_[frameSize_ - i - 1] = iSamples[i];
		}

		// Filter a frame/block (frameSize_) of samples
		// 64-bit accumulator
		int64_t accumulator = 0;
		for (i = 0; i < static_cast<int32_t>(frameSize_); i++)
		{
			accumulator = 0;
			for (k = 0; k < (kDLCLPF96kFiltOrder + 1); k++)
			{
				accumulator += LowPassFilterCoeffs[k] * lpfDelayline_[(frameSize_ - 1 - i) + k];
			}

			// Saving to output buffer
			// Dev note, as LowPassFilterCoeffs is multiplied by (2 ^ kDLCLPFCoeffIntBitLength)
			// bits during interger conversion, filtered outputs need to be scaled down accordingly.
			// Scaling down = (>> kDLCLPFCoeffIntBitLength)
			//
			oSamples[i] = static_cast<int32_t>(accumulator >> kDLCLPFCoeffIntBitLength);
		}

		// Shift samples in lpfDelayline_. Leave 1st frameSize_ free and prepare
		// for next interation
		for (i = (kDLCLPF96kFiltOrder - 1); i >= 0; i--)
		{
			lpfDelayline_[i + frameSize_] = lpfDelayline_[i];
		}

		return StatusCode_OK;
	}

	SimpleEncoder::StatusCode SimpleEncoder::delayPCM96k(const int32_t * iSamples, uint32_t iSampleCount, int32_t * oSamples)
	{
		int32_t i;

		int32_t swapBuffer[kDLCTotal96kFiltGrpDelay];

		if (iSamples == NULL || iSampleCount == 0 || oSamples == NULL
			|| (iSampleCount != frameSize_))
		{
			return StatusCode_FATAL_ERROR;
		}

		// Bypass but with delay = LowPassFilterCoeffs delay + upsampling delay
		// ** Dev note: implemented such that method can be called for in-place processing if necessary.

		// Copy input samples that are delayed to next frame to swapBuffer
		for (i = 0; i < kDLCTotal96kFiltGrpDelay; i++)
		{
			swapBuffer[i] = iSamples[i + frameSize_ - kDLCTotal96kFiltGrpDelay];
		}

		// Copy (frameSize_ - kDLCTotal96kFiltGrpDelay) delayed samples to output
		// This must be done  first before copy in samples delayed from last frame from
		// delayBuffer96k_ below
		for (i = frameSize_ - 1; i >= kDLCTotal96kFiltGrpDelay; i--)
		{
			oSamples[i] = iSamples[i - kDLCTotal96kFiltGrpDelay];
		}

		// Copy in samples delayed from last frame from delayBuffer96k_
		// These are placed before the delayed samples from current frame
		// Also copy samples in swapBuffer to delayBuffer96k_ (swapping)
		for (i = 0; i < kDLCTotal96kFiltGrpDelay; i++)
		{
			oSamples[i] = delayBuffer96k_[i];
			delayBuffer96k_[i] = swapBuffer[i];
		}

		return StatusCode_OK;
	}

	// Downsampling to 48k
	SimpleEncoder::StatusCode SimpleEncoder::downSampling96kTo48k(int32_t * iSamples, uint32_t iSampleCount, int32_t * oSamples)
	{
		for (uint32_t i = 0; i < iSampleCount / 2; i++)
		{
			oSamples[i] = iSamples[2 * i];
			iSamples[2 * i + 1] = 0;			// Prep for upsampling back to 96k
		}

		return StatusCode_OK;
	}

	// Upsampling 48k samples back to 96k (for dev test only)
	SimpleEncoder::StatusCode SimpleEncoder::upSampling48kTo96k(int32_t * iSamples, int32_t * oSamples)
	{
		int32_t buffer[64] = { 0 };
		int32_t index1 = 0;
		int32_t index2 = 0;
		int64_t accum64 = 0;
		int32_t i, k, n;

		if (iSamples == NULL || oSamples == NULL)
		{
			return StatusCode_FATAL_ERROR;
		}

		// Upsample 48 kHz samples to 96 kHz
		// Code below is a direct implementation as specified in ST2098-2, Section B.9
		//
		index1 = 0;

		for (n = 0, k = 0; n < static_cast<int32_t>(frameSize_); n += 2, k++)
		{
			buffer[index1] = iSamples[k];
			index2 = index1;
			index2 -= 8;
			index2 &= 63;
			oSamples[n] = buffer[index2];

			index2 = index1;
			accum64 = 0;
			for (i = 1; i < 33; i += 2)
			{
				accum64 += (static_cast<int64_t>(buffer[index2]) * InterpolatorFilterCoeffs[i]);
				index2--;
				index2 &= 63;
			}
			oSamples[n + 1] = static_cast<int32_t>(accum64 >> 15);

			index1++;
			index1 &= 63;
		}

		return StatusCode_OK;
	}

} // namespace dlc