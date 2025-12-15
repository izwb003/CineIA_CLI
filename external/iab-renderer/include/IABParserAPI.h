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
 * IABParserAPI.h
 *
 */

#ifndef __IABPARSERAPI_H__
#define	__IABPARSERAPI_H__

#include <memory>
#include <vector>

#include "IABDataTypes.h"
#include "IABErrors.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /**
     *
     * Represent an interface for an IAB Parser. Must be defined by implementation.
     *
     * @class IABParserInterface
     */

    class IABParserInterface
    {
    public:
        
		/**
		* Creates an IABParser instance on iInputStream.
		* Note that Caller retains ownership to object pointed to by iInputStream.
		*
		* Note that IABParser instance created in this way to support API ParseIABFrame()
		* below. This is most useful typically when parsing a single IAB stream/file
		* that contains multiple continuous frames.
		*
		* @memberof IABParserInterface
		*
		* @param[in] iInputStream pointer to input bitstream for parsing IAB from.
		* @returns a pointer to IABParserInterface instance created
		*/
		static IABParserInterface* Create(std::istream* iInputStream);

		/**
		* Creates an IABParser instance, without setting up input data stream.
		*
		* Note that IABParser instance created in this way to work with frame parsing API
		* ParseIABFrame(char* iIABFrameDataBuffer, uint32_t iBufferSize) below, where caller
		* provides IAB Frame data per call. This is most useful when parsing multiple
		* frames where each is contained in its own data buffer/stream/file. A typical
		* application example is from MXF-unwrapped frames.
		*
		* @memberof IABParserInterface
		*
		* @returns a pointer to IABParserInterface instance created
		*/
		static IABParserInterface* Create();

        /**
         * Deletes an IABParser instance
         *
         * @memberof IABParserInterface
         *
         * @param[in] iInstance pointer to the instance of the IABParserInterface
         */
        static void Delete(IABParserInterface* iInstance);

		/// Destructor
        virtual ~IABParserInterface() {}

        /**
         *
         * Returns the API version of the Parser.
         *
         * @memberof IABParserInterface
         *
         * @param[out] oVersion IABAPIVersion with the API version of the Parser
         *
         */
        virtual void GetAPIVersion(IABAPIVersionType& oVersion) = 0;
        
        /**
         *
         * Gets bitstream version. Note that this API should only be used after ParseIABFrame() has been called
         * and without errors, otherwise, the current default bitstream version number is returned.
         *
         * @memberof IABParserInterface
         *
         * @param[out] oVersion value read from IABParser. A returned value of 0 or 2 indicate an error.
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         *
         */
        virtual iabError GetBitstreamVersion(IABVersionNumberType& oVersion) const = 0;

        /**
         * Set Parser behaviour with invalid bitstream versions
         * 
         * Use this function to indicate whether the Parser should fail on
         * illegal bitstream versions or not.
         * 
         * Default behaviour is to fail on illegal bitstream versions.
         * 
         * The IABCore Library does not support bitstreams with illegal version
         * numbers. Use of this method is entirely at the customer's risk. 
         * 
         * @memberof IABParserInterface
         * 
         * @param failOnVersionError When true the parsing process will fail with illegal
         *                          bitstream versions (default behaviour). When false
         *                          the Parser will emit a kIABParserInvalidVersionNumberError
         *                          for streams with illegal bitstream versions, but the
         *                          parse shall continue.
         */
        virtual void SetParseFailsOnVersionError(bool failOnVersionError) = 0;

        /**
         * Get Parser behaviour with invalid bitstream versions.
         * 
         * @return true - Parser will not parse a bitstream with an invalid version
         * @return false - Parser will attempt to parse bitstream with an invalid version
         */
        virtual bool GetParseFailsOnVersionError() const = 0;
        
        /**
         *
         * Parses an IAB frame into a IAB frame data structure.
         *
         * @memberof IABParserInterface
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError ParseIABFrame() = 0;
        
		/**
		*
		* Parses an IAB frame into a IAB frame data structure from IAB frame data buffer
		* iIABFrameDataBuffer.
		*
		* !Note: Caller must provide frame data in entirety using "iIABFrameDataBuffer"
		* and "iBufferSize", for the API call. The data must start exactly at frame start.
		* Any extra random bytes data before frame data will result in parsing errors.
		* Any excessive data after end of 1 frame will be discarded/lost.
		*
		* @memberof IABParserInterface
		*
		* @param[in] iIABFrameDataBuffer points to beginning of IAB frame data buffer.
		* @param[in] iBufferSize Size of IAB frame data buffer, in bytes.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError ParseIABFrame(char* iIABFrameDataBuffer, uint32_t iBufferSize) = 0;

		/** Gets a constant reference to IABFrameInterface pointer.
         *
         * An example use case could be to pass the pointer to an IAB renderer to render the frame.
         *
         * The client must call ParseIABFrame first to parse a frame from the bitstream before calling GetIABFrame.
		 *
		 * Note that frame object pointed to by oIABFrame is still owned by IABParser object. When
		 * IABParser object is deleted, frame object (*oIABFrame) is also deleted.
         *
         * @memberof IABParserInterface
         *
         * @param[out] oIABFrame constant reference to IABFrameInterface pointer.
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError GetIABFrame(const IABFrameInterface*& oIABFrame) = 0;
        
		/** Get, take over and own IABFrameInterface pointer to parsed frame.
		*
		* Note that caller takes over ownership of the parsed frame, and must delete object after use.
		* This is the critical difference from GetIABFrame() API.
		*
		* An example use case is running IAB parsing and rendering in 2 threads. Caller can use
		* this API to take over parsed frame from IABParser thread, and then pass the released frame
		* pointer to an IAB renderer running in a separate thread, to render the frame.
		*
		* The client must call ParseIABFrame first to parse a frame from the bitstream before calling
		* GetIABFrameReleased().
		*
		* @memberof IABParserInterface
		*
		* @param[out] oIABFrame released IABFrameInterface pointer.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError GetIABFrameReleased(IABFrameInterface*& oIABFrame) = 0;

		/** Gets audio sample rate of IA bitstream
         *
         * @memberof IABParserInterface
         *
         * @return Sample rate of audio samples in the bitstream
         */
        virtual IABSampleRateType GetSampleRate() = 0;
        
        /** Gets frames rate of the bitstream
         *
         * @memberof IABParserInterface
         *
         * @return frame rate in frames per second
         */
        virtual IABFrameRateType GetFrameRate() = 0;
        
        /** Gets number of samples per IA bed channel or object in the frame
         *
         * @memberof IABParserInterface
         *
         * @return number of samples
         */
        virtual uint32_t GetFrameSampleCount() = 0;
        
        /** Gets number of sub-elements in the frame
         *
         * @memberof IABParserInterface
         *
         * @return number of frame sub-elements
         */
        virtual IABElementCountType GetFrameSubElementCount() = 0;
        
		/** Get the cumulative count of unallowed (illegal/unknown) frame sub-elements encountered
		*
		* @sa IABParserInterface
		*
		*/
		virtual uint32_t GetUnallowedFrameSubElementCount() = 0;

		/** Gets maximum number of audio assets that are intended to be rendered during playback
         * of the frame.
         *
         * @memberof IABParserInterface
         *
         * @return maximum number of audio assets to be rendered
         */
        virtual IABMaxRenderedRangeType GetMaximumAssetsToBeRendered() = 0;
        
        /** Gets a string for the specified IAB error code.
         *
         * @memberof IABParserInterface
         *
         * @param[in] iErrorCode error code to get string for
         *
         * @returns a reference to the error string associated with the specified code.
         * If the specified code is not defined in the IAB library, the string indicates an unspecified error condition.
         */
        virtual const std::string& getStringForErrorCode(commonErrorCodes iErrorCode) = 0;

    };
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABPARSERAPI_H__
