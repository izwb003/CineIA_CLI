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

#ifndef DLC_CODEC_H
#define DLC_CODEC_H

#include "DLC/DLCAudioData.h"

/**
 * Defines interface classes for DLC encoders and decoders.
 *
 * A DLC encoder takes baseband PCM audio samples as input, and fills in a dlc::AudioData instance with the information necessary to construct an
 * IAB AudioDataDLC element, as defined in ST 2098-2.
 *
 * Conversly, a DLC decoder takes a dlc::AudioData instance as input, and decodes it to baseband PCM audio samples.
 *
 * <em>NOTE: This documenation assumes the reader has access to SMPTE ST 2098-2, and does not duplicate information therein. In particular,
 * maximum and minimum permissible values are not repeated here unless undefined in SMPTE ST 2098-2.</em>
 *
 * @file
 */

namespace dlc
{
    /**
     * Represents a DLC encoder
     * Interface class that must be subclassed.
     */
    class Encoder
    {
    public:
        
        /**
         * Encodes a sequence (generally a frame) of 24-bit samples into a DLCAudioData element
		 *
		 * Integer PCM samples are expected at input, in 32-bit, MSB-aligned. For example, if input is
		 * of 24-bit PCM, the 3-byte samples must occupy the 3 MSBs (Most Significant Bytes) of the
		 * 4-byte (ie. 32-bit) integer container.
		 *
         * @param[in] iSamples Pointer to a sequence of sampleCount 24-bit audio samples to be encoded.
		 *            Each samples must be in the range [-8,388,608, 8,388,607] << 8.
		 *            Must not be nullptr.
         * @param[in] iSampleCount Number of samples to encode. Must be larger than 0.
         * @param[in] iSampleRateCode Sample rate of the samples
         * @param[out] oDLCData AudioData instance to be set according to the results of the encoding
         * @throws std::invalid_argument If samples is NULL or sampleCount is 0
         */
        virtual void encode(const int32_t * iSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, AudioData& oDLCData) = 0;
        
        virtual ~Encoder() {}
    };
    
    /**
     * Represents a DLC decoder.
     * Interface class that must be subclassed.
     */
    class Decoder
    {
    public:
        
        /**
         * Decodes a DLCAudioData element into a sequence of 24-bit samples.
		 *
		 * Note that "iSampleRateCode" is the _desired_ sample rate od the decoded samples. For example,
		 * "iSampleRateCode" can be set to 48kHz for an "iDLCData" that contains both 48kHz and 96kHz
		 * extension data. In such case, only the 48kHz base band data will be used in DLC decoding.
         *
         * @param[out] oSamples Pointer to a sequence of sampleCount 24-bit audio samples into which 
		 *             decoded samples will be stored. Caller must pre-allocate buffer of sufficient 
		 *             size to accept output samples.
		 *             Must not be nullptr.
         * @param[in] iSampleCount Number of decoded samples at sampleRateCode.
         *            iSampleCount must be equal to 
		 *               1) dlcData.getSampleCount48() if sampleRateCode == FS_48000, or
		 *               2) dlcData.getSampleCount96() if sampleRateCode == FS_96000.
         * @param[in] iSampleRateCode Desired sample rate of the decoded samples
         * @param[in] iDLCData AudioData instance that contains the encoded samples
         * @throws std::invalid_argument if samples is NULL or sampleCount is 0
         *                               if sampleRateCode is FS_96000 but dlcData.getSampleRate() is FS_48000
         *                               if sampleCount is not equal to dlcData.getSampleCount()
         */
        virtual void decode(int32_t * oSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, const AudioData& iDLCData) = 0;
        
        virtual ~Decoder() {}
    };

	// **************************
	// Codec constants and filter coefficients
	//

	// Maximum DLC frame/block sizes for 48k and 96k, in samples
	// Defined according ST2098-2 spec. See Table 18 of ST2098-2. 
	static const uint32_t kDLCMaxFrameSize96k = 4000;
	static const uint32_t kDLCMaxFrameSize48k = kDLCMaxFrameSize96k >> 1;

