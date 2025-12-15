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
 * Header file for the IAB Packer implementation.
 *
 * @file
 */


#ifndef __IABPARSER_H__
#define	__IABPARSER_H__

#include "common/IABElements.h"
#include "IABParserAPI.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /**
     *
     * IAB Parser class to parse IAB frame elements from input bitstream.
     *
     */
    
    class IABParser : public IABParserInterface
    {
    public:

		// Constructor
		IABParser(std::istream* iInputStream);
		IABParser();

		// Destructor
		~IABParser();

        /**
         *
         * Returns the API version of the Packer.
         *
         * @sa IABParserInterface
         *
         */
        void GetAPIVersion(IABAPIVersionType& oVersion);
        
        /**
         *
         * Gets bitstream version. Note that this API should only be used after ParseIABFrame() has been called
         * and without errors, otherwise, a value of 0 is returned.
         *
         * @sa IABParserInterface
         *
         */
        iabError  GetBitstreamVersion(IABVersionNumberType& oVersion) const;

        /**
         * Set Parser behaviour with invalid bitstream versions
         * 
         * Use this function to indicate whether the Parser should fail on
         * illegal bitstream versions or not.
         * 
         * Default behaviour is to fail on illegal bitstream versions.
         * 
         * @sa IABParserInterface
         */
        void SetParseFailsOnVersionError(bool failOnVersionError);

        /**
         * Get Parser behaviour with invalid bitstream versions.
         *
         * @sa IABParserInterface 
         */
        bool GetParseFailsOnVersionError() const;
        
		/** Parse an IABFrame
         *
         * @sa IABParserInterface
         *
         */
        iabError ParseIABFrame();

		/** Parse an IABFrame from buffer iIABFrameDataBuffer.
		*
		* @sa IABParserInterface
		*
		*/
		iabError ParseIABFrame(char* iIABFrameDataBuffer, uint32_t iBufferSize);

        /** Gets a constant reference to IABFrameInterface pointer
         *
         * @sa IABParserInterface
         *
         */
        iabError GetIABFrame(const IABFrameInterface*& oIABFrame);

		/** Get, take over and own IABFrameInterface pointer to parsed frame.
		* Note that caller takes over ownership of the parsed frame, and must delete object after use.
		*
		* @sa IABParserInterface
		*
		*/
		iabError GetIABFrameReleased(IABFrameInterface*& oIABFrame);

		/** Gets audio sample rate of IA bitstream
         *
         * @sa IABParserInterface
         *
         */
        IABSampleRateType GetSampleRate();
        
        /** Gets frame rate of IA bitstream
         *
         * @sa IABParserInterface
         *
         */
        IABFrameRateType GetFrameRate();
        
        /** Gets number of samples per IA bed channel or object in the frame
         *
         * @sa IABParserInterface
         *
         */
        uint32_t GetFrameSampleCount();
        
        /** Gets number of sub-elements in the frame
         *
         * @sa IABParserInterface
         *
         */
        IABElementCountType GetFrameSubElementCount();
        
		/** Get the cumulative count of unallowed (illegal/unknown) frame sub-elements encountered
		*
		* @sa IABParserInterface
		*
		*/
		uint32_t GetUnallowedFrameSubElementCount();

		/** Gets maximum number of bed channels and objects in the frame to render together
         *
         * @sa IABParserInterface
         *
         */
        IABMaxRenderedRangeType GetMaximumAssetsToBeRendered();
                
        /** Gets a string reference for the specified IAB error code.
         *
         * @sa IABParserInterface
         *
         */
        const std::string& getStringForErrorCode(commonErrorCodes iErrorCode);

    private:
        
        // GetAudioAsset assoicated with the given audio data ID.
        // The function calls DLC decoder to decode the channel into audioSample.
        iabError GetAudioAssetFromDLC(IABAudioDataIDType iAudioDataID, uint32_t iNumSamples, int32_t *oAudioSamples);
        
        // Pointer to the IAB frame
        IABFrameInterface*          iabParserFrame_;
        
        // Pointer to input stream that contains the IA bitstream to be parsed
        std::istream*               iabStream_;
        
		// Number of times unknown element IDs are found in parsing.
		// Cumulative over frames for the lifetime of IABParser instance.
		uint32_t unAllowedFrameSubElementsCount_;

        // When true the parser will fail on a bitstream version error
        bool failOnBitstreamVersionError_;

        // mapping from error codes to human-readable strings
        static std::map<commonErrorCodes, std::string> errorCodeMap_;
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABPARSER_H__
