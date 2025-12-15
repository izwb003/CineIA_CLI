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

#include "DLC/DLCFullDecoder.h"
#include <algorithm>
#include <stdexcept>

namespace dlc
{
	// Private structure that allows arrays of integers to be stores as elements of std::vector<>
	struct _TempCoeffs
	{
		int32_t c_[32];
	};

	// *** See ST 2098-2 for a description of the algorithm.
	//
	void FullDecoder::decode(int32_t * oSamples
		, uint32_t iSampleCount
		, SampleRate iSampleRateCode
		, const AudioData& iDLCData)
	{
		// Flag is the decoding is in "decoding 96k iDLCData into 48k output" mode
		// Default to false.
		bool isDecoding96kTo48kOutput = false;

		if (oSamples == NULL || iSampleCount == 0)
		{
			throw std::invalid_argument("No samples provided.");
		}

		SampleRate dlcSampleRate = iDLCData.getSampleRate();

		// Set decoding based on client's desired output sample rate, iSampleRateCode
		switch (iSampleRateCode)
		{
		case eSampleRate_48000:
			if (iDLCData.getSampleCount48() != iSampleCount)
			{
				throw std::invalid_argument("Output sample buffer size does not match coded signal size");
			}

			// If iDLCData contains 96k, set isDecoding96kTo48kOutput to enable decoding
			// 96k to 48k output only
			if (dlcSampleRate == eSampleRate_96000)
			{
				isDecoding96kTo48kOutput = true;
			}

			break;

		case eSampleRate_96000:
			if (dlcSampleRate != eSampleRate_96000)
			{
				throw std::invalid_argument("Cannot decode to 96 kHz from 48 kHz coded signal.");
			}

			if (iDLCData.getSampleCount96() != iSampleCount)
			{
				throw std::invalid_argument("Output sample buffer size does not match coded signal size");
			}

			break;
		}

		// The samples vector is used to hold intermediary results during the decoding process.
		//
		int32_t stride48 = 1;

		// In particular, in the case of decoding to full 96 kHz output samples, the 48 kHz base 
		// layer samples are stored every second sample (stride = 2)
		if ((iSampleRateCode == eSampleRate_96000) && !isDecoding96kTo48kOutput)
		{
			stride48 = 2;
		}

		// *** extract 48 kHz layer residuals
		//
		int32_t* curSample = oSamples;

		for (uint8_t i = 0; i < iDLCData.getNumDLCSubBlocks(); i++)
		{
			const ResidualSubBlock *rsb = iDLCData.getDLCSubBlock48(i);

			if (rsb == NULL)
			{
				throw std::runtime_error("At least one 48 kHz DLC Subblock is undefined.");
			}

			// inverse entropy coding

			if (rsb->getCodeType() == eCodeType_PCM_CODE_TYPE)
			{
				const PCMResidualsSubBlock *pcmr = static_cast<const PCMResidualsSubBlock*>(rsb);

				const int32_t *curRes = pcmr->getResiduals();
				const int32_t *endRes = curRes + pcmr->getSize();

				while (curRes < endRes)
				{
					*curSample = *curRes;
					curRes++;
					curSample += stride48;
				}
			}
			else
			{
				const RiceResidualsSubBlock *res = static_cast<const RiceResidualsSubBlock*>(rsb);

				const RiceResidual * curRes = res->getResiduals();
				const RiceResidual *endRes = curRes + res->getSize();

				while (curRes < endRes)
				{
					*curSample = curRes->getSign() * ((curRes->getQuotient() << res->getRiceRemBits()) + curRes->getRemainder());
					curRes++;
					curSample += stride48;
				}
			}
		}  // End of "for (uint8_t i = 0; i < iDLCData.getNumDLCSubBlocks(); i++)"

		// 48 kHz Lattice Prediction Coefficients to Direct Form Coefficients

		std::vector<_TempCoeffs> aCoeffs48(iDLCData.getNumPredRegions48());

		for (uint2_t n = 0; n < iDLCData.getNumPredRegions48(); n++)
		{
			int32_t aCoeffsTemp[32];

			aCoeffs48[n].c_[0] = 1048576;

			const PredRegion &pr = iDLCData.getPredRegion48(n);

			for (int32_t j = 1; j <= pr.getOrder(); j++)
			{
				int32_t kCoeff48_nj = pr.getKCoeff()[j];

				kCoeff48_nj -= 512;
				kCoeff48_nj <<= 11;

				aCoeffs48[n].c_[j] = 0;

				for (int32_t k = 1; k <= j; k++)
				{
					int64_t accum64 = static_cast<int64_t>(kCoeff48_nj) * aCoeffs48[n].c_[j - k];
					aCoeffsTemp[k] = aCoeffs48[n].c_[k] + static_cast<int32_t>(accum64 >> 20);
				}

				for (int32_t k = 1; k <= j; k++)
				{
					aCoeffs48[n].c_[k] = aCoeffsTemp[k];
				}
			}
		}  // End of "for (uint2_t n = 0; n < iDLCData.getNumPredRegions48(); n++)"

		// apply filters to 48 KHz residuals

		curSample = oSamples;

		int32_t buffer[64] = { 0 };

		int32_t index1 = 0;

		uint8_t curBlock = 0;

		// Inverse prediction processing only when number of predictor region is greater than 0.
		// oSamples already contain residual samples, which are PCM samples when no prediction.
		if (iDLCData.getNumPredRegions48() > 0)
		{
			for (uint2_t i = 0; i < iDLCData.getNumPredRegions48(); i++)
			{
				const PredRegion &pr = iDLCData.getPredRegion48(i);

				for (uint4_t j = 0; j < pr.getRegionLength(); j++)
				{
					const ResidualSubBlock *rsb = iDLCData.getDLCSubBlock48(curBlock);

					if (rsb == NULL)
					{
						throw std::runtime_error("At least one 48 kHz DLC Subblock is undefined.");
					}

					for (uint32_t k = 0; k < rsb->getSize(); k++)
					{
						int32_t index2 = index1;
						int64_t accum64 = 0;

						for (int32_t p = 1; p <= pr.getOrder(); p++)
						{
							accum64 -= (static_cast<int64_t>(buffer[index2]) *
								static_cast<int64_t>(aCoeffs48[i].c_[p]));

							index2--;
							index2 &= 63;
						}

						int32_t output = static_cast<int32_t>(accum64 >> 20);
						output += *curSample;

						index1++;
						index1 &= 63;
						buffer[index1] = output;

						*curSample = output;
						curSample += stride48;
					}
				}
			}
		}  // End of "if (iDLCData.getNumPredRegions48() > 0)"

		// *** handle 96 kHz layer
		//
		if (dlcSampleRate == eSampleRate_96000 && !isDecoding96kTo48kOutput)
		{
			// 96 kHz layer residuals

			std::vector<int32_t> residuals96(iSampleCount, 0);

			std::vector<int32_t>::iterator curResidual96 = residuals96.begin();

			for (uint8_t i = 0; i < iDLCData.getNumDLCSubBlocks(); i++)
			{
				const ResidualSubBlock *rsb = iDLCData.getDLCSubBlock96(i);

				if (rsb == NULL)
				{
					throw std::runtime_error("At least one 96 kHz DLC Subblock is undefined.");
				}

				if (rsb->getCodeType() == eCodeType_PCM_CODE_TYPE)
				{
					const PCMResidualsSubBlock *pcmr = static_cast<const PCMResidualsSubBlock*>(rsb);

					std::copy(pcmr->getResiduals(), pcmr->getResiduals() + pcmr->getSize(), curResidual96);

					curResidual96 += pcmr->getSize();
				}
				else
				{
					const RiceResidualsSubBlock *res = static_cast<const RiceResidualsSubBlock*>(rsb);

					const RiceResidual *curRes = res->getResiduals();
					const RiceResidual *endRes = res->getResiduals() + res->getSize();

					while (curRes < endRes)
					{
						*(curResidual96++) = curRes->getSign() * ((curRes->getQuotient() << res->getRiceRemBits()) + curRes->getRemainder());

						curRes++;
					}
				}
			}  // End of "for (uint8_t i = 0; i < iDLCData.getNumDLCSubBlocks(); i++)"

			// 96 kHz Lattice Prediction Coefficients to Direct Form Coefficients

			std::vector<_TempCoeffs> aCoeffs96(iDLCData.getNumPredRegions96());

			for (uint2_t n = 0; n < iDLCData.getNumPredRegions96(); n++)
			{
				int32_t aCoeffsTemp[32];

				aCoeffs96[n].c_[0] = 1048576;

				const PredRegion &pr = iDLCData.getPredRegion48(n);

				for (int32_t j = 1; j <= pr.getOrder(); j++)
				{
					int32_t kCoeff96_nj = pr.getKCoeff()[j];

					kCoeff96_nj -= 512;
					kCoeff96_nj <<= 11;

					aCoeffs96[n].c_[j] = 0;

					for (int32_t k = 1; k <= j; k++)
					{
						int64_t accum64 = static_cast<int64_t>(kCoeff96_nj) * aCoeffs96[n].c_[j - k];
						aCoeffsTemp[k] = aCoeffs96[n].c_[k] + static_cast<int32_t>(accum64 >> 20);
					}

					for (int32_t k = 1; k <= j; k++)
					{
						aCoeffs96[n].c_[k] = aCoeffsTemp[k];
					}
				}
			}  // End of "for (uint2_t n = 0; n < iDLCData.getNumPredRegions96(); n++)"

			// apply filters to 96 KHz residuals

			curResidual96 = residuals96.begin();

			std::fill(buffer, (buffer + 64), 0);

			index1 = 0;

			curBlock = 0;

			// Inverse prediction processing only when number of predictor region is greater than 0.
			// residuals96 already contain residual samples, which are PCM samples when no prediction.
			if (iDLCData.getNumPredRegions96() > 0)
			{
				for (uint2_t i = 0; i < iDLCData.getNumPredRegions96(); i++)
				{
					const PredRegion &pr = iDLCData.getPredRegion96(i);

					for (uint8_t j = 0; j < pr.getRegionLength(); j++)
					{
						const ResidualSubBlock *rsb = iDLCData.getDLCSubBlock96(curBlock);

						if (rsb == NULL)
						{
							throw std::runtime_error("At least one 96 kHz DLC Subblock is undefined.");
						}

						for (uint16_t k = 0; k < rsb->getSize(); k++)
						{
							int32_t index2 = index1;
							int64_t accum64 = 0;

							for (int32_t p = 1; p <= pr.getOrder(); p++)
							{
								accum64 -= (static_cast<int64_t>(buffer[index2]) *
									static_cast<int64_t>(aCoeffs96[i].c_[p]));

								index2--;
								index2 &= 63;
							}

							int32_t output = static_cast<int32_t>(accum64 >> 20);
							output += *curResidual96;

							index1++;
							index1 &= 63;
							buffer[index1] = output;

							*curResidual96++ = output;
						}
					}
				}
			}  // End of "for (uint2_t n = 0; n < iDLCData.getNumPredRegions96(); n++)"

			// upsample 48 kHz samples to 96 kHz and add 96 Hz residuals

			std::fill(buffer, (buffer + 64), 0);

			index1 = 0;

			curSample = oSamples;

			curResidual96 = residuals96.begin();

			while (curSample < oSamples + iSampleCount)
			{
				buffer[index1] = *curSample;

				int32_t index2 = index1;
				index2 -= 8;
				index2 &= 63;

				*(curSample++) = buffer[index2] + *(curResidual96++);

				index2 = index1;

				int64_t accum64 = 0;

				for (int32_t i = 1; i < 33; i += 2)
				{
					accum64 += (static_cast<int64_t>(buffer[index2]) * InterpolatorFilterCoeffs[i]);
					index2--;
					index2 &= 63;
				}

				*(curSample++) = static_cast<int32_t>(accum64 >> 15) + *(curResidual96++);

				index1++;
				index1 &= 63;
			}  // End of "while (curSample < oSamples + iSampleCount)"

		}  // End of "if (dlcSampleRate == eSampleRate_96000 && !isDecoding96kTo48kOutput)"

		// shift bits

		for (curSample = oSamples; curSample < oSamples + iSampleCount; curSample++)
		{
			*curSample <<= iDLCData.getShiftBits();
		}

		return;
	}

	FullDecoder::StatusCode FullDecoder::decode_noexcept(int32_t * oSamples
		, uint32_t iSampleCount
		, SampleRate iSampleRateCode
		, const AudioData& iDLCData)
	{
		try
		{
			this->decode(oSamples, iSampleCount, iSampleRateCode, iDLCData);
		}
		catch (...)
		{
			return StatusCode_FATAL_ERROR;
		}

		return StatusCode_OK;
	}

} // namespace dlc