	// Interpolator (2x upsampling) filter order 
	static const uint32_t kDLCInterpFiltOrder = 32;

	// Interpolator filter group delay 
	static const uint32_t kDLCInterpFiltGrpDelay = kDLCInterpFiltOrder >> 1;	// = (kDLCInterpFiltOrder / 2), in samples

	// InterpolatorFilterCoeffs: Coefficients for the interpolation filter. See Table 33 of ST2098-2 SMPTE spec.
	static const int64_t InterpolatorFilterCoeffs[kDLCInterpFiltOrder + 1] =
	{
		0,
		-138,
		0,
		305,
		0,
		-618,
		0,
		1128,
		0,
		-1952,
		0,
		3377,
		0,
		-6450,
		0,
		20688,
		32767,
		20688,
		0,
		-6450,
		0,
		3377,
		0,
		-1952,
		0,
		1128,
		0,
		-618,
		0,
		305,
		0,
		-138,
		0
	};

	// 96k LPF filter order 
	static const uint32_t kDLCLPF96kFiltOrder = 128;

	// Interpolator filter group delay 
	static const uint32_t kDLCLPF96kFiltGrpDelay = kDLCLPF96kFiltOrder >> 1;    // = (kDLCLPF96kFiltOrder / 2), in samples

	// Bit length for converting LPF filter floating-point coefficients to Interger
	static const uint32_t kDLCLPFCoeffIntBitLength = 18;

	// LowPassFilterCoeffs: Anti-aliasing LPF coefficients for 96k encoding.
	// Converted to int64_t integer format with multiplication by (2 ^ kDLCLPFCoeffIntBitLength).
	// in Q18 form
	static const int64_t LowPassFilterCoeffs[kDLCLPF96kFiltOrder + 1] =
	{
		51,
		215,
		187,
		-26,
		-148,
		26,
		169,
		-21,
		-207,
		10,
		253,
		9,
		-306,
		-35,
		365,
		71,
		-431,
		-117,
		501,
		175,
		-578,
		-246,
		659,
		333,
		-745,
		-436,
		835,
		559,
		-928,
		-703,
		1023,
		872,
		-1121,
		-1069,
		1219,
		1299,
		-1317,
		-1565,
		1415,
		1876,
		-1510,
		-2239,
		1603,
		2668,
		-1691,
		-3178,
		1775,
		3796,
		-1853,
		-4560,
		1924,
		5533,
		-1987,
		-6824,
		2042,
		8640,
		-2088,
		-11424,
		2124,
		16331,
		-2150,
		-27599,
		2166,
		83371,
		128901,
		83371,
		2166,
		-27599,
		-2150,
		16331,
		2124,
		-11424,
		-2088,
		8640,
		2042,
		-6824,
		-1987,
		5533,
		1924,
		-4560,
		-1853,
		3796,
		1775,
		-3178,
		-1691,
		2668,
		1603,
		-2239,
		-1510,
		1876,
		1415,
		-1565,
		-1317,
		1299,
		1219,
		-1069,
		-1121,
		872,
		1023,
		-703,
		-928,
		559,
		835,
		-436,
		-745,
		333,
		659,
		-246,
		-578,
		175,
		501,
		-117,
		-431,
		71,
		365,
		-35,
		-306,
		9,
		253,
		10,
		-207,
		-21,
		169,
		26,
		-148,
		-26,
		187,
		215,
		51
	};

	// Total (LPF + Interpolator) delay - the same amount need to be applied to 
	// 96k samples to re-align base band and extension band during 96k DLC encoding.
	//
	static const uint32_t kDLCTotal96kFiltGrpDelay = kDLCLPF96kFiltGrpDelay + kDLCInterpFiltGrpDelay;	// in samples

} // namespace dlc

#endif // #ifndef DLC_CODEC_H
