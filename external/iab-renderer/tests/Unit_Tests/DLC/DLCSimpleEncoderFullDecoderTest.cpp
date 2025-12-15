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

#include "gtest/gtest.h"
#include "DLC/DLCSimpleEncoder.h"
#include "DLC/DLCFullDecoder.h"
#include <vector>

namespace
{

    class SimpleEncoderFullDecoderTest : public ::testing::TestWithParam<uint32_t>
    {
	protected:

		void SetUp()
		{
			// Frame size to frame rate map init for 48k
			frameSizeToFrameRateMap48k_[2000] = dlc::eFrameRate_24;
			frameSizeToFrameRateMap48k_[1920] = dlc::eFrameRate_25;
			frameSizeToFrameRateMap48k_[1600] = dlc::eFrameRate_30;
			frameSizeToFrameRateMap48k_[1000] = dlc::eFrameRate_48;
			frameSizeToFrameRateMap48k_[960] = dlc::eFrameRate_50;
			frameSizeToFrameRateMap48k_[800] = dlc::eFrameRate_60;
			frameSizeToFrameRateMap48k_[500] = dlc::eFrameRate_96;
			frameSizeToFrameRateMap48k_[480] = dlc::eFrameRate_100;
			frameSizeToFrameRateMap48k_[400] = dlc::eFrameRate_120;

			// Frame size to frame rate map init for 96k
			frameSizeToFrameRateMap96k_[4000] = dlc::eFrameRate_24;
			frameSizeToFrameRateMap96k_[3840] = dlc::eFrameRate_25;
			frameSizeToFrameRateMap96k_[3200] = dlc::eFrameRate_30;
			frameSizeToFrameRateMap96k_[2000] = dlc::eFrameRate_48;
			frameSizeToFrameRateMap96k_[1920] = dlc::eFrameRate_50;
			frameSizeToFrameRateMap96k_[1600] = dlc::eFrameRate_60;
			frameSizeToFrameRateMap96k_[1000] = dlc::eFrameRate_96;
			frameSizeToFrameRateMap96k_[960] = dlc::eFrameRate_100;
			frameSizeToFrameRateMap96k_[800] = dlc::eFrameRate_120;
		}

		std::map<uint32_t, dlc::FrameRate> frameSizeToFrameRateMap48k_;
		std::map<uint32_t, dlc::FrameRate> frameSizeToFrameRateMap96k_;
	};

    TEST_F(SimpleEncoderFullDecoderTest, NoException)
    {
        std::vector<int32_t> in_samples(2000, 0);
        std::vector<int32_t> out_samples(in_samples.size());

        dlc::SimpleEncoder e;
        dlc::FullDecoder d;
        dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_48000, dlc::eFrameRate_24);

		ASSERT_EQ(dlc::SimpleEncoder::StatusCode_OK, e.encode_noexcept(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad));

        // tests error code return if no sample buffer are provided to the encoder
        ASSERT_EQ(dlc::SimpleEncoder::StatusCode_FATAL_ERROR, e.encode_noexcept(NULL, static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad));

        ASSERT_EQ(dlc::FullDecoder::StatusCode_OK, d.decode_noexcept(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad));

