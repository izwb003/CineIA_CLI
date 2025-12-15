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

#ifndef DLC_FULLDECODER_H
#define DLC_FULLDECODER_H

#include "DLCCodec.h"

namespace dlc
{

    /**
    * Implements the DLC decoder specified in ST 2098-2.
    */
    class FullDecoder : public Decoder
    {
    public:

        enum StatusCode
        {
            StatusCode_OK,
            StatusCode_FATAL_ERROR /**< Unrecoverable error occurred. */
        };

        /**
        * @throws std::invalid_argument if sampleRateCode is not supported
        *                               if sampleCount is not supported
        * @throws std::runtime_error if there is any inconsistency in dlcData
        */
        void decode(int32_t * oSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, const AudioData& iDLCData);

        /**
        * Identical semantics to decode(), but catches all exceptions and returns an error code instead.
        *
        * @return Status following the decoding operation
        */
        StatusCode decode_noexcept(int32_t * oSamples
			, uint32_t iSampleCount
			, SampleRate iSampleRateCode
			, const AudioData& iDLCData);
    };

} // namespace dlc

#endif // #ifndef DLC_FULLDECODER_H
