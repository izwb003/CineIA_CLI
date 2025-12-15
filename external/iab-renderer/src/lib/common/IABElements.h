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
 * Class definitions header file for IABElements.
 *
 * Defining classes for Immersive Audio Bitstream elements, according to
 * specification.
 *
 * @file
 */

#ifndef __IABELEMENTS_H__
#define	__IABELEMENTS_H__

// IAB library headers
#include "IABDataTypes.h"
#include "IABElementsAPI.h"
#include "IABErrors.h"
#include "common/IABConstants.h"

// Common stream headers
#include "commonstream/stream/StreamTypes.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/bitstream/BitStreamReaderT.h"

// DLC codec lib headers
#include "DLC/DLCAudioData.h"
#include "DLC/DLCSimpleEncoder.h"
#include "DLC/DLCFullDecoder.h"

//using namespace CMNSTRM;
using namespace CommonStream;

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	typedef BitStreamWriterT<std::ostream> StreamWriter;
	typedef BitStreamReaderT<std::istream> StreamReader;

	/*****************************************************************************
	*
	* Defining classes for container/subframe structures, according to
	* specification.
	*
	*****************************************************************************/

	/**
	* @brief IABContainerSubFrame, Base class.
	* Per specification, IABFrame must be wrapped inside a Tag-Length-Value structure called IABIASubFrame
	* (aka container "SubFrame"). Note that an IABIASubFrame "contains" an IABFrame.
	*
	* IABContainerSubFrame has 2 derived/child classes
	* 1. IABIASubFrame
	* 2. IABPreamble (included in v4.31 draft spec). Presence of IABPreamble is mandatory
	*    (IABPreamble may also be called IABPCMSubFrame sometimes during early 25CSS discussion.)
	*
	* Also note that, the sum combination of "IABPreamble + IABIASubFrame" is called
	* "IABistreamFrame" in specification.
	*
	* IABContainerSubFrame and its child classes are implemented as internals to IAB libraries.
	* This means that the layer of "container" is currently transparent to IAB library users. No public
	* APIs are currently offered to library users.
	*
	*/
	class IABContainerSubFrame
	{
	public:

		// Constructor
		IABContainerSubFrame(IABContainerSubFrameTagType iSubFrameTag);

		// Destructor
		virtual ~IABContainerSubFrame();

		// Get subFrame length
		void GetSubframeLength(IABSubframeLengthType &oSubframeLength);

		// Set subFrame length
		iabError SetSubframeLength(IABSubframeLengthType iSubframeLength);

		// Processing method Serilize() to a buffer: packing subframe
		// Must be implemented by child class
		virtual iabError Serialize(std::ostream &outStream) = 0;

		// Parser base class deserialize function.
		// Must be implemented by child class
		virtual iabError DeSerialize(StreamReader& streamReader) = 0;

	protected:

		// Protected functions

		// Protected variables

		// SubFrame tag (8 bits).
		IABContainerSubFrameTagType subframeTag_;					// 8-bit

		// SubFrame length
		IABSubframeLengthType subframeLength_;						// 32-bit

		// SubFrame payload
		uint8_t* subframePayload_;									// pointer to "subframeLength_" number of bytes. 
																	// (Not currently used. TBD: May be used in future for holding Preamble payload.)

		// ************
		// For packing, stream writer and working buffer for container subframe tag & length
		StreamWriter		containerHeadWriter_;
		std::stringstream	containerHeadBuffer_;
	};

	/**
	* @brief IABIASubFrame class.
	*
	* An container level subframe that contains an IAB frame.
	* Although container sits above the IABFrame level (as it contains an IABFrame), the
	* current IAB libraries uses this as a simple "container" wrapper so to pack to/parse
	* from MXF-ready IABitstreamFrame.
	*
	*/
	class IABIASubFrame : public virtual IABContainerSubFrame
	{
	public:

		// Constructor
		IABIASubFrame();

		// Destructor
		~IABIASubFrame() {}

		// Processing method Serilize() to a buffer: packing tag and length
		iabError Serialize(std::ostream &outStream);

		// Processing method deserilize() from a buffer: parsing tag, length and payload
		iabError DeSerialize(StreamReader& streamReader);
	};

	/**
	* @brief IABPreamble class.
	*
	* An container level subframe that contains monitoring mono PCM (as preamble).
	*
	*/
	class IABPreamble : public virtual IABContainerSubFrame
	{
	public:

		// Constructor
		IABPreamble();

		// Destructor
		~IABPreamble() {}

		// Get preamble payload
		void GetPreamblePayload(uint8_t* &oPreamblePayload, IABSubframeLengthType &oLength) const;

		// Set preamble payload (!Note: transfer ownership of underlying payload buffer)
		iabError SetPreamblePayload(uint8_t* iPreamblePayload, IABSubframeLengthType iLength);

		// Processing method Serilize() to a buffer: packing tag, length and optionally value (depending on presence)
		iabError Serialize(std::ostream &outStream);

		// Processing method deserilize() from a buffer: parsing tag, length and value/payload
		iabError DeSerialize(StreamReader& streamReader);
	};

    /*****************************************************************************
     *
     * Defining classes for Immersive Audio Bitstream elements, according to
     * specification.
     *
     *****************************************************************************/

    /**
     * @brief IAB Element, Base class.
     *
     */
    class IABElement
    {
    public:

        // Constructor
        IABElement(IABElementIDType iElementID);

        // Destructor
		virtual ~IABElement();

        // Get element ID
        void GetElementID(IABElementIDType &oElementID) const;

        // Get element size
        void GetElementSize(IABElementSizeType &oElementSize) const;

		// Enable packing of this element
		void EnablePacking();

		// Disable packing of this element
		void DisablePacking();

		// Interrogate if the element is included for packing
		bool IsIncludedForPacking();

		// Processing method Serilize() to a buffer: packing an element
		// Must be properly implemented by child class
		virtual iabError Serialize(std::ostream &outStream) { return kIABNotImplementedError; }

		// Processing method DeSerilize() from a buffer: parsing an element
		// Must be properly implemented by child class
        virtual iabError DeSerialize(StreamReader& streamReader) { return kIABNotImplementedError; }
        
        // Validate element against specified IAB constraint sets.
        virtual bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const { return false; }

	protected:

        // Protected functions

		/**
		* Serialize element ID and size only, into elementHeadBuffer_. The buffer can be accessed
		* and retrieved by all derived classes of all element types.
		* This is typically called after writing element payload, and as a result, the element size
		* becomes known. Caller need to set element size before serializing updated element
		* head into elementHeadBuffer_.
		*
		*/
		iabError SerializeHead();

		/**
		* Deserialize element ID and size only... (To be expanded)
		*
		*/
		iabError DeSerializeHead(StreamReader& streamReader);

        // Set element size
        iabError SetElementSize(IABElementSizeType iElementSize);
        
        // Skip deSerializing current element, advance stream reader by the element's size to new stream position
        iabError SkipDeSerializeCurrentElement(StreamReader& streamReader);

		// Peek next sub-element ID
		iabError PeekNextElementID(StreamReader& streamReader, uint32_t &oElementID);

		// Protected variables

        // Element identifier (32 bits).
        IABElementIDType elementID_;			// plex(8) coded (with range set to uint64_t in enum)

        // Element payload size, in # of byte, not including elementID_ and elementSize_.
        IABElementSizeType elementSize_;		// plex(8) coded (with range set to uint64_t)

		// ************
		// Packing enable flag - is this element included (enabled) in IAB packing?
		int32_t packingEnable_;					// default: 1, packing enabled

		// ************
		// For packing, stream writer and working buffer for elementID_ and elementSize_
		StreamWriter		elementHeadWriter_;
		std::stringstream	elementHeadBuffer_;

		// stream writer and working buffer for element payload
		StreamWriter		elementPayloadWriter_;
		std::stringstream	elementPayloadBuffer_;

		// ************
		// For parsing, stream reader
		StreamReader*		elementReader_;				// stream reader for parsing
	};

    /**
     * @brief IAB Frame class.
     *
     * An IAB frame contains all audio and metadata elements required to decode
     * one frame of audio.
     *
     */
    class IABFrame : public virtual IABElement, public IABFrameInterface
    {
    public:

        // Constructors
        IABFrame();											// Default contructor for client-constructed IAB frame (content creation)
        IABFrame(std::istream* inputStream);				// Contructor with input stream, for IAB frame constructed from parsing an inpuit IAB bitstream/frame

        // Destructor
        ~IABFrame();

		// Get bitstream version
        void GetVersion(IABVersionNumberType &oVersion) const;

        // Set bitstream version
        iabError SetVersion(IABVersionNumberType iVersion);

        // Get audio sampling rate
        void GetSampleRate(IABSampleRateType &oSampleRate) const;

        // Set audio sampling rate
        iabError SetSampleRate(IABSampleRateType iSampleRate);

        // Get audio bit depth
        void GetBitDepth(IABBitDepthType &oBitDepth) const;

        // Set audio bit depth
        iabError SetBitDepth(IABBitDepthType iBitDepth);

        // Get frame rate code
        void GetFrameRate(IABFrameRateType &oFrameRate) const;

        // Set frame rate code
        iabError SetFrameRate(IABFrameRateType iFrameRate);

        // Get the maximum number of audio assets to be rendered for the frame
        void GetMaxRendered(IABMaxRenderedRangeType &oMaxRendered) const;

        // Set the maximum number of audio assets to be rendered for the frame
        iabError SetMaxRendered(IABMaxRenderedRangeType iMaxRendered);

        // Get sub element count
        void GetSubElementCount(IABElementCountType &oCount) const;

        // Get sub element pointer list
        void GetSubElements(std::vector<IABElement*> &oSubElements) const;

        // Set sub element pointer list
        iabError SetSubElements(const std::vector<IABElement*> iSubElements);

		// Clear sub element pointer list without deletion.
		iabError ClearSubElements();

		// Check if an element is in sub element list
		bool IsSubElement(IABElement* iSubElement);

		// Delete a sub-element with iSubElementMetaID
		iabError RemoveSubElement(IABElement* iSubElement);

		// Add a sub element to existing list of sub-element
		iabError AddSubElement(IABElement* iSubElement);

		// Enable packing of all sub elements contained in this IABFrame element.
		void EnablePackingSubElements();

		// Disable packing of any of the sub elements contained in this IABFrame element.
		void DisablePackingSubElements();

		// Check if sub elements contained in this IABFrame element are enabled (include/used) in packing.
		bool AreSubElementsEnabledForPacking();

		// Serialize IAB frame element to output stream.
        // This is the only Serialize() that is exposed to users
		iabError Serialize(std::ostream &outStream);

        // Processing method DeSerilize() from a buffer: parsing a frame
        // This is the only DeSerialize() that is exposed to users
        iabError DeSerialize();

		// Set whether DeSerialization should continue for illegal
		// bitstream versions
		void SetDeSerializeFailsOnVersionError(bool failOnVersionError);

		// Get the failOnVersionError setting
		virtual bool GetDeSerializeFailsOnVersionError() const;

		// Get number of known but unallowed frame sub-elements parsed
		uint32_t GetNumUnallowedSubElements() const;

		// Get number of undefined/unknown frame sub-elements parsed
		uint32_t GetNumUndefinedSubElements() const;

		// Validate an IAB frame against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    private:

		// Processing method DeSerilize() from a reader
		// This overloading call is NOT used for frame. It is will be implemented as ... (TBD)
		iabError DeSerialize(StreamReader& streamReader) { return kIABNotImplementedError; }

		void DeleteSubElements();

        // IA bitstream version
        uint8_t	version_;

        // Sampling rate of audio data
        IABSampleRateType sampleRate_;			// 2-bit

        // Bit depth of audio Data
        IABBitDepthType bitDepth_;				// 2-bit

        // IAB frame rate code. Refer table 6 for codes.
        IABFrameRateType frameRate_;			// 4-bit

        // Maximum number of audio assets to be rendered for the frame
        IABMaxRenderedRangeType maxRendered_;	// plex(8) (with range set to uint32_t)

        // Number of sub-elements contained in the frame
        IABElementCountType subElementCount_;  	// plex(8) (with range set to uint32_t)

        // A vector of sub/child IABElements contained in the frame
        std::vector<IABElement*> frameSubElements_;
        
        // Parse fields in frame header: sample rate, bitdepth, frame rate etc...
        iabError ParseFrameDataFields();
        
		// Parse IABFrame sub-elements
		iabError ParseFrameSubElement();
        
        // *****************************************
        // Class internal variable
        
        // Number of bitstream frame sub-elements that have skipped by the parser
        IABElementCountType numSkippedFrameSubElementsInParsing_;

		// Number of times undefined/unknown sub element IDs are found in frame parsing
		uint32_t numUndefinedFrameSubElements_;

		// Number of times known-but-unallowed sub element IDs are found in frame parsing
		uint32_t numUnallowedFrameSubElements_;

		// *****************************************
		// Class internal variable only, supporting dynamic element packing

		// Number of packing-enabled sub-elements contained in the frame
		// This variable must be updated right before packing IABFrame (Serialize())
		// to ensure integrity/consistency of packed element
		// Not used for parsing, etc
		IABElementCountType packedSubElementCount_;  	// plex(8) (with range set to uint32_t)

		// Count number of packing-enabled elements in frameSubElements_ and update to packedSubElementCount_
		void UpdatePackingEnabledSubElementCount();

		// *****************************************
		// Packer/Parser variables below are for "container" level processing. Internal to libraries.

		// for supporting (packing or parsing) a subframe wrap to the content of this IABFrame
		IABIASubFrame containerSubframeWrapper_;

		// for adding/parsing an optional preamble which may or may not be in the input frame/bitstream
		IABPreamble aPreamble_;

		// when true (default) the DeSerialization of a frame will fail if an illegal
		// version is detected. When false the library will attempt to continue
		// DeSerialization of invalid bitstreams
		bool failOnVersionError_;
	};

    /**
     * @brief IABChannel class.
     *
     * An IABChannel defines a bed channel (used in IABBedDefinition). It contains audio reference
     * and metadata elements pertaining to the bed channel.
     *
     */
    class IABChannel : public IABChannelInterface
    {
    public:

        // Constructor
        IABChannel();

        // Destructor
        ~IABChannel() {}

        // Get channel ID
        void GetChannelID(IABChannelIDType &oChannelID) const;

        // Set channel ID
        iabError SetChannelID(IABChannelIDType iChannelID);

        // Get audio data ID
        void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const;

        // Set audio data ID
        iabError SetAudioDataID(IABAudioDataIDType iAudioDataID);

        // Get channel gain
        void GetChannelGain(IABGain &oChannelGain) const;

        // Set channel gain
        iabError SetChannelGain(IABGain const& iChannelGain);

        // Get channel decor info exists
        void GetDecorInfoExists(uint1_t &oDecorInfoExists) const;

        // Set channel decor info exists
        iabError SetDecorInfoExists(uint1_t iDecorInfoExists);

        // Get channel decor coef
        void GetDecorCoef(IABDecorCoeff &oDecorCoef) const;

        // Set channel decor coef
        iabError SetDecorCoef(IABDecorCoeff const& iDecorCoef);

		// Serialize an IAB Channel to buffer.
		iabError Serialize(StreamWriter& streamWriter);

		// DeSerialize an IAB Channel from buffer.
		iabError DeSerialize(StreamReader& streamReader);

        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

    private:

        // ID code that identifies the channel. Refer to table 8
        IABChannelIDType channelID_;		// plex(4) (with range set to uint16_t in enum)

        // Identifies associated mono audio asset carried in the bitstream. NULL(0) == no audio asset.
        IABAudioDataIDType audioDataID_;	// plex(8) (with range set to uint32_t)

        // Specifies channel gain in IABGain
        IABGain channelGain_;

        // Specifies if channel decor info exists
        uint1_t channelDecorInfoExists_;	// 1-bit

        // A conditional 4-bit reserved field, set to fixed 0x0 (conditional on channelDecorInfoExists_ == 1)
        uint4_t reserved_;					// 4-bit

        // Specifies the amount of decorrelation to be applied
        IABDecorCoeff channelDecorCoef_;
    };

    /**
     * @brief IAB Bed Definition class.
     *
     * An IAB BedDefinition contains metadata and pointers to audio essence to
     * generate one frame of one audio bed.
     *
     */
    class IABBedDefinition : public virtual IABElement, public IABBedDefinitionInterface
    {
    public:

        // Constructor
        IABBedDefinition(IABFrameRateType iFrameRate);

        // Destructor
        ~IABBedDefinition();

		// Get metadata ID
        void GetMetadataID(IABMetadataIDType &oMetaID) const;

        // Set metadata ID
        iabError SetMetadataID(IABMetadataIDType iMetaID);

        // Get conditional bed, 1-bit
        void GetConditionalBed(uint1_t &oConditionalBed) const;

        // Set conditional bed, 1-bit
        iabError SetConditionalBed(uint1_t iConditionalBed);

        // Get bed use case
        void GetBedUseCase(IABUseCaseType &oBedUseCase) const;

        // Set bed use case
        iabError SetBedUseCase(IABUseCaseType iBedUseCase);

        // Get channel count of bed
        void GetChannelCount(IABChannelCountType &oChannelCount) const;

        // Get bed channels (a list of pointers)
        void GetBedChannels(std::vector<IABChannel*> &oBedChannels) const;

        // Set bed channels (a list of pointers)
        iabError SetBedChannels(const std::vector<IABChannel*> iBedChannels);

        // Get audio description
        void GetAudioDescription(IABAudioDescription &oAudioDescription) const;

        // Set audio description
        iabError SetAudioDescription(IABAudioDescription const& iAudioDescription);

        // Get sub element count
        void GetSubElementCount(IABElementCountType &oCount) const;

        // Get sub element pointer list
        void GetSubElements(std::vector<IABElement*> &oSubElements) const;

        // Set sub element pointer list
        iabError SetSubElements(const std::vector<IABElement*> iSubElements);

		// Clear sub element pointer list without deletion.
		iabError ClearSubElements();

		// Check if an element is in sub element list
		bool IsSubElement(IABElement* iSubElement);

		// Delete a sub-element with iSubElementMetaID
		iabError RemoveSubElement(IABElement* iSubElement);

		// Add a sub element to existing list of sub-element
		iabError AddSubElement(IABElement* iSubElement);

		// Enable packing of this bed definition element
		void EnablePacking();

		// Disable packing of this bed definition element
		void DisablePacking();

		// Interrogate if the bed definition element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB bed definition element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB bed definition element from buffer.
		iabError DeSerialize(StreamReader& streamReader);

		// Get number of known but unallowed bed sub-elements parsed
		uint32_t GetNumUnallowedSubElements() const;

		// Get number of undefined/unknown bed sub-elements parsed
		uint32_t GetNumUndefinedSubElements() const;

		// Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    private:

		// Protected functions

		void DeleteSubElements();
		void DeleteBedChannels();
        
        // Unique ID for tracking metadata information between frames
        IABMetadataIDType metadataID_;		// plex(8) (with range set to uint32_t)

        // 1-bit for identifying if bed definition is conditional
        uint1_t	conditionalBed_;			// 1-bit (can be true/false)

        // Specifies bed use case code for a conditional bed. See table 1 for codes
        IABUseCaseType	bedUseCase_;        // 8-bit

        // Number of channels in the bed
        IABChannelCountType channelCount_;	// plex(4) (with range set to uint16_t)

        // Pointers to all of IABChannel contained in the bed definition element
        std::vector<IABChannel*> bedChannels_;

        // A reserved 10-bit field, set to fixed 0x180
        uint10_t reserved_;					// 10-bit

        // Audio description
        IABAudioDescription audioDescription_;

        // Number of sub-elements contained in the bed definition
        IABElementCountType subElementCount_;	// plex(8) (with range set to uint32_t)

        // Pointers to all of the sub/child IABElements contained in the bed definition element
        std::vector<IABElement*> bedSubElements_;

		// bitstream frame rate
		IABFrameRateType parentFrameRate_;			// Frame rate for which the bed is defined. Not a data field for bed definition.

        // *****************************************
        // Class internal variable
        
        // Number of bitstream BedDefinition sub-elements that have skipped by the parser
        IABElementCountType numSkippedBedSubElementsInParsing_;

		// Number of times undefined/unknown sub element IDs are found in BedDefinition parsing
		uint32_t numUndefinedBedSubElements_;

		// Number of times known-but-unallowed sub element IDs are found in BedDefinition parsing
		uint32_t numUnallowedBedSubElements_;

		// *****************************************
		// Class internal variable only, supporting dynamic element packing

		// Number of packing-enabled sub-elements contained in the frame
		// This variable must be updated right before packing IABFrame (Serialize())
		// to ensure integrity/consistency of packed element
		// Not used for parsing, etc
		IABElementCountType packedSubElementCount_;  	// plex(8) (with range set to uint32_t)

		// Count number of packing-enabled elements in bedSubElements_ and update to packedSubElementCount_
		void UpdatePackingEnabledSubElementCount();

        // Parse a bed channel from bitstream
        iabError ParseBedChannel(StreamReader& streamReader);

        // Parse bed sub-element from bitstream
        iabError ParseBedSubElement(StreamReader& streamReader);
    };

    /**
     * @brief IABBedRemapSubBlock class.
     *
	 * An IABBedRemapSubBlock defines a sub-block for an IABBedRemap element. The number
	 * of sub blocks are specified "fixed" per "frame rate" by specification (Table 12).
	 *
	 * An IABBedRemapSubBlock contains information that describes how to convert an audio bed from
     * the distribution configuration to a different playback configuration, for the sub block duration.
     *
     */
    class IABBedRemapSubBlock : public IABBedRemapSubBlockInterface
    {
    public:

		// Constructor
		IABBedRemapSubBlock(uint16_t numberDestinaion, uint16_t numberSource);

        // Destructor
		~IABBedRemapSubBlock();

        // Setters and Getters

		// Get remap info exist flag
		void GetRemapInfoExists(uint1_t &oRemapInfoExists) const;

		// Set remap info exist flag
		iabError SetRemapInfoExists(uint1_t iRemapInfoExists);

		// Get remap coefficient array
		void GetRemapCoeffArray(std::vector<IABRemapCoeff*> &oRemapCoeffArray) const;

		// Set remap coefficient array
		iabError SetRemapCoeffArray(const std::vector<IABRemapCoeff*> iRemapCoeffArray);

		// Check remap coefficient array consistency (eg. input-output dimension etc)
		iabError checkRemapCoeffArray();

		// Get remap source number
		void GetRemapSourceNumber(uint16_t &oSourceNumber) const;

		// Get remap destination number
		void GetRemapDestinationNumber(uint16_t &oDestinationNumber) const;

		// Serialize IAB bed remap sub block to buffer. This is called to serialize all remap sub-blocks except the first block in a bed remap.
		iabError Serialize(StreamWriter& streamWriter);

		// Specicial serialization for first block. This is called directly to serialize first remap sub-block of a bed remap element.
		iabError SerializeSubBlock(StreamWriter& streamWriter);

		// Deserialize IAB bed remap sub block from buffer. This is called to deserialize all remap sub-blocks except the first.
		iabError DeSerialize(StreamReader& streamReader);

		// This is called directly to deserialize first remap sub-block of a bed remap element.
		iabError DeSerializeSubBlock(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

    private:

        void deleteRemapCoeffArray();

		// Specifies if remap information exists for the sub block
		uint1_t remapInfoExists_;								// 1-bit

		// Specifies object gain
		std::vector<IABRemapCoeff*> destRemapCoeffs_;			// contains all remap coeffs if present

		// variables for internal checking
		uint16_t numberSources_;
		uint16_t numberDestinations_;
	};

    /**
     * @brief IABBedRemap class.
     *
     * An IABBedRemap contains information that describes how to convert an audio bed from
     * the distribution configuration to a different playback configuration.
     *
     */
    class IABBedRemap : public virtual IABElement, public IABBedRemapInterface
    {
    public:

		// Constructor
		// This constructor is expected to be used lib-internally during parsing, as both numberDestinaion 
		// and numberSource parameters are yet to be parsed in (unknown) at the time of instantiaion.
		// Constructed instance using this constructor is almost a skeleton and expected to be set up by callers.
		// State at instantiation time would fail validation if not updated.
		//
		IABBedRemap(IABFrameRateType iFrameRate);

		// Constructor, exposed to API
		IABBedRemap(uint16_t numberDestinaion, uint16_t numberSource, IABFrameRateType iFrameRate);

        // Destructor
		~IABBedRemap();

		// Setters and Getters (TBD)

		// Get metadata ID
		void GetMetadataID(IABMetadataIDType &oMetaID) const;

		// Set metadata ID
		iabError SetMetadataID(IABMetadataIDType iMetaID);

		// Get remap use case
		void GetRemapUseCase(IABUseCaseType &oRemapUseCase) const;

		// Set remap use case
		iabError SetRemapUseCase(IABUseCaseType iRemapUseCase);

		// Get remap source channel count
		void GetSourceChannels(IABChannelCountType &oSourceChannels) const;

		// Get remap destimation channel count
		void GetDestinationChannels(IABChannelCountType &oDestinationChannels) const;

		// Get number of remap sub-blocks
		void GetNumRemapSubBlocks(uint8_t &oNumRemapSubBlocks) const;

		// Get bed remap sub-blocks (a list of pointers)
		void GetRemapSubBlocks(std::vector<IABBedRemapSubBlock*> &oRemapSubBlocks) const;

		// Set bed remap sub-blocks (a list of pointers)
		iabError SetRemapSubBlocks(const std::vector<IABBedRemapSubBlock*> iRemapSubBlocks);

		// Enable packing of this bed remap element
		void EnablePacking();

		// Disable packing of this bed remap element
		void DisablePacking();

		// Interrogate if the bed remap element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB bed remap element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB bed remap element from buffer.
		iabError DeSerialize(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    private:

        // Unique ID for tracking metadata information between frames
        IABMetadataIDType metadataID_;		// plex(8) (with range set to uint32_t)

        // Specifies use case for which the element is activated
        IABUseCaseType remapUseCase_;		// 8-bit

        // Number of channels (input) in paren IABBedDefinition element
        IABChannelCountType sourceChannels_;   	// plex(4) (with range set to uint16_t)

        // Number of output channels in the remapped-to bed
        IABChannelCountType destinationChannels_;	// plex(4) (with range set to uint16_t)

        // Pointers to all of IABBedRemapSubBlock contained in the IABBedRemap element
        std::vector<IABBedRemapSubBlock*> bedRemapSubBlocks_;

		// Number of remap sub-blocks
		uint8_t numRemapSubBlocks_;					// Defined in spec. Not a bitstream field

		void DeleteRemapSubBlocks();
    };

    /**
     * @brief IABObjectSubBlock class.
     *
     * An IABObjectSubBlock defines a sub-block for an IABObjectDefinition element. The number
     * of sub blocks are specified "fixed" per "frame rate" by specification (Table 12).
     * It contains object rendering parameters such as position, extent, snap, zone control, etc.
     * for the pan sub block duration.
     *
     */
    class IABObjectSubBlock : public IABObjectSubBlockInterface
    {
    public:

        // Constructor
        IABObjectSubBlock();

        // Destructor
        ~IABObjectSubBlock();

        // Get pan info exists flag
        void GetPanInfoExists(uint1_t &oPanInfoExists) const;

        // Set pan info exists flag
        iabError SetPanInfoExists(uint1_t iPanInfoExists);

        // Get object gain
        void GetObjectGain(IABGain &oObjectGain) const;

        // Set object gain
        iabError SetObjectGain(IABGain const& iObjectGain);

        // Get object postion and convert/map to unit cube coordinates (using mapping in section 6.4)
        void GetObjectPositionToUnitCube(CartesianPosInUnitCube &oObjectPositionUC) const;

        // Set IAB object position from unit cube coordinates (using mapping in section 6.4)
        iabError SetObjectPositionFromUnitCube(CartesianPosInUnitCube const& iObjectPositionUC);

        // Get object snap
        void GetObjectSnap(IABObjectSnap &oObjectSnap) const;

        // Set object snap
        iabError SetObjectSnap(IABObjectSnap const& iObjectSnap);

        // Get object 9-zone gains
        void GetObjectZoneGains9(IABObjectZoneGain9 &oObjectZoneGains9) const;

        // Set object 9-zone gains
        iabError SetObjectZoneGains9(IABObjectZoneGain9 const& iObjectZoneGains9);

        // Get object spread
        void GetObjectSpread(IABObjectSpread &oObjectSpread) const;

        // Set object spread
        iabError SetObjectSpread(IABObjectSpread const& iObjectSpread);

        // Get object decor coef
        void GetDecorCoef(IABDecorCoeff &oDecorCoef) const;

        // Set object decor coef
        iabError SetDecorCoef(IABDecorCoeff const& iDecorCoef);

		// Serialize IAB object sub block to buffer. This is called to serialize all pan sub-blocks except the first block in a frame.
		iabError Serialize(StreamWriter& streamWriter);

		// Specicial serialization for first block. This is called directly to serialize first pan sub-block of a frame, for object definition.
		iabError SerializeSubBlock(StreamWriter& streamWriter);

		// DeSerialize IAB object sub block from buffer. This is called to deserialize all pan sub-blocks except the first block in a frame.
		iabError DeSerialize(StreamReader& streamReader);

		// This is called directly to deserialize first pan sub-block of a frame, for object definition.
		iabError DeSerializeSubBlock(StreamReader& streamReader);

        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    private:

        // Specifies if panning information exists for the sub block
        uint1_t panInfoExists_;				// 1-bit

        // Specifies object gain
        IABGain objectGain_;

        // A 3-bit reserved field, set to fixed 0b001
        static const uint3_t reserved1_ = 1;					// 3-bit

        // Specifies object position (objectPosX, objectPosY, objectPosZ)
		// Note that postions to serialize to/deserialize from, is a member of this variable
		CartesianPosInUnitCube objectPosition_;

        // Specifies presence of object snap and tolerance
        IABObjectSnap objectSnap_;

        // Specifies presence and value of 9-zone gains for the sub-block
        IABObjectZoneGain9 zoneGain_;

        // Specifies object spread mode and amount
        IABObjectSpread objectSpread_;

        // A 4-bit reserved field, set to fixed 0x0
        static const uint4_t reserved2_ = 0;					// 4-bit

        // Specifies the amount of decorrelation to be applied
        IABDecorCoeff objectDecorCoef_;
    };

    /**
     * @brief IAB Object Definition class.
     *
     * An IAB ObjectDefinition contains metadata and pointers to audio essence to generate
     * one frame of one audio object.
     *
     */
    class IABObjectDefinition : public virtual IABElement, public IABObjectDefinitionInterface
    {
    public:

        // Constructor
        IABObjectDefinition(IABFrameRateType iFrameRate);

        // Destructor
        ~IABObjectDefinition();

		// Get metadata ID
        void GetMetadataID(IABMetadataIDType &oMetaID) const;

        // Set metadata ID
        iabError SetMetadataID(IABMetadataIDType iMetaID);

        // Get audio data ID
        void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const;

        // Set audio data ID
        iabError SetAudioDataID(IABAudioDataIDType iAudioDataID);

        // Get conditional object, 1-bit
        void GetConditionalObject(uint1_t &oConditionalObject) const;

        // Set conditional object, 1-bit
        iabError SetConditionalObject(uint1_t iConditionalObject);

        // Get object use case
        void GetObjectUseCase(IABUseCaseType &oObjectUseCase) const;

        // Set object use case
        iabError SetObjectUseCase(IABUseCaseType iObjectUseCase);

        // Get number of panning sub-blocks
        void GetNumPanSubBlocks(uint8_t &oNumPanSubBlocks) const;
        
        // Get object pan sub-blocks (a list of pointers)
        void GetPanSubBlocks(std::vector<IABObjectSubBlock*> &oPanSubBlocks) const;

        // Set object pan sub-blocks (a list of pointers)
        iabError SetPanSubBlocks(const std::vector<IABObjectSubBlock*> iPanSubBlocks);

        // Get audio description
        void GetAudioDescription(IABAudioDescription &oAudioDescription) const;

        // Set audio description
        iabError SetAudioDescription(IABAudioDescription const& iAudioDescription);

        // Get sub element count
        void GetSubElementCount(IABElementCountType &oCount) const;

        // Get sub element pointer list
        void GetSubElements(std::vector<IABElement*> &oSubElements) const;

        // Set sub element pointer list
        iabError SetSubElements(const std::vector<IABElement*> iSubElements);

		// Clear sub element pointer list without deletion.
		iabError ClearSubElements();

		// Check if an element is in sub element list
		bool IsSubElement(IABElement* iSubElement);

		// Delete a sub-element with iSubElementMetaID
		iabError RemoveSubElement(IABElement* iSubElement);

		// Add a sub element to existing list of sub-element
		iabError AddSubElement(IABElement* iSubElement);

		// Enable packing of this object definition element
		void EnablePacking();

		// Disable packing of this object definition element
		void DisablePacking();

		// Interrogate if the object definition element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB object definition element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB object definition element from buffer.
		iabError DeSerialize(StreamReader& streamReader);

		// Get number of known but unallowed object sub-elements parsed
		uint32_t GetNumUnallowedSubElements() const;

		// Get number of undefined/unknown object sub-elements parsed
		uint32_t GetNumUndefinedSubElements() const;

		// Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    protected:

        void DeletePanSubBlocks();
        
        void DeleteSubElements();
        
        // Unique ID for tracking metadata information between frames
        IABMetadataIDType metadataID_;		// plex(8) (with range set to uint32_t)

        // Identifies associated mono audio asset carried in the bitstream. NULL(0) == no audio asset.
        IABAudioDataIDType audioDataID_;	// plex(8) (with range set to uint32_t)

        // 1-bit for identifying if object definition is conditional
        uint1_t	conditionalObject_;			// 1-bit (can be true/false)

        // A conditional 1-bit reserved field, set to fixed 0x1 (conditional on conditionalObject_ == 1)
        static const uint1_t reserved1_ = 1;					// 1-bit

        // Specifies object use case code for a conditional object. See table 1 for codes
        IABUseCaseType	objectUseCase_;     // 8-bit

        // A reserved 1-bit field, set to fixed 0x0
        static const uint1_t reserved2_ = 0;					// 1-bit

        // bitstream frame rate
		IABFrameRateType parentFrameRate_;			// Frame rate for which the object is defined. Not a data field for object definition.

        // Number of object sub-blocks in a frame
        uint8_t numPanSubBlocks_;					// Defined in spec. Not a bitstream field
        
        // Pointers to object pan sub blocks
        std::vector<IABObjectSubBlock*> objectPanSubBlocks_;

        // Audio description
        IABAudioDescription audioDescription_;

        // Number of sub-elements contained in the object definition
        IABElementCountType subElementCount_;	// plex(8) (with range set to uint32_t)

        // Pointers to all of the sub/child IABElements contained in the object definition element
        std::vector<IABElement*> objectSubElements_;

    private:
        
		// *****************************************
		// Class internal variable

		// Number of bitstream ObjectDefinition sub-elements that have skipped by the parser
		IABElementCountType numSkippedObjectSubElementsInParsing_;

		// Number of times undefined/unknown sub element IDs are found in ObjectDefinition parsing
		uint32_t numUndefinedObjectSubElements_;

		// Number of times known-but-unallowed sub element IDs are found in ObjectDefinition parsing
		uint32_t numUnallowedObjectSubElements_;

		// *****************************************
		// Class internal variable only, supporting dynamic element packing

		// Number of packing-enabled sub-elements contained in the frame
		// This variable must be updated right before packing IABFrame (Serialize())
		// to ensure integrity/consistency of packed element
		// Not used for parsing, etc
		IABElementCountType packedSubElementCount_;  	// plex(8) (with range set to uint32_t)

		// Count number of packing-enabled elements in objectSubElements_ and update to packedSubElementCount_
		void UpdatePackingEnabledSubElementCount();

        // Parse object sub-element from bitstream
        iabError ParseObjectSubElement(StreamReader& streamReader);
    };

	/**
	* @brief IABZone19SubBlock class.
	*
	* An IABZone19SubBlock defines a sub-block for an IABObjectZoneDefinition19 element. The number
	* of sub blocks are specified "fixed" per "frame rate" by specification (Table 12).
	* It contains alternative (enhanced) zone definition and gains for object rendering, for the sub block.
	*
	*/
	class IABZone19SubBlock : public IABZone19SubBlockInterface
	{
	public:

		// Constructor
		IABZone19SubBlock();

		// Destructor
		~IABZone19SubBlock() {}

		// Get object 19-zone gains
		void GetObjectZoneGains19(IABObjectZoneGain19 &oObjectZoneGains19) const;

		// Set object 19-zone gains
		iabError SetObjectZoneGains19(IABObjectZoneGain19 const& iObjectZoneGains19);

		// Serialize IAB zone19 sub block to buffer. This is called to serialize all zone19 sub-blocks except the first block in a frame.
		iabError Serialize(StreamWriter& streamWriter);

		// Specicial serialization for first block. This is called directly to serialize first zone19 sub-block of a frame, for IABObjectZoneDefinition19.
		iabError SerializeSubBlock(StreamWriter& streamWriter);

		// DeSerialize IAB zone19 sub block from buffer. This is called to deserialize all zone19 sub-blocks except the first block in a frame.
		iabError DeSerialize(StreamReader& streamReader);

		// This is called directly to deserialize first zone19 sub-block of a frame, for IABObjectZoneDefinition19.
		iabError DeSerializeSubBlock(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
	private:

		// Specifies presence and value of 19-zone gains for the sub-block
		IABObjectZoneGain19 zone19Gain_;
	};

	/**
     * @brief IABObjectZoneDefinition19 class.
     *
     * An IABObjectZoneDefinition19 contains zone control data associated with the zones listed in
     * table 17.
     *
     */
    class IABObjectZoneDefinition19 : public virtual IABElement, public IABObjectZoneDefinition19Interface
    {
    public:

        // Constructor
        IABObjectZoneDefinition19(IABFrameRateType iFrameRate);

        // Destructor
		~IABObjectZoneDefinition19();

		// Setters and Getters

		// Get number of zone19 sub-blocks
		void GetNumZone19SubBlocks(uint8_t &oNumZone19SubBlocks) const;

		// Get zone19 sub-blocks (a list of pointers)
		void GetZone19SubBlocks(std::vector<IABZone19SubBlock*> &oZone19SubBlocks) const;

		// Set zone19 sub-blocks (a list of pointers)
		iabError SetZone19SubBlocks(const std::vector<IABZone19SubBlock*> iZone19SubBlocks);

		// Enable packing of this zone19 element
		void EnablePacking();

		// Disable packing of this zone19 element
		void DisablePacking();

		// Interrogate if the zone19 element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB object definition element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB object zone 19 definition element from buffer.
		iabError DeSerialize(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;
        
    private:

		void DeleteZone19SubBlocks();

		// Number of zone19 sub-blocks in a frame
		uint8_t numZone19SubBlocks_;			// Defined in spec. Not a bitstream field

		// Pointers to Zone19 sub blocks
		std::vector<IABZone19SubBlock*> zone19SubBlocks_;
    };

	/**
	* @brief IABAuthoringToolInfo class.
	*
	* An IABAuthoringToolInfo contains the vendor and tool (including version) with which 
	* the Immersive Audio Frame (IABFrame) was created.
	*
	*/
	class IABAuthoringToolInfo : public virtual IABElement, public IABAuthoringToolInfoInterface
	{
	public:

		// Constructor
		IABAuthoringToolInfo();

		// Destructor
		~IABAuthoringToolInfo() {}

		// Setters and Getters

		// Get authoring Tool Info URI
		void GetAuthoringToolInfo(const char* &oAuthoringToolURI) const;

		// Set authoring Tool Info URI
		iabError SetAuthoringToolInfo(const char* iAuthoringToolURI);

		// Enable packing of this authoringtoolinfo element
		void EnablePacking();

		// Disable packing of this authoringtoolinfo element
		void DisablePacking();

		// Interrogate if the authoringtoolinfo element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB PCM data audio element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB PCM audio data element to buffer.
		iabError DeSerialize(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

	private:

		// String built from null-terminated text, or parsed from bitstream
		// strictly ASCII. Last character being NULL
		// Note the variable authoringToolURI_ specifically does NOT contain 
		// NULL character at the end, and a NULL/'\0'/0 must be inserted 
		// during writing/packing to bitstream. Similarly, the last '\0' from
		// parsing a bitstream must NOT be inserted to authoringToolURI_.
		std::string authoringToolURI_;
	};

	/**
	* @brief IABUserData class.
	*
	* An IABUserData contains the vendor and tool (including version) with which
	* the Immersive Audio Frame (IABFrame) was created.
	*
	*/
	class IABUserData : public virtual IABElement, public IABUserDataInterface
	{
	public:

		// Constructor
		IABUserData();

		// Destructor
		~IABUserData() {}

		// Setters and Getters

		// Get user ID for IAB user data element 
		void GetUserID(uint8_t (&oUserID)[16]) const;

		// Set user ID for IAB user data element
		iabError SetUserID(const uint8_t (&iUserID)[16]);

		// Get user data bytes for IAB user data element
		void GetUserDataBytes(std::vector<uint8_t> &oUserDataBytes) const;

		// Set user data bytes for IAB user data element
		iabError SetUserDataBytes(std::vector<uint8_t> iUserDataBytes);

		// Enable packing of this user data element
		void EnablePacking();

		// Disable packing of this user data element
		void DisablePacking();

		// Interrogate if the user data element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB PCM data audio element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB PCM audio data element to buffer.
		iabError DeSerialize(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

	private:

		// User ID, a SMPTE label, 16-byte/128-bit long.
		uint8_t userID_[16];

		// User data block (payload). Variable length.
		std::vector<uint8_t> userDataBytes_;
	};

	/**
     * @brief IABAudioDataDLC class.
     *
     * An IABAudioDataDLC contains losslessly-coded audio for one frame of one monaural audio waveform.
     *
     */
    class IABAudioDataDLC : public virtual IABElement, public IABAudioDataDLCInterface
    {
    public:

        // Constructor
		// ! Note DLC constructors specifically returns an error code, oConstructionReturnError.
		// If not kIABNoError, it indicates the instance is incorrectly set up, in business logic
		// sense. If error occurs, delete object immediately.
        IABAudioDataDLC(IABFrameRateType iFrameRateCode, iabError& oConstructionReturnError);
		IABAudioDataDLC(IABFrameRateType iFrameRateCode, IABSampleRateType iSampleRate, iabError& oConstructionReturnError);

        // Destructor
		~IABAudioDataDLC();

		// Get audio data ID
        void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const;

        // Set audio data ID
        iabError SetAudioDataID(IABAudioDataIDType iAudioDataID);

        // Get DLC data size (in byte)
        void GetDLCSize(uint16_t &oDLCSize) const;

        // Set DLC data size (in byte)
        iabError SetDLCSize(uint16_t iDLCSize);

		// Get DLC sample rate
		iabError GetDLCSampleRate(IABSampleRateType &oDLCSampleRate) const;

		// Set DLC sample rate
		iabError SetDLCSampleRate(IABSampleRateType iDLCSampleRate);

		// Encode a block of 24-bit PCM samples (as 3 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		iabError EncodeMonoPCMToDLC(const int32_t* iSamples, uint32_t iSampleCount);

		// Decode a block of 24-bit PCM samples (as 3 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		iabError DecodeDLCToMonoPCM(int32_t* oSamples, uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate);

		// Decode a block of 24-bit PCM samples (as 3 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		// and store decoded integer PCM sample in DLC element internal buffer
		iabError DecodeDLCToMonoPCMInternal(uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate);

		// Returns a pointer to internal integer buffer holding decoded PCM samples.
		int32_t* GetDecodedSampleBuffer();

		// Enable packing of this DLC element
		void EnablePacking();

		// Disable packing of this DLC element
		void DisablePacking();

		// Interrogate if the DLC element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB DLC audio data element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB DLC audio data element to buffer.
		iabError DeSerialize(StreamReader& streamReader);
        
        // Validate contents against SMPTE IAB Specification
        bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

    private:

		/**
		* Encoder instance used to encode PCM into dlc:AudioData
		* (The simple encoder supports PCM wrapping only)
		*
		*/
		dlc::SimpleEncoder dlcSimpleEncoder_;
        
        /**
         * Decoder instance used to decode dlc:AudioData into PCM
         * (The decoder supports PCM wrapping only)
         *
         */
        dlc::FullDecoder dlcFullDecoder_;

		/**
		* AudioData instance (DLC lib). Owned and managed by IABAudioDataDLC
		*
		*/
		dlc::AudioData audioData_;

		// Audio sample count of the DLC element (value per ST2098-2 spec)
		uint32_t sampleCount_;

		// Pointer to internal buffer that stores decoded PCM samples
		// from audioData_
		// Intialized to nullptr, allocated upon DecodeDLCToMonoPCM() call
		int32_t* decodedPCM_;

		// Identifies the instance of a mono audio essence
        IABAudioDataIDType audioDataID_;				// plex(8) (with range set to uint32_t)

        // Size in bytes of DLC data (payload)
        uint16_t DLCSize_;								// 16-bit

		// Identifies the sample rate of this IABAudioDataDLC instance
		// (Note: This could be different from Frame sample rate)
		dlc::SampleRate DLCSampleRate_;					// 2-bit

        // Number of prediction regions, for 48kHz
		uint2_t dlcNumPredRegions48_;					// 2-bit
        
        // Frame rate for bitstream
        IABFrameRateType frameRateCode_;

		// A vector of PredRegion is expected for 48k. DLC lib offers getter to
		// individual ones using index
		// Also the following list of sub-fields within a DLC prediction region is 
		// contained inside PredRegion instances (internal detail to DLC codec).
		//
		// RegionLength48
		// Order48
		// KCoeff48
		//

		// Number of DLC sub-blocks. This is field specified in spec, but not in bitstream
		uint2_t dlcNumDLCSubBlocks_;
        
        /// Number of samples per DLC subBlock
        uint32_t dlcBlockLength_;

		// A vector of ResidualSubBlock is expected for 48k. DLC lib offers getter to
		// individual ones using index
		// Also the following list of sub-fields within a DLC sub block is 
		// contained inside ResidualSubBlock instances (internal detail to DLC codec).
		//
		// CodeType
		// for PCM block
		//     BitDepth
		//     Residual48
		//     sign
		// for Rice/Golomb block
		//     RiceRemBits
		//     UnaryBit (1+)
		//     Residual48
		//     sign
		//

		// Number of prediction regions, if DLC element is 96kHz
		uint2_t dlcNumPredRegions96_;					// 2-bit

		// A vector of PredRegion is expected for 96k, if present. DLC lib offers getter to
		// individual ones using index
		// Also the following list of sub-fields within a DLC prediction region is 
		// contained inside PredRegion instances (internal detail to DLC codec).
		//
		// RegionLength96
		// Order96
		// KCoeff96
		//

		// A vector of ResidualSubBlock is expected for 96k, if present. DLC lib offers getter to
		// individual ones using index
		// Also the following list of sub-fields within a DLC sub block is 
		// contained inside ResidualSubBlock instances (internal detail to DLC codec).
		//
		// CodeType
		// for PCM block
		//     BitDepth
		//     Residual96
		//     sign
		// for Rice/Golomb block
		//     RiceRemBits
		//     UnaryBit (1+)
		//     Residual96
		//     sign
		//

        // Setup DLC subblocks parameters
        iabError SetupDLCSubblock();
   };

    /**
     * @brief IABAudioDataPCM class.
     *
     * An IABAudioDataPCM contains PCM audio for one frame of one monaural audio waveform.
     *
     */
	class IABAudioDataPCM : public virtual IABElement, public IABAudioDataPCMInterface
	{
	public:

		// Constructor
		IABAudioDataPCM(IABFrameRateType iFrameRateCode
			, IABSampleRateType iSampleRate
			, IABBitDepthType iBitDepth);

		// Destructor
		~IABAudioDataPCM();

		// Setters and Getters

		// Get audio data ID
		void GetAudioDataID(IABAudioDataIDType &oAudioDataID) const;

		// Set audio data ID
		iabError SetAudioDataID(IABAudioDataIDType iAudioDataID);

		// Get the frame rate for which the PCM element is used
		IABFrameRateType GetPCMFrameRate() const;

		// Get PCM sample rate
		IABSampleRateType GetPCMSampleRate() const;

		// Get PCM bit depth
		IABBitDepthType GetPCMBitDepth() const;

		// Get sample count
		uint32_t GetPCMSampleCount() const;

		// Pack a block of 24-/16-bit PCM samples (as 3/2 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		iabError PackMonoSamplesToPCM(const int32_t* iSamples, uint32_t iSampleCount);

		// Unpack a block of 24-/16-bit PCM samples (as 3/2 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		iabError UnpackPCMToMonoSamples(int32_t* oSamples, uint32_t iSampleCount);

		// Unpack a block of 24-/16-bit PCM samples (as 3 MSBs of 32-bit int32_t), of "iSampleCount" # of samples
		// and store unpacked integer PCM samples in PCM element internal buffer
		iabError UnpackPCMToMonoSamplesInternal(uint32_t iSampleCount);

		// Returns a pointer to internal integer buffer holding unpacked PCM samples.
		int32_t* GetUnpackedSampleBuffer();

		// Enable packing of this PCM element
		void EnablePacking();

		// Disable packing of this PCM element
		void DisablePacking();

		// Interrogate if the PCM element is included for packing
		bool IsIncludedForPacking();

		// Serialize IAB PCM data audio element to buffer.
		iabError Serialize(std::ostream &outStream);

		// DeSerialize IAB PCM audio data element to buffer.
		iabError DeSerialize(StreamReader& streamReader);

		// Validate contents against SMPTE IAB Specification
		bool Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const;

	private:

		// Identifies the instance of a PCM mono audio essence
		IABAudioDataIDType audioDataID_;				// plex(8) (with range set to uint32_t)

        // Frame rate for bitstream
        IABFrameRateType frameRateCode_;

		// Sample rate
		IABSampleRateType sampleRateCode_;

		// Bit depth
		IABBitDepthType bitDepthCode_;

		// Sample count of PCM element (value per ST2098-2 spec)
		uint32_t sampleCount_;

		// Number of bytes per sample
		uint8_t numBytePerSample_;

		// Buffer size in byte for PCM element
		uint32_t totalByteCount_;

		// Size in bytes of PCM data (payload)
		uint8_t* pcmBytes_;								// 16-bit or 24-bit PCM samples in bytes, little endian

		// Pointer to internal buffer that stores unpacked PCM samples
		// Intialized to nullptr, allocated upon UnpackPCMToMonoSamplesInternal() call
		int32_t* unpackedPCM_;
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABELEMENTS_H__