        // tests error code return if no sample buffer are provided to the decoder
        ASSERT_EQ(dlc::FullDecoder::StatusCode_FATAL_ERROR, d.decode_noexcept(NULL, static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad));
    }

    TEST_F(SimpleEncoderFullDecoderTest, IllegalParameters)
    {
        std::vector<int32_t> in_samples(2000, 0);
        std::vector<int32_t> out_samples(in_samples.size());

        dlc::SimpleEncoder e;
        dlc::FullDecoder d;
        dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_48000, dlc::eFrameRate_24);
	
		// tests error if no sample buffer is provided to the encoder
        ASSERT_THROW(e.encode(NULL, static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad), std::invalid_argument);

        // tests error if no samples are provided to the encoder
        ASSERT_THROW(e.encode(&(in_samples[0]), 0, dlc::eSampleRate_48000, ad), std::invalid_argument);

        e.encode(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad);

        // tests error if insufficient sample buffer is provided to the decoder
        ASSERT_THROW(d.decode(&(out_samples[0]), 0, dlc::eSampleRate_48000, ad), std::invalid_argument);

        // tests error if insufficient sample buffer is provided to the decoder
        ASSERT_THROW(d.decode(&(out_samples[0]), 1, dlc::eSampleRate_48000, ad), std::invalid_argument);

        // tests error if no sample buffer is provided to the decoder
        ASSERT_THROW(d.decode(NULL, static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad), std::invalid_argument);

        // tests error if 96 kHz samples are requested if DLC data is 48 kHz
        ASSERT_THROW(d.decode(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_96000, ad), std::invalid_argument);

    }

    TEST_P(SimpleEncoderFullDecoderTest, Silent)
    {
        std::vector<int32_t> in_samples(GetParam(), 0);
        std::vector<int32_t> out_samples(in_samples.size());

		uint32_t frameSize = static_cast<uint32_t>(in_samples.size());

        dlc::SimpleEncoder e;
        dlc::FullDecoder d;
        dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_48000, frameSizeToFrameRateMap48k_[frameSize]);

        e.encode(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad);

        /* verify that 0 bits are used since the audio is silent */
        for (uint8_t i = 0; i < ad.getNumDLCSubBlocks(); i++)
        {
            ASSERT_EQ(0, static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock48(i))->getBitDepth());
        }

        d.decode(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad);

        std::vector<int32_t>::iterator in_it = in_samples.begin();
        std::vector<int32_t>::iterator out_it = out_samples.begin();

        /* tests the decoded output is identical to the input */
        while (in_it < in_samples.end())
        {
            ASSERT_EQ(*(in_it++), *(out_it++));
        }
    }

    TEST_P(SimpleEncoderFullDecoderTest, Ramp)
    {
        std::vector<int32_t> in_samples(GetParam());
		uint32_t frameSize = static_cast<uint32_t>(in_samples.size());

        for (uint32_t i = 0; i < in_samples.size(); i++)
        {
            in_samples[i] = ((i % 64) - 32) << 8;						// Use 3 MSBs (24-bit) of the 32-bit integer
        }

        std::vector<int32_t> out_samples(in_samples.size());

        dlc::SimpleEncoder e;
        dlc::FullDecoder d;
        dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_48000, frameSizeToFrameRateMap48k_[frameSize]);

		e.encode(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_48000, ad);
        d.decode(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad);

        std::vector<int32_t>::iterator in_it = in_samples.begin();
        std::vector<int32_t>::iterator out_it = out_samples.begin();

        // tests the decoded output is identical to the input
        while (in_it < in_samples.end())
        {
            ASSERT_EQ(*(in_it++), *(out_it++));
        }
    }

	// Frame rate default to 24 fps throughout tests, unless specified otherwise.
	//
	TEST_F(SimpleEncoderFullDecoderTest, NoException_96k)
	{
		std::vector<int32_t> in_samples(4000, 0);
		std::vector<int32_t> out_samples(in_samples.size());

		dlc::SimpleEncoder e;
		dlc::FullDecoder d;
		dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_96000, dlc::eFrameRate_24);

		ASSERT_EQ(dlc::SimpleEncoder::StatusCode_OK, e.encode_noexcept(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_96000, ad));

		// tests error code return if no sample buffer are provided to the encoder
		ASSERT_EQ(dlc::SimpleEncoder::StatusCode_FATAL_ERROR, e.encode_noexcept(NULL, static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_96000, ad));

		ASSERT_EQ(dlc::FullDecoder::StatusCode_OK, d.decode_noexcept(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_96000, ad));

		// tests error code return if no sample buffer are provided to the decoder
		ASSERT_EQ(dlc::FullDecoder::StatusCode_FATAL_ERROR, d.decode_noexcept(NULL, static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_96000, ad));
	}

	TEST_F(SimpleEncoderFullDecoderTest, IllegalParameters_96k)
	{
		std::vector<int32_t> in_samples(4000, 0);
		std::vector<int32_t> out_samples(in_samples.size());

		dlc::SimpleEncoder e;
		dlc::FullDecoder d;
		dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_96000, dlc::eFrameRate_24);

		// tests error if no sample buffer is provided to the encoder
		ASSERT_THROW(e.encode(NULL, static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_96000, ad), std::invalid_argument);

		// tests error if no samples are provided to the encoder
		ASSERT_THROW(e.encode(&(in_samples[0]), 0, dlc::eSampleRate_96000, ad), std::invalid_argument);

		e.encode(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_96000, ad);

		// tests error if insufficient sample buffer is provided to the decoder
		ASSERT_THROW(d.decode(&(out_samples[0]), 0, dlc::eSampleRate_96000, ad), std::invalid_argument);

		// tests error if insufficient sample buffer is provided to the decoder
		ASSERT_THROW(d.decode(&(out_samples[0]), 1, dlc::eSampleRate_96000, ad), std::invalid_argument);

		// tests error if no sample buffer is provided to the decoder
		ASSERT_THROW(d.decode(NULL, static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_96000, ad), std::invalid_argument);

		// tests error if 48 kHz samples are requested if DLC data is 96 kHz
		ASSERT_THROW(d.decode(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_48000, ad), std::invalid_argument);
	}

	TEST_P(SimpleEncoderFullDecoderTest, Silent_96k)
	{
		std::vector<int32_t> in_samples((2 * GetParam()), 0);			// Double size for 96k tests
		std::vector<int32_t> out_samples(in_samples.size());

		uint32_t frameSize = static_cast<uint32_t>(in_samples.size());

		dlc::SimpleEncoder e;
		dlc::FullDecoder d;
		dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_96000, frameSizeToFrameRateMap96k_[frameSize]);

		e.encode(&(in_samples[0]), static_cast<uint32_t>(in_samples.size()), dlc::eSampleRate_96000, ad);

		/* verify that 0 bits are used since the audio is silent */
		for (uint8_t i = 0; i < ad.getNumDLCSubBlocks(); i++)
		{
			ASSERT_EQ(0, static_cast<dlc::PCMResidualsSubBlock*>(ad.getDLCSubBlock96(i))->getBitDepth());
		}

		d.decode(&(out_samples[0]), static_cast<uint32_t>(out_samples.size()), dlc::eSampleRate_96000, ad);

		std::vector<int32_t>::iterator in_it = in_samples.begin();
		// Note for 96k encoding introduce filtering delay, dlc::kDLCTotal96kFiltGrpDelay.
		// Hence, in compare, output buffer samples are delayed by dlc::kDLCTotal96kFiltGrpDelay
		std::vector<int32_t>::iterator out_it = out_samples.begin() + dlc::kDLCTotal96kFiltGrpDelay;

		// tests the decoded output is identical to the input
		// Note out_it runs to end before in_it hits its end, by dlc::kDLCTotal96kFiltGrpDelay
		// sample. Comparison
		{
			ASSERT_EQ(*(in_it++), *(out_it++));
		}
	}

	TEST_P(SimpleEncoderFullDecoderTest, Ramp_96k)
	{
		uint32_t i = 0;

		std::vector<int32_t> in_samples(2 * GetParam());	// Double size for 96k tests
		uint32_t frameSize = static_cast<uint32_t>(in_samples.size());
		std::vector<int32_t> out_samples(frameSize);

		// 2nd frame, continous from the 1st frame
		std::vector<int32_t> in_samples_2(frameSize);
		std::vector<int32_t> out_samples_2(frameSize);

		for (i = 0; i < frameSize; i++)
		{
			in_samples[i] = ((i % 64) - 32) << 8;
		}

		// i index continues ...
		for (; i < (frameSize * 2); i++)
		{
			in_samples_2[i - frameSize] = ((i % 64) - 32) << 8;
		}

		dlc::SimpleEncoder e;
		dlc::FullDecoder d;
		dlc::AudioData ad;

		// set up encoder with frame rate and sampling rate
		e.setup(dlc::eSampleRate_96000, frameSizeToFrameRateMap96k_[frameSize]);

		// Encode & decode 1st frame
		e.encode(&(in_samples[0]), frameSize, dlc::eSampleRate_96000, ad);
		d.decode(&(out_samples[0]), frameSize, dlc::eSampleRate_96000, ad);

		// Continue to encode & decode 2nd frame
		e.encode(&(in_samples_2[0]), frameSize, dlc::eSampleRate_96000, ad);
		d.decode(&(out_samples_2[0]), frameSize, dlc::eSampleRate_96000, ad);

		std::vector<int32_t>::iterator in_it = in_samples.begin();
		// Note for 96k encoding introduce filtering delay, dlc::kDLCTotal96kFiltGrpDelay.
		// Hence, in compare, output buffer samples are delayed by dlc::kDLCTotal96kFiltGrpDelay
		std::vector<int32_t>::iterator out_it = out_samples.begin() + dlc::kDLCTotal96kFiltGrpDelay;

		// tests the decoded output is identical to the input
		// Note out_it runs to end before in_it hits its end, by dlc::kDLCTotal96kFiltGrpDelay
		// sample.
		while (in_it < (in_samples.end() - dlc::kDLCTotal96kFiltGrpDelay))
		{
			ASSERT_EQ(*(in_it++), *(out_it++));
		}

		// Compare remaining samples from frame 1 input, to beginning of frame 2 output
		out_it = out_samples_2.begin();

		while (in_it < in_samples.end())
		{
			ASSERT_EQ(*(in_it++), *(out_it++));
		}

		// Compare frame 2 input to delayed frame 2 output. It is possible to
		// compare the first (frameSize - dlc::kDLCTotal96kFiltGrpDelay) sample from
		// in_samples_2.
		in_it = in_samples_2.begin();

		while (in_it < (in_samples_2.end() - dlc::kDLCTotal96kFiltGrpDelay))
		{
			ASSERT_EQ(*(in_it++), *(out_it++));
		}
	}

	// tests the encoder and decoder across frame sizes currently specified in ST 2098-2
	INSTANTIATE_TEST_CASE_P(FS48FrameSizes,
		SimpleEncoderFullDecoderTest,
		::testing::Values(2000, 1920, 1600, 1000, 960, 800, 500, 480, 400)
		);
}
