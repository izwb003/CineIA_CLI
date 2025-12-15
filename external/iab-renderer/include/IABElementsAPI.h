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
 * IAB Elements API header file. Common for IAB Packer, Parser and Renderer libraries.
 *
 * Definition of APIs for creating, modifying and deleting IAB elements.
 *
 * @file
 */

#ifndef __IABELEMENTSAPI_H__
#define	__IABELEMENTSAPI_H__

#include <ostream>

#include "IABDataTypes.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    // Forward declarations
    class IABElement;
    class IABChannel;
    class IABObjectSubBlock;
	class IABBedRemapSubBlock;
	class IABZone19SubBlock;
	class IABEventHandler;

    /**
     * @brief IABFrameInterface
     *
     * Defining APIs for IABFrame.
	 *
	 * List of sub-element handling APIs that will/may delete sub-elements of IABFrame.
	 *    Delete() - all sub elements in list are deleted
	 *    SetSubElements() - current sub elements which are not in the new list, are deleted.
	 *
	 * List of sub-element handling APIs where instance is NOT deleted.
	 *    GetSubElements()
	 *    ClearSubElements()
	 *    IsSubElement()
	 *    RemoveSubElement()
	 *    AddSubElement()
	 *    EnablePackingSubElements()
	 *    DisablePackingSubElements()
	 *    AreSubElementsEnabledForPacking()
	 *
	 * List of sub-element handling APIs where ownership of sub element instance is returned or
	 * released back to caller.
	 *    ClearSubElements() - Ownership of all sub element instances in current list is released
	 *    RemoveSubElement() - Ownership of a single specific sub element instance, if found in 
	 *                         current list, is released
	 *
	 * @class IABFrameInterface
     *
     */
    class IABFrameInterface
    {
    public:
        
        /**
         * Creates an IABFrameInterface instance.
         *
         * @memberof IABFrameInterface
         *
		 * @param[in] inputStream pointer to input bitstream for parsing IAB from. Can be set to nullptr if not parsing.
		 *
		 * @returns pointer to IABFrameInterface instance created
         */
        static IABFrameInterface* Create(std::istream* inputStream);
        
        /**
         * Deletes an IABFrameInterface instance
		 *
		 * @warning During deletion of IABFrameInterface (IABFrame) instance, all sub-element instances owned
		 * by the IABFrame (ie. those in its sub element list) are deleted.
         *
         * @memberof IABFrameInterface
         *
         * @param[in] iInstance pointer to the instance of the IABFrameInterface
         */
        static void Delete(IABFrameInterface* iInstance);

        /// Destructor.
        virtual ~IABFrameInterface() {}

        // Setters and Getters
        
        /**
         * Gets Immersive Audio Bit Stream (IAB) version number
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oVersion bitstream version
         *
         */
        virtual void GetVersion(IABVersionNumberType &oVersion) const = 0;
        
        /**
         * Sets Immersive Audio Bit Stream (IAB) version number
         *
         * @memberof IABFrameInterface
         *
         * @param[in] iVersion bitstream version
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetVersion(IABVersionNumberType iVersion) = 0;
        
        /**
         * Gets audio data sampling rate. This sampling rate applies to all audio data in the bitstream.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oSampleRate audio data sampling rate
         *
         */
        virtual void GetSampleRate(IABSampleRateType &oSampleRate) const = 0;
        
        /**
         * Sets audio data sampling rate. This sampling rate applies to all audio data in the bitstream.
         *
         * @memberof IABFrameInterface
         *
         * @param[in] iSampleRate audio data sampling rate
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetSampleRate(IABSampleRateType iSampleRate) = 0;
        
        /**
         * Gets audio data sample bit depth. This bit depth applies to all audio data in the bitstream.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oBitDepth audio data sample bit depth
         *
         */
        virtual void GetBitDepth(IABBitDepthType &oBitDepth) const = 0;
        
        /**
         * Sets audio data sample bit depth. This bit depth applies to all audio data in the bitstream.
         *
         * @memberof IABFrameInterface
         *
         * @param[in] iBitDepth audio data sample bit depth
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetBitDepth(IABBitDepthType iBitDepth) = 0;
        
        /**
         * Gets frame rate code.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oFrameRateCode frame rate code
         *
         */
        virtual void GetFrameRate(IABFrameRateType &oFrameRateCode) const = 0;
        
        /**
         * Sets frame rate code to the IABFrame parameter.
		 *
		 * Note - IABFrameInterface::SetFrameRate() API is designed te be used during initialization of 
		 * IABFrame instance. It does not update frame rate for any of the sub-elements contained in the 
		 * IABFrame instance.
		 * 
		 * !Caution - For IABFrame instances that are contructed or partially constructed with sub-elements,
		 * changing frame rate using SetFrameRate() likely results in mis-alignment between frame
		 * rate code at IABFrame level, and frame rates of its sub-elements. Do not use it in this manner.
         *
         * @memberof IABFrameInterface
         *
         * @param[in] iFrameRateCode frame rate code
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetFrameRate(IABFrameRateType iFrameRateCode) = 0;
        
        /**
         * Gets MaxRendered. This value indicates maximum number of audio assets that are intended to be rendered during playback of the frame.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oMaxRendered MaxRendered value
         *
         */
        virtual void GetMaxRendered(IABMaxRenderedRangeType &oMaxRendered) const = 0;
        
        /**
         * Sets MaxRendered. This value indicates maximum number of audio assets that are intended to be rendered during playback of the frame.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] iMaxRendered MaxRendered value
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetMaxRendered(IABMaxRenderedRangeType iMaxRendered) = 0;
        
        /**
         * Gets number of elements (sub-elements) contained in the frame.
         * This count matches size of the sub-element vector returned by GetSubElements().
         * If the IAB frame was created from parsing a bitstream, then the sub-element vector will contain only
         * frame sub-element types currently supported by the parser. All unsupported sub-element types
         * would be skipped by the parser and the frame sub-element count value read from the bitstream would
         * be replaced with number of sub-element successfully parsed.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oCount number of sub-elements
         *
         */
        virtual void GetSubElementCount(IABElementCountType &oCount) const = 0;
        
        /**
         * Gets elements (sub-elements) contained in the frame.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed 
		 * within a frame element.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oSubElements vector to save pointers to the sub-elements
         *
         */
        virtual void GetSubElements(std::vector<IABElement*> &oSubElements) const = 0;
        
        /**
         * Sets elements (sub-elements) contained in the frame.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed 
		 * within a frame element.
		 *
		 * @warning Deletion of sub-element instance may occur if client calls API more than once.
		 *
		 * @note: Client to construct elements stored (pointed to) in "iSubElements".
		 * Once passed to an IABFrame, IABFrame takes over ownership and management of all elements
		 * whose pointers are stored in "iSubElements". Clients must not delete any of the elements
		 * referenced from "iSubElements" outside the IABFrame instance.
		 *
		 * @warning If the current sub element list is not empty (eg. when this API is called the
		 * 2nd time or multiple times), a sub element instance object in the current list is deleted, unless
		 * the same sub element also appears in the "iSubElements" list to be set by this call.
         * 
         * @memberof IABFrameInterface
         *
         * @param[in] iSubElements vector containing pointers to the sub-elements
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetSubElements(const std::vector<IABElement*> iSubElements) = 0;
        
		/**
		* Clear sub-element list and reset sub-element count to 0.
		* This call simply clears the list, but does not delete any of the pointed sub element
		* instances. By clearing the list, the ownship to all of the sub element instance is
		* returned to caller. Caller is responsible for managing and cleaning out released
		* IAB element instances.
		*
		* @memberof IABFrameInterface
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError ClearSubElements() = 0;

		/**
		* Check if an IABElement instance is a direct sub element of the IABFrame element.
		*
		* @memberof IABFrameInterface
		*
		* @param[in] iSubElement pointer to IABElement instance for checking
		*
		* @returns bool true: is a sub element, false: not a sub-element
		*/
		virtual bool IsSubElement(IABElement* iSubElement) = 0;

		/**
		* Remove a single IABElement instance from the direct sub element list of IABFrame element.
		* Note that this API simply removes the input instance, if found, from the
		* sub element list of IABFrame element. It by design does not delete the underlying
		* instance *iSubElement. Caller takes over the ownership of instance, for
		* re-purposing for example. Caller must delete the instance outside this call
		* if it is no longer needed.
		*
		* @memberof IABFrameInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be removed.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate
		*          an error (eg iSubElement not found in subelement list, etc)
		*/
		virtual iabError RemoveSubElement(IABElement* iSubElement) = 0;

		/**
		* Add a single IABElement instance to the sub element list of IABFrame element.
		* Note that ownership of a successfully added instance *iSubElement is transferred to
		* this IABFrame element.
		*
		* @memberof IABFrameInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be added.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate
		*       an error (eg *iSubElement is not a type allowed, or already a sub element
		*/
		virtual iabError AddSubElement(IABElement* iSubElement) = 0;

		/**
		* Enable packing of all sub elements contained in this IABFrame element.
		*
		* @memberof IABFrameInterface
		*/
		virtual void EnablePackingSubElements() = 0;

		/**
		* Disable packing of any of the sub elements contained in this IABFrame element.
		*
		* @memberof IABFrameInterface
		*/
		virtual void DisablePackingSubElements() = 0;

		/**
		* Check if sub elements contained in this IABFrame element are enabled (include/used) in packing.
		*
		* @memberof IABFrameInterface
		*/
		virtual bool AreSubElementsEnabledForPacking() = 0;

		// Processing methods: Serilize, DeSerialize, etc
        //
        /**
         * Serializes current frame into a packed buffer.
         *
         * @memberof IABFrameInterface
         *
         * @param[out] oBuffer buffer to contain the serialized frame
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError Serialize(std::ostream &oBuffer) = 0;
        
        /**
         * De-serializes an IAB frame from input stream.
		 * For IAB parsing application, an IABframe instance is constructed on std::istream.
		 * Upon construction, a valid bitstream reader is instantiated. DeSerialize() parses
		 * the input bitstream using the reader and contructed the IABFrame content from
		 * parsed elements and data fields.
		 *
		 * For an IABFrame instance without a valid bitstream reader, 
		 * "iabKParserBitstreamReaderNotPresentError" code will be returned.
         *
         * @memberof IABFrameInterface
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError DeSerialize() = 0;

		/**
		 * Set DeSerialization behaviour when an invalid bitstream version is encountered
		 *  
		 * Default behaviour is to fail on illegal bitstream versions.
		 * 
		 * The IABCore Library does not support bitstreams with illegal version
         * numbers. Use of this method is entirely at the customer's risk. 
		 * 
		 * @param failOnVersionError true - abort DeSerialization.
		 * 							 false - return 
		 *                           kIABParserInvalidVersionNumberError
		 * 							 but continue DeSerialization.
		 *                          
		 */
		virtual void SetDeSerializeFailsOnVersionError(bool failOnVersionError) = 0;

        /**
         * Get DeSerialization behaviour with invalid bitstream versions.
         * 
         * @return true - Parser will not parse a bitstream with an invalid version
         * @return false - Parser will attempt to parse bitstream with an invalid version
         */

		virtual bool GetDeSerializeFailsOnVersionError() const = 0;
        
		/**
		* Get number of known but unallowed frame sub-elements.
		* This API can be used to check/retrieve number of unallowed frame level sublements encountered
		* during parsing IABFrame, eg. after DeSerialize() call.
		*
		* @memberof IABFrameInterface
		*/
		virtual uint32_t GetNumUnallowedSubElements() const = 0;

		/**
		* Get number of undefined frame sub-elements.
		* This API can be used to check/retrieve number of undefined frame level sublements encountered
		* during parsing IABFrame, eg. after DeSerialize() call.
		*
		* @memberof IABFrameInterface
		*/
		virtual uint32_t GetNumUndefinedSubElements() const = 0;

		/**
         * Validates an initialized IAB frame against SMPTE IAB Specification.
         *
         * @memberof IABFrameInterface
		 *
		 * @returns if false, validation stopped due to fatal error
		 *          if true, frame validation completed in full, and ok to continue.
		 *          (Note that "true != valid frame". It indicates that fatal/stop errors have NOT occurred.)
		 */
        virtual bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const = 0;
	};

    /**
     * @brief IABChannelInterface. IAB channels are associated with Bed definitions.
     *
     * Defining APIs for IABChannel.
     *
     * @class IABChannelInterface
     */
    class IABChannelInterface
    {
    public:
        
        /**
         * Creates an IABChannelInterface instance
         *
         * @memberof IABChannelInterface
         *
         * @returns pointer to IABChannelInterface instance created
         */
        static IABChannelInterface* Create();
        
        /**
         * Deletes an IABChannelInterface instance
         *
         * @memberof IABChannelInterface
         *
         * @param[in] iInstance pointer to the instance of the IABChannelInterface
         */
        static void Delete(IABChannelInterface* iInstance);

        /// Destructor.
        virtual ~IABChannelInterface() {}

        // Setters and Getters
        
        /**
         * Gets ID of current channel. The channel ID identifies a bed channel.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ChannelID"
         *
         * @memberof IABChannelInterface
         *
         * @param[out] oChannelID channel ID
         *
         */
        virtual void GetChannelID(IABChannelIDType &oChannelID) const = 0;
        
        /**
         * Sets ID of current channel. The channel ID identifies a bed channel.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ChannelID"
         *
         * @memberof IABChannelInterface
         *
         * @param[in] iChannelID channel ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetChannelID(IABChannelIDType iChannelID) = 0;
        
        /**
         * Gets ID of audio data that is associated with the current channel. An AudioDataID of NULL (0) indicates no audio asset associated.
         *
         * @memberof IABChannelInterface
         *
         * @param[out] oAudioDataID audio data ID
         *
         */
        virtual void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const = 0;
        
        /**
         * Sets ID of audio data that is associated with the current channel. An AudioDataID of NULL (0) indicates no audio asset associated.
         *
         * @memberof IABChannelInterface
         *
         * @param[out] iAudioDataID audio data ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetAudioDataID(IABAudioDataIDType iAudioDataID) = 0;
        
        /**
         * Gets the gain to be applied to current channel.
         *
         * @memberof IABChannelInterface
         *
         * @param[out] oChannelGain channel gain
         *
         */
        virtual void GetChannelGain(IABGain &oChannelGain) const = 0;
        
        /**
         * Sets the gain to be applied to current channel.
         *
         * @memberof IABChannelInterface
         *
         * @param[in] iChannelGain channel gain
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetChannelGain(IABGain const& iChannelGain) = 0;
        
        /**
         * Gets the channel decorrelation information exists flag. It indicates if the information exists in the bitstream or not.
         *
         * @memberof IABChannelInterface
         *
         * @param[out] oDecorInfoExists information exists flag
         *
         */
        virtual void GetDecorInfoExists(uint8_t &oDecorInfoExists) const = 0;
        
        /**
         * Sets the channel decorrelation information exists flag. It indicates if the information exists in the bitstream or not.
         *
         * @memberof IABChannelInterface
         *
         * @param[in] iDecorInfoExists information exists flag
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetDecorInfoExists(uint8_t iDecorInfoExists) = 0;
        
        /**
         * Gets decorrelation coefficient for current channel.
         *
         * @memberof IABChannelInterface
         *
         * @param[out] oDecorCoef channel decorrelation coefficient
         *
         */
        virtual void GetDecorCoef(IABDecorCoeff &oDecorCoef) const = 0;
        
        /**
         * Sets decorrelation coefficient for current channel.
         *
         * @memberof IABChannelInterface
         *
         * @param[in] iDecorCoef channel decorrelation coefficient
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetDecorCoef(IABDecorCoeff const& iDecorCoef) = 0;
    };
 
    /**
     * @brief IABBedDefinitionInterface
     *
     * Defining APIs for IABBedDefinition.
     *
	 * List of sub-element handling APIs that will/may delete sub-elements of IABBedDefinition.
	 *    Delete() - all sub elements in list are deleted
	 *    SetSubElements() - current sub elements which are not in the new list, are deleted.
	 *
	 * List of sub-element handling APIs where instance is NOT deleted.
	 *    GetSubElements()
	 *    ClearSubElements()
	 *    IsSubElement()
	 *    RemoveSubElement()
	 *    AddSubElement()
	 *
	 * List of sub-element handling APIs where ownership of sub element instance is returned or
	 * released back to caller.
	 *    ClearSubElements() - Ownership of all sub element instances in current list is released
	 *    RemoveSubElement() - Ownership of a single specific sub element instance, if found in
	 *                         current list, is released
	 *
	 * @class IABBedDefinitionInterface
     */
    class IABBedDefinitionInterface
    {
    public:
        
        /**
         * Creates an IABBedDefinitionInterface instance
         *
		 * @param[in] iFrameRate target frame rate of bitstream for which the bed is defined
		 *
		 * @memberof IABBedDefinitionInterface
         *
         * @returns pointer to IABBedDefinitionInterface instance created
         */
        static IABBedDefinitionInterface* Create(IABFrameRateType iFrameRate);
        
        /**
         * Deletes an IABBedDefinitionInterface instance
         *
		 * @warning During deletion of IABFrameInterface (IABFrame) instance, all sub-element instances owned
		 * by the IABFrame (ie. those in its sub element list) are deleted.
		 *
		 * @memberof IABBedDefinitionInterface
         *
         * @param[in] iInstance pointer to the instance of the IABBedDefinitionInterface
         */
        static void Delete(IABBedDefinitionInterface* iInstance);

        /// Destructor.
        virtual ~IABBedDefinitionInterface() {}

        // Setters and Getters
        
        /**
         * Gets metadata ID of current bed defintion element.
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oMetaID metadata ID
         *
         */
        virtual void GetMetadataID(IABMetadataIDType &oMetaID) const = 0;
        
        /**
         * Sets metadata ID of this element.
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[in] iMetaID metadata ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetMetadataID(IABMetadataIDType iMetaID) = 0;
        
        /**
         * Gets conditional bed code.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ConditionalBed"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oConditionalBed conditional bed code.
         *
         */
        virtual void GetConditionalBed(uint8_t &oConditionalBed) const = 0;
        
        /**
         * Sets conditionalBed code.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ConditionalBed"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] iConditionalBed conditional Bed code.
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetConditionalBed(uint8_t iConditionalBed) = 0;
        
        /**
         * Gets bed use case code for this element. It indicates the use case for which this element is intended.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "BedUseCase"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oBedUseCase bed use case code.
         *
         */
        virtual void GetBedUseCase(IABUseCaseType &oBedUseCase) const = 0;
        
        /**
         * Sets bed use case code for this element. It indicates the use case for which this element is intended.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "BedUseCase"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[in] iBedUseCase bed use case code.
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetBedUseCase(IABUseCaseType iBedUseCase) = 0;
        
        /**
         * Gets number of channels contained in this element.
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oChannelCount channel count.
         *
         */
        virtual void GetChannelCount(IABChannelCountType &oChannelCount) const = 0;
        
        /**
         * Gets bed channels contained in this element.
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oBedChannels vector to save pointers to the bed channels
         *
         */
        virtual void GetBedChannels(std::vector<IABChannel*> &oBedChannels) const = 0;
        
        /**
         * Sets bed channels contained in this element.
		 * @note: Client to construct IABChannel instances stored (pointed to) in "iBedChannels".
		 * Once passed to an IABBedDefinition, IABBedDefinition takes over ownership and management of all instances
		 * whose pointers are stored in "iBedChannels". Clients must not delete any of the instances
		 * referenced from "iBedChannels" outside the IABBedDefinition instance.
		 *
         * @memberof IABBedDefinitionInterface
         *
         * @param[in] iBedChannels vector containing pointers to the bed channels
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetBedChannels(const std::vector<IABChannel*> iBedChannels) = 0;
        
        /**
         * Gets audio description for this element.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "AudioDescription"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oAudioDescription audio description
         *
         */
        virtual void GetAudioDescription(IABAudioDescription &oAudioDescription) const = 0;
        
        /**
         * Sets audio description for this element.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "AudioDescription"
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[in] iAudioDescription audio description
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetAudioDescription(IABAudioDescription const& iAudioDescription) = 0;
        
        /**
         * Gets number of elements (sub-elements) contained in this element.
         *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oCount number of sub-elements
         *
         */
        virtual void GetSubElementCount(IABElementCountType &oCount) const = 0;
        
		/**
         * Gets elements (sub-elements) contained in this element.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed within a bed definiton element.
		 *
         * @memberof IABBedDefinitionInterface
         *
         * @param[out] oSubElements vector of sub-elements, by reference
         *
         */
        virtual void GetSubElements(std::vector<IABElement*> &oSubElements) const = 0;
        
        /**
         * Sets elements (sub-elements) contained in this element.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed 
		 * within a bed definiton element.
		 *
		 * @warning Deletion of sub-element instance may occur if client calls API more than once.
		 *
		 * @note: Client to construct elements stored (pointed to) in "iSubElements".
		 * Once passed to an IABBedDefinition, IABBedDefinition takes over ownership and management 
		 * of all elements whose pointers are stored in "iSubElements". Clients must not delete any of 
		 * the elements referenced from "iSubElements" outside the IABBedDefinition instance.
		 *
		 * @warning If the current sub element list is not empty (eg. when this API is called the
		 * 2nd time or multiple times), a sub element instance object in the current list is deleted, unless
		 * the same sub element also appears in the "iSubElements" list to be set by this call.
		 *
         * @memberof IABBedDefinitionInterface
         *
         * @param[in] iSubElements vector of sub-elements, by reference
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetSubElements(const std::vector<IABElement*> iSubElements) = 0;

		/**
		* Clear sub-element list and reset sub-element count to 0.
		* This call simply clears the list, but does not delete any of the pointed sub element
		* instances. By clearing the list, the ownship to all of the sub element instance is
		* returned to caller. Caller is responsible for managing and cleaning out released
		* IAB element instances.
		*
		* @memberof IABBedDefinitionInterface
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError ClearSubElements() = 0;

		/**
		* Check if an IABElement instance is a sub element of this element.
		*
		* @memberof IABBedDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance for checking
		*
		* @returns bool true: is a sub element, false: not a sub-element
		*/
		virtual bool IsSubElement(IABElement* iSubElement) = 0;

		/**
		* Remove a single IABElement instance from the sub element list of this element.
		* Note that this API simply removes the input instance, if found, from the
		* sub element list of this element. It by design does not delete the underlying
		* instance *iSubElement. Caller takes over the ownership of instance, for
		* re-purposing for example. Caller must delete the instance outside this call
		* if it is no longer needed.
		*
		* @memberof IABBedDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be removed.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate 
		*          an error (eg iSubElement not found in subelement list, etc) 
		*/
		virtual iabError RemoveSubElement(IABElement* iSubElement) = 0;

		/**
		* Add a single IABElement instance to the sub element list of this element.
		* Note that ownership of a successfully added instance *iSubElement is transferred to
		* this element.
		*
		* @memberof IABBedDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be added.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate
		*       an error (eg *iSubElement is not a type allowed, or already a sub element
		*/
		virtual iabError AddSubElement(IABElement* iSubElement) = 0;

		/**
		* Enable packing of this bed definition element.
		*
		* @memberof IABBedDefinitionInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this bed definition element.
		*
		* @memberof IABBedDefinitionInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this bed definition element is enabled (include/used) in packing.
		*
		* @memberof IABBedDefinitionInterface
		*/
		virtual bool IsIncludedForPacking() = 0;

		/**
		* Get number of known but unallowed bed sub-elements.
		* This API can be used to check/retrieve number of unallowed bed sublements encountered
		* during parsing BedDefinition, eg. after DeSerialize() call.
		*
		* @memberof IABBedDefinitionInterface
		*/
		virtual uint32_t GetNumUnallowedSubElements() const = 0;

		/**
		* Get number of undefined bed sub-elements.
		* This API can be used to check/retrieve number of undefined bed sublements encountered
		* during parsing BedDefinition, eg. after DeSerialize() call.
		*
		* @memberof IABBedDefinitionInterface
		*/
		virtual uint32_t GetNumUndefinedSubElements() const = 0;
	};

    /**
     * @brief IABBedRemapSubBlockInterface
     *
     * Defining APIs for IABBedRemapSubBlock.
     *
     * @class IABBedRemapSubBlockInterface
     */
    class IABBedRemapSubBlockInterface
    {
    public:
        
        /**
         * Creates an IABBedRemapSubBlockInterface instance.
		 * The factory method create an IABBedRemapSubBlock instance that contains
		 * numberDestinaion of IABRemapCoeff, each containing numberSource of coefficients. All default values.
		 *
		 * Client need to update destination channel IDs and remap coefficients to actual gains as intended.
         *
         * @memberof IABBedRemapSubBlockInterface
         *
		 * @param[in] numberDestinaion number of destination channels
		 * @param[in] numberSource number of source channels
		 *
		 * @returns pointer to IABBedRemapSubBlockInterface instance created
         */
        static IABBedRemapSubBlockInterface* Create(uint16_t numberDestinaion, uint16_t numberSource);

        /**
         * Deletes an IABBedRemapSubBlockInterface instance
         *
         * @memberof IABBedRemapSubBlockInterface
         *
         * @param[in] iInstance pointer to the instance of the IABBedRemapSubBlockInterface
         */
        static void Delete(IABBedRemapSubBlockInterface* iInstance);

		/// Destructor.
        virtual ~IABBedRemapSubBlockInterface() {}

        // Setters and Getters

		/**
		* Gets remap info exist flag. It indicates if remap information exists in the current sub-bock or not.
		* For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "PanInfoExists"
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[out] oRemapInfoExists remap information exists flag
		*
		*/
		virtual void GetRemapInfoExists(uint1_t &oRemapInfoExists) const = 0;

		// Set remap info exist flag
		/**
		* Sets remap info exist flag. It indicates if panning information exists in the current sub-bock or not.
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[in] iRemapInfoExists remap information exists flag
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetRemapInfoExists(uint1_t iRemapInfoExists) = 0;

		/**
		* Gets remap coefficient array for the sub block.
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[out] oRemapCoeffArray vector of pointers to IABRemapCoeff
		*
		*/
		virtual void GetRemapCoeffArray(std::vector<IABRemapCoeff*> &oRemapCoeffArray) const = 0;

		/**
		* Sets remap coefficient array for the remap sub block.
		* @note: Client to create instances of IABRemapCoeff, stored (pointed to) in "iRemapCoeffArray".
		* Once passed to an IABBedRemapSubBlock, IABBedRemapSubBlock takes over ownership and management.
		* Clients must not delete any of the instances referenced from "iRemapCoeffArray".
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[in] iRemapCoeffArray vector containing pointers to the sub-IABRemapCoeff
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetRemapCoeffArray(const std::vector<IABRemapCoeff*> iRemapCoeffArray) = 0;

		/**
		* Check internal remap coefficient array consistency (eg. input-output dimension 
		* and other internal variables etc) contained within the remap block.
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		// 
		virtual iabError checkRemapCoeffArray() = 0;

		/**
		* Gets number of source channels in remap.
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[out] oSourceNumber number of source channels for remap
		*
		*/
		virtual void GetRemapSourceNumber(uint16_t &oSourceNumber) const = 0;

		/**
		* Gets number of destination channels in remap.
		*
		* @memberof IABBedRemapSubBlockInterface
		*
		* @param[out] oSourceNumber number of destination channels for remap
		*
		*/
		virtual void GetRemapDestinationNumber(uint16_t &oDestinationNumber) const = 0;
	};

    /**
     * @brief IABBedRemapInterface
     *
     * Defining APIs for IABBedRemap.
     *
     * @class IABBedRemapInterface
     */
    class IABBedRemapInterface
    {
    public:
        
        /**
         * Creates an IABBedRemapInterface instance
         *
         * @memberof IABBedRemapInterface
         *
		 * @param[in] numberDestinaion number of destination channels
		 * @param[in] numberSource number of source channels
		 * @param[in] iFrameRate target frame rate of bitstream for which the remap element is defined
		 *
		 * @returns pointer to IABBedRemapInterface instance created
         */
		static IABBedRemapInterface* Create(uint16_t numberDestinaion, uint16_t numberSource, IABFrameRateType iFrameRate);

        /**
         * Deletes an IABBedRemapInterface instance
         *
         * @memberof IABBedRemapInterface
         *
         * @param[in] iInstance pointer to the instance of the IABBedRemapInterface
         */
        static void Delete(IABBedRemapInterface* iInstance);

        // Destructor.
        virtual ~IABBedRemapInterface() {}

        // Setters and Getters

		/**
		* Gets metadata ID of bed remap element.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oMetaID metadata ID
		*
		*/
		virtual void GetMetadataID(IABMetadataIDType &oMetaID) const = 0;

		/**
		* Sets metadata ID of bed remap element.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[in] iMetaID metadata ID
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetMetadataID(IABMetadataIDType iMetaID) = 0;

		/**
		* Gets remap use case code for this element. It indicates the use case for which this remap element is intended.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oRemapUseCase bed remap use case code.
		*
		*/
		virtual void GetRemapUseCase(IABUseCaseType &oRemapUseCase) const = 0;

		/**
		* Gets remap use case code for this element. It indicates the use case for which this remap element is intended.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] iRemapUseCase bed remap use case code to set to.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetRemapUseCase(IABUseCaseType iRemapUseCase) = 0;

		/**
		* Gets number of source channels for remapping.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oSourceChannels remap source channel count.
		*
		*/
		virtual void GetSourceChannels(IABChannelCountType &oSourceChannels) const = 0;

		/**
		* Gets number of destination channels for remapping.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oDestinationChannels remap destination channel count.
		*
		*/
		virtual void GetDestinationChannels(IABChannelCountType &oDestinationChannels) const = 0;

		/**
		* Gets number of remap sub-blocks.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oNumRemapSubBlocks number of remap sub-blocks
		*
		*/
		virtual void GetNumRemapSubBlocks(uint8_t &oNumRemapSubBlocks) const = 0;

		/**
		* Gets bed remap sub-blocks.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[out] oPanSubBlocks a list of pointers to remap sub-blocks
		*
		*/
		virtual void GetRemapSubBlocks(std::vector<IABBedRemapSubBlock*> &oRemapSubBlocks) const = 0;

		/**
		* Sets bed remap sub-block.
		* @note: Client to construct remap sub block instances stored (pointed to) in "iRemapSubBlocks".
		* Once passed to an IABBedRemap, IABBedRemap takes over ownership and management of all instances
		* whose pointers are stored in "iRemapSubBlocks". Clients must not delete any of the instances
		* referenced from "iRemapSubBlocks" outside the IABBedRemap instance.
		*
		* @memberof IABBedRemapInterface
		*
		* @param[in] iRemapSubBlocks a list of pointers to bed remap sub-blocks
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetRemapSubBlocks(const std::vector<IABBedRemapSubBlock*> iRemapSubBlocks) = 0;

		/**
		* Enable packing of this bed remap element.
		*
		* @memberof IABBedRemapInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this bed remap element.
		*
		* @memberof IABBedRemapInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this bed remap element is enabled (include/used) in packing.
		*
		* @memberof IABBedRemapInterface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

    /**
     * @brief IABObjectSubBlockInterface
     *
     * Defining APIs for IABObjectSubBlock.
     *
     * @class IABObjectSubBlockInterface
     */
    class IABObjectSubBlockInterface
    {
    public:
        
        /**
         * Creates an IABObjectSubBlockInterface instance
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @returns pointer to IABObjectSubBlockInterface instance created
         */
        static IABObjectSubBlockInterface* Create();
        
        /**
         * Deletes an IABObjectSubBlockInterface instance
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iInstance pointer to the instance of the IABObjectSubBlockInterface
         */
        static void Delete(IABObjectSubBlockInterface* iInstance);

        /// Destructor.
        virtual ~IABObjectSubBlockInterface() {}

        // Setters and Getters
        
        /**
         * Gets PanInfoExists flag. It indicates if panning information exists in the current sub-bock or not.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "PanInfoExists"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oPanInfoExists panning information exists flag
         *
         */
        virtual void GetPanInfoExists(uint8_t &oPanInfoExists) const = 0;
        
        /**
         * Sets PanInfoExists flag. It indicates if panning information exists in the current sub-bock or not.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "PanInfoExists"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iPanInfoExists panning information exists flag
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetPanInfoExists(uint8_t iPanInfoExists) = 0;
        
        /**
         * Gets the gain to be applied to this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oObjectGain object gain
         *
         */
        virtual void GetObjectGain(IABGain &oObjectGain) const = 0;
        
        /**
         * Sets the gain to be applied to this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iObjectGain object gain
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectGain(IABGain const& iObjectGain) = 0;
        
        /**
         * Gets the Cartesian position for this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oObjectPositionUC object Cartesian position within a unit cube space
         *
         */
        virtual void GetObjectPositionToUnitCube(CartesianPosInUnitCube &oObjectPositionUC) const = 0;
        
        /**
         * Sets the Cartesian position for this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iObjectPositionUC object Cartesian position within a unit cube space
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectPositionFromUnitCube(CartesianPosInUnitCube const& iObjectPositionUC) = 0;
        
        /**
         * Gets the object snap flag.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectSnap"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oObjectSnap object snap flag
         *
         */
        virtual void GetObjectSnap(IABObjectSnap &oObjectSnap) const = 0;
        
        /**
         * Sets the object snap flag.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectSnap"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iObjectSnap object snap flag
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectSnap(IABObjectSnap const& iObjectSnap) = 0;
        
        /**
         * Gets the object zone gain for 9 zones.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "Zone Definition"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oObjectZoneGains9 zone gain for 9 zones
         *
         */
        virtual void GetObjectZoneGains9(IABObjectZoneGain9 &oObjectZoneGains9) const = 0;
        
        /**
         * Sets the object zone gain for 9 zones.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "Zone Definition"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iObjectZoneGains9 zone gain for 9 zones
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectZoneGains9(IABObjectZoneGain9 const& iObjectZoneGains9) = 0;
        
        /**
         * Gets the object spread.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectSpread"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oObjectSpread object spread
         *
         */
        virtual void GetObjectSpread(IABObjectSpread &oObjectSpread) const = 0;
        
        /**
         * Sets the object spread.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectSpread"
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iObjectSpread object spread
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectSpread(IABObjectSpread const& iObjectSpread) = 0;
        
        /**
         * Gets decorrelation coefficient for this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[out] oDecorCoef channel decorrelation coefficient
         *
         */
        virtual void GetDecorCoef(IABDecorCoeff &oDecorCoef) const = 0;
        
        /**
         * Sets decorrelation coefficient for this object.
         *
         * @memberof IABObjectSubBlockInterface
         *
         * @param[in] iDecorCoef channel decorrelation coefficient
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetDecorCoef(IABDecorCoeff const& iDecorCoef) = 0;
    };

    /**
     * @brief IABObjectDefinitionInterface
     *
     * Defining APIs for IABObjectDefinition.
     *
	 * List of sub-element handling APIs that will/may delete sub-elements of IABObjectDefinition.
	 *    Delete() - all sub elements in list are deleted
	 *    SetSubElements() - current sub elements which are not in the new list, are deleted.
	 *
	 * List of sub-element handling APIs where instance is NOT deleted.
	 *    GetSubElements()
	 *    ClearSubElements()
	 *    IsSubElement()
	 *    RemoveSubElement()
	 *    AddSubElement()
	 *
	 * List of sub-element handling APIs where ownership of sub element instance is returned or
	 * released back to caller.
	 *    ClearSubElements() - Ownership of all sub element instances in current list is released
	 *    RemoveSubElement() - Ownership of a single specific sub element instance, if found in
	 *                         current list, is released
	 *
	 * @class IABObjectDefinitionInterface
     */
    class IABObjectDefinitionInterface
    {
    public:
        
        /**
         * Creates an IABObjectDefinitionInterface instance
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iFrameRate target frame rate of bitstream for which the object is defined
         *
         * @returns pointer to IABObjectDefinitionInterface instance created
         */
        static IABObjectDefinitionInterface* Create(IABFrameRateType iFrameRate);
        
        /**
         * Deletes an IABObjectDefinitionInterface instance
		 *
		 * @warning During deletion of IABFrameInterface (IABFrame) instance, all sub-element instances owned
		 * by the IABFrame (ie. those in its sub element list) are deleted.
		 *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iInstance pointer to the instance of the IABObjectDefinitionInterface
         */
        static void Delete(IABObjectDefinitionInterface* iInstance);

        /// Destructor.
        virtual ~IABObjectDefinitionInterface() {}

        // Setters and Getters
        
        /**
         * Gets metadata ID of this object.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oMetaID metadata ID
         *
         */
        virtual void GetMetadataID(IABMetadataIDType &oMetaID) const = 0;
        
        /**
         * Sets metadata ID of this object.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iMetaID metadata ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetMetadataID(IABMetadataIDType iMetaID) = 0;
        
        /**
         * Gets ID of audio data that is associated with this object. An AudioDataID of NULL (0) indicates no audio asset associated.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oAudioDataID audio data ID
         *
         */
        virtual void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const = 0;
        
        /**
         * Sets ID of audio data that is associated with this object. An AudioDataID of NULL (0) indicates no audio asset associated.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] oAudioDataID audio data ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetAudioDataID(IABAudioDataIDType iAudioDataID) = 0;
        
        /**
         * Gets conditional object code.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ConditionalObject"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oConditionalObject conditional object code.
         *
         */
        virtual void GetConditionalObject(uint8_t &oConditionalObject) const = 0;
        
        /**
         * Sets conditional object code.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ConditionalObject"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] oConditionalObject conditional object code.
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetConditionalObject(uint8_t iConditionalObject) = 0;
        
        /**
         * Gets object use case code. It indicates the use case for which this element is intended.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectUseCase"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oObjectUseCase object use case code.
         *
         */
        virtual void GetObjectUseCase(IABUseCaseType &oObjectUseCase) const = 0;
        
        /**
         * Sets object use case code. It indicates the use case for which this element is intended.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "ObjectUseCase"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iObjectUseCase object use case code.
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetObjectUseCase(IABUseCaseType iObjectUseCase) = 0;
        
        /**
         * Gets number of panning sub-blocks.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oNumPanSubBlocks number of panning sub-blocks
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual void GetNumPanSubBlocks(uint8_t &oNumPanSubBlocks) const = 0;
        
        /**
         * Gets sub-block panning parameters.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oPanSubBlocks sub-block panning parameters
         *
         */
        virtual void GetPanSubBlocks(std::vector<IABObjectSubBlock*> &oPanSubBlocks) const = 0;
        
        /**
         * Sets sub-block panning parameters.
		 * @note: Client to construct pan sub block instances stored (pointed to) in "iPanSubBlocks".
		 * Once passed to an IAObjectDefinition, IAObjectDefinition takes over ownership and management of all instances
		 * whose pointers are stored in "iPanSubBlocks". Clients must not delete any of the instances
		 * referenced from "iPanSubBlocks" outside the IAObjectDefinition instance.
		 *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iPanSubBlocks sub-block panning parameters
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetPanSubBlocks(const std::vector<IABObjectSubBlock*> iPanSubBlocks) = 0;
        
        /**
         * Gets audio description for this object.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "AudioDescription"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oAudioDescription audio description
         *
         */
        virtual void GetAudioDescription(IABAudioDescription &oAudioDescription) const = 0;
        
        /**
         * Sets audio description for this object.
         * For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "AudioDescription"
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iAudioDescription audio description
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetAudioDescription(IABAudioDescription const& iAudioDescription) = 0;
        
        /**
         * Gets number of elements (sub-elements) contained in this element.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oCount number of sub-elements
         *
         */
        virtual void GetSubElementCount(IABElementCountType &oCount) const = 0;
        
        /**
         * Gets elements (sub-elements) contained in this element.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed within a object definiton element.
         *
         * @memberof IABObjectDefinitionInterface
         *
         * @param[out] oSubElements vector to save pointers to the sub-elements
         *
         */
        virtual void GetSubElements(std::vector<IABElement*> &oSubElements) const = 0;
        
        /**
         * Sets elements (sub-elements) contained in this element.
         * See SMPTE Immersive Audio Bitstream specification document for types of sub-element allowed 
		 * within a object definiton element.
		 *
		 * @warning Deletion of sub-element instance may occur if client calls API more than once.
		 *
		 * @note: Client to construct elements stored (pointed to) in "iSubElements".
		 * Once passed to an IAObjectDefinition, IAObjectDefinition takes over ownership and management 
		 * of all elements whose pointers are stored in "iSubElements". Clients must not delete any of 
		 * the elements referenced from "iSubElements" outside the IAObjectDefinition instance.
		 *
		 * @warning If the current sub element list is not empty (eg. when this API is called the
		 * 2nd time or multiple times), a sub element instance object in the current list is deleted, unless
		 * the same sub element also appears in the "iSubElements" list to be set by this call.
         * 
         * @memberof IABObjectDefinitionInterface
         *
         * @param[in] iSubElements vector to save pointers to the sub-elements
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetSubElements(const std::vector<IABElement*> iSubElements) = 0;

		/**
		* Clear sub-element list and reset sub-element count to 0.
		* This call simply clears the list, but does not delete any of the pointed sub element
		* instances. By clearing the list, the ownship to all of the sub element instance is
		* returned to caller. Caller is responsible for managing and cleaning out released
		* IAB element instances.
		*
		* @memberof IABObjectDefinitionInterface
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError ClearSubElements() = 0;

		/**
		* Check if an IABElement instance is a sub element of this element.
		*
		* @memberof IABObjectDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance for checking
		*
		* @returns bool true: is a sub element, false: not a sub-element
		*/
		virtual bool IsSubElement(IABElement* iSubElement) = 0;

		/**
		* Remove a single IABElement instance from the sub element list of this element.
		* Note that this API simply removes the input instance, if found, from the
		* sub element list of this element. It by design does not delete the underlying
		* instance *iSubElement. Caller takes over the ownership of instance, for
		* re-purposing for example. Caller must delete the instance outside this call
		* if it is no longer needed.
		*
		* @memberof IABObjectDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be removed.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate
		*          an error (eg iSubElement not found in subelement list, etc)
		*/
		virtual iabError RemoveSubElement(IABElement* iSubElement) = 0;

		/**
		* Add a single IABElement instance to the sub element list of this element.
		* Note that ownership of a successfully added instance *iSubElement is transferred to
		* this element.
		*
		* @memberof IABObjectDefinitionInterface
		*
		* @param[in] iSubElement pointer to IABElement instance to be added.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate
		*       an error (eg *iSubElement is not a type allowed, or already a sub element
		*/
		virtual iabError AddSubElement(IABElement* iSubElement) = 0;

		/**
		* Enable packing of this object definition element.
		*
		* @memberof IABObjectDefinitionInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this object definition element.
		*
		* @memberof IABObjectDefinitionInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this object definition element is enabled (include/used) in packing.
		*
		* @memberof IABObjectDefinitionInterface
		*/
		virtual bool IsIncludedForPacking() = 0;

		/**
		* Get number of known but unallowed object sub-elements.
		* This API can be used to check/retrieve number of unallowed object sublements encountered
		* during parsing ObjectDefinition, eg. after DeSerialize() call.
		*
		* @memberof IABObjectDefinitionInterface
		*/
		virtual uint32_t GetNumUnallowedSubElements() const = 0;

		/**
		* Get number of undefined object sub-elements.
		* This API can be used to check/retrieve number of undefined object sublements encountered
		* during parsing ObjectDefinition, eg. after DeSerialize() call.
		*
		* @memberof IABObjectDefinitionInterface
		*/
		virtual uint32_t GetNumUndefinedSubElements() const = 0;
	};

	/**
	* @brief IABZone19SubBlockInterface
	*
	* Defining APIs for IABZone19SubBlock.
	*
	* @class IABZone19SubBlockInterface
	*/
	class IABZone19SubBlockInterface
	{
	public:

		/**
		* Creates an IABZone19SubBlockInterface instance
		*
		* @memberof IABZone19SubBlockInterface
		*
		* @returns pointer to IABZone19SubBlockInterface instance created
		*/
		static IABZone19SubBlockInterface* Create();

        /**
         * Deletes an IABZone19SubBlockInterface instance
         *
         * @memberof IABZone19SubBlockInterface
         *
         * @param[in] iInstance pointer to the instance of the IABZone19SubBlockInterface
         */
        static void Delete(IABZone19SubBlockInterface* iInstance);

		/// Destructor.
		virtual ~IABZone19SubBlockInterface() {}

		// Setters and Getters

		/**
		* Gets the object zone gain for 19 zones.
		* For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "Zone Definition"
		*
		* @memberof IABZone19SubBlockInterface
		*
		* @param[out] oObjectZoneGains19 zone gain for 19 zones
		*
		*/
		virtual void GetObjectZoneGains19(IABObjectZoneGain19 &oObjectZoneGains19) const = 0;

		/**
		* Sets the object zone gain for 19 zones.
		* For more information, see section in SMPTE Immersive Audio Bitstream specification document related to "Zone Definition"
		*
		* @memberof IABZone19SubBlockInterface
		*
		* @param[in] iObjectZoneGains19 zone gain for 19 zones
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetObjectZoneGains19(IABObjectZoneGain19 const& iObjectZoneGains19) = 0;

	};

	/**
     * @brief IABObjectZoneDefinition19Interface
     *
     * Defining APIs for IABObjectZoneDefinition19.
     *
     * @class IABObjectZoneDefinition19Interface
     */
    class IABObjectZoneDefinition19Interface
    {
    public:
        
        /**
         * Creates an IABObjectZoneDefinition19Interface instance
         *
         * @memberof IABObjectZoneDefinition19Interface
         *
		 * @param[in] iFrameRate target frame rate of bitstream for which zone19 element is defined
		 *
		 * @returns pointer to IABObjectZoneDefinition19Interface instance created
         */
        static IABObjectZoneDefinition19Interface* Create(IABFrameRateType iFrameRate);
        
        /**
         * Deletes an IABObjectZoneDefinition19Interface instance
         *
         * @memberof IABObjectZoneDefinition19Interface
         *
         * @param[in] iInstance pointer to the instance of the IABObjectZoneDefinition19Interface
         */
        static void Delete(IABObjectZoneDefinition19Interface* iInstance);

        /// Destructor.
        virtual ~IABObjectZoneDefinition19Interface() {}

		// Setters and Getters

		/**
		* Gets number of zone19 sub-blocks.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*
		* @param[out] oNumZone19SubBlocks number of zone19 sub-blocks
		*
		*/
		virtual void GetNumZone19SubBlocks(uint8_t &oNumZone19SubBlocks) const = 0;

		/**
		* Gets zone19 sub-block of zone gain coefficients.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*
		* @param[out] oZone19SubBlocks a list of pointers to zone19 sub-blocks
		*
		*/
		virtual void GetZone19SubBlocks(std::vector<IABZone19SubBlock*> &oZone19SubBlocks) const = 0;

		/**
		* Sets zone19 sub-blocks.
		* @note: Client to construct zone19 sub block instances stored (pointed to) in "iZone19SubBlocks".
		* Once passed to an IABObjectZoneDefinition19, IABObjectZoneDefinition19 takes over ownership and 
		* management of all instances whose pointers are stored in "iZone19SubBlocks". Clients must not 
		* delete any of the instances referenced from "iZone19SubBlocks" outside the 
		* IABObjectZoneDefinition19 instance.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*
		* @param[in] iZone19SubBlocks a list of pointers to existent zone19 sub-blocks
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetZone19SubBlocks(const std::vector<IABZone19SubBlock*> iZone19SubBlocks) = 0;

		/**
		* Enable packing of this object zone 19 element.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this object zone 19 element.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this object zone 19 element is enabled (include/used) in packing.
		*
		* @memberof IABObjectZoneDefinition19Interface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

	/**
	* @brief IABAuthoringToolInfoInterface
	*
	* Defining APIs for IABAuthoringToolInfo.
	*
	* @class IABAuthoringToolInfoInterface
	*/
	class IABAuthoringToolInfoInterface
	{
	public:

		/**
		* Creates an IABAuthoringToolInfoInterface instance
		*
		* @memberof IABAuthoringToolInfoInterface
		*
		* @returns pointer to IABAuthoringToolInfoInterface instance created
		*/
		static IABAuthoringToolInfoInterface* Create();

        /**
         * Deletes an IABAuthoringToolInfoInterface instance
         *
         * @memberof IABAuthoringToolInfoInterface
         *
         * @param[in] iInstance pointer to the instance of the IABAuthoringToolInfoInterface
         */
        static void Delete(IABAuthoringToolInfoInterface* iInstance);

		// Destructor.
		virtual ~IABAuthoringToolInfoInterface() {}

		// Setters and Getters

		/**
		* Gets authoring Tool Info URI. Authoring Tool Info text is defined as "NULL-terminated, 
		* strict ASCII Text".
		*
		* @memberof IABAuthoringToolInfoInterface
		*
		* @param[out] oAuthoringToolURI pointer (by refernce) to NULL-terminated authroing tool info text.
		*
		*/
		virtual void GetAuthoringToolInfo(const char* &oAuthoringToolURI) const = 0;

		/**
		* Sets authoring Tool Info URI. Authoring Tool Info text is defined as "NULL-terminated,
		* strict ASCII Text".
		*
		* @memberof IABAuthoringToolInfoInterface
		*
		* @param[in] iAuthoringToolURI pointer to client defined, NULL-terminated authroing tool info text.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetAuthoringToolInfo(const char* iAuthoringToolURI) = 0;

		/**
		* Enable packing of this authoring tool info element.
		*
		* @memberof IABAuthoringToolInfoInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this authoring tool info element.
		*
		* @memberof IABAuthoringToolInfoInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this authoring tool info element is enabled (include/used) in packing.
		*
		* @memberof IABAuthoringToolInfoInterface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

	/**
	* @brief IABUserDataInterface
	*
	* Defining APIs for IABUserData.
	*
	* @class IABUserDataInterface
	*/
	class IABUserDataInterface
	{
	public:

		/**
		* Creates an IABUserDataInterface instance
		*
		* @memberof IABUserDataInterface
		*
		* @returns pointer to IABUserDataInterface instance created
		*/
		static IABUserDataInterface* Create();

        /**
         * Deletes an IABUserDataInterface instance
         *
         * @memberof IABUserDataInterface
         *
         * @param[in] iInstance pointer to the instance of the IABUserDataInterface
         */
        static void Delete(IABUserDataInterface* iInstance);

		// Destructor.
		virtual ~IABUserDataInterface() {}

		// Setters and Getters

		/**
		* Gets user ID for IAB user data element.
		*
		* @memberof IABUserDataInterface
		*
		* @param[out] oUserID 16-byte User ID for the data element (a SMPTE label).
		*
		*/
		virtual void GetUserID(uint8_t (&oUserID)[16]) const = 0;

		/**
		* Sets user ID for IAB user data element.
		*
		* @memberof IABUserDataInterface
		*
		* @param[in] iUserID 16-byte User ID for the data element (a SMPTE label). iUserID must
		* be an array of 16-byte long (128-bit) size, and compliant with SMPTE label authority 
		* requirements.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetUserID(const uint8_t (&iUserID)[16]) = 0;

		/**
		* Gets user data bytes for IAB user data element.
		*
		* @memberof IABUserDataInterface
		*
		* @param[out] oUserDataBytes User data currently held in user data element.
		*
		*/
		virtual void GetUserDataBytes(std::vector<uint8_t> &oUserDataBytes) const = 0;

		/**
		* Sets user data bytes for IAB user data element.
		*
		* @memberof IABUserDataInterface
		*
		* @param[in] iUserDataBytes User data to set to for the user data element.
		* It is accepted as uint8_t bytes but can contained any binary data as user defines. 
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetUserDataBytes(std::vector<uint8_t> iUserDataBytes) = 0;

		/**
		* Enable packing of this user data element.
		*
		* @memberof IABUserDataInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this user data element.
		*
		* @memberof IABUserDataInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this user data element is enabled (include/used) in packing.
		*
		* @memberof IABUserDataInterface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

	/**
     * @brief IABAudioDataDLCInterface
     *
     * Defining APIs for IABAudioDataDLC.
     *
     * @class IABAudioDataDLCInterface
     */
    class IABAudioDataDLCInterface
    {
    public:
        
        /**
         * Creates an IABAudioDataDLCInterface instance
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[in] iFrameRateCode Frame rate of the IA bitstream. This determines the number of DLC subblocks to use.
		 * @param[in] iSampleRate Sample rate of the IA bitstream.
		 *
         * @returns pointer to IABAudioDataDLCInterface instance created
         */
        static IABAudioDataDLCInterface* Create(IABFrameRateType iFrameRateCode, IABSampleRateType iSampleRate);
        
        /**
         * Deletes an IABAudioDataDLCInterface instance
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[in] iInstance pointer to the instance of the IABAudioDataDLCInterface
         */
        static void Delete(IABAudioDataDLCInterface* iInstance);

        /// Destructor.
        virtual ~IABAudioDataDLCInterface() {}

        // Setters and Getters
        
        /**
         * Gets ID of this audio data element.
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[out] oAudioDataID audio data ID
         *
         */
        virtual void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const = 0;
        
        /**
         * Sets ID of this audio data element.
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[in] iAudioDataID audio data ID
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetAudioDataID(IABAudioDataIDType iAudioDataID) = 0;
        
        /**
         * Gets size of DLC audio data block
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[out] oDLCSize DLC audio data block in bytes
         *
         */
        virtual void GetDLCSize(uint16_t &oDLCSize) const = 0;
        
        /**
         * Sets size of DLC audio data block
         *
         * @memberof IABAudioDataDLCInterface
         *
         * @param[in] iDLCSize DLC audio data block in bytes
         *
         * @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError SetDLCSize(uint16_t iDLCSize) = 0;

		/**
		* Gets sample rate of DLC audio data block
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @param[out] oDLCSampleRate DLC audio sample rate in enum IABSampleRateType
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError GetDLCSampleRate(IABSampleRateType &oDLCSampleRate) const = 0;

		/**
		* Sets sample rate of DLC audio data block.
		* Note: It is important to set correct sample rate for the DLC element
		* before encoding a block of samples (by calling EncodeMonoPCMToDLC()).
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @param[in] iDLCSampleRate DLC sample rate in enum IABSampleRateType
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetDLCSampleRate(IABSampleRateType iDLCSampleRate) = 0;

		/**
		* Encode a block of 24-bit PCM samples (as 3 MSBs of 32-bit int32_t) into DLC data.
		* Note: It is important that sample rate for the DLC element is set correctly before calling 
		* this function for encoding a block of samples. The encoded DLC data is stored internally 
		* in DLC element. 
		*
		* !Note on parameter "iSampleCount"
		* The value of input parameter "iSampleCount" can only be set to a limited number of fixed values,
		* as specified in SMPTE standard ST2098-2:2019, Table 18. The values are defined for a fixed
		* number of supported "frameRate & sampleRate" combinations. For example, for a frameRate of
		* 24fps and sampleRate of 48k, iSampleCount must be set to the frame size of 2000 only, as defined
		* by the standard. Otherwise, calls to EncodeMonoPCMToDLC() will result in error being returned.
		*
		* Client may call utility function in the IAB SDK, GetIABNumFrameSamples(), to get the frame sample 
		* count with the frameRate and sampleRate of instance.
		*
		* It is important to note that, "iSampleCount" is not designed to accept any free integer numbers.
		* Rather, in this API, it serves the following 2 purposes.
		* 1) "iSampleCount" is to support EncodeMonoPCMToDLC() with permitted "frameRate & sampleRate"
		*    combinations;
		* 2) "iSampleCount" is used as a means for checking & verifying client calls of EncodeMonoPCMToDLC()
		*    are compliant with the internal configuration of IABAudioDataDLC instance.
		*
		* Therefore, client must always make EncodeMonoPCMToDLC() calls with an iSampleCount value that
		* is equal to the frame size defined for the "frameRate & sampleRate" combination configured for
		* the IABAudioDataDLC instance. Furthermore, client must ensure that input buffer, iSamples,
		* contain iSampleCount valid audio samples. If the number of audio samples is less than frame size,
		* eg. during encoding the last/remainder part of a long program, it must be padded with 0 value
		* samples (silence) as appropriate to the full frame size before making EncodeMonoPCMToDLC() call.
		* By ST2098-2, IAB supports integral frame-based processing. It does not have provisions for partial
		* frame DLC encoding or decoding.
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @param[in] iSamples Pointer to a sequence of 24-bit audio samples to be encoded. 
		*            Each samples must be in the range [-8,388,608, 8,388,607].
		*            Must not be nullptr.
		* @param[in] iSampleCount number of samples to be encoded. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataDLC element.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError EncodeMonoPCMToDLC(const int32_t* iSamples, uint32_t iSampleCount) = 0;

		/**
		* Decode (from internal DLC data) to a block of 24-bit PCM samples (as 3 MSBs of 32-bit int32_t).
		* This function is intended to be used during DLC decoding process, such as
		* after parsing IAB bitstream data into a DLC element. The DLC data to be decoded 
		* is stored internally in DLC element.
		*
		* !Note on parameter "iSampleCount" and "iDecodeSampleRate"
		* The value of input parameter "iSampleCount" can only be set to a limited number of fixed values,
		* as specified in SMPTE standard ST2098-2:2019, Table 18. The values are defined for a fixed
		* number of supported "frameRate & sampleRate" combinations. For example, for a frameRate of
		* 24fps and sampleRate of 48k, iSampleCount must be set to the frame size of 2000 only, as defined
		* by the standard. Otherwise, calls to DecodeDLCToMonoPCM() will result in error being returned.
		*
		* It is important to note that, "iSampleCount" is not designed to accept any free integer numbers.
		* Rather, in this API, it serves the following 2 purposes.
		* 1) "iSampleCount" is to support DecodeDLCToMonoPCM() with permitted "frameRate & sampleRate"
		*    combinations;
		* 2) "iSampleCount" is used as a means for checking & verifying client calls of DecodeDLCToMonoPCM()
		*    are compliant with the internal configuration of IABAudioDataDLC instance.
		*
		* In addition, the DecodeDLCToMonoPCM() has an additional parameter of "iDecodeSampleRate". This
		* parameter is designed to support decoding 96k IABAudioDataDLC element into 48k output PCM. This
		* possibility adds another dimension.
		* 1) When the iDecodeSampleRate value in a call is equal to the samplerate of the IABAudioDataDLC
		*    instance, iSampleCount value must be equal to frame size
		* 2) When the iDecodeSampleRate value is 48k during a call to an 96k IABAudioDataDLC instance,
		*    iSampleCount value then must be set to half of the 96k frame size. This is due to down sampling
		*    by a factor of 2.
		*
		* Therefore, client must always make DecodeDLCToMonoPCM() calls with an iSampleCount value that
		* is equal to the frame size, or half of that when down sampling by 2, as defined for the "frameRate & 
		* sampleRate" combination configured for the IABAudioDataDLC instance. For an instance that is parsed
		* and constructed from an IAB bitstream, client may simply call utility function in the IAB SDK,
		* GetIABNumFrameSamples() to get the sample count using the frameRate and sampleRate of instance.
		* Also consider if down-sampling is used in determining the iSampleCount value when making a
		* DecodeDLCToMonoPCM() call. By ST2098-2, IAB supports integral frame-based processing. It does not 
		* have provisions for partial frame packing or unpacking.
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @param[in/out] oSamples Pointer to a sequence of 32-bit audio samples into which 
		*                decoded samples will be stored. The 3 MSBs are the 24-bit PCM. 
		*                Caller must pre-allocate buffer of sufficient size to accept output samples.
		*                Must not be nullptr.
		* @param[in] iSampleCount number of samples to be decoded. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataDLC element. See API notes above.
		* @param[in] iDecodeSampleRate Desired sample rate of the decoded samples. This can be used for 
		*            decoding 96k DLC into 48k output.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError DecodeDLCToMonoPCM(int32_t* oSamples, uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate) = 0;


		/**
		* Similar to DecodeDLCToMonoPCM(int32_t* oSamples, uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate), 
		* decoded PCM samples are stored internally.
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @param[in] iSampleCount number of samples to be decoded. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataDLC element. See API notes above.
		* @param[in] iDecodeSampleRate Desired sample rate of the decoded samples. This can be used for
		*            decoding 96k DLC into 48k output.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError DecodeDLCToMonoPCMInternal(uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate) = 0;

		/**
		* Return pointer to decoded PCM samples (in int32_t) held in internal buffer.
		*
		* @memberof IABAudioDataDLCInterface
		*
		* @returns \Pointer to internal buffer.
		*/
		virtual int32_t* GetDecodedSampleBuffer() = 0;

		/**
		* Enable packing of this DLC element.
		*
		* @memberof IABAudioDataDLCInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this DLC element.
		*
		* @memberof IABAudioDataDLCInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this DLC element is enabled (include/used) in packing.
		*
		* @memberof IABAudioDataDLCInterface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

    /**
     * @brief IABAudioDataPCMInterface
     *
     * Defining APIs for IABAudioDataPCM.
     *
     * @class IABAudioDataPCMInterface
     */
    class IABAudioDataPCMInterface
    {
    public:
        
		/**
		* Creates an IABAudioDataPCMInterface instance
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[in] iFrameRateCode Frame rate of the IA bitstream for which the PCM element is used.
		* @param[in] iSampleRate PCM sample rate.
		* @param[in] iBitDepth PCM sample bit depth.
		*
		* @returns pointer to IABAudioDataPCMInterface instance created
		*/
		static IABAudioDataPCMInterface* Create(IABFrameRateType iFrameRateCode
			, IABSampleRateType iSampleRate
			, IABBitDepthType iBitDepth);

        /**
         * Deletes an IABAudioDataPCMInterface instance
         *
         * @memberof IABAudioDataPCMInterface
         *
         * @param[in] iInstance pointer to the instance of the IABAudioDataPCMInterface
         */
        static void Delete(IABAudioDataPCMInterface* iInstance);

        // Destructor.
        virtual ~IABAudioDataPCMInterface() {}

        // Setters and Getters

		/**
		* Gets ID of this audio data element.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[out] oAudioDataID audio data ID
		*
		*/
		virtual void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const = 0;

		/**
		* Sets ID of this audio data element.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[in] iAudioDataID audio data ID
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetAudioDataID(IABAudioDataIDType iAudioDataID) = 0;

		/**
		* Gets frame rate for which PCM audio data block is intended
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @return PCM frame rate code
		*/
		virtual IABFrameRateType GetPCMFrameRate() const = 0;

		/**
		* Gets sample rate of PCM audio data block
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @return PCM audio sample rate code
		*/
		virtual IABSampleRateType GetPCMSampleRate() const = 0;

		/**
		* Gets bit depth of PCM audio data block
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @return PCM audio bit depth code
		*/
		virtual IABBitDepthType GetPCMBitDepth() const = 0;

		/**
		* Gets sample count of PCM audio data block
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @return PCM buffer sample count
		*/
		virtual uint32_t GetPCMSampleCount() const = 0;

		/**
		* Pack a block of 24-/16-bit PCM samples (as 3/2 MSBs of 32-bit int32_t) into AudioDataPCM data.
		* Note: It is important that frame rate, sample rate and bit depth pamaters for the PCM element
		* are set correctly in instantiation. The iSampleCount must match these parameters per ST2098-2
		* specification. (iSampleCount can be queried through API GetPCMSampleCount().
		* Packed PCM data is stored as sequence of unsigned bytes internally in PCM element.
		*
		* !Note on parameter "iSampleCount"
		* The value of input parameter "iSampleCount" can only be set to a limited number of fixed values,
		* as specified in SMPTE standard ST2098-2:2019, Table 18. The values are defined for a fixed
		* number of supported "frameRate & sampleRate" combinations. For example, for a frameRate of
		* 24fps and sampleRate of 48k, iSampleCount must be set to the frame size of 2000 only, as defined
		* by the standard. Otherwise, calls to PackMonoSamplesToPCM() will result in error being returned.
		*
		* Client may call utility function in the IAB SDK, GetIABNumFrameSamples(), to get the frame sample
		* count with the frameRate and sampleRate of instance.
		*
		* It is important to note that, "iSampleCount" is not designed to accept any free integer numbers.
		* Rather, in this API, it serves the following 2 purposes.
		* 1) "iSampleCount" is to support PackMonoSamplesToPCM() with defined "frameRate & sampleRate" 
		*    combinations;
		* 2) "iSampleCount" is used as a means for checking & verifying client calls of PackMonoSamplesToPCM()
		*    are compliant with the internal configuration of IABAudioDataPCM instance.
		*
		* Therefore, client must always make PackMonoSamplesToPCM() calls with an iSampleCount value that 
		* is equal to the frame size defined for the "frameRate & sampleRate" combination configured for
		* the IABAudioDataPCM instance. Furthermore, client must ensure that input buffer, iSamples,
		* contain iSampleCount valid audio samples. If the number of audio samples is less than frame size,
		* eg. during encoding the last/remainder part of a long program, it must be padded with 0 value
		* samples (silence) as appropriate to the full frame size before making PackMonoSamplesToPCM() call. 
		* By ST2098-2, IAB supports integral frame-based processing. It does not have provisions for partial 
		* frame packing or unpacking.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[in] iSamples Pointer to a sequence of 24-/16-bit audio samples to be packed.
		*            Each samples must be in the range [-8,388,608, 8,388,607] << 8, if 24-bit, or
		*            in the range [-32,768, 32,767] << 16, if 16-bit.
		*            Must not be nullptr.
		* @param[in] iSampleCount number of samples to be packed. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataPCM element.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError PackMonoSamplesToPCM(const int32_t* iSamples, uint32_t iSampleCount) = 0;

		/**
		* Unpack from PCM element to a block of 24-/16-bit PCM samples (as 3/2 MSBs of 32-bit int32_t).
		* The bit depth is determined inherently by PCM element parameter, PCMBitDepth, which
		* can be queried by using API GetPCMBitDepth().
		*
		* This function is intended to be used during IAB decoding/rendering process, such as
		* after parsing IAB bitstream data into a PCM element. The PCM bytes to be unpacked
		* is stored internally in PCM element.
		*
		* !Note on parameter "iSampleCount"
		* The value of input parameter "iSampleCount" can only be set to a limited number of fixed values,
		* as specified in SMPTE standard ST2098-2:2019, Table 18. The values are defined for a fixed
		* number of supported "frameRate & sampleRate" combinations. For example, for a frameRate of
		* 24fps and sampleRate of 48k, iSampleCount must be set to the frame size of 2000 only, as defined
		* by the standard. Otherwise, calls to UnpackPCMToMonoSamples() will result in error being returned.
		*
		* It is important to note that, "iSampleCount" is not designed to accept any free integer numbers.
		* Rather, in this API, it serves the following 2 purposes.
		* 1) "iSampleCount" is to support UnpackPCMToMonoSamples() with defined "frameRate & sampleRate"
		*    combinations;
		* 2) "iSampleCount" is used as a means for checking & verifying client calls of UnpackPCMToMonoSamples()
		*    are compliant with the internal configuration of IABAudioDataPCM instance.
		*
		* Therefore, client must always make UnpackPCMToMonoSamples() calls with an iSampleCount value that
		* is equal to the frame size defined for the "frameRate & sampleRate" combination configured for
		* the IABAudioDataPCM instance. For an instance that is parsed and constructed from an IAB bitstream,
		* client may simply call GetPCMSampleCount() to get the sample count (also equal to frame size) and
		* use the value in making a UnpackPCMToMonoSamples() call. By ST2098-2, IAB supports integral 
		* frame-based processing. It does not have provisions for partial frame packing or unpacking.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[in/out] oSamples Pointer to a sequence of 32-bit audio samples into which
		*                unpacked samples will be stored. The 3 or 2 MSBs are the 24-bit or 16-bit PCM samples.
		*                Caller must pre-allocate buffer of sufficient size to accept output samples.
		*                Must not be nullptr.
		* @param[in] iSampleCount number of samples to be unpacked. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataPCM element.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError UnpackPCMToMonoSamples(int32_t* oSamples, uint32_t iSampleCount) = 0;

		/**
		* Similar to UnpackPCMToMonoSamples(int32_t* oSamples, uint32_t iSampleCount), unpacked PCM samples
		* are stored internally.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @param[in] iSampleCount number of samples to be unpacked. Note that passed value must agree with
		*            frameRate & sampleRate configured for the IABAudioDataPCM element.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError UnpackPCMToMonoSamplesInternal(uint32_t iSampleCount) = 0;

		/**
		* Return pointer to unpacked PCM samples (in int32_t) held in internal buffer.
		*
		* @memberof IABAudioDataPCMInterface
		*
		* @returns \Pointer to internal buffer.
		*/
		virtual int32_t* GetUnpackedSampleBuffer() = 0;

		/**
		* Enable packing of this PCM element.
		*
		* @memberof IABAudioDataPCMInterface
		*/
		virtual void EnablePacking() = 0;

		/**
		* Disable packing of this PCM element.
		*
		* @memberof IABAudioDataPCMInterface
		*/
		virtual void DisablePacking() = 0;

		/**
		* Check if this PCM element is enabled (include/used) in packing.
		*
		* @memberof IABAudioDataPCMInterface
		*/
		virtual bool IsIncludedForPacking() = 0;
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABELEMENTSAPI_H__
