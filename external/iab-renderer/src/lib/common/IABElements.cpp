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

#include <set>

/* Implementation of IABElements class functions functions */

#include "common/IABElements.h"
#include "IABDataTypes.h"
#include "IABUtilities.h"
#include "commonstream/bitstream/BitStreamIOHelpers.h"
#include "commonstream/utils/opbacksbuf.h"

// To support validation and event handler
#include "IABValidatorAPI.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{

	// ****************************************************************************
	// Factory methods for creating instances of IAB elements or
    // sub-structures (components of certain elements).
	// ****************************************************************************

	// Create IABFrame instance
    IABFrameInterface* IABFrameInterface::Create(std::istream* inputStream)
    {
		IABFrame* iabFrame = nullptr;
		
		if (inputStream)
		{
			iabFrame = new IABFrame(inputStream);
		}
		else
		{
			iabFrame = new IABFrame();
		}

        return iabFrame;
    }

    void IABFrameInterface::Delete(IABFrameInterface* iInstance)
    {
        delete iInstance;
    }

    // Create IABChannel instance
    IABChannelInterface* IABChannelInterface::Create()
    {
        IABChannel* iabChannel = new IABChannel();
        return iabChannel;
    }

    void IABChannelInterface::Delete(IABChannelInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABBedDefinition instance
    IABBedDefinitionInterface* IABBedDefinitionInterface::Create(IABFrameRateType iFrameRate)
    {
        IABBedDefinition* iabBedDefinition = new IABBedDefinition(iFrameRate);
        return iabBedDefinition;
    }

    void IABBedDefinitionInterface::Delete(IABBedDefinitionInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABBedRemapSubBlock instance
    IABBedRemapSubBlockInterface* IABBedRemapSubBlockInterface::Create(uint16_t numberDestinaion, uint16_t numberSource)
    {
        IABBedRemapSubBlock* iabBedRemapSubBlock = new IABBedRemapSubBlock(numberDestinaion, numberSource);
        return iabBedRemapSubBlock;
    }

    void IABBedRemapSubBlockInterface::Delete(IABBedRemapSubBlockInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABBedRemap instance
    IABBedRemapInterface* IABBedRemapInterface::Create(uint16_t numberDestinaion, uint16_t numberSource, IABFrameRateType iFrameRate)
    {
        IABBedRemap* iabBedRemap = new IABBedRemap(numberDestinaion, numberSource, iFrameRate);
        return iabBedRemap;
    }

    void IABBedRemapInterface::Delete(IABBedRemapInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABObjectSubBlock instance
    IABObjectSubBlockInterface* IABObjectSubBlockInterface::Create()
    {
        IABObjectSubBlock* iabObjectSubBlock = new IABObjectSubBlock();
        return iabObjectSubBlock;
    }

    void IABObjectSubBlockInterface::Delete(IABObjectSubBlockInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABObjectDefinition instance
    IABObjectDefinitionInterface* IABObjectDefinitionInterface::Create(IABFrameRateType iFrameRate)
    {
        IABObjectDefinition* iabObjectDefinition = new IABObjectDefinition(iFrameRate);
        return iabObjectDefinition;
    }

    void IABObjectDefinitionInterface::Delete(IABObjectDefinitionInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABZone19SubBlock instance
	IABZone19SubBlockInterface* IABZone19SubBlockInterface::Create()
	{
		IABZone19SubBlock* iabZone19SubBlock = new IABZone19SubBlock();
		return iabZone19SubBlock;
	}

    void IABZone19SubBlockInterface::Delete(IABZone19SubBlockInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABObjectZoneDefinition19 instance
    IABObjectZoneDefinition19Interface* IABObjectZoneDefinition19Interface::Create(IABFrameRateType iFrameRate)
    {
        IABObjectZoneDefinition19* iabObjectZoneDefinition19 = new IABObjectZoneDefinition19(iFrameRate);
        return iabObjectZoneDefinition19;
    }

    void IABObjectZoneDefinition19Interface::Delete(IABObjectZoneDefinition19Interface* iInstance)
    {
        delete iInstance;
    }

	// Create IABAuthoringToolInfo instance
	IABAuthoringToolInfoInterface* IABAuthoringToolInfoInterface::Create()
	{
		IABAuthoringToolInfo* iabAuthoringToolInfo = new IABAuthoringToolInfo();
		return iabAuthoringToolInfo;
	}

    void IABAuthoringToolInfoInterface::Delete(IABAuthoringToolInfoInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABUserData instance
	IABUserDataInterface* IABUserDataInterface::Create()
	{
		IABUserData* iabUserData = new IABUserData();
		return iabUserData;
	}

    void IABUserDataInterface::Delete(IABUserDataInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABAudioDataDLC instance
    IABAudioDataDLCInterface* IABAudioDataDLCInterface::Create(IABFrameRateType iFrameRateCode, IABSampleRateType iSampleRate)
    {
		iabError iabReturnCode = kIABNoError;

        IABAudioDataDLC* iabAudioDataDLC = new IABAudioDataDLC(iFrameRateCode, iSampleRate, iabReturnCode);

		if (iabReturnCode == kIABNoError)
		{
			return iabAudioDataDLC;
		}
		else
		{
			delete iabAudioDataDLC;
			return nullptr;
		}
    }

    void IABAudioDataDLCInterface::Delete(IABAudioDataDLCInterface* iInstance)
    {
        delete iInstance;
    }

	// Create IABAudioDataPCM instance
	IABAudioDataPCMInterface* IABAudioDataPCMInterface::Create(IABFrameRateType iFrameRateCode
		, IABSampleRateType iSampleRate
		, IABBitDepthType iBitDepth)
	{
		IABAudioDataPCM* iabAudioDataPCM = new IABAudioDataPCM(iFrameRateCode, iSampleRate, iBitDepth);
		return iabAudioDataPCM;
	}

    void IABAudioDataPCMInterface::Delete(IABAudioDataPCMInterface* iInstance)
    {
        delete iInstance;
    }

	// ****************************************************************************
	// IABContainerSubFrame class implementation
	// ****************************************************************************

	// Constructor implementation
	IABContainerSubFrame::IABContainerSubFrame(IABContainerSubFrameTagType iSubFrameTag) :
		subframeTag_(iSubFrameTag),
		subframeLength_(0),
		subframePayload_(nullptr),
		containerHeadWriter_(containerHeadBuffer_),
		containerHeadBuffer_(std::stringstream::in | std::stringstream::out | std::stringstream::binary)
	{
	}

	// Destructor implementation
	IABContainerSubFrame::~IABContainerSubFrame()
	{
		if (subframePayload_)
		{
			delete[] subframePayload_;
		}
	}

	// IABContainerSubFrame::GetSubframeLength() implementation
	void IABContainerSubFrame::GetSubframeLength(IABSubframeLengthType &oSubframeLength)
	{
		oSubframeLength = subframeLength_;
	}

	// IABContainerSubFrame::SetSubframeLength() implementation
	iabError IABContainerSubFrame::SetSubframeLength(IABSubframeLengthType iSubframeLength)
	{
		subframeLength_ = iSubframeLength;
		return kIABNoError;
	}

	// ****************************************************************************
	// IABIASubFrame class implementation
	// ****************************************************************************

	// Constructor implementation
	IABIASubFrame::IABIASubFrame() :
		IABContainerSubFrame(kIABContainerSubFrameTag_IAFrame)
	{
	}

	// IABIASubFrame::Serialize() implementation
	iabError IABIASubFrame::Serialize(std::ostream &outStream)
	{
		// Set up writer and buffer
		containerHeadWriter_.align();										// reset writer byte alignment
		containerHeadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		containerHeadBuffer_.clear();

		// Writing tag and length
		containerHeadWriter_.write(static_cast<uint8_t>(subframeTag_), 8);						// Pack IAFrame tag
		containerHeadWriter_.write(static_cast<uint32_t>(subframeLength_), 32);					// Pack IAFrame length (assuming length has been properly updated!)

		// Not packing IAFrame payload (Value), as this is used by IABFrame as a wrapper only.
		// 
		// Output ...
		outStream << containerHeadBuffer_.rdbuf();

		// Clear stream contents after output
		containerHeadBuffer_.str("");

		return kIABNoError;
	}

	// IABIASubFrame::DeSerialize() implementation
	iabError IABIASubFrame::DeSerialize(StreamReader& streamReader)
	{
		uint8_t parsedTag = 0;
        CommonStream::ReturnCode rCode = CMNSTRM_OK;

		// Expect input stream position to be at beginning of IAB subframe.
		// If not, parsed tag will be incorrect, and error will be reported.
        
        // Read tag
        rCode = streamReader.read(parsedTag);
        
        if (CMNSTRM_OK == rCode)
        {
            // If it is a IA subframe, read length
            // (subframeTag_ has the correct tag enum value on construction, and never changes.)
            if (static_cast<uint8_t>(subframeTag_) == parsedTag)
            {
                // Parse subframe length, 32-bit uint
                rCode = streamReader.read(subframeLength_);
                
                if (CMNSTRM_OK == rCode)
                {
                    // Expect tag length to be > 0
                    if (subframeLength_ > 0)
                    {
                        return kIABNoError;
                    }
                    
                    return kIABParserIASubFrameHeaderError;
                }
            }
            else
            {
                // Not a IAB subframe, report error
                subframeLength_ = 0;
                return kIABParserIASubFrameHeaderError;
            }
        }
        
        if (CMNSTRM_OK != rCode)
        {
            // NOTE:
            // CMNSTRM_IO_EOF or CMNSTRM_IO_FAIL can be reported from
            // StreamReader library indicating EoF condition. Hence the
            // following conditional check
            if ((CMNSTRM_IO_EOF == rCode) || (CMNSTRM_IO_FAIL == rCode))
            {
                return kIABParserEndOfStreamReached;
            }
            else
            {
                return kIABSystemIOError;
            }            
        }
        else
        {
            return kIABNoError;
        }
	}

	// ****************************************************************************
	// IABPreamble class implementation
	// ****************************************************************************

	// Constructor implementation
	IABPreamble::IABPreamble() :
		IABContainerSubFrame(kIABContainerSubFrameTag_Preamble)
	{
	}

	// IABPreamble::GetPreamblePayload() implementation
	void IABPreamble::GetPreamblePayload(uint8_t* &oPreamblePayload, IABSubframeLengthType &oLength) const
	{
		oPreamblePayload = subframePayload_;
		oLength = subframeLength_;
	}

	// IABPreamble::SetPreamblePayload() implementation
	iabError IABPreamble::SetPreamblePayload(uint8_t* iPreamblePayload, IABSubframeLengthType iLength)
	{
		if (subframePayload_)
		{
			delete[] subframePayload_;							// Delete any existent payload
		}

		subframePayload_ = iPreamblePayload;
		subframeLength_ = iLength;
		return kIABNoError;
	}

	// IABPreamble::Serialize() implementation
	iabError IABPreamble::Serialize(std::ostream &outStream)
	{
		// Set up writer and buffer
		containerHeadWriter_.align();										// reset writer byte alignment
		containerHeadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		containerHeadBuffer_.clear();

		// Writing tag and length
		containerHeadWriter_.write(static_cast<uint8_t>(subframeTag_), 8);						// Pack IAFrame tag
		containerHeadWriter_.write(static_cast<uint32_t>(subframeLength_), 32);					// Pack IAFrame length (assuming length has been properly updated!)

																								// pack payload if the following are satisfied
		if ((subframePayload_ != nullptr) && (subframeLength_ != 0))
		{
			containerHeadWriter_.write(static_cast<const uint8_t*>(subframePayload_), subframeLength_);
		}
		else if ((subframePayload_ == nullptr) && (subframeLength_ != 0))	// report error for this combination
		{
			// Clear stream contents after output
			containerHeadBuffer_.str("");

			return kIABGeneralError;
		}
		else;

		// Output ...
		outStream << containerHeadBuffer_.rdbuf();

		// Clear stream contents after output
		containerHeadBuffer_.str("");

		return kIABNoError;
	}

	// IABPreamble::DeSerialize() implementation
	iabError IABPreamble::DeSerialize(StreamReader& streamReader)
	{
        uint8_t parsedTag = 0;
        CommonStream::ReturnCode rCode = CMNSTRM_OK;
        
		// Expect input stream position to be at beginning of preamble subframe.
		// If not, parsed tag will be incorrect, and error will be reported.
	
		// Peek tag
        rCode = streamReader.peek(parsedTag);
        
        if (CMNSTRM_OK == rCode)
        {
            // If it is a preamble, read length and value
            // (subframeTag_ has the correct tag enum value on construction, and never changes.)
            if (static_cast<uint8_t>(subframeTag_) == parsedTag)
            {
                // Dummy read, so to move streamPos past the tag
                streamReader.read(parsedTag);
                
                // Parse subframe length, 32-bit uint
                rCode = streamReader.read(subframeLength_);
                
                if (CMNSTRM_OK == rCode)
                {
                    if (subframeLength_ > 0)
                    {
						if (subframePayload_)
						{
							delete[] subframePayload_;							// Delete any existent payload
						}

						subframePayload_ = new uint8_t[subframeLength_];
                        memset(subframePayload_, 0, subframeLength_);
                        
                        rCode = streamReader.readAligned(subframePayload_, subframeLength_);
                        if (CMNSTRM_OK == rCode)
                        {
                            // No error
                            return kIABNoError;
                        }
                        
                        // Not OK, decode condition at end of method
                    }
                }
            }
            else
            {
                // Preamble not present
                subframeLength_ = 0;
				if (subframePayload_)
				{
					delete[] subframePayload_;							// Delete any existent payload
				}
				subframePayload_ = nullptr;
                return kIABParserMissingPreambleError;
            }
        }

        if (CMNSTRM_OK != rCode)
        {
            // NOTE:
            // CMNSTRM_IO_EOF or CMNSTRM_IO_FAIL can be reported from
            // StreamReader library indicating EoF condition. Hence the
            // following conditional check
            if ((CMNSTRM_IO_EOF == rCode) || (CMNSTRM_IO_FAIL == rCode))
            {
                return kIABParserEndOfStreamReached;
            }
            else
            {
                return kIABSystemIOError;
            }          
        }
        else
        {
            return kIABNoError;
        }
	}

	// ****************************************************************************
	// IABElement class implementation
	// ****************************************************************************

	// Constructor implementation
	IABElement::IABElement(IABElementIDType iElementID) :
		elementID_(iElementID),
		elementSize_(0),
		packingEnable_(1),
		elementHeadWriter_(elementHeadBuffer_),
		elementHeadBuffer_(std::stringstream::in | std::stringstream::out | std::stringstream::binary),
		elementPayloadWriter_(elementPayloadBuffer_),
		elementPayloadBuffer_(std::stringstream::in | std::stringstream::out | std::stringstream::binary),
		elementReader_(nullptr)
	{
	}

	// Destructor implementation
	IABElement::~IABElement()
	{
		delete elementReader_;
		elementReader_ = nullptr;
	}

	// IABElement::GetElementID() implementation
	void IABElement::GetElementID(IABElementIDType &oElementID) const
    {
        oElementID = elementID_;
	}

	// IABElement::GetElementSize() implementation
	void IABElement::GetElementSize(IABElementSizeType &oElementSize) const
    {
        oElementSize = elementSize_;
	}

	// IABElement::SetElementSize() implementation
	iabError IABElement::SetElementSize(IABElementSizeType iElementSize)
    {
        elementSize_ = iElementSize;
        return kIABNoError;
	}

	// IABElement::EnablePacking() implementation
	void IABElement::EnablePacking()
	{
		packingEnable_ = 1;
	}

	// IABElement::DisablePacking() implementation
	void IABElement::DisablePacking()
	{
		packingEnable_ = 0;
	}

	// IABElement::IsIncludedForPacking() implementation
	bool IABElement::IsIncludedForPacking()
	{
		return (packingEnable_ != 0);
	}

	// IABElement::SerializeHead() implementation
	iabError IABElement::SerializeHead()
	{
		// Calling method for writing Plex encoded fields.
		write(elementHeadWriter_, Plex<8>(elementID_));
		write(elementHeadWriter_, Plex<8>(static_cast<uint32_t>(elementSize_)));

		return kIABNoError;
	}

	// IABElement::DeSerializeHead() implementation
	iabError IABElement::DeSerializeHead(StreamReader& streamReader)
	{
		// Element ID code is a Plex(8) coded data field
        Plex<8> parsedElementID;
        
        if (CMNSTRM_OK != read(streamReader, parsedElementID))
        {
            return kIABParserIABElementHeaderError;
        }

		// Is the parsed element ID as expected?
		if (parsedElementID != static_cast<uint32_t>(elementID_))
		{
			return kIABParserIABElementHeaderError;
		}

		// Element size is a Plex(8) coded data field
        Plex<8> parsedElementSize;
        
        if (CMNSTRM_OK != read(streamReader, parsedElementSize))
        {
            return kIABParserIABElementHeaderError;
        }

		if (parsedElementSize == 0)
		{
			return kIABParserIABElementHeaderError;
		}

        elementSize_ = static_cast<IABElementSizeType>(parsedElementSize);
        
		return kIABNoError;
	}
    
    // IABElement::SkipDeSerializeCurrentElement() implementation
    iabError IABElement::SkipDeSerializeCurrentElement(StreamReader& streamReader)
    {
        // Read Element ID and element size, then skip to new bitstream position according
        // to number of bytes given by element size
        
        // Element ID code is a Plex(8) coded data field
        Plex<8> parsedElementID;
        
        if (CMNSTRM_OK != read(streamReader, parsedElementID))
        {
            return kIABParserIABElementHeaderError;
        }
        
        // Element size is a Plex(8) coded data field
        Plex<8> parsedElementSize;
        
        if (CMNSTRM_OK != read(streamReader, parsedElementSize))
        {
            return kIABParserIABElementHeaderError;
        }
        
        if (parsedElementSize == 0)
        {
            // no further bytes to skip, if there is a bitstream error due to element size = 0
            // it will be detected and reported when parsing the next element.
            return kIABNoError;
        }
        
        // Skip the element body
        std::streampos currentPosition = streamReader.streamPosition();
        streamReader.streamPosition(currentPosition + static_cast<std::streampos>(parsedElementSize));

        return kIABNoError;
    }


	// IABElement::PeekNextElementID() implementation
	iabError IABElement::PeekNextElementID(StreamReader& streamReader, uint32_t &oElementID)
	{
        // Create a restorer to restore streamReader to the state before the peeking.
        // BitStreamStateRestorerT destructor will automatically restore streamReader state when exiting this method
        // Note that BitStreamStateRestorerT stream type must match streamReader stream type, see typedef at top of IABElements.h
        // typedef BitStreamReaderT<std::istream> StreamReader;

        BitStreamStateRestorerT<std::istream> streamReaderStateRestorer(&streamReader);

		Plex<8> nextElementID;

		if (CMNSTRM_OK != read(streamReader, nextElementID))
		{
			return kIABParserIABFrameError;
		}

		oElementID = static_cast<uint32_t>(nextElementID);

		return kIABNoError;
	}

	// ****************************************************************************
	// IABFrame class implementation
	// ****************************************************************************

	// Constructors implementation
	IABFrame::IABFrame() : 
		IABElement(kIABElementID_IAFrame)
    {
        version_ = kIABDefaultFrameVersion;
        sampleRate_ =  kIABSampleRate_48000Hz;
        bitDepth_ = kIABBitDepth_24Bit;
        frameRate_ = kIABFrameRate_24FPS;
        maxRendered_ = 0;
        subElementCount_ = 0;
        numSkippedFrameSubElementsInParsing_ = 0;
		numUndefinedFrameSubElements_ = 0;
		numUnallowedFrameSubElements_ = 0;

		packedSubElementCount_ = 0;				// To match number of element in frameSubElements_ that is packing enabled
		failOnVersionError_ = true;
	}

	IABFrame::IABFrame(std::istream* inputStream) :
		IABElement(kIABElementID_IAFrame)
	{
		version_ = kIABDefaultFrameVersion;
		sampleRate_ = kIABSampleRate_48000Hz;
		bitDepth_ = kIABBitDepth_24Bit;
		frameRate_ = kIABFrameRate_24FPS;
		maxRendered_ = 0;
		subElementCount_ = 0;
        numSkippedFrameSubElementsInParsing_ = 0;
		numUndefinedFrameSubElements_ = 0;
		numUnallowedFrameSubElements_ = 0;

		packedSubElementCount_ = 0;
		failOnVersionError_ = true;

		// Instantiate elementReader_ on inputStream
		if (inputStream && inputStream->good())
		{
			elementReader_ = new StreamReader(*inputStream);
		}
		else
		{
			// Error to console for now.
			// May move this to outside constrcutor as an initilization step for error reporting.
			std::cerr << "!Error: Unable to create StreamReader due to errors in inputStream." << std::endl;
		}
	}

    // Destructor
	IABFrame::~IABFrame()
    {
		DeleteSubElements();
    }

	// IABFrame::GetVersion() implementation
    void IABFrame::GetVersion(IABVersionNumberType &oVersion) const
    {
        oVersion = version_;
    }

	// IABFrame::SetVersion() implementation
    iabError IABFrame::SetVersion(IABVersionNumberType iVersion)
    {
        version_ = iVersion;
        return kIABNoError;
    }

	// IABFrame::GetSampleRate() implementation
    void IABFrame::GetSampleRate(IABSampleRateType &oSampleRate) const
    {
        oSampleRate = sampleRate_;
    }

	// IABFrame::SetSampleRate() implementation
    iabError IABFrame::SetSampleRate(IABSampleRateType iSampleRate)
    {
        sampleRate_ = iSampleRate;
        return kIABNoError;
    }

	// IABFrame::GetBitDepth() implementation
    void IABFrame::GetBitDepth(IABBitDepthType &oBitDepth) const
    {
        oBitDepth = bitDepth_;
    }

	// IABFrame::SetBitDepth() implementation
    iabError IABFrame::SetBitDepth(IABBitDepthType iBitDepth)
    {
        bitDepth_ = iBitDepth;
        return kIABNoError;
    }

	// IABFrame::GetFrameRate() implementation
    void IABFrame::GetFrameRate(IABFrameRateType &oFrameRate) const
    {
        oFrameRate = frameRate_;
    }

	// IABFrame::SetFrameRate() implementation
    iabError IABFrame::SetFrameRate(IABFrameRateType iFrameRate)
    {
        frameRate_ = iFrameRate;
        return kIABNoError;
    }

	// IABFrame::GetMaxRendered() implementation
    void IABFrame::GetMaxRendered(IABMaxRenderedRangeType &oMaxRendered) const
    {
        oMaxRendered = maxRendered_;
    }

	// IABFrame::SetMaxRendered() implementation
    iabError IABFrame::SetMaxRendered(IABMaxRenderedRangeType iMaxRendered)
    {
        maxRendered_ = iMaxRendered;
        return kIABNoError;
    }

	// IABFrame::GetSubElementCount() implementation
    void IABFrame::GetSubElementCount(IABElementCountType &oCount) const
    {
        oCount = subElementCount_;
    }

	// IABFrame::GetSubElements() implementation
    void IABFrame::GetSubElements(std::vector<IABElement*> &oSubElements) const
    {
        oSubElements = frameSubElements_;
    }

	// IABFrame::SetSubElements() implementation
    // TODO: Need to revise the submement deletion inside this method.
    // MAC xcode lets user to use the deleted pointer. That may endup in crash or in undefined behaviour at userspace.
    // Setting NULL to delete subment here also will not have any effect on userspace.
    iabError IABFrame::SetSubElements(const std::vector<IABElement*> iSubElements)
    {
        IABElementCountType count = static_cast<IABElementCountType>(iSubElements.size());
        
        IABElementIDType elementID;
        
        // Checking if any of the pointers is a nullptr or not a valid sub-element type
        for (uint32_t i = 0; i < count; i++)
        {
			if (nullptr == iSubElements[i])
			{
				return kIABBadArgumentsError;
			}

			iSubElements[i]->GetElementID(elementID);
            
            if ((kIABElementID_BedDefinition != elementID) &&
                 (kIABElementID_ObjectDefinition != elementID) &&
                 (kIABElementID_AuthoringToolInfo != elementID) &&
                 (kIABElementID_UserData != elementID) &&
                 (kIABElementID_AudioDataDLC != elementID) &&
                 (kIABElementID_AudioDataPCM != elementID))
            {
                return kIABBadArgumentsError;
            }
        }
        
        // Report error if duplicate sub-elements are in the list.
        std::set<IABElement*> tempSet; //Using Set datastructure, Existence check can be done easily. i.e set doenst allow the dublicates.
        for(std::vector<IABElement*>::const_iterator iter = iSubElements.begin(); iter != iSubElements.end(); ++iter)
        {
            // finds the duplicate
            if (!tempSet.insert(*iter).second)
                return kIABDuplicateSubElementsError;
        }
        
        // Update existing sub-elements list carefully.
        for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
        {
            bool found = false;
            for (std::vector<IABElement*>::const_iterator sub = iSubElements.begin(); sub != iSubElements.end(); sub++)
            {
                if (*iter == *sub)
                {
                    found = true;
                    break;
                }
            }
            // if sub-elmenent not found in the current list, just delete the sub-element.
            if (found == false)
            {
                delete *iter;
            }
        }
        
        // Now set the new list as current list.
        frameSubElements_ = iSubElements;
        subElementCount_ = count;
        
        return kIABNoError;

    }

    // IABFrame::Validate() implementation
    bool IABFrame::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
        // Developer note:
        // iValidationIssue fields are set every time an issue is being reported and can often create unnecessary assign (due to no changes)
        // but it definitely ensures that 1) they are not ever out of sync, & 2) much easier for future development and maintenance.
        // This pattern should be followed for when reporting every issue.
        
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################
        
		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// Cinema validation starts below at IAFrame level
		iValidationIssue.id_ = kIssueID_IAFrame;

		// bitstream version
		if (version_ != ConstraintSet_ST2098_2_2018.validVersion_)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameIllegalBitstreamVersion;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// General count var
		size_t foundCount = 0;
		
		// Sample rate
		foundCount = ConstraintSet_ST2098_2_2018.validSampleRates_.count(sampleRate_);

		// If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedSampleRate;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}
        
		// Bit depth
		foundCount = ConstraintSet_ST2098_2_2018.validBitDepth_.count(bitDepth_);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedBitDepth;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Frame rate
		foundCount = ConstraintSet_ST2098_2_2018.validFrameRates_.count(frameRate_);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedFrameRate;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// MaxRendered limits - check if limits are exceeded, in combination with valid sample rates
		if ( ((sampleRate_ == kIABSampleRate_48000Hz) && (maxRendered_ > ConstraintSet_ST2098_2_2018.maxRenderedLimit48k_)) 
			|| ((sampleRate_ == kIABSampleRate_96000Hz) && (maxRendered_ > ConstraintSet_ST2098_2_2018.maxRenderedLimit96k_)) )
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameMaxRenderedExceeded;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// If MaxRendered matches sub-element list
		// Re-calculate MaxRendered value from frame sub-element list for validation
		IABMaxRenderedRangeType calculatedMaxRendered;
		CalculateFrameMaxRendered(frameSubElements_, calculatedMaxRendered);

		if (calculatedMaxRendered != maxRendered_)
		{
			// Update issue variable and log
			iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningIAFrameMaxRenderedNotMatchObjectNumbers;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Sub-element count vs. sub-element size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (subElementCount_ != frameSubElements_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorIAFrameSubElementCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on IAFrame sub-elements, through the hierarchy
		std::vector<IABElement*>::const_iterator iterFrameSubElement;
		IABElementIDType elementID;

		for (iterFrameSubElement = frameSubElements_.begin(); iterFrameSubElement != frameSubElements_.end(); iterFrameSubElement++)
		{
			// Retrieve elementID
			(*iterFrameSubElement)->GetElementID(elementID);

			// Validate sub-elements
			//
			switch (elementID)
			{
			case kIABElementID_IAFrame:
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningFrameContainFrame;
				isToContinue = iEvenHandler.Handle(iValidationIssue);
				break;

			case kIABElementID_BedDefinition:
				isToContinue = dynamic_cast<IABBedDefinition*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			case kIABElementID_BedRemap:
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningFrameContainBedRemap;
				isToContinue = iEvenHandler.Handle(iValidationIssue);
				break;

			case kIABElementID_ObjectDefinition:
				isToContinue = dynamic_cast<IABObjectDefinition*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			case kIABElementID_ObjectZoneDefinition19:
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningFrameContainObjectZoneDefinition19;
				isToContinue = iEvenHandler.Handle(iValidationIssue);
				break;

			case kIABElementID_AudioDataDLC:
				isToContinue = dynamic_cast<IABAudioDataDLC*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			case kIABElementID_AudioDataPCM:
				isToContinue = dynamic_cast<IABAudioDataPCM*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			case kIABElementID_AuthoringToolInfo:
				isToContinue = dynamic_cast<IABAuthoringToolInfo*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			case kIABElementID_UserData:
				isToContinue = dynamic_cast<IABUserData*>(*iterFrameSubElement)->Validate(iEvenHandler, iValidationIssue);
				break;

			default:
				// Update issue variable and log
				// (Dev note: this is the default/defensive code. If the frame under validation is from IABParser,
				// it is unlikely to reach here. The specific warning of "kValidateWarningFrameContainUndefinedSubElement"
				// is issued through a different route in IABValidator lib.)
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningFrameContainUndefinedSubElement;
				isToContinue = iEvenHandler.Handle(iValidationIssue);
				break;
			}
		}

        if (!isToContinue)
        {
            return isToContinue;
        }
        
        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // ST429_18_2019 additional checks on IABFrame here (see each sub-element type for its own additional ST429_18_2019 checks) :
        // 1. Frame size
        // 2. Bitdepth - 24-bit only

		// Continue validation at IAFrame level
        iValidationIssue.id_ = kIssueID_IAFrame;

        // Check frame size (limit is frame rate dependent)
        
        // Frame rate should have been checked and issue reported already
        // Here only process frame rates within valid ST-429-18-2019 range
        if ((frameRate_ >= kIABFrameRate_24FPS) && (frameRate_ <= kIABFrameRate_120FPS))
        {
            IABElementSizeType frameSizeInBytes = 0;
            IABElementSizeType frameSizeLimit = 0;
            frameSizeLimit = static_cast<IABElementSizeType>(ConstraintSet_ST429_18_2019.frameSizeLimits_[static_cast<uint32_t>(frameRate_)]);
            GetElementSize(frameSizeInBytes);
            if (frameSizeInBytes > frameSizeLimit)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorIAFrameSizeLimitExceeded;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // Bit depth
        foundCount = ConstraintSet_ST429_18_2019.validBitDepth_.count(bitDepth_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Only report issue if it is allowed by ST2098_2_2018 and not st-429 to avoid duplicate reporting
            
            foundCount = ConstraintSet_ST2098_2_2018.validBitDepth_.count(bitDepth_);
            
            if (foundCount == 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedBitDepth;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // Note that DbyCinema is a super set of ST429_18_2019 and the preceding checks are required to validate
        // bitstream against DbyCinema
        
        // DbyCinema additional checks on IABFrame here (see each sub-element type for its own additional DbyCinema checks) :
        // 1. 48kHz sample rate only

        // Sample rate
        foundCount = ConstraintSet_DbyCinema.validSampleRates_.count(sampleRate_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Only report issue if it is allowed by ST2098_2_2018 and not st-429 to avoid duplicate reporting
            foundCount = ConstraintSet_ST2098_2_2018.validSampleRates_.count(sampleRate_);
            
            if (foundCount == 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinIAFrameUnsupportedSampleRate;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }

        // #################################################
        // Cinema constraint sets validation ends
        // #################################################

        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################

        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
		// IMF validation starts below at IAFrame level
		iValidationIssue.id_ = kIssueID_IAFrame;
	
		// bitstream version
        if (version_ != ConstraintSet_IMF_ST2098_2_2019.validVersion_)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameIllegalBitstreamVersion;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Sample rate
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validSampleRates_.count(sampleRate_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedSampleRate;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Bit depth
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validBitDepth_.count(bitDepth_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedBitDepth;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Frame rate
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validFrameRates_.count(frameRate_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedFrameRate;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // MaxRendered limits - check if limits are exceeded, in combination with valid sample rates
        if ( ((sampleRate_ == kIABSampleRate_48000Hz) && (maxRendered_ > ConstraintSet_IMF_ST2098_2_2019.maxRenderedLimit48k_))
            || ((sampleRate_ == kIABSampleRate_96000Hz) && (maxRendered_ > ConstraintSet_IMF_ST2098_2_2019.maxRenderedLimit96k_)) )
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameMaxRenderedExceeded;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
		// If MaxRendered matches sub-element list
		// Note calculatedMaxRendered is already calculated and unmodified during ST2098-2-2018 validation
		if (calculatedMaxRendered != maxRendered_)
		{
			// Update issue variable and log
			iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningIAFrameMaxRenderedNotMatchObjectNumbers;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Sub-element count vs. sub-element size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (subElementCount_ != frameSubElements_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorIAFrameSubElementCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // ST2098_2_2019 checks completed, change validation constriant set to kConstraints_set_IMF_ST2067_201_2019
        // Note that ST2067_201_2019 is a super set of ST2098_2_2019 and the preceding ST2098_2_2019 checks are required
        // to validate bitstream against ST2067_201_2019
        
        // First, ensure bit depth is valid for ST2098_2_2019 to avoid duplicate reporting
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validBitDepth_.count(bitDepth_);
        
        if (foundCount == 1)
        {
            // If bit depth is valid for ST2098_2_2019, check against ST2067_201_2019
            foundCount = ConstraintSet_IMF_ST2067_201_2019.validBitDepth_.count(bitDepth_);

            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorIAFrameUnsupportedBitDepth;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################
        

        return isToContinue;
    }

	// IABFrame::DeleteSubElements() implementation
	void IABFrame::DeleteSubElements()
	{
		for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
		{
			delete *iter;
		}

		frameSubElements_.clear();
		subElementCount_ = 0;
	}
    
	// IABFrame::ClearSubElements() implementation
	iabError IABFrame::ClearSubElements()
	{
		frameSubElements_.clear();
		subElementCount_ = 0;

		// Reset maxRendered_ to 0, as all sub elements are removed.
		maxRendered_ = 0;

		return kIABNoError;
	}

	// IABFrame::IsSubElement() implementation
	bool IABFrame::IsSubElement(IABElement* iSubElement)
	{
		if (nullptr == iSubElement)
		{
			return false;
		}

		// Check against sub element list
		for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
		{
			// Find the matching entry (same ponter address) from the list...
			if (iSubElement == *iter)
			{
				// found, return true
				return true;
			}
		}

		// Not found
		return false;
	}

	// IABFrame::RemoveSubElement() implementation
	iabError IABFrame::RemoveSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
		{
			// Find the matching entry from the list...
			if (iSubElement == *iter)
			{
				// Erase the entry from list (but not delete *iter).
				frameSubElements_.erase(iter);

				subElementCount_ -= 1;				// Decrement subElementCount_ to account for removal of 1 entry

				errorCode = kIABNoError;
				break;								// Entry found and removed. Break.
			}
		}

		return errorCode;
	}

	// IABFrame::AddSubElement() implementation
	iabError IABFrame::AddSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;
		IABElementIDType elementID;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		// Check if element is of types allowed as BedDefinition sub element
		iSubElement->GetElementID(elementID);

		if ((kIABElementID_BedDefinition != elementID) &&
			(kIABElementID_ObjectDefinition != elementID) &&
			(kIABElementID_AuthoringToolInfo != elementID) &&
			(kIABElementID_UserData != elementID) &&
			(kIABElementID_AudioDataDLC != elementID) &&
			(kIABElementID_AudioDataPCM != elementID))
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
		{
			// Is iSubElement already in sub-element list?
			if (iSubElement == *iter)
			{
				// If yes, exit. Entry already in sub element list.
				return kIABDuplicateSubElementsError;
			}
		}

		// Not in sub element list, add.
		frameSubElements_.push_back(iSubElement);
		subElementCount_ += 1;						// Increment subElementCount_ to account for addition of 1 entry

		return kIABNoError;
	}

	// IABFrame::EnablePackingSubElements() implementation
	void IABFrame::EnablePackingSubElements()
	{
		// Call base class method EnablePacking()
		IABElement::EnablePacking();
	}

	// IABFrame::DisablePackingSubElements() implementation
	void IABFrame::DisablePackingSubElements()
	{
		// Call base class method DisablePacking()
		IABElement::DisablePacking();
	}

	// IABFrame::AreSubElementsEnabledForPacking() implementation
	bool IABFrame::AreSubElementsEnabledForPacking()
	{
		// Call base class method IsIncludedForPacking()
		return IABElement::IsIncludedForPacking();
	}

	// IABFrame::Serialize() implementation
	iabError IABFrame::Serialize(std::ostream &outStream)
	{
		// TODO: Error code for each write() should be checked.
		// It is not, yet.

		// *****************
		// Wrapping the following for each call seem excessive? (Something for team discussion?)
		// ReturnCode err = OK;
		// 
		// err = xxx.write();
		// if (err != OK)
		// {
		// 	return err;
		// }
		// 
		// // another write() call
		// // ...
		// 
		// return err;
		// *****************

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		elementPayloadWriter_.write(version_, 8);
		elementPayloadWriter_.write(static_cast<uint8_t>(sampleRate_), 2);
		elementPayloadWriter_.write(static_cast<uint8_t>(bitDepth_), 2);
		elementPayloadWriter_.write(static_cast<uint8_t>(frameRate_), 4);
		// Calling method for writing Plex encoded field.
		write(elementPayloadWriter_, Plex<8>(maxRendered_));

		// Prepare to pack IABFrame sub elements, but is overall sub element packing disabled?
		if (AreSubElementsEnabledForPacking())
		{
			// Yes, update packing enabled sub element count
			UpdatePackingEnabledSubElementCount();
		}
		else
		{
			// No, 0 subelements will be packed below (skipped)
			packedSubElementCount_ = 0;
		}

		elementPayloadWriter_.align();										// Alignment to byte
		write(elementPayloadWriter_, Plex<8>(packedSubElementCount_));		// Note to use packedSubElementCount_

		// Pack sub elements, only when it is not disabled
		if (AreSubElementsEnabledForPacking())
		{
			// Serialize all sub-elements, in sequence
			// Append to end of IABFrame's "elementPayloadBuffer_"
			for (IABElementCountType i = 0; i < subElementCount_; i++)
			{
				// Temp StringStream to hold packed output from sub-elements (TODO: optimization?)
				std::stringstream subElementPayload(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				frameSubElements_[i]->Serialize(subElementPayload);
				const std::string payLoad = subElementPayload.str();
				uint32_t length = static_cast<uint32_t>(payLoad.size());
				elementPayloadWriter_.write((uint8_t*)payLoad.data(), length);
			}
		}

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Update packedPayloadLength to wrapper subframe length
		packedPayloadLength += static_cast<IABElementSizeType>(elementHeadBuffer_.tellp());

		// Set length for containerSubframeWrapper_
		containerSubframeWrapper_.SetSubframeLength(static_cast<IABSubframeLengthType>(packedPayloadLength));

		// Serialize preamble, as the first subframe of IABitstreamFrame
		aPreamble_.Serialize(outStream);

		// Serialize/Wrapping IABFrame, the wrapper for the 2nd subframe of IABitstreamFrame
		containerSubframeWrapper_.Serialize(outStream);

		// The combination of following forms the "Value" of IA subframe
		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		// Are we done? (TBD)

		return kIABNoError;
	}

	void IABFrame::SetDeSerializeFailsOnVersionError(bool failOnVersionError)
	{
		failOnVersionError_ = failOnVersionError;
	}

	bool IABFrame::GetDeSerializeFailsOnVersionError() const
	{
		return failOnVersionError_;
	}

	// IABFrame::DeSerialize() implementation
	iabError IABFrame::DeSerialize()
	{
		iabError errorCode = kIABNoError;
		iabError errorCodePreamble = kIABNoError;

		if (!elementReader_)
		{
			return kIABParserBitstreamReaderNotPresentError;
		}

		// Align reader at beginning (do we need to?)
		elementReader_->align();

		// Per IAB specification, parsing (DeSerialize) of aPreamble_ and containerSubframeWrapper_
		// shall be called in the following sequence to acertain subframe structures.
		//
		// Parsing PCM preamble sub-frame first
		errorCode = aPreamble_.DeSerialize(*elementReader_);
		
		if (errorCode != kIABNoError)
		{
			if (errorCode != kIABParserMissingPreambleError )
			{
				// Exit right away if I/O or system errors occur
				return errorCode;
			}
			else
			{
				// Save preamble missing error code to be returned to client.
				// (Preamble is mandatory per ST2098-2 specification. However, a non-hard exit
				// approach is taken to enable IABValidator, as well offer continuous parsing
				// of IA Frames data.)
				// (Dev note: Subject to review and change.)
				errorCodePreamble = errorCode;
			}
		}

		// Parsing IA subframe wrapper header
		errorCode = containerSubframeWrapper_.DeSerialize(*elementReader_);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

		// After subframe structures or headers are validated, moving to parse the IA frame contents
		//
		// At the beginning/ first byte of IABFrame element
		// Parsing element ID and size
		errorCode = DeSerializeHead(*elementReader_);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

		// Parsing Frame data fields
		errorCode = ParseFrameDataFields();

		if ( (errorCode != kIABNoError) &&
			(failOnVersionError_ || errorCode != kIABParserInvalidVersionNumberError))
		{

			return errorCode;
		}

		// Parse subElementCount_ number of sub-element
		for (uint32_t i = 0; i < static_cast<uint32_t>(subElementCount_); i++)
        {
			iabError errorCodeSubElement = ParseFrameSubElement();

			if (errorCodeSubElement != kIABNoError)
			{
				return errorCodeSubElement;
			}
        }
        
        // If any bitstream elements have been skipped by the parser, deduct them from sub-element count read from the bitstream
        subElementCount_ -= numSkippedFrameSubElementsInParsing_;

		if (errorCodePreamble != kIABNoError)
		{
			return errorCodePreamble;
		}

		// If errorCodePreamble is OK then errorCode
		// might still return kIABParserInvalidVersionNumberError
		// if failOnVersionError_ is set to false
		return errorCode;
	}
    
	// IABFrame::ParseFrameDataFields() implementation
	iabError IABFrame::ParseFrameDataFields()
    {
		iabError versionNumberCheckResult = kIABNoError;

        uint8_t	fixedLengthField = 0;
        
        // bitstream version is 8-bit
        if (CMNSTRM_OK != elementReader_->read(fixedLengthField))
        {
            return kIABParserIABFrameError;
        }
        
        if (fixedLengthField != kIABDefaultFrameVersion)
        {
            versionNumberCheckResult = kIABParserInvalidVersionNumberError;
        }
        
        version_ = fixedLengthField;
        
        // Audio sample rate is 2-bit
        if (CMNSTRM_OK != elementReader_->read(fixedLengthField, 2))
        {
            return kIABParserIABFrameError;
        }

        if ((fixedLengthField != static_cast<uint8_t>(kIABSampleRate_48000Hz)) &&
            (fixedLengthField != static_cast<uint8_t>(kIABSampleRate_96000Hz)))
        {
            return kIABParserInvalidSampleRateError;
        }

        sampleRate_ = static_cast<IABSampleRateType>(fixedLengthField);
        
        // Audio sample bit depth is 2-bit
        if (CMNSTRM_OK != elementReader_->read(fixedLengthField, 2))
        {
            return kIABParserIABFrameError;
        }
        
        if ((fixedLengthField != static_cast<IABBitDepthType>(kIABBitDepth_16Bit)) &&
            (fixedLengthField != static_cast<IABBitDepthType>(kIABBitDepth_24Bit)))
        {
            return kIABParserInvalidBitDepthError;
        }

        bitDepth_ = static_cast<IABBitDepthType>(fixedLengthField);
        
        // Frame rate is 4-bit
        if (CMNSTRM_OK != elementReader_->read(fixedLengthField, 4))
        {
            return kIABParserIABFrameError;
        }

        if (fixedLengthField >= static_cast<uint8_t>(kIABFrameRate_BoundReserved))
        {
            return kIABParserInvalidFrameRateError;
        }

        frameRate_ = static_cast<IABFrameRateType>(fixedLengthField);

        Plex<8> plexField;
        
        // Maxrendered field is plex 8 coded
        if (CMNSTRM_OK != read(*elementReader_, plexField))
        {
            return kIABParserIABFrameError;
        }
        
        maxRendered_ = static_cast<IABMaxRenderedRangeType>(plexField);
        
        // Frame sub-element count field is plex 8 coded
        if (CMNSTRM_OK != read(*elementReader_, plexField))
        {
            return kIABParserIABFrameError;
        }
        
        subElementCount_ = static_cast<IABElementCountType>(plexField);

        return versionNumberCheckResult;
    }
    
	// IABFrame::ParseFrameSubElement() implementation
	iabError IABFrame::ParseFrameSubElement()
    {
		uint32_t parsedElementIDField;
        IABElementIDType elementID;

		// Peek the next (Plex<8> coded) element/sub-element ID field
        if (kIABNoError != PeekNextElementID(*elementReader_, parsedElementIDField))
        {
            return kIABParserParsingError;
        }

		elementID = static_cast<IABElementIDType>(parsedElementIDField);

		IABElement* frameSubElement = nullptr;
        
        switch (elementID)
        {
            case kIABElementID_BedDefinition:
				frameSubElement = new IABBedDefinition(frameRate_);
				break;
                
            case kIABElementID_ObjectDefinition:
				frameSubElement = new IABObjectDefinition(frameRate_);
				break;
                
            case kIABElementID_AudioDataDLC:
				frameSubElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRate_, sampleRate_));

				// For DCL, check if a valid frameSubElement is returned
				// DLC is not supported for all frame rates
				if (!frameSubElement)
				{
					return kIABDataFieldInvalidDLC;
				}
                break;
                
            case kIABElementID_AuthoringToolInfo:
                frameSubElement = new IABAuthoringToolInfo();
                break;
                
            case kIABElementID_UserData:
                frameSubElement = new IABUserData();
                break;

			case kIABElementID_AudioDataPCM:
				// AudioDataPCM element instantiated using 3 parameter values already parsed in during ParseFrameDataFields() call
				frameSubElement = new IABAudioDataPCM(frameRate_, sampleRate_, bitDepth_);
				break;

			// Known type but invalid type as IAB frame sub-element
			// Total number of unallowed element IDs for the frame is tracked in numUnallowedFrameSubElements_.
			// IAFrame having a sub-element of IAFrame type is also not allowed.
			case kIABElementID_IAFrame:
			case kIABElementID_BedRemap:
			case kIABElementID_ObjectZoneDefinition19:
				SkipDeSerializeCurrentElement(*elementReader_);
				numSkippedFrameSubElementsInParsing_++;
				numUnallowedFrameSubElements_++;
				return kIABNoError;										// Exit, as frameSubElement == nullptr for this case

			// Undefined/Unknown type as IAB frame sub-element
			// Total number of unallowed element IDs for the frame is tracked in numUndefinedFrameSubElements_.
			default:
				SkipDeSerializeCurrentElement(*elementReader_);
                numSkippedFrameSubElementsInParsing_++;
				numUndefinedFrameSubElements_++;
                return kIABNoError;										// Exit, as frameSubElement == nullptr for default case
        }

		// Check validity of frameSubElement
		if (nullptr == frameSubElement)
		{
			return kIABMemoryError;
		}

		// Parse sub-element
        iabError returnCode = kIABNoError;
        
        returnCode = frameSubElement->DeSerialize(*elementReader_);
        
        if (returnCode != kIABNoError)
        {
            delete frameSubElement;
            return returnCode;
        }
        
		frameSubElements_.push_back(frameSubElement);				// Save to frame sub element list

        return kIABNoError;
    }

	// IABFrame::UpdatePackEnabledSubElementCount() implementation
	void IABFrame::UpdatePackingEnabledSubElementCount()
	{
		IABElementCountType numPackingEnabledElement = 0;

		for (std::vector<IABElement*>::iterator iter = frameSubElements_.begin(); iter != frameSubElements_.end(); iter++)
		{
			if ((*iter)->IsIncludedForPacking())
			{
				numPackingEnabledElement += 1;
			}
		}

		packedSubElementCount_ = numPackingEnabledElement;
	}

	// IABFrame::GetNumUnallowedSubElements() implementation
	uint32_t IABFrame::GetNumUnallowedSubElements() const
	{
		return numUnallowedFrameSubElements_;
	}

	// IABFrame::GetNumUndefinedSubElements() implementation
	uint32_t IABFrame::GetNumUndefinedSubElements() const
	{
		return numUndefinedFrameSubElements_;
	}


	// ****************************************************************************
	// IABChannel class implementation
	// ****************************************************************************

	// Constructor implementation
	IABChannel::IABChannel()
    {
        channelID_ = kIABChannelID_Center;              // Default to Center channel
        audioDataID_ = 0;
        channelGain_.setIABGain(1.0f);  // Gain = 1.0f
        channelDecorInfoExists_ = 0;                    // No decorrelation info
        reserved_ = 0x00;;                              // a fixed 4-bit reserved field, present in stream only when (channelDecorInfoExists_ == 1)
        channelDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;  // No Decor.
        channelDecorCoef_.decorCoef_ = 0;
	}

	// IABChannel::GetChannelID() implementation
    void IABChannel::GetChannelID(IABChannelIDType &oChannelID) const
    {
        oChannelID = channelID_;
    }

	// IABChannel::SetChannelID() implementation
    iabError IABChannel::SetChannelID(IABChannelIDType iChannelID)
    {
        channelID_ = iChannelID;
        return kIABNoError;
    }

	// IABChannel::GetAudioDataID() implementation
    void IABChannel::GetAudioDataID(IABAudioDataIDType &oAudioDataID) const
    {
        oAudioDataID = audioDataID_;
    }

	// IABChannel::SetAudioDataID() implementation
    iabError IABChannel::SetAudioDataID(IABAudioDataIDType iAudioDataID)
    {
        audioDataID_ = iAudioDataID;
        return kIABNoError;
    }

	// IABChannel::GetChannelGain() implementation
    void IABChannel::GetChannelGain(IABGain &oChannelGain) const
    {
        oChannelGain = channelGain_;
    }

	// IABChannel::SetChannelGain() implementation
    iabError IABChannel::SetChannelGain(IABGain const& iChannelGain)
    {
        channelGain_ = iChannelGain;
        return kIABNoError;
    }

	// IABChannel::GetDecorInfoExists() implementation
    void IABChannel::GetDecorInfoExists(uint1_t &oDecorInfoExists) const
    {
        oDecorInfoExists = channelDecorInfoExists_;
    }

	// IABChannel::SetDecorInfoExists() implementation
    iabError IABChannel::SetDecorInfoExists(uint1_t iDecorInfoExists)
    {
        channelDecorInfoExists_ = iDecorInfoExists;
        return kIABNoError;
    }

	// IABChannel::GetDecorCoef() implementation
    void IABChannel::GetDecorCoef(IABDecorCoeff &oDecorCoef) const
    {
        oDecorCoef = channelDecorCoef_;
    }

	// IABChannel::SetDecorCoef() implementation
    iabError IABChannel::SetDecorCoef(IABDecorCoeff const& iDecorCoef)
    {
        channelDecorCoef_ = iDecorCoef;
        return kIABNoError;
    }

	// IABChannel::Serialize() implementation
	iabError IABChannel::Serialize(StreamWriter& streamWriter)
	{
		// The payload stream writer for parent BedDefinition element is passed in.
		// Individual channel instances are packed into the payload buffer/ostream of  
		// BedDefinition element, without alignment manipulation
		// ! Do not change alignment when writing individual IAB channels

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		write(streamWriter, Plex<4>(channelID_));
		write(streamWriter, Plex<8>(audioDataID_));

		streamWriter.write(static_cast<uint8_t>(channelGain_.getIABGainPrefix()), 2);
		if (kIABGainPrefix_InStream == channelGain_.getIABGainPrefix())
		{
			streamWriter.write(channelGain_.getIABGainInStreamValue(), 10);
		}

		streamWriter.write(channelDecorInfoExists_, 1);
		if (channelDecorInfoExists_ == 1)
		{
			streamWriter.write(reserved_, 4);
			streamWriter.write(static_cast<uint8_t>(channelDecorCoef_.decorCoefPrefix_), 2);
			// Enforce a more strict checking than 
			// if (channelDecorCoef_.decorCoefPrefix_ > kIABDecorCoeffPrefix_MaxDecor)?
			if (channelDecorCoef_.decorCoefPrefix_ == kIABDecorCoeffPrefix_DecorCoeffInStream)
			{
				streamWriter.write(channelDecorCoef_.decorCoef_);   // write 8-bit decor value
			}
		}

		return kIABNoError;
	}

	// IABChannel::DeSerialize() implementation
	iabError IABChannel::DeSerialize(StreamReader& streamReader)
	{
        uint8_t	fixedLengthFieldMax8 = 0;
        uint16_t fixedLengthFieldMax16 = 0;
        Plex<8> plex8Field = 0;
        Plex<4> plex4Field = 0;
        
        // Read channel ID
        if (CMNSTRM_OK != read(streamReader, plex4Field))
        {
            return kIABParserIABChannelError;
        }

        channelID_ = static_cast<IABChannelIDType>(static_cast<uint32_t>(plex4Field));
        
        // Read audio data ID
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABChannelError;         
        }
        
        audioDataID_ = static_cast<IABAudioDataIDType>(plex8Field);
        
        // Read 2-bit gain prefix
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABChannelError;
        }
        
        if (fixedLengthFieldMax8 == 0)
        {
            channelGain_.setIABGain(1.0f);
        }
        else if (fixedLengthFieldMax8 == 1)
        {
            channelGain_.setIABGain(0.0f);
        }
        else if (fixedLengthFieldMax8 == 2)
        {
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10)) // 10-bit gain in bitstream
            {
                return kIABParserIABChannelError;
            }
            
            channelGain_.setIABGainFromStreamValue(fixedLengthFieldMax16);
        }
        else
        {
            return kIABParserIABChannelError;
        }

        // Read 1-bit channel decor exist flag
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABChannelError;
        }
        
        channelDecorInfoExists_ = static_cast<uint1_t>(fixedLengthFieldMax8);
        
        if (1 == channelDecorInfoExists_)
        {
            // Read 4-bit reserved field
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 4))
            {
                return kIABParserIABChannelError;
            }
            
            reserved_ = static_cast<uint4_t>(fixedLengthFieldMax8);

            // Read 2-bit channel decor prefix
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
            {
                return kIABParserIABChannelError;
            }
            
            if (fixedLengthFieldMax8 <= 2)
            {
                channelDecorCoef_.decorCoefPrefix_ = static_cast<IABDecorCoeffPrefixType>(fixedLengthFieldMax8);
                channelDecorCoef_.decorCoef_ = 0;
                if (2 == fixedLengthFieldMax8)
                {
                    // Read 8-bit channel decor value
                    if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8))
                    {
                        return kIABParserIABChannelError;
                    }
                    
                    channelDecorCoef_.decorCoef_ = fixedLengthFieldMax8;
                }
            }
            else
            {
                return kIABParserIABChannelError;
            }
        }

		return kIABNoError;
	}
    
    // IABChannel::Validate() implementation
    bool IABChannel::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// Channel AudioDataID
		// ok

		// General count var
		size_t foundCount = 0;

		// ChannelID
		foundCount = ConstraintSet_ST2098_2_2018.validChannelIDs_.count(channelID_);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedChannelID;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// ChannelID
		// ok

		// Channel gain prefix code
		foundCount = ConstraintSet_ST2098_2_2018.validGainPrefix_.count(channelGain_.getIABGainPrefix());

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorBedDefinitionUnsupportedGainPrefix;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Channel decorrelation coefficient prefix code
		// Only check if channelDecorInfoExists_ is enabled
		if (channelDecorInfoExists_)
		{
			foundCount = ConstraintSet_ST2098_2_2018.validDecorCoeffPrefix_.count(channelDecorCoef_.decorCoefPrefix_);

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorBedDefinitionUnsupportedDecorPrefix;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // ST429_18_2019 additional checks on IABChannel :
        // 1. Invalid channel IDs : any code > 0x7F

        if (channelID_ > ConstraintSet_ST429_18_2019.upperBoundForValidChannelID_)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionInvalidChannelID;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }

        // Note that kConstraints_set_DbyCinema is a super set of ST-429-18-2019 and the preceding checks are required to validate
        // bitstream against DbyCinema
        
        // DbyCinema additional checks on IABChannel :
        // 1. Supports 9.1OH channel IDs only
        // 2. ChannelGainPrefix = 0x00
        // 3. ChannelDecorinfoExists = 0
        
        // ChannelID
        foundCount = ConstraintSet_DbyCinema.validChannelIDs_.count(channelID_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorDolCinBedDefinitionInvalidChannelID;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Channel gain prefix code
        foundCount = ConstraintSet_DbyCinema.validGainPrefix_.count(channelGain_.getIABGainPrefix());
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorDolCinBedDefinitionInvalidGainPrefix;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Channel decorrelation coefficient prefix code
        if (channelDecorInfoExists_)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorDolCinBedDefinitionChannelDecorInfoExistNotZero;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################

        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Channel AudioDataID
        // ok
        
        // ChannelID
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validChannelIDs_.count(channelID_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = WarningEvent;
            iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedChannelID;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // ChannelID
        // ok
        
        // Channel gain prefix code
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validGainPrefix_.count(channelGain_.getIABGainPrefix());
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionUnsupportedGainPrefix;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Channel decorrelation coefficient prefix code
        // Only check if channelDecorInfoExists_ is enabled
        if (channelDecorInfoExists_)
        {
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validDecorCoeffPrefix_.count(channelDecorCoef_.decorCoefPrefix_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorBedDefinitionUnsupportedDecorPrefix;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }

		// Note that kConstraints_set_DbyIMF is a super set of ST2067-201-2019 and the preceding IMF
		// checks are required to validate bitstream against DbyIMF

		// DbyIMF additional checks on IABChannel :
		// 1. Supports 9.1OH and 5.1 channel IDs only
		// 2. ChannelGainPrefix = 0x00
		// 3. ChannelDecorinfoExists = 0

		// ChannelID
		foundCount = ConstraintSet_DbyIMF.validChannelIDs_.count(channelID_);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
			iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorDolIMFBedDefinitionInvalidChannelID;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Channel gain prefix code
		foundCount = ConstraintSet_DbyIMF.validChannelGainPrefix_.count(channelGain_.getIABGainPrefix());

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
			iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorDolIMFBedDefinitionInvalidGainPrefix;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Channel decorrelation coefficient prefix code
		if (channelDecorInfoExists_)
		{
			// Update issue variable and log
			iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorDolIMFBedDefinitionChannelDecorInfoExistNotZero;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

        // #################################################
        // IMF constraint sets validation ends
        // #################################################

        return isToContinue;
    }

	// ****************************************************************************
	// IABBedDefinition class implementation
	// ****************************************************************************

	// Constructor implementation
	IABBedDefinition::IABBedDefinition(IABFrameRateType iFrameRate) :
		IABElement(kIABElementID_BedDefinition)
    {
        metadataID_ = 0;
        conditionalBed_ = 0;                            // Conditional bed = 0, ie unconditional/default bed
        bedUseCase_ = kIABUseCase_9_1_OH;               // 9.1OH
        channelCount_ = 0;                              // should match number of channels in bedChannels_
        reserved_ = 0x180;                              // A reserved bit field of 0x180, per specification
        audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription_.audioDescriptionText_.clear();
        subElementCount_ = 0;							// should match number of element in bedSubElements_
		numSkippedBedSubElementsInParsing_ = 0;
		numUndefinedBedSubElements_ = 0;
		numUnallowedBedSubElements_ = 0;

		packedSubElementCount_ = 0;						// To match number of element in bedSubElements_ that is packing enabled
		parentFrameRate_ = iFrameRate;
	}

    // Destructor
	IABBedDefinition::~IABBedDefinition()
    {
		DeleteBedChannels();
		DeleteSubElements();
    }

	// IABBedDefinition::GetMetadataID() implementation
    void IABBedDefinition::GetMetadataID(IABMetadataIDType &oMetaID) const
    {
        oMetaID = metadataID_;
    }

	// IABBedDefinition::SetMetadataID() implementation
    iabError IABBedDefinition::SetMetadataID(IABMetadataIDType iMetaID)
    {
        metadataID_ = iMetaID;
        return kIABNoError;
    }

	// IABBedDefinition::GetConditionalBed() implementation
    void IABBedDefinition::GetConditionalBed(uint1_t &oConditionalBed) const
    {
        oConditionalBed = conditionalBed_;
    }

	// IABBedDefinition::SetConditionalBed() implementation
    iabError IABBedDefinition::SetConditionalBed(uint1_t iConditionalBed)
    {
        conditionalBed_ = iConditionalBed;
        return kIABNoError;
    }

	// IABBedDefinition::GetBedUseCase() implementation
    void IABBedDefinition::GetBedUseCase(IABUseCaseType &oBedUseCase) const
    {
        oBedUseCase = bedUseCase_;
    }

	// IABBedDefinition::SetBedUseCase() implementation
    iabError IABBedDefinition::SetBedUseCase(IABUseCaseType iBedUseCase)
    {
        bedUseCase_ = iBedUseCase;
        return kIABNoError;
    }

	// IABBedDefinition::GetChannelCount() implementation
    void IABBedDefinition::GetChannelCount(IABChannelCountType &oChannelCount) const
    {
        oChannelCount = channelCount_;
    }

	// IABBedDefinition::GetBedChannels() implementation
    void IABBedDefinition::GetBedChannels(std::vector<IABChannel*> &oBedChannels) const
    {
        oBedChannels = bedChannels_;
    }

	// IABBedDefinition::SetBedChannels() implementation
    iabError IABBedDefinition::SetBedChannels(const std::vector<IABChannel*> iBedChannels)
    {
		DeleteBedChannels();

		channelCount_ = static_cast<IABChannelCountType>(iBedChannels.size());

		// Checking if any of the pointers is a nullptr
		for (uint32_t i = 0; i < channelCount_; i++)
		{
			if (iBedChannels[i] == nullptr)
			{
				return kIABBadArgumentsError;
			}
		}

		bedChannels_ = iBedChannels;

		return kIABNoError;
    }

	// IABBedDefinition::GetAudioDescription() implementation
    void IABBedDefinition::GetAudioDescription(IABAudioDescription &oAudioDescription) const
    {
        oAudioDescription = audioDescription_;
    }

	// IABBedDefinition::SetAudioDescription() implementation
    iabError IABBedDefinition::SetAudioDescription(IABAudioDescription const& iAudioDescription)
    {
        audioDescription_ = iAudioDescription;
        return kIABNoError;
    }

	// IABBedDefinition::GetSubElementCount() implementation
    void IABBedDefinition::GetSubElementCount(IABElementCountType &oCount) const
    {
        oCount = subElementCount_;
    }

	// IABBedDefinition::GetSubElements() implementation
    void IABBedDefinition::GetSubElements(std::vector<IABElement*> &oSubElements) const
    {
        oSubElements = bedSubElements_;
    }

	// IABBedDefinition::SetSubElements() implementation
    // TODO: Need to revise the submement deletion inside this method.
    // MAC xcode lets user to use the deleted pointer. That may endup in crash or in undefined behaviour at userspace.
    // Setting NULL to delete subment here also will not have any effect on userspace.
    iabError IABBedDefinition::SetSubElements(const std::vector<IABElement*> iSubElements)
    {
        IABElementCountType count = static_cast<IABElementCountType>(iSubElements.size());
        IABElementIDType elementID;
        
        // Checking if any of the pointers is a nullptr or not a valid sub element type
        for (uint32_t i = 0; i < count; i++)
        {
			if (nullptr == iSubElements[i])
			{
				return kIABBadArgumentsError;
			}

			iSubElements[i]->GetElementID(elementID);
            
            if ((kIABElementID_BedDefinition != elementID) && (kIABElementID_BedRemap != elementID))
            {
                return kIABBadArgumentsError;
            }
        }
        
        // Report error if duplicate sub-elements are in the list.
        std::set<IABElement*> tempSet; //Using Set datastructure, Existence check can be done easily. i.e set doenst allow the dublicates.
        for(std::vector<IABElement*>::const_iterator iter = iSubElements.begin(); iter != iSubElements.end(); ++iter)
        {
            // finds the duplicate
            if (!tempSet.insert(*iter).second)
                return kIABDuplicateSubElementsError;
        }

        
        // Update existing sub-elements list carefully.
        for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
        {
            bool found = false;
            for (std::vector<IABElement*>::const_iterator sub = iSubElements.begin(); sub != iSubElements.end(); sub++)
            {
                if (*iter == *sub)
                {
                    found = true;
                    break;
                }
            }
            // if sub-elmenent not found in the current list, just delete the sub-element.
            if (found == false)
            {
                delete *iter;
            }
        }
        
        // Now set the new list as current list.
        bedSubElements_ = iSubElements;
        subElementCount_ = count;
        
        return kIABNoError;
    }

	// IABBedDefinition::DeleteSubElements() implementation
	void IABBedDefinition::DeleteSubElements()
	{
		for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
		{
			delete *iter;
		}

		bedSubElements_.clear();
		subElementCount_ = 0;
	}

	// IABBedDefinition::DeleteBedChannels() implementation
	void IABBedDefinition::DeleteBedChannels()
	{
		for (std::vector<IABChannel*>::iterator iter = bedChannels_.begin(); iter != bedChannels_.end(); iter++)
		{
			delete *iter;
		}

		bedChannels_.clear();
	}

	// IABBedDefinition::ClearSubElements() implementation
	iabError IABBedDefinition::ClearSubElements()
	{
		bedSubElements_.clear();
		subElementCount_ = 0;

		return kIABNoError;
	}

	// IABBedDefinition::IsSubElement() implementation
	bool IABBedDefinition::IsSubElement(IABElement* iSubElement)
	{
		if (nullptr == iSubElement)
		{
			return false;
		}

		// Check against sub element list
		for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
		{
			// Find the matching entry (same ponter address) from the list...
			if (iSubElement == *iter)
			{
				// found, return true
				return true;
			}
		}

		// Not found
		return false;
	}

	// IABBedDefinition::RemoveSubElement() implementation
	iabError IABBedDefinition::RemoveSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
		{
			// Find the matching entry from the list...
			if (iSubElement == *iter)
			{
				// Erase the entry from list (but not delete *iter).
				bedSubElements_.erase(iter);

				subElementCount_ -= 1;				// Decrement subElementCount_ to account for removal of 1 entry

				errorCode = kIABNoError;
				break;								// Entry found abnd removed. Break.
			}
		}

		return errorCode;
	}

	// IABBedDefinition::AddSubElement() implementation
	iabError IABBedDefinition::AddSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;
		IABElementIDType elementID;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		// Check if element is of types allowed as BedDefinition sub element
		iSubElement->GetElementID(elementID);

		if ((kIABElementID_BedDefinition != elementID) 
			&& (kIABElementID_BedRemap != elementID))
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
		{
			// Is iSubElement already in sub-element list?
			if (iSubElement == *iter)
			{
				// If yes, exit. Entry already in sub element list.
				return kIABDuplicateSubElementsError;
			}
		}

		// Not in sub element list, add.
		bedSubElements_.push_back(iSubElement);
		subElementCount_ += 1;						// Increment subElementCount_ to account for addition of 1 entry

		return kIABNoError;
	}

	// IABBedDefinition::EnablePacking() implementation
	void IABBedDefinition::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABBedDefinition::DisablePacking() implementation
	void IABBedDefinition::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABBedDefinition::IsIncludedForPacking() implementation
	bool IABBedDefinition::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABBedDefinition::Serialize() implementation
	iabError IABBedDefinition::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}
		else
		{
			// Yes, included. Update sub element count for sub element packing
			UpdatePackingEnabledSubElementCount();
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		// Calling method for writing Plex encoded field.
		write(elementPayloadWriter_, Plex<8>(metadataID_));

		elementPayloadWriter_.write(conditionalBed_, 1);
		if (conditionalBed_ == 1)
		{
			elementPayloadWriter_.write(static_cast<uint8_t>(bedUseCase_), 8);
		}

		write(elementPayloadWriter_, Plex<4>(channelCount_));

		// Serialize all bed channels, in sequence, into payload writer
		for (IABChannelCountType i = 0; i < channelCount_; i++)
		{
			bedChannels_[i]->Serialize(elementPayloadWriter_);
		}

		elementPayloadWriter_.write(reserved_, 10);							// Reserved field, after all channels
		elementPayloadWriter_.align();										// Align to byte

		elementPayloadWriter_.write(static_cast<uint8_t>(audioDescription_.audioDescription_), 8);

		if (audioDescription_.audioDescription_ & 0x80)
		{
            // Write audio description text to bitstream as a null terminated string (i.e. character sequence)
            // writeStringAppendNull() will add a null byte after the last character of the string.
            writeStringAppendNull(elementPayloadWriter_, audioDescription_.audioDescriptionText_);
		}

		write(elementPayloadWriter_, Plex<8>(packedSubElementCount_));		// Note to write packedSubElementCount_ instead of subElementCount_
		// Serialize all sub-elements, in sequence
		// Append to end of IABBedDefinition's "elementPayloadBuffer_"
		for (IABElementCountType i = 0; i < subElementCount_; i++)			// Still loop through all subElementCount_ of sub elements
		{
			// Temp StringStream to hold packed output from sub-elements (TODO: optimization?)
			std::stringstream subElementPayload(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			bedSubElements_[i]->Serialize(subElementPayload);
			const std::string payLoad = subElementPayload.str();
			uint32_t length = static_cast<uint32_t>(payLoad.size());
			elementPayloadWriter_.write((uint8_t*)payLoad.data(), length);
		}

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABBedDefinition::DeSerialize() implementation
	iabError IABBedDefinition::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;

		// Parsing element ID and size
		errorCode = DeSerializeHead(streamReader);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

        uint8_t	fixedLengthField = 0;
        Plex<8> plex8Field = 0;
        Plex<4> plex4Field = 0;

        // Read meta data ID
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABBedDefinitionError;
        }
        
        metadataID_ = static_cast<IABMetadataIDType>(plex8Field);
        
        // Read 1-bit conditional bed
        if (CMNSTRM_OK != streamReader.read(fixedLengthField, 1))
        {
            return kIABParserIABBedDefinitionError;
        }
        
        conditionalBed_ = static_cast<uint1_t>(fixedLengthField);
        
        // Read 8-bit use case
        if (1 == conditionalBed_)
        {
            if (CMNSTRM_OK != streamReader.read(fixedLengthField))
            {
                return kIABParserIABBedDefinitionError;
            }
            
            bedUseCase_ = static_cast<IABUseCaseType>(fixedLengthField);
        }
        
        // Read channel count
        if (CMNSTRM_OK != read(streamReader, plex4Field))
        {
            return kIABParserIABBedDefinitionError;
        }
        
        channelCount_ = static_cast<IABChannelCountType>(plex4Field);
        
		for (uint32_t i = 0; i < static_cast<uint32_t>(channelCount_); i++)
        {
			errorCode = ParseBedChannel(streamReader);

			if (errorCode != kIABNoError)
			{
				return errorCode;
			}
        }

        // Read 10-bit reserved field
        uint16_t reserved10 = 0;
        if (CMNSTRM_OK != streamReader.read(reserved10, 10))
        {
            return kIABParserIABBedDefinitionError;
        }

        reserved_ = static_cast<uint10_t>(reserved10);
        if (CMNSTRM_OK != streamReader.align())  // byte alignment
        {
            return kIABParserIABBedDefinitionError;
        }
        
        // Read audio description
        uint8_t audioDescription = 0;
        if (CMNSTRM_OK != streamReader.read(audioDescription))
        {
            return kIABParserIABBedDefinitionError;
        }
        
        audioDescription_.audioDescription_ = static_cast<IABAudioDescriptionType>(audioDescription);
        
        if (audioDescription & 0x80)
        {
            // Read audio description text string from the bitstream and the string is expected to be
            // terminated by a null byte. The null byte is removed before saving into audioDescription_.audioDescriptionText_.
            if (CMNSTRM_OK != readToNull(streamReader, audioDescription_.audioDescriptionText_))
            {
                return kIABParserIABBedDefinitionError;
            }
        }

        // Read bed sub-element count
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABBedDefinitionError;
        }
        
        subElementCount_ = static_cast<IABElementCountType>(plex8Field);
        
        for (uint32_t i = 0; i < static_cast<uint32_t>(subElementCount_); i++)
        {
			errorCode = ParseBedSubElement(streamReader);

			if (errorCode != kIABNoError)
			{
				return errorCode;
			}
        }

		// If any sub elements have been skipped by the parser, deduct them from sub-element count read from the bitstream
		subElementCount_ -= numSkippedBedSubElementsInParsing_;

        if (streamReader.good())
        {
            return kIABNoError;
        }
        else
        {
            return kIABParserIABBedDefinitionError;
        }
	}
    
	// IABBedDefinition::ParseBedChannel() implementation
	iabError IABBedDefinition::ParseBedChannel(StreamReader& streamReader)
    {
		iabError errorCode = kIABNoError;
	
		IABChannel* bedChannel = nullptr;
        bedChannel = new IABChannel();

        if (nullptr == bedChannel)
        {
            return kIABMemoryError;
        }
        
		errorCode = bedChannel->DeSerialize(streamReader);

		if (errorCode != kIABNoError)
		{
			delete bedChannel;
			return errorCode;
		}

		// Save to channel list
		bedChannels_.push_back(bedChannel);

		return errorCode;
    }

	// IABBedDefinition::ParseBedSubElement() implementation
	iabError IABBedDefinition::ParseBedSubElement(StreamReader& streamReader)
    {
		uint32_t parsedElementIDField;
		IABElementIDType elementID;

		// Peek the next (Plex<8> coded) element/sub-element ID field
		if (kIABNoError != PeekNextElementID(streamReader, parsedElementIDField))
		{
			return kIABParserParsingError;
		}

		elementID = static_cast<IABElementIDType>(parsedElementIDField);

		IABElement* bedDefinitionSubElement = nullptr;

		switch (elementID)
		{
			case kIABElementID_BedDefinition:
				bedDefinitionSubElement = new IABBedDefinition(parentFrameRate_);
				break;

			case kIABElementID_BedRemap:
				bedDefinitionSubElement = new IABBedRemap(parentFrameRate_);
				break;

			// Known type but invalid type as IAB bed sub-element
			// Total number of unallowed element IDs for the bed is tracked in numUnallowedBedSubElements_.
			case kIABElementID_ObjectDefinition:
			case kIABElementID_ObjectZoneDefinition19:
			case kIABElementID_AuthoringToolInfo:
			case kIABElementID_UserData:
			case kIABElementID_AudioDataDLC:
			case kIABElementID_AudioDataPCM:
			case kIABElementID_IAFrame:
				SkipDeSerializeCurrentElement(*elementReader_);
				numSkippedBedSubElementsInParsing_++;
				numUnallowedBedSubElements_++;
				return kIABNoError;										// Exit, as bedDefinitionSubElement == nullptr for this case

			// Undefined/Unknown type as IAB BedDefinition sub-element
			// Total number of unallowed element IDs for the bed is tracked in numUndefinedBedSubElements_.
			default:
				SkipDeSerializeCurrentElement(*elementReader_);
				numSkippedBedSubElementsInParsing_++;
				numUndefinedBedSubElements_++;
				return kIABNoError;										// Exit, as bedDefinitionSubElement == nullptr for default case
		}

		// Check validity of bedDefinitionSubElement
		if (nullptr == bedDefinitionSubElement)
		{
			return kIABMemoryError;
		}

		// Parse sub-element
		iabError returnCode = kIABNoError;

		returnCode = bedDefinitionSubElement->DeSerialize(streamReader);

		if (returnCode != kIABNoError)
		{
			delete bedDefinitionSubElement;
			return returnCode;
		}

		bedSubElements_.push_back(bedDefinitionSubElement);				// Save to Bed Definition sub element list

		return kIABNoError;
	}
    
	// IABBedDefinition::UpdatePackEnabledSubElementCount() implementation
	void IABBedDefinition::UpdatePackingEnabledSubElementCount()
	{
		IABElementCountType numPackingEnabledElement = 0;

		for (std::vector<IABElement*>::iterator iter = bedSubElements_.begin(); iter != bedSubElements_.end(); iter++)
		{
			if ((*iter)->IsIncludedForPacking())
			{
				numPackingEnabledElement += 1;
			}
		}

		packedSubElementCount_ = numPackingEnabledElement;
	}

	// IABBedDefinition::GetNumUnallowedSubElements() implementation
	uint32_t IABBedDefinition::GetNumUnallowedSubElements() const
	{
		return numUnallowedBedSubElements_;
	}

	// IABBedDefinition::GetNumUndefinedSubElements() implementation
	uint32_t IABBedDefinition::GetNumUndefinedSubElements() const
	{
		return numUndefinedBedSubElements_;
	}

    // IABBedDefinition::Validate() implementation
    bool IABBedDefinition::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;
        
        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// Update iValidationIssue.id_ so that any issues reported from this Validate()
		// function is for BedDefinition element with metadataID_ value.
		//
		iValidationIssue.id_ = metadataID_;

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// Bed MetaID

		// General count var
		size_t foundCount = 0;
		
		// Bed bedUseCase_
		// Check only when conditionalBed_ is enabled
		if (conditionalBed_)
		{
			foundCount = ConstraintSet_ST2098_2_2018.validUseCases_.count(bedUseCase_);

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedUseCase;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

		// Channel count vs. channel list size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (channelCount_ != bedChannels_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorBedDefinitionChannelCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on all bed channels
        // Due to IABChannel->Validate its own validation for all constraint sets, this block will cover
        // IMF constraint sets.
		std::vector<IABChannel*>::const_iterator iterChannel;
		for (iterChannel = bedChannels_.begin(); iterChannel != bedChannels_.end(); iterChannel++)
		{
			if (isToContinue = (*iterChannel)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}
        
		// AudioDescription code
		// Check if reserved code 0x40 (bit 6) is on, using AND (& 0x40)
		foundCount = ConstraintSet_ST2098_2_2018.reservedAudioDescriptionCodes_.count(audioDescription_.audioDescription_ & 0x40);

		// If it is on, issue warning
		if (foundCount == 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedAudioDescription;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// AudioDescriptionText[n] length limit
		// Check this limit only if AudioDescription code has bit 7 on, by (& 0x80)
		if (audioDescription_.audioDescription_ & 0x80)
		{
			if (audioDescription_.audioDescriptionText_.length() > 64)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorBedDefinitionAudioDescriptionTextExceeded;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

		// Sub-element count vs. sub-element size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (subElementCount_ != bedSubElements_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorBedDefinitionSubElementCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on BedDefinition sub-elements
        // Due to sub-elements having their own validation for all constraint sets, this block will cover
        // IMF constraint sets as well.
		std::vector<IABElement*>::const_iterator iterBedSubElement;
		IABElementIDType elementID;

		for (iterBedSubElement = bedSubElements_.begin(); iterBedSubElement != bedSubElements_.end(); iterBedSubElement++)
		{
			// Retrieve elementID
			(*iterBedSubElement)->GetElementID(elementID);

			// Validate sub-elements
			//
			switch (elementID)
			{
			case kIABElementID_BedDefinition:
				isToContinue = dynamic_cast<IABBedDefinition*>(*iterBedSubElement)->Validate(iEvenHandler, iValidationIssue);
                if (!isToContinue)
                {
                    return isToContinue;
                }

				break;

			case kIABElementID_BedRemap:
				isToContinue = dynamic_cast<IABBedRemap*>(*iterBedSubElement)->Validate(iEvenHandler, iValidationIssue);
                
                if (!isToContinue)
                {
                    return isToContinue;
                }

				// Check if remap source channel count agrees with channel count of this bed
				IABChannelCountType remapSourceChannelCount;
				dynamic_cast<IABBedRemap*>(*iterBedSubElement)->GetSourceChannels(remapSourceChannelCount);

				if (remapSourceChannelCount != channelCount_)
				{
					// Update issue variable and log for Cinema ST2098_2_2018
					iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
					iValidationIssue.event_ = ErrorEvent;
					iValidationIssue.errorCode_ = kValidateErrorBedRemapSourceChannelCountNotEqualToBed;

					if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
					{
						return isToContinue;
					}

					// Sub-element processing should be run once only to avoid duplicate reporting
					// So, do sub-element IMF validation here as well
					// Update issue variable and log for IMF ST2098_2_2019
					iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
					iValidationIssue.event_ = ErrorEvent;
					iValidationIssue.errorCode_ = kValidateErrorBedRemapSourceChannelCountNotEqualToBed;

					if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
					{
						return isToContinue;
					}
				}

				// Register presence of IABBedRemap in bitstream as error for DbyCinema
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;                
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinBedRemapNotAnAllowedSubElement;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                
                // Remap is not an allowed element type for ST2067_201_2019
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorBedRemapNotAnAllowedSubElement;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                
				break;

			default:
				// Update issue variable and log for Cinema ST2098_2_2018
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningBedDefinitionContainUnsupportedSubElement;
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                    
                // Update issue variable and log for IMF ST2098_2_2019
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = WarningEvent;
                iValidationIssue.errorCode_ = kValidateWarningBedDefinitionContainUnsupportedSubElement;
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }

				break;
			}
            
			// Restore issue id_ here to parent level, as some of the sub-element validation
			// may have set the issue id_ during element-internal validation
			//
			iValidationIssue.id_ = metadataID_;

			// Update issue variable and log for DbyCinema
            iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorDolCinBedDefinitionSubElementsNotAllowed;
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }

            // Update issue variable and log for ST2067_201_2019
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionSubElementsNotAllowed;
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
		}

        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // ST429_18_2019 additional checks on IABBedDefinition :
        // 1. Invalid use cases : 0x30 - 0xFE inclusive

		// Restore issue id_ to parent bed
		//
		iValidationIssue.id_ = metadataID_;

        if ((bedUseCase_ >= ConstraintSet_ST429_18_2019.inValidUseCaseLowerBound_) &&
            (bedUseCase_ <= ConstraintSet_ST429_18_2019.inValidUseCaseUpperBound_))
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionInvalidUseCase;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }

        // Note that kConstraints_set_DbyCinema is a super set of ST-429-18-2019 and the preceding checks are required to validate
        // bitstream against DbyCinema
        
        // DbyCinema additional checks on IABBedDefinition :
        // 1. No sub-element (already checked above)
        // 2. Root level bed cannot be "true" conditional
        //
        if (conditionalBed_)
        {
            foundCount = ConstraintSet_DbyCinema.validUseCases_.count(bedUseCase_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinBedDefinitionInvalidUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################

        // Update iValidationIssue.id_ so that any issues reported from this Validate()
        // function is for BedDefinition element with metadataID_ value.
        //
        iValidationIssue.id_ = metadataID_;
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Bed MetaID
        
        // Bed bedUseCase_
        // Check only when conditionalBed_ is enabled
        if (conditionalBed_)
        {
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validUseCases_.count(bedUseCase_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = WarningEvent;
                iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // Channel count vs. channel list size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (channelCount_ != bedChannels_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionChannelCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // AudioDescription code
        // Check if reserved code 0x40 (bit 6) is on, using AND (& 0x40)
        foundCount = ConstraintSet_IMF_ST2098_2_2019.reservedAudioDescriptionCodes_.count(audioDescription_.audioDescription_ & 0x40);
        
        // If it is on, issue warning
        if (foundCount == 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = WarningEvent;
            iValidationIssue.errorCode_ = kValidateWarningBedDefinitionUndefinedAudioDescription;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // AudioDescriptionText[n] length limit
        // Check this limit only if AudioDescription code has bit 7 on, by (& 0x80)
        if (audioDescription_.audioDescription_ & 0x80)
        {
            if (audioDescription_.audioDescriptionText_.length() > 64)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorBedDefinitionAudioDescriptionTextExceeded;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                
            }
        }
        
        // Sub-element count vs. sub-element size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (subElementCount_ != bedSubElements_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedDefinitionSubElementCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // ST2098_2_2019 checks completed, change validation constriant set to kConstraints_set_IMF_ST2067_201_2019
        // Note that ST2067_201_2019 is a super set of ST2098_2_2019 and the preceding ST2098_2_2019 checks are required
        // to validate bitstream against ST2067_201_2019
        
        // Bed bedUseCase_
        // Check only when conditionalBed_ is enabled
        if (conditionalBed_)
        {
            // Check use case against ST2067_201_2019
            foundCount = ConstraintSet_IMF_ST2067_201_2019.validUseCases_.count(bedUseCase_);
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorBedDefinitionInvalidUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################


        return isToContinue;
    }

	// *****************************
	// IABBedRemapSubBlock implementation
	// *****************************

	// Constructor implementation
	IABBedRemapSubBlock::IABBedRemapSubBlock(uint16_t numberDestinaion, uint16_t numberSource) :
        remapInfoExists_(0),
        numberSources_(numberSource),
		numberDestinations_(numberDestinaion)
	{
		// Create numberDestinaion of IABRemapCoeff, each containing numberSource of
		// coefficients. All carries default values.
		// Client will need to update destination channel IDs and remap coefficients
		// to actual gain
		for (uint16_t i = 0; i < numberDestinations_; i++)
		{
			destRemapCoeffs_.push_back(new IABRemapCoeff(kIABChannelID_Center, numberSources_));
		}
	}

	// Destructor
	IABBedRemapSubBlock::~IABBedRemapSubBlock()
	{
		deleteRemapCoeffArray();
	}

	// IABBedRemapSubBlock::GetRemapInfoExists() implementation
	void IABBedRemapSubBlock::GetRemapInfoExists(uint1_t &oRemapInfoExists) const
	{
		oRemapInfoExists = remapInfoExists_;
	}

	// IABBedRemapSubBlock::SetRemapInfoExists() implementation
	iabError IABBedRemapSubBlock::SetRemapInfoExists(uint1_t iRemapInfoExists)
	{
		remapInfoExists_ = iRemapInfoExists;
		return kIABNoError;
	}

	// IABBedRemapSubBlock::GetRemapCoeffArray() implementation
	void IABBedRemapSubBlock::GetRemapCoeffArray(std::vector<IABRemapCoeff*> &oRemapCoeffArray) const
	{
		oRemapCoeffArray = destRemapCoeffs_;
	}

	// IABBedRemapSubBlock::SetRemapCoeffArray() implementation
	iabError IABBedRemapSubBlock::SetRemapCoeffArray(const std::vector<IABRemapCoeff*> iRemapCoeffArray)
	{
		iabError errorCode = kIABNoError;

		// Delete any existing array
		deleteRemapCoeffArray();

		destRemapCoeffs_ = iRemapCoeffArray;

		numberDestinations_ = static_cast<uint16_t>(destRemapCoeffs_.size());
		if (numberDestinations_ != 0)
		{
			numberSources_ = destRemapCoeffs_[0]->getRemapSourceNumber();
		}
		else
		{
			numberSources_ = 0;
		}

		// check
		errorCode = checkRemapCoeffArray();

		// If inconsistency, delete and report error.
		if (errorCode != kIABNoError)
		{
			deleteRemapCoeffArray();
			numberDestinations_ = 0;
			numberSources_ = 0;

			remapInfoExists_ = 0;

			return errorCode;
		}

		return kIABNoError;
	}

	// IABBedRemapSubBlock::deleteRemapCoeffArray() implementation
	void IABBedRemapSubBlock::deleteRemapCoeffArray()
	{
		for (std::vector<IABRemapCoeff*>::iterator iter = destRemapCoeffs_.begin(); iter != destRemapCoeffs_.end(); iter++)
		{
			delete *iter;
		}

		destRemapCoeffs_.clear();
	}

	// IABBedRemapSubBlock::checkRemapCoeffArray() implementation
	iabError IABBedRemapSubBlock::checkRemapCoeffArray()
	{
		// Check destination channel number against coefficient array size
		if (destRemapCoeffs_.size() != numberDestinations_)
		{
			return kIABGeneralError;
		}

		// Check source channel number consistency across all
		for (uint16_t i = 0; i < numberDestinations_; i++)
		{
			if (destRemapCoeffs_[i]->getRemapSourceNumber() != numberSources_)
			{
				return kIABGeneralError;
			}
		}

		if (0 == numberDestinations_)
		{
			std::cerr << "Remap block contains no destination channels." << std::endl;
			return kIABGeneralError;
		}

		if (0 == numberSources_)
		{
			std::cerr << "Remap block contains no source channels." << std::endl;
			return kIABGeneralError;
		}

		return kIABNoError;
	}

	// IABBedRemapSubBlock::GetRemapSourceNumber() implementation
	void IABBedRemapSubBlock::GetRemapSourceNumber(uint16_t &oSourceNumber) const
	{
		oSourceNumber = numberSources_;
	}

	// IABBedRemapSubBlock::GetRemapDestinationNumber() implementation
	void IABBedRemapSubBlock::GetRemapDestinationNumber(uint16_t &oDestinationNumber) const
	{
		oDestinationNumber = numberDestinations_;
	}

	// IABBedRemapSubBlock::Serialize() implementation
	iabError IABBedRemapSubBlock::Serialize(StreamWriter& streamWriter)
	{
		iabError returnCode = kIABNoError;

		// The payload stream writer for parent IABBedRemap element is passed in.
		// Individual remap sub-clock instances are packed into the payload buffer/ostream of  
		// IABBedRemap element, without alignment manipulation
		// ! Do not change alignment when writing individual remap sub blocks.

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		streamWriter.write(remapInfoExists_, 1);

		if (remapInfoExists_ == 1)
		{
			returnCode = SerializeSubBlock(streamWriter);
		}		// if (remapInfoExists_ == 1)

		return returnCode;
	}

	// IABBedRemapSubBlock::SerializeSubBlock() implementation
	iabError IABBedRemapSubBlock::SerializeSubBlock(StreamWriter& streamWriter)
	{
		iabError returnCode = kIABNoError;

		// The payload stream writer for parent IABBedRemap element is passed in.
		// Individual remap sub-clock instances are packed into the payload buffer/ostream of  
		// IABBedRemap element, without alignment manipulation
		// ! Do not change alignment when writing individual remap sub blocks.

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")

		// For first remap sub block, remapInfoExists_ is always 1 by specification. (This flag is not in bitstream.)

		// (optional, if not already performed by parent)

		returnCode = checkRemapCoeffArray();
		if (returnCode != kIABNoError)
		{
			return returnCode;												// early exit if check fails
		}

		// Writing remap coefficients specified for the block.
		for (uint16_t i = 0; i < numberDestinations_; i++)
		{
			write(streamWriter, Plex<4>(destRemapCoeffs_[i]->getDestinationChannelID()));

			for (uint16_t j = 0; j < numberSources_; j++)
			{
				IABGain remapCoefficient;
				IABGainPrefixType remapCoeffPrefix;

				returnCode = destRemapCoeffs_[i]->getRemapCoeff(remapCoefficient, j);
				if (returnCode != kIABNoError)
				{
					return returnCode;										// exit if check fails
				}

				remapCoeffPrefix = remapCoefficient.getIABGainPrefix();
				streamWriter.write(static_cast<uint8_t>(remapCoeffPrefix), 2);

				if (kIABGainPrefix_InStream == remapCoeffPrefix)
				{
					streamWriter.write(remapCoefficient.getIABGainInStreamValue(), 10);
				}
			}
		}

		return kIABNoError;
	}

	// IABBedRemapSubBlock::DeSerialize() implementation
	iabError IABBedRemapSubBlock::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;

		uint8_t fixedLengthField = 0;

		// Read 1-bit remapInfoExists_ from inputStream
		if (CMNSTRM_OK != streamReader.read(fixedLengthField, 1))
        {
            return kIABParserIABBedRemapSubBlockError;
        }
        
		remapInfoExists_ = static_cast<uint1_t>(fixedLengthField);

		if (remapInfoExists_ == 1)
		{
			errorCode = DeSerializeSubBlock(streamReader);
		}

		return errorCode;
	}

	// IABBedRemapSubBlock::DeSerializeSubBlock() implementation
	iabError IABBedRemapSubBlock::DeSerializeSubBlock(StreamReader& streamReader)
	{
		iabError returnCode = kIABNoError;

		uint8_t fixedLengthFieldMax8 = 0;
		uint16_t fixedLengthFieldMax16 = 0;
		Plex<4> plex4Field = 0;

		// During parsing, the parent BedRemap element is responsible for setting up
		// correct number of destination and source channel for remapping.
		// Internally, the channels numbers are used for parsing.
		// perform checking first
		returnCode = checkRemapCoeffArray();

		if (returnCode != kIABNoError)
		{
			return returnCode;												// early exit if check fails
		}

		// Parse
		for (uint16_t i = 0; i < numberDestinations_; i++)
		{
			// Read destination channel ID
			if (CMNSTRM_OK != read(streamReader, plex4Field))
            {
                return kIABParserIABBedRemapSubBlockError;
            }
            
			destRemapCoeffs_[i]->setDestinationChannelID(static_cast<IABChannelIDType>(static_cast<uint16_t>(plex4Field)));

			for (uint16_t j = 0; j < numberSources_; j++)
			{
				IABGain remapCoefficient;

				// Read 2-bit gain prefix
				if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
                {
                    return kIABParserIABBedRemapSubBlockError;
                }

				if (fixedLengthFieldMax8 == 0)
				{
					remapCoefficient.setIABGain(1.0f);
				}
				else if (fixedLengthFieldMax8 == 1)
				{
					remapCoefficient.setIABGain(0.0f);
				}
				else if (fixedLengthFieldMax8 == 2)
				{
					if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10)) // 10-bit remap gain coeff in bitstream
                    {
                        return kIABParserIABBedRemapSubBlockError;
                    }

                    returnCode = remapCoefficient.setIABGainFromStreamValue(fixedLengthFieldMax16);
					if (returnCode != kIABNoError)
					{
						return returnCode;										// exit if check fails
					}
				}
				else
				{
					return kIABParserIABBedRemapSubBlockError;
				}

				returnCode = destRemapCoeffs_[i]->setRemapCoeff(remapCoefficient, j);

				if (returnCode != kIABNoError)
				{
					return returnCode;										// exit if check fails
				}
			}
		}

		return kIABNoError;
	}
    
    // IABBedRemapSubBlock::Validate() implementation
    bool IABBedRemapSubBlock::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// General count var
		size_t foundCount = 0;

		// Run remap sub-block validation only when remapInfoExists_ is enabled
		if (remapInfoExists_)
		{
			// Check destination channel count against coefficient array size
			if (destRemapCoeffs_.size() != numberDestinations_)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorBedRemapDestinationChannelCountConflict;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			// Check each remap coefficient row
			for (uint16_t i = 0; i < numberDestinations_; i++)
			{
				// Check source channel count consistency
				if (destRemapCoeffs_[i]->getRemapSourceNumber() != numberSources_)
				{
					// Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
					iValidationIssue.event_ = ErrorEvent;
					iValidationIssue.errorCode_ = kValidateErrorBedRemapSourceChannelCountConflict;

					if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
					{
						return isToContinue;
					}
				}

				// DestinationChannelID
                IABChannelIDType destChannelID = kIABChannelID_Left;
                destChannelID = destRemapCoeffs_[i]->getDestinationChannelID();
				foundCount = ConstraintSet_ST2098_2_2018.validChannelIDs_.count(destChannelID);

				// If it is not uniquely found
				if (foundCount != 1)
				{
					// Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
					iValidationIssue.event_ = WarningEvent;
					iValidationIssue.errorCode_ = kValidateWarningBedRemapUndefinedChannelID;

					if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
					{
						return isToContinue;
					}
				}
                
                // ST429_18_2019 check on IABBedRemapSubBlock : Invalid destination channel IDs : any code > 0x7F
                if (destChannelID > ConstraintSet_ST429_18_2019.upperBoundForValidChannelID_)
                {
                    // Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
                    iValidationIssue.event_ = ErrorEvent;
                    iValidationIssue.errorCode_ = kValidateErrorBedRemapInvalidDestChannelID;
                    
                    if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                    {
                        return isToContinue;
                    }
                }

				// Check each remap coefficient in the row
				for (uint16_t j = 0; j < numberSources_; j++)
				{
					IABGain remapCoefficient;
					destRemapCoeffs_[i]->getRemapCoeff(remapCoefficient, j);

					foundCount = ConstraintSet_ST2098_2_2018.validGainPrefix_.count(remapCoefficient.getIABGainPrefix());

					// If it is not uniquely found
					if (foundCount != 1)
					{
						// Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
						iValidationIssue.event_ = ErrorEvent;
						iValidationIssue.errorCode_ = kValidateErrorDolCinBedRemapUnsupportedGainPrefix;

						if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
						{
							return isToContinue;
						}
					}
				}
			}
		}
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Run remap sub-block validation only when remapInfoExists_ is enabled
        if (remapInfoExists_)
        {
            // Check destination channel count against coefficient array size
            if (destRemapCoeffs_.size() != numberDestinations_)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorBedRemapDestinationChannelCountConflict;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            // Check each remap coefficient row
            for (uint16_t i = 0; i < numberDestinations_; i++)
            {
                // Check source channel count consistency
                if (destRemapCoeffs_[i]->getRemapSourceNumber() != numberSources_)
                {
                    // Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                    iValidationIssue.event_ = ErrorEvent;
                    iValidationIssue.errorCode_ = kValidateErrorBedRemapSourceChannelCountConflict;
                    
                    if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                    {
                        return isToContinue;
                    }
                }
                
                // DestinationChannelID
                IABChannelIDType destChannelID = kIABChannelID_Left;
                destChannelID = destRemapCoeffs_[i]->getDestinationChannelID();
                foundCount = ConstraintSet_IMF_ST2098_2_2019.validChannelIDs_.count(destChannelID);
                
                // If it is not uniquely found
                if (foundCount != 1)
                {
                    // Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                    iValidationIssue.event_ = WarningEvent;
                    iValidationIssue.errorCode_ = kValidateWarningBedRemapUndefinedChannelID;
                    
                    if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                    {
                        return isToContinue;
                    }
                }
                
                // Check each remap coefficient in the row
                for (uint16_t j = 0; j < numberSources_; j++)
                {
                    IABGain remapCoefficient;
                    destRemapCoeffs_[i]->getRemapCoeff(remapCoefficient, j);
                    
                    foundCount = ConstraintSet_IMF_ST2098_2_2019.validGainPrefix_.count(remapCoefficient.getIABGainPrefix());
                    
                    // If it is not uniquely found
                    if (foundCount != 1)
                    {
                        // Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                        iValidationIssue.event_ = ErrorEvent;
                        iValidationIssue.errorCode_ = kValidateErrorDolCinBedRemapUnsupportedGainPrefix;
                        
                        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                        {
                            return isToContinue;
                        }
                    }
                }
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

        
		return isToContinue;
	}

	// ****************************************************************************
	// IABBedRemap class implementation
	// ****************************************************************************

	// Constructor implementation 
	IABBedRemap::IABBedRemap(IABFrameRateType iFrameRate) :
		IABElement(kIABElementID_BedRemap),
		sourceChannels_(0),
		destinationChannels_(0)
	{
		metadataID_ = 0;
		remapUseCase_ = kIABUseCase_5_1;						// Default, must be updated by client
		numRemapSubBlocks_ = GetIABNumSubBlocks(iFrameRate);

		// bedRemapSubBlocks_ not initiated, as it is expected to be replaced.
	}

	// Constructor implementation
	IABBedRemap::IABBedRemap(uint16_t numberDestinaion, uint16_t numberSource, IABFrameRateType iFrameRate) :
        IABElement(kIABElementID_BedRemap),
        sourceChannels_(numberSource),
		destinationChannels_(numberDestinaion)
    {
		metadataID_ = 0;
		remapUseCase_ = kIABUseCase_5_1;						// Default, must be updated by client
		numRemapSubBlocks_ = GetIABNumSubBlocks(iFrameRate);

		for (uint8_t i = 0; i < numRemapSubBlocks_; i++)
		{
			bedRemapSubBlocks_.push_back(new IABBedRemapSubBlock(numberDestinaion, numberSource));
		}
	}

	// Destructor
	IABBedRemap::~IABBedRemap()
	{
		DeleteRemapSubBlocks();
	}

	// IABBedRemap::GetMetadataID() implementation
	void IABBedRemap::GetMetadataID(IABMetadataIDType &oMetaID) const
	{
		oMetaID = metadataID_;
	}

	// IABBedRemap::SetMetadataID() implementation
	iabError IABBedRemap::SetMetadataID(IABMetadataIDType iMetaID)
	{
		metadataID_ = iMetaID;
		return kIABNoError;
	}

	// IABBedRemap::GetRemapUseCase() implementation
	void IABBedRemap::GetRemapUseCase(IABUseCaseType &oRemapUseCase) const
	{
		oRemapUseCase = remapUseCase_;
	}

	// IABBedRemap::SetRemapUseCase() implementation
	iabError IABBedRemap::SetRemapUseCase(IABUseCaseType iRemapUseCase)
	{
		remapUseCase_ = iRemapUseCase;
		return kIABNoError;
	}

	// IABBedRemap::GetSourceChannels() implementation
	void IABBedRemap::GetSourceChannels(IABChannelCountType &oSourceChannels) const
	{
		oSourceChannels = sourceChannels_;
	}

	// IABBedRemap::GetDestinationChannels() implementation
	void IABBedRemap::GetDestinationChannels(IABChannelCountType &oDestinationChannels) const
	{
		oDestinationChannels = destinationChannels_;
	}

	// IABBedRemap::GetNumRemapSubBlocks() implementation
	void IABBedRemap::GetNumRemapSubBlocks(uint8_t &oNumRemapSubBlocks) const
	{
		oNumRemapSubBlocks = numRemapSubBlocks_;
	}

	// IABBedRemap::GetRemapSubBlocks() implementation
	void IABBedRemap::GetRemapSubBlocks(std::vector<IABBedRemapSubBlock*> &oRemapSubBlocks) const
	{
		oRemapSubBlocks = bedRemapSubBlocks_;
	}

	// IABBedRemap::SetRemapSubBlocks() implementation
	iabError IABBedRemap::SetRemapSubBlocks(const std::vector<IABBedRemapSubBlock*> iRemapSubBlocks)
	{
		DeleteRemapSubBlocks();

		numRemapSubBlocks_ = static_cast<uint8_t>(iRemapSubBlocks.size());

		// Checking if any of the pointers is a nullptr
		for (uint32_t i = 0; i < numRemapSubBlocks_; i++)
		{
			if (iRemapSubBlocks[i] == nullptr)
			{
				return kIABBadArgumentsError;
			}
		}

		bedRemapSubBlocks_ = iRemapSubBlocks;

		return kIABNoError;
	}

	// IABBedRemap::DeleteRemapSubBlocks() implementation
	void IABBedRemap::DeleteRemapSubBlocks()
	{
		for (std::vector<IABBedRemapSubBlock*>::iterator iter = bedRemapSubBlocks_.begin(); iter != bedRemapSubBlocks_.end(); iter++)
		{
			delete *iter;
		}

		bedRemapSubBlocks_.clear();
	}

	// IABBedRemap::EnablePacking() implementation
	void IABBedRemap::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABBedRemap::DisablePacking() implementation
	void IABBedRemap::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABBedRemap::IsIncludedForPacking() implementation
	bool IABBedRemap::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABBedRemap::Serialize() implementation
	iabError IABBedRemap::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		// Calling method for writing Plex encoded field.
		write(elementPayloadWriter_, Plex<8>(metadataID_));
		elementPayloadWriter_.write(static_cast<uint8_t>(remapUseCase_), 8);
		write(elementPayloadWriter_, Plex<4>(sourceChannels_));
		write(elementPayloadWriter_, Plex<4>(destinationChannels_));

		// Stop writing if any is 0
		if ((sourceChannels_ == 0) || (destinationChannels_ == 0) || (numRemapSubBlocks_ == 0))
		{
			return kIABBadArgumentsError;
		}

		// Serialize all remap sub blocks, in sequence, into payload writer

		// First block, skip "panInfoExists_" and always write panning parameters
		bedRemapSubBlocks_[0]->SerializeSubBlock(elementPayloadWriter_);

		for (uint8_t i = 1; i < numRemapSubBlocks_; i++)
		{
			bedRemapSubBlocks_[i]->Serialize(elementPayloadWriter_);
		}

		// Align to byte after writing remap sub blocks
		elementPayloadWriter_.align();										// Align to byte

		// Write "Reserved" which is hard set to "0" per specification (v4.34 of Dec 15, 2017), using Plex(8)
		// (This is a strange one, to write a hard coded "0" with Plex(8). Will send questo reflector.)
		// !! To be followed up
		write(elementPayloadWriter_, Plex<8>(0));

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABBedRemap::DeSerialize() implementation
	iabError IABBedRemap::DeSerialize(StreamReader& streamReader)
	{
        iabError errorCode = kIABNoError;
        
        errorCode = DeSerializeHead(streamReader);
        
        if (errorCode != kIABNoError)
        {
            return errorCode;
        }

		uint8_t	fixedLengthFieldMax8 = 0;
		Plex<8> plex8Field = 0;
		Plex<4> plex4Field = 0;

		// Read meta data ID
		if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABBedRemapError;
        }
        
		metadataID_ = static_cast<IABMetadataIDType>(plex8Field);

		// Read 8-bit remap use case
		if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8))
        {
            return kIABParserIABBedRemapError;
        }

        remapUseCase_ = static_cast<IABUseCaseType>(fixedLengthFieldMax8);

		// Read source channel count
		if (CMNSTRM_OK != read(streamReader, plex4Field))
        {
            return kIABParserIABBedRemapError;
        }

        sourceChannels_ = static_cast<IABChannelCountType>(plex4Field);

		// Read destination channel count
		if (CMNSTRM_OK != read(streamReader, plex4Field))
        {
            return kIABParserIABBedRemapError;
        }

        destinationChannels_ = static_cast<IABChannelCountType>(plex4Field);

		// Stop parsing if sourceChannels_ or destinationChannels_, is greater than ...
		if ((sourceChannels_ == 0) || 
			(destinationChannels_ == 0))
		{
			return kIABParserIABBedRemapError;
		}

		// Cear any existent remap sub blocks
		if (bedRemapSubBlocks_.size() > 0)
		{
			DeleteRemapSubBlocks();
		}

		// Read panning subblocks
		for (uint8_t i = 0; i < numRemapSubBlocks_; i++)
		{
			IABBedRemapSubBlock* remapSubBlock = new IABBedRemapSubBlock(destinationChannels_, sourceChannels_);

			if (nullptr == remapSubBlock)
			{
				return kIABMemoryError;
			}

			if (0 == i)
			{
				errorCode = remapSubBlock->DeSerializeSubBlock(streamReader);
				remapSubBlock->SetRemapInfoExists(1);
			}
			else
			{
				errorCode = remapSubBlock->DeSerialize(streamReader);
			}

			if (errorCode != kIABNoError)
			{
                delete remapSubBlock;
				return errorCode;
			}

			if (streamReader.good())
			{
				bedRemapSubBlocks_.push_back(remapSubBlock);
			}
			else
			{
				delete remapSubBlock;
				return kIABParserIABBedRemapError;
			}
		}

		if (CMNSTRM_OK != streamReader.align())  // byte alignment
        {
            return kIABParserIABBedRemapError;
        }

		// Read "Reserved" using plex decoding coding. This is expected to be 0 per v4.34 IAB spec.
		// The parsed data is not saved, as it is not used or useful in any decoding. "Reading" serves
		// to move the reader position.
		if ((CMNSTRM_OK == read(streamReader, plex8Field)) && streamReader.good())
        {
            return kIABNoError;
        }
		else 
		{
			return kIABParserIABBedRemapError;
		}
	}

    // IABBedRemap::Validate() implementation
    bool IABBedRemap::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// Update iValidationIssue.id_ so that any issues reported from this Validate()
		// function is for BedRemap element with metadataID_ value.
		//
		iValidationIssue.id_ = metadataID_;

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// BedRemap MetaID

		// General count var
		size_t foundCount = 0;

		// remapUseCase_
		foundCount = ConstraintSet_ST2098_2_2018.validUseCases_.count(remapUseCase_);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningBedRemapUndefinedUseCase;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// sourceChannels_: to be checked in IABBedRemapSubBlock::Validate().
		// destinationChannels_: to be checked in IABBedRemapSubBlock::Validate().

		// Remap sub-block count vs. remap sub-block list size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (numRemapSubBlocks_ != bedRemapSubBlocks_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorBedRemapSubblockCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on all pan sub-blocks
		std::vector<IABBedRemapSubBlock*>::const_iterator iterRemapSubblock;
		for (iterRemapSubblock = bedRemapSubBlocks_.begin(); iterRemapSubblock != bedRemapSubBlocks_.end(); iterRemapSubblock++)
		{
			if (isToContinue = (*iterRemapSubblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // ST429_18_2019 additional checks on IABBedRemap :
        // 1. Invaid use cases : 0x30 to 0xFE inclusive
        
        if ((remapUseCase_ >= ConstraintSet_ST429_18_2019.inValidUseCaseLowerBound_) &&
            (remapUseCase_ <= ConstraintSet_ST429_18_2019.inValidUseCaseUpperBound_))
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedRemapInvalidUseCase;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }

        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // Update iValidationIssue.id_ so that any issues reported from this Validate()
        // function is for BedRemap element with metadataID_ value.
        //
        iValidationIssue.id_ = metadataID_;
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // BedRemap MetaID
        
        // remapUseCase_
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validUseCases_.count(remapUseCase_);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = WarningEvent;
            iValidationIssue.errorCode_ = kValidateWarningBedRemapUndefinedUseCase;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // sourceChannels_: to be checked in IABBedRemapSubBlock::Validate().
        // destinationChannels_: to be checked in IABBedRemapSubBlock::Validate().
        
        // Remap sub-block count vs. remap sub-block list size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (numRemapSubBlocks_ != bedRemapSubBlocks_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorBedRemapSubblockCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Run Validate() on all pan sub-blocks
        for (iterRemapSubblock = bedRemapSubBlocks_.begin(); iterRemapSubblock != bedRemapSubBlocks_.end(); iterRemapSubblock++)
        {
            if (isToContinue = (*iterRemapSubblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

        
		return isToContinue;
	}

    // ****************************************************************************
    // IABObjectSubBlock class implementation
    // ****************************************************************************

    // Constructor
    IABObjectSubBlock::IABObjectSubBlock()
    {
        panInfoExists_ = 0;
        
        objectGain_.setIABGain(1.0f);  // Gain = 1.0f;
        
        objectDecorCoef_.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
        objectDecorCoef_.decorCoef_ = 0;
        
		objectPosition_.setIABObjectPosition(0.0f, 0.0f, 0.0f);				// Init. (These should already be defaults upon construction.)
        
        objectSnap_.objectSnapPresent_ = 0;
        objectSnap_.objectSnapTolExists_ = 0;
        objectSnap_.objectSnapTolerance_ = 0;
        objectSnap_.reservedBit_ = 0;
        
        zoneGain_.objectZoneControl_ = 0;

        objectSpread_.setIABObjectSpread(kIABSpreadMode_None,  0.0f, 0.0f, 0.0f);
    }
    
    // Destructor
    IABObjectSubBlock::~IABObjectSubBlock()
    {
    }
    
    // IABObjectSubBlock::GetPanInfoExists() implementation
    void IABObjectSubBlock::GetPanInfoExists(uint1_t &oPanInfoExists) const
    {
        oPanInfoExists = panInfoExists_;
    }
    
    // IABObjectSubBlock::SetPanInfoExists() implementation
    iabError IABObjectSubBlock::SetPanInfoExists(uint1_t iPanInfoExists)
    {
        panInfoExists_ = iPanInfoExists;
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetObjectGain() implementation
    void IABObjectSubBlock::GetObjectGain(IABGain &oObjectGain) const
    {
        oObjectGain = objectGain_;
    }
    
    // IABObjectSubBlock::SetObjectGain() implementation
    iabError IABObjectSubBlock::SetObjectGain(IABGain const& iObjectGain)
    {
        objectGain_ = iObjectGain;
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetObjectPositionToUnitCube() implementation
    void IABObjectSubBlock::GetObjectPositionToUnitCube(CartesianPosInUnitCube &oObjectPositionUC) const
    {
		oObjectPositionUC = objectPosition_;
    }
    
    // IABObjectSubBlock::SetObjectPositionFromUnitCube() implementation
    iabError IABObjectSubBlock::SetObjectPositionFromUnitCube(CartesianPosInUnitCube const& iObjectPositionUC)
    {
		objectPosition_ = iObjectPositionUC;
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetObjectSnap() implementation
    void IABObjectSubBlock::GetObjectSnap(IABObjectSnap &oObjectSnap) const
    {
        oObjectSnap = objectSnap_;
    }
    
    // IABObjectSubBlock::SetObjectSnap() implementation
    iabError IABObjectSubBlock::SetObjectSnap(IABObjectSnap const& iObjectSnap)
    {
        objectSnap_ = iObjectSnap;
        if (0 == objectSnap_.objectSnapPresent_)
        {
            objectSnap_.objectSnapTolExists_ = 0;
            objectSnap_.objectSnapTolerance_ = 0;
        }
        else if (0 == objectSnap_.objectSnapTolExists_)
        {
            objectSnap_.objectSnapTolerance_ = 0;
        }
        
        // reservedBit_ is always zero
        objectSnap_.reservedBit_ = 0;
        
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetObjectZoneGains9() implementation
    void IABObjectSubBlock::GetObjectZoneGains9(IABObjectZoneGain9 &oObjectZoneGains9) const
    {
        oObjectZoneGains9 = zoneGain_;
    }
    
    // IABObjectSubBlock::SetObjectZoneGains9() implementation
    iabError IABObjectSubBlock::SetObjectZoneGains9(IABObjectZoneGain9 const& iObjectZoneGains9)
    {
        zoneGain_ = iObjectZoneGains9;
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetObjectSpread() implementation
    void IABObjectSubBlock::GetObjectSpread(IABObjectSpread &oObjectSpread) const
    {
        oObjectSpread = objectSpread_;
    }
    
    // IABObjectSubBlock::SetObjectSpread() implementation
    iabError IABObjectSubBlock::SetObjectSpread(IABObjectSpread const& iObjectSpread)
    {
        objectSpread_ = iObjectSpread;
        
        return kIABNoError;
    }
    
    // IABObjectSubBlock::GetDecorCoef() implementation
    void IABObjectSubBlock::GetDecorCoef(IABDecorCoeff &oDecorCoef) const
    {
        oDecorCoef = objectDecorCoef_;
    }
    
    // IABObjectSubBlock::SetDecorCoef() implementation
    iabError IABObjectSubBlock::SetDecorCoef(IABDecorCoeff const& iDecorCoef)
    {
        objectDecorCoef_ = iDecorCoef;
        return kIABNoError;
    }

	// IABObjectSubBlock::Serialize() implementation
	iabError IABObjectSubBlock::Serialize(StreamWriter& streamWriter)
	{
		iabError returnCode = kIABNoError;

		// The payload stream writer for parent ObjectDefinition element is passed in.
		// Individual sub-clock instances are packed into the payload buffer/ostream of  
		// ObjectDefinition element, without alignment manipulation
		// ! Do not change alignment when writing individual sub blocks.

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		streamWriter.write(panInfoExists_, 1);

		if (panInfoExists_ == 1)
		{
			returnCode = SerializeSubBlock(streamWriter);
		}		// if (panInfoExists_ == 1)

		return returnCode;
	}

	// IABObjectSubBlock::SerializeSubBlock() implementation
	iabError IABObjectSubBlock::SerializeSubBlock(StreamWriter& streamWriter)
	{
		// The payload stream writer for parent ObjectDefinition element is passed in.
		// Individual sub-clock instances are packed into the payload buffer/ostream of  
		// ObjectDefinition element, without alignment manipulation
		// ! Do not change alignment when writing individual sub blocks.

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")

		// For first pan sub block, panInfoExists_ is always 1 by specification. It is not in bitstream.
		// Writing object panning parameters specified for the block.

		streamWriter.write(static_cast<uint8_t>(objectGain_.getIABGainPrefix()), 2);
		if (kIABGainPrefix_InStream == objectGain_.getIABGainPrefix())
		{
			streamWriter.write(objectGain_.getIABGainInStreamValue(), 10);
		}

		streamWriter.write(reserved1_, 3);									// A 3-bit reserved field

																			// Write object position
		streamWriter.write(objectPosition_.getStreamPosX(), 16);			// x
		streamWriter.write(objectPosition_.getStreamPosY(), 16);			// y
		streamWriter.write(objectPosition_.getStreamPosZ(), 16);			// z

																			// ObjectSnap
		streamWriter.write(objectSnap_.objectSnapPresent_, 1);

		if (objectSnap_.objectSnapPresent_ == 1)
		{
			streamWriter.write(objectSnap_.objectSnapTolExists_, 1);

			if (objectSnap_.objectSnapTolExists_ == 1)
			{
				streamWriter.write(objectSnap_.objectSnapTolerance_, 12);
			}

			streamWriter.write(objectSnap_.reservedBit_, 1);
		}

		// ObjectZoneControl
		streamWriter.write(zoneGain_.objectZoneControl_, 1);

		if (zoneGain_.objectZoneControl_ == 1)
		{
			for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
			{
				streamWriter.write(static_cast<uint8_t>(zoneGain_.zoneGains_[i].getIABZoneGainPrefix()), 2);
				if (kIABZoneGainPrefix_InStream == zoneGain_.zoneGains_[i].getIABZoneGainPrefix())
				{
					streamWriter.write(zoneGain_.zoneGains_[i].getIABZoneGainInStreamValue(), 10);
				}
			}
		}

		// ObjectSpreadMode
		streamWriter.write(static_cast<uint8_t>(objectSpread_.getIABObjectSpreadMode()), 2);

		if (objectSpread_.getIABObjectSpreadMode() == kIABSpreadMode_LowResolution_1D)
		{
			streamWriter.write(objectSpread_.getStreamSpreadXYZ(), 8);
		}
		else if (objectSpread_.getIABObjectSpreadMode() == kIABSpreadMode_None)
		{
			// Nothing goes into stream for kIABSpreadMode_None
		}
		else if (objectSpread_.getIABObjectSpreadMode() == kIABSpreadMode_HighResolution_1D)
		{
			streamWriter.write(objectSpread_.getStreamSpreadXYZ(), 12);
		}
		else if (objectSpread_.getIABObjectSpreadMode() == kIABSpreadMode_HighResolution_3D)
		{
			streamWriter.write(objectSpread_.getStreamSpreadXYZ(), 12);
			streamWriter.write(objectSpread_.getStreamSpreadY(), 12);
			streamWriter.write(objectSpread_.getStreamSpreadZ(), 12);
		}
		else
		{
			return kIABPackerObjectSpreadModeError;					// unknownn spread mode code
		}

		streamWriter.write(reserved2_, 4);	// A 4-bit reserved field

		// ObjectDecorCoefPrefix
		streamWriter.write(static_cast<uint8_t>(objectDecorCoef_.decorCoefPrefix_), 2);

		// Enforce a more strict checking than 
		// if (objectDecorCoef_.decorCoefPrefix_ > kIABDecorCoeffPrefix_MaxDecor)?
		if (objectDecorCoef_.decorCoefPrefix_ == kIABDecorCoeffPrefix_DecorCoeffInStream)
		{
			streamWriter.write(objectDecorCoef_.decorCoef_, 8);
		}

		return kIABNoError;
	}

	// IABObjectSubBlock::DeSerialize() implementation
	iabError IABObjectSubBlock::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;
        uint8_t fixedLengthField = 0;

		// Read 1-bit panInfoExists_ from inputStream
        if (CMNSTRM_OK != streamReader.read(fixedLengthField, 1))
        {
            return kIABParserIABObjectSubBlockError;
        }
        
        panInfoExists_ = static_cast<uint1_t>(fixedLengthField);

		if (panInfoExists_ == 1)
		{
			errorCode = DeSerializeSubBlock(streamReader);
		}

		return errorCode;
	}

	// IABObjectSubBlock::DeSerializeSubBlock() implementation
	iabError IABObjectSubBlock::DeSerializeSubBlock(StreamReader& streamReader)
	{
        uint8_t fixedLengthFieldMax8 = 0;
        uint16_t fixedLengthFieldMax16 = 0;
        uint16_t posX = 0;
        uint16_t posY = 0;
        uint16_t posZ = 0;
        
        // Read 2-bit gain prefix
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABObjectSubBlockError;
        }
        
        if (fixedLengthFieldMax8 == 0)
        {
            objectGain_.setIABGain(1.0f);
        }
        else if (fixedLengthFieldMax8 == 1)
        {
            objectGain_.setIABGain(0.0f);
        }
        else if (fixedLengthFieldMax8 == 2)
        {
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10)) // 10-bit gain in bitstream
            {
                return kIABParserIABObjectSubBlockError;
            }

            objectGain_.setIABGainFromStreamValue(fixedLengthFieldMax16);
        }
        else
        {
            return kIABParserIABObjectSubBlockError;
        }
        
        // Read 3-bit reserved field
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 3))
        {
            return kIABParserIABObjectSubBlockError;
        }

        // Read 16-bit position code for X, Y, Z in that order
        if (CMNSTRM_OK != streamReader.read(posX))
        {
            return kIABParserIABObjectSubBlockError;
        }

        if (CMNSTRM_OK != streamReader.read(posY))
        {
            return kIABParserIABObjectSubBlockError;
        }

        if (CMNSTRM_OK != streamReader.read(posZ))
        {
            return kIABParserIABObjectSubBlockError;
        }

        objectPosition_.setIABObjectPosition(posX, posY, posZ);
        
        // Read 1-bit snap present flag
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABObjectSubBlockError;
        }

        objectSnap_.objectSnapPresent_ = static_cast<uint1_t>(fixedLengthFieldMax8);
        
        if (1 == objectSnap_.objectSnapPresent_)
        {
            // Read 1-bit snap to exist flag
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
            {
                return kIABParserIABObjectSubBlockError;
            }

            objectSnap_.objectSnapTolExists_ = static_cast<uint1_t>(fixedLengthFieldMax8);

            if (1 == objectSnap_.objectSnapTolExists_)
            {
                // Read 12-bit snap tolerance
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 12))
                {
                    return kIABParserIABObjectSubBlockError;
                }

                objectSnap_.objectSnapTolerance_ = fixedLengthFieldMax16;
            }
            else
            {
                // Not given in bitstream, set snap tolerance to default value 
                objectSnap_.objectSnapTolerance_ = kIABDefaultObjectSnapTolerance;
            }
            
            // Read 1-bit reserved field
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
            {
                return kIABParserIABObjectSubBlockError;
            }
        }
        
        // Object Zone
        
        // Read 1-bit zone coontrol
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABObjectSubBlockError;
        }

        zoneGain_.objectZoneControl_ = fixedLengthFieldMax8;

        if (1 == zoneGain_.objectZoneControl_)
        {
            for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
            {
				// Read zone gain prefix
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
                {
                    return kIABParserIABObjectSubBlockError;
                }
                
                if (fixedLengthFieldMax8 == 0)
                {
                    // Silence gain
                    zoneGain_.zoneGains_[i].setIABZoneGain(0.0f);
                }
                else if (fixedLengthFieldMax8 == 1)
                {
                    // unity gain
                    zoneGain_.zoneGains_[i].setIABZoneGain(1.0f);
                }
                else if (fixedLengthFieldMax8 == 2)
                {
                    if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10)) // 10-bit gain in bitstream
                    {
                        return kIABParserIABObjectSubBlockError;
                    }

                    zoneGain_.zoneGains_[i].setIABZoneGainFromStreamValue(fixedLengthFieldMax16);
                }
                else
                {
                    return kIABParserIABObjectSubBlockError;
                }
            }
        }
       
        // Object spread

        // Read 2-bit spread mode
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABObjectSubBlockError;
        }
        
        if (fixedLengthFieldMax8 == 0)
        {
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 8)) // read a single 8-bit spread value in bitstream
            {
                return kIABParserIABObjectSubBlockError;
            }

            objectSpread_.setIABObjectSpread(kIABSpreadMode_LowResolution_1D, fixedLengthFieldMax16, fixedLengthFieldMax16, fixedLengthFieldMax16);
        }
		else if (fixedLengthFieldMax8 == 1)
		{
			// Point source, no ObjectSpread value is sent in bitstream
			fixedLengthFieldMax16 = 0;									// Hard code spread to 0, not reading from bitstream
			objectSpread_.setIABObjectSpread(kIABSpreadMode_None, fixedLengthFieldMax16, fixedLengthFieldMax16, fixedLengthFieldMax16);
		}
		else if (fixedLengthFieldMax8 == 2)
        {
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 12)) // read a single 12-bit spread value in bitstream
            {
                return kIABParserIABObjectSubBlockError;
            }

            objectSpread_.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, fixedLengthFieldMax16, fixedLengthFieldMax16, fixedLengthFieldMax16);
        }
        else if (fixedLengthFieldMax8 == 3)
        {
            uint16_t fixedLengthFieldMax16_2 = 0;
            uint16_t fixedLengthFieldMax16_3 = 0;
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 12)) // read single 12-bit spread value for x
            {
                return kIABParserIABObjectSubBlockError;
            }

            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16_2, 12))	// read single 12-bit spread value for y
            {
                return kIABParserIABObjectSubBlockError;
            }

            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16_3, 12))	// read single 12-bit spread value for z
            {
                return kIABParserIABObjectSubBlockError;
            }

            objectSpread_.setIABObjectSpread(kIABSpreadMode_HighResolution_3D, fixedLengthFieldMax16, fixedLengthFieldMax16_2, fixedLengthFieldMax16_3);
        }
        else
        {
            return kIABParserIABObjectSubBlockError;  // unknownn spread mode code
        }

        // Read 4-bit reserved field
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 4))
        {
            return kIABParserIABObjectSubBlockError;
        }

        // Read 2-bit channel decor prefix
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABObjectSubBlockError;
        }
        
        if (fixedLengthFieldMax8 <= 2)
        {
            objectDecorCoef_.decorCoefPrefix_ = static_cast<IABDecorCoeffPrefixType>(fixedLengthFieldMax8);
            objectDecorCoef_.decorCoef_ = 0;

            if (2 == fixedLengthFieldMax8)
            {
                // Read 8-bit channel decor value
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8))
                {
                    return kIABParserIABObjectSubBlockError;
                }

                objectDecorCoef_.decorCoef_ = fixedLengthFieldMax8;
            }
        }
        else
        {
            return kIABParserIABObjectSubBlockError;
        }

		return kIABNoError;
	}

    // IABObjectSubBlock::Validate() implementation
    bool IABObjectSubBlock::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// General count var
		size_t foundCount = 0;

		// Run pan sub-block validation only when panInfoExists_ is enabled
		if (panInfoExists_)
		{
			// Object gain prefix code
			foundCount = ConstraintSet_ST2098_2_2018.validGainPrefix_.count(objectGain_.getIABGainPrefix());

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedGainPrefix;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			// No checking for object positions

			// No checking for object snap

			// Object zone gain prefix
			// Run zone gain prefix validation check only when zoneGain_.objectZoneControl_ is enabled
			if (zoneGain_.objectZoneControl_)
			{
				for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
				{
					foundCount = ConstraintSet_ST2098_2_2018.validZoneGainPrefix_.count(zoneGain_.zoneGains_[i].getIABZoneGainPrefix());

					// If it is not uniquely found
					if (foundCount != 1)
					{
						// Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
						iValidationIssue.event_ = ErrorEvent;
						iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedZoneGainPrefix;

						if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
						{
							return isToContinue;
						}
					}
				}
			}

			// Object spread mode code
			foundCount = ConstraintSet_ST2098_2_2018.validObjectSpreadModes_.count(objectSpread_.getIABObjectSpreadMode());

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedSpreadMode;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			// Object decorrelation coefficient prefix code
			foundCount = ConstraintSet_ST2098_2_2018.validDecorCoeffPrefix_.count(objectDecorCoef_.decorCoefPrefix_);

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedDecorPrefix;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
            
            // Note: No additional checks for ST-429-18-2019 constraint set
            
            // Note that kConstraints_set_DbyCinema is a super set of ST-429-18-2019 and the preceding checks are required to validate
            // bitstream against DbyCinema
            
            // DbyCinema additional checks on IABObjectSubBlock :
            // 1. ObjectGainPrefix = 0x00
            // 2. ZoneGainPrefix = 0x00 or 0x01
            // 3. Warning if ZoneGainPrefix are not set according as Dby recommended
            // 4. Spread mode = 0x02
            // 5. Decor coeff prefix = 0x00 or 0x01
            // 6. ObjectSnapTolExists = 0
            
            // Object gain prefix code
            foundCount = ConstraintSet_DbyCinema.validGainPrefix_.count(objectGain_.getIABGainPrefix());
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionInvalidGainPrefix;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            // Object zone gain prefix
            // Run zone gain prefix validation check only when zoneGain_.objectZoneControl_ is enabled
            IABZoneGainPrefixType zoneGainPrefix;
            
            if (zoneGain_.objectZoneControl_)
            {
                bool allGainPrefixValid = true;
                
                for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
                {
                    zoneGainPrefix = zoneGain_.zoneGains_[i].getIABZoneGainPrefix();
                    
                    foundCount = ConstraintSet_DbyCinema.validZoneGainPrefix_.count(zoneGainPrefix);
                    
                    // If it is not uniquely found
                    if (foundCount != 1)
                    {
                        // Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                        iValidationIssue.event_ = ErrorEvent;
                        iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionInvalidZoneGainPrefix;
                        
                        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                        {
                            return isToContinue;
                        }
                        
                        allGainPrefixValid = false;
                    }
                }
                
                // If all zone gains are valid, additionally check against Dby recommended presets
                // Issue warning if zone gains do not match any one of the 11 sets exactly
                if (allGainPrefixValid)
                {
					IABZoneGainPrefixType currentZoneGainPrefixSet[9];
					bool foundMatchingSet = false;

					// Retrieve current zone9 gain prefix set for validation
					for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
					{
						currentZoneGainPrefixSet[i] = zoneGain_.zoneGains_[i].getIABZoneGainPrefix();
					}

					bool FoundUnmatchedGainPrefix = false;

					// Check against eact set in turn
					for (uint32_t preset = 0; preset < 12; preset++)
					{
						// Starting as matched, until 1 unmatched found 
						FoundUnmatchedGainPrefix = false;

						for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
						{
							if (currentZoneGainPrefixSet[i] != ConstraintSet_DbyIMF.dbyIMFZoneGainPresets_[preset][i])
							{
								FoundUnmatchedGainPrefix = true;
								break;							// break for(i) loop when encountering first unmatched
							}
						}

						if (FoundUnmatchedGainPrefix == false)
						{
							foundMatchingSet = true;
							break;								// break for(preset) loop as soon as a preset is matched
						}
					}

                    if (!foundMatchingSet)
                    {
                        // Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                        iValidationIssue.event_ = ErrorEvent;
                        iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionZoneGainsNotAPreset;
                        
                        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                        {
                            return isToContinue;
                        }
                    }
                }
            }
            
            // Object spread mode code
            foundCount = ConstraintSet_DbyCinema.validObjectSpreadModes_.count(objectSpread_.getIABObjectSpreadMode());
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionInvalidSpreadMode;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            // Object decorrelation coefficient prefix code
            foundCount = ConstraintSet_DbyCinema.validDecorCoeffPrefix_.count(objectDecorCoef_.decorCoefPrefix_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionInvalidDecorPrefix;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            if (objectSnap_.objectSnapPresent_ && objectSnap_.objectSnapTolExists_ != 0)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionSnapTolExistsNotZero;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
		}

        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Run pan sub-block validation only when panInfoExists_ is enabled
        if (panInfoExists_)
        {
            // Object gain prefix code
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validGainPrefix_.count(objectGain_.getIABGainPrefix());
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedGainPrefix;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            // No checking for object positions
            
            // No checking for object snap
            
            // Object zone gain prefix
            // Run zone gain prefix validation check only when zoneGain_.objectZoneControl_ is enabled
            if (zoneGain_.objectZoneControl_)
            {
                for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
                {
                    foundCount = ConstraintSet_IMF_ST2098_2_2019.validZoneGainPrefix_.count(zoneGain_.zoneGains_[i].getIABZoneGainPrefix());
                    
                    // If it is not uniquely found
                    if (foundCount != 1)
                    {
                        // Update issue variable and log
                        iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                        iValidationIssue.event_ = ErrorEvent;
                        iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedZoneGainPrefix;
                        
                        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                        {
                            return isToContinue;
                        }
                    }
                }
            }
            
            // Object spread mode code
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validObjectSpreadModes_.count(objectSpread_.getIABObjectSpreadMode());
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedSpreadMode;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
            
            // Object decorrelation coefficient prefix code
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validDecorCoeffPrefix_.count(objectDecorCoef_.decorCoefPrefix_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionUnsupportedDecorPrefix;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }

			// Note: No additional checks for ST2067-201-2019 constraint set

			// Note that kConstraints_set_DbyIMF is a super set of ST2067-201-2019 and the preceding IMF
			// checks are required to validate bitstream against DbyIMF.

			// DbyIMF additional checks on IABObjectSubBlock :
			// 1. ObjectGainPrefix = 0x0 or 0x1
			// 2. ZoneGainPrefix = 0x0 or 0x1
			// 3. Warning if ZoneGainPrefix are not set according as DbyIMF recommended presets
			// 4. Spread mode = 0x02
			// 5. Decor coeff prefix = 0x0 or 0x1
			// 6. ObjectSnapTolExists = 0

			// Object gain prefix code
			foundCount = ConstraintSet_DbyIMF.validObjectGainPrefix_.count(objectGain_.getIABGainPrefix());

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
				iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorDolIMFObjectDefinitionInvalidGainPrefix;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			// Object zone gain prefix
			// Run zone gain prefix validation check only when zoneGain_.objectZoneControl_ is enabled
			IABZoneGainPrefixType zoneGainPrefix;

			if (zoneGain_.objectZoneControl_)
			{
				bool allGainPrefixValid = true;

				for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
				{
					zoneGainPrefix = zoneGain_.zoneGains_[i].getIABZoneGainPrefix();

					foundCount = ConstraintSet_DbyIMF.validZoneGainPrefix_.count(zoneGainPrefix);

					// If it is not uniquely found
					if (foundCount != 1)
					{
						// Update issue variable and log
						iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
						iValidationIssue.event_ = ErrorEvent;
						iValidationIssue.errorCode_ = kValidateErrorDolIMFObjectDefinitionInvalidZoneGainPrefix;

						if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
						{
							return isToContinue;
						}

						allGainPrefixValid = false;
					}
				}

				// If all zone gains are valid, additionally check against Dby recommended presets
				// Issue warning if zone gains do not match any one of the 12 sets exactly
				if (allGainPrefixValid)
				{
					IABZoneGainPrefixType currentZoneGainPrefixSet[9];
					bool foundMatchingSet = false;

					// Retrieve current zone9 gain prefix set for validation
					for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
					{
						currentZoneGainPrefixSet[i] = zoneGain_.zoneGains_[i].getIABZoneGainPrefix();
					}

					bool FoundUnmatchedGainPrefix = false;

					// Check against eact set in turn
					for (uint32_t preset = 0; preset < 12; preset++)
					{
						// Starting as matched, until 1 unmatched found 
						FoundUnmatchedGainPrefix = false;

						for (uint32_t i = 0; i < kIABObjectZoneCount9; i++)
						{
							if (currentZoneGainPrefixSet[i] != ConstraintSet_DbyIMF.dbyIMFZoneGainPresets_[preset][i])
							{
								FoundUnmatchedGainPrefix = true;
								break;							// break for(i) loop when encountering first unmatched
							}
						}

						if (FoundUnmatchedGainPrefix == false)
						{
							foundMatchingSet = true;
							break;								// break for(preset) loop as soon as a preset is matched
						}
					}

					if (!foundMatchingSet)
					{
						// Update issue variable and log
						iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
						iValidationIssue.event_ = WarningEvent;
						iValidationIssue.errorCode_ = kValidateWarningDolIMFObjectDefinitionZoneGainsNotAPreset;

						if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
						{
							return isToContinue;
						}
					}
				}
			}

			// Object spread mode code
			foundCount = ConstraintSet_DbyIMF.validObjectSpreadModes_.count(objectSpread_.getIABObjectSpreadMode());

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
				iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorDolIMFObjectDefinitionInvalidSpreadMode;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			// Object decorrelation coefficient prefix code
			foundCount = ConstraintSet_DbyIMF.validDecorCoeffPrefix_.count(objectDecorCoef_.decorCoefPrefix_);

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
				iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorDolIMFObjectDefinitionInvalidDecorPrefix;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}

			if (objectSnap_.objectSnapPresent_ && objectSnap_.objectSnapTolExists_ != 0)
			{
				// Update issue variable and log
				iValidationIssue.isBeingValidated_ = kConstraints_set_DbyIMF;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorDolIMFObjectDefinitionSnapTolExistsNotZero;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

        // #################################################
        // IMF constraint sets validation ends
        // #################################################

		return isToContinue;
    }

	// ****************************************************************************
	// IABObjectDefinition class implementation
	// ****************************************************************************

	// Constructor implementation
	IABObjectDefinition::IABObjectDefinition(IABFrameRateType iFrameRate) :
		IABElement(kIABElementID_ObjectDefinition)
    {
        metadataID_ = 0;
        audioDataID_ = 0;
        conditionalObject_ = 0;
        objectUseCase_ = kIABUseCase_7_1_DS;
		parentFrameRate_ = iFrameRate;
        numPanSubBlocks_ = GetIABNumSubBlocks(iFrameRate);
        subElementCount_ = 0;
        audioDescription_.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription_.audioDescriptionText_.clear();
		numSkippedObjectSubElementsInParsing_ = 0;
		numUndefinedObjectSubElements_ = 0;
		numUnallowedObjectSubElements_ = 0;

		packedSubElementCount_ = 0;						// To match number of element in objectSubElements_ that is packing enabled
	}

    IABObjectDefinition::~IABObjectDefinition()
    {
        DeletePanSubBlocks();
        DeleteSubElements();
    }
    
    // IABObjectDefinition::GetMetadataID() implementation
    void IABObjectDefinition::GetMetadataID(IABMetadataIDType &oMetaID) const
    {
        oMetaID = metadataID_;
    }
    
    // IABObjectDefinition::SetMetadataID() implementation
    iabError IABObjectDefinition::SetMetadataID(IABMetadataIDType iMetaID)
    {
        metadataID_ = iMetaID;
        return kIABNoError;
    }
    
    // IABObjectDefinition::GetAudioDataID() implementation
    void IABObjectDefinition::GetAudioDataID(IABAudioDataIDType &oAudioDataID) const
    {
        oAudioDataID = audioDataID_;
    }
    
    // IABObjectDefinition::SetAudioDataID() implementation
    iabError IABObjectDefinition::SetAudioDataID(IABAudioDataIDType iAudioDataID)
    {
        audioDataID_ = iAudioDataID;
        return kIABNoError;
    }
    
    // IABObjectDefinition::GetConditionalObject() implementation
    void IABObjectDefinition::GetConditionalObject(uint1_t &oConditionalObject) const
    {
        oConditionalObject = conditionalObject_;
    }
    
    // IABObjectDefinition::SetConditionalObject() implementation
    iabError IABObjectDefinition::SetConditionalObject(uint1_t iConditionalObject)
    {
        conditionalObject_ = iConditionalObject;
        return kIABNoError;
    }
    
    // IABObjectDefinition::GetObjectUseCase() implementation
    void IABObjectDefinition::GetObjectUseCase(IABUseCaseType &oObjectUseCase) const
    {
        oObjectUseCase = objectUseCase_;
    }
    
    // IABObjectDefinition::SetObjectUseCase() implementation
    iabError IABObjectDefinition::SetObjectUseCase(IABUseCaseType iObjectUseCase)
    {
        objectUseCase_= iObjectUseCase;
        return kIABNoError;
    }
    
    // IABObjectDefinition::GetNumPanSubBlocks() implementation
    void IABObjectDefinition::GetNumPanSubBlocks(uint8_t &oNumPanSubBlocks) const
    {
        oNumPanSubBlocks = numPanSubBlocks_;
    }
    
    // IABObjectDefinition::GetPanSubBlocks() implementation
    void IABObjectDefinition::GetPanSubBlocks(std::vector<IABObjectSubBlock*> &oPanSubBlocks) const
    {
        oPanSubBlocks = objectPanSubBlocks_;
    }
    
    // IABObjectDefinition::SetPanSubBlocks() implementation
    iabError IABObjectDefinition::SetPanSubBlocks(const std::vector<IABObjectSubBlock*> iPanSubBlocks)
    {
        // Delete old entries first
        DeletePanSubBlocks();

        numPanSubBlocks_ = static_cast<uint8_t>(iPanSubBlocks.size());
        
		// Checking if any of the pointers is a nullptr
		for (uint32_t i = 0; i < numPanSubBlocks_; i++)
		{
			if (iPanSubBlocks[i] == nullptr)
			{
				return kIABBadArgumentsError;
			}
		}

		objectPanSubBlocks_ = iPanSubBlocks;

		return kIABNoError;
    }
    
    // IABObjectDefinition::GetAudioDescription() implementation
    void IABObjectDefinition::GetAudioDescription(IABAudioDescription &oAudioDescription) const
    {
        oAudioDescription = audioDescription_;
    }
    
    // IABObjectDefinition::SetAudioDescription() implementation
    iabError IABObjectDefinition::SetAudioDescription(IABAudioDescription const& iAudioDescription)
    {
        audioDescription_ = iAudioDescription;
        return kIABNoError;
    }
    
    // IABObjectDefinition::GetSubElementCount() implementation
    void IABObjectDefinition::GetSubElementCount(IABElementCountType &oCount) const
    {
        oCount = (IABElementCountType)objectSubElements_.size();
    }
    
    // IABObjectDefinition::GetSubElements() implementation
    void IABObjectDefinition::GetSubElements(std::vector<IABElement*> &oSubElements) const
    {
        oSubElements = objectSubElements_;
    }
    
    // IABObjectDefinition::SetSubElements() implementation
    // TODO: Need to revise the submement deletion inside this method.
    // MAC xcode lets user to use the deleted pointer. That may endup in crash or in undefined behaviour at userspace.
    // Setting NULL to delete subment here also will not have any effect on userspace.
    iabError IABObjectDefinition::SetSubElements(const std::vector<IABElement*> iSubElements)
    {
        IABElementCountType count = static_cast<IABElementCountType>(iSubElements.size());
        
        IABElementIDType elementID;
        
        // Checking if any of the pointers is a nullptr or not a valid sub-element type
        for (uint32_t i = 0; i < count; i++)
        {
			if (nullptr == iSubElements[i])
			{
				return kIABBadArgumentsError;
			}

			iSubElements[i]->GetElementID(elementID);
            
            if ((kIABElementID_ObjectDefinition != elementID) &&
                 (kIABElementID_ObjectZoneDefinition19 != elementID))
            {
                return kIABBadArgumentsError;
            }
        }
        
        // Report error if duplicate sub-elements are in the list.
        std::set<IABElement*> tempSet; //Using Set datastructure, Existence check can be done easily. i.e set doenst allow the dublicates.
        for(std::vector<IABElement*>::const_iterator iter = iSubElements.begin(); iter != iSubElements.end(); ++iter)
        {
            // finds the duplicate
            if (!tempSet.insert(*iter).second)
                return kIABDuplicateSubElementsError;
        }

        
        // Update existing sub-elements list carefully.
        for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
        {
            bool found = false;
            for (std::vector<IABElement*>::const_iterator sub = iSubElements.begin(); sub != iSubElements.end(); sub++)
            {
                if (*iter == *sub)
                {
                    found = true;
                    break;
                }
            }
            // if sub-elmenent not found in the current list, just delete the sub-element.
            if (found == false)
            {
                delete *iter;
                *iter = NULL;
            }
        }
        
        // Now set the new list as current list.
        objectSubElements_ = iSubElements;
        subElementCount_ = count;
        
        return kIABNoError;

    }

    // IABObjectDefinition::DeletePanSubBlocks() implementation
    void IABObjectDefinition::DeletePanSubBlocks()
    {
		for (std::vector<IABObjectSubBlock*>::iterator iter = objectPanSubBlocks_.begin(); iter != objectPanSubBlocks_.end(); iter++)
		{
			delete *iter;
		}

		objectPanSubBlocks_.clear();
    }
    
    // IABObjectDefinition::DeleteSubElements() implementation
    void IABObjectDefinition::DeleteSubElements()
    {
		for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
		{
			delete *iter;
		}

		objectSubElements_.clear();
		subElementCount_ = 0;
    }

	// IABObjectDefinition::ClearSubElements() implementation
	iabError IABObjectDefinition::ClearSubElements()
	{
		objectSubElements_.clear();
		subElementCount_ = 0;

		return kIABNoError;
	}

	// IABObjectDefinition::IsSubElement() implementation
	bool IABObjectDefinition::IsSubElement(IABElement* iSubElement)
	{
		if (nullptr == iSubElement)
		{
			return false;
		}

		// Check against sub element list
		for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
		{
			// Find the matching entry (same ponter address) from the list...
			if (iSubElement == *iter)
			{
				// found, return true
				return true;
			}
		}

		// Not found
		return false;
	}

	// IABObjectDefinition::RemoveSubElement() implementation
	iabError IABObjectDefinition::RemoveSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
		{
			// Find the matching entry from the list...
			if (iSubElement == *iter)
			{
				// Erase the entry from list (but not delete *iter).
				objectSubElements_.erase(iter);

				subElementCount_ -= 1;				// Decrement subElementCount_ to account for removal of 1 entry

				errorCode = kIABNoError;
				break;								// Entry found and removed. Break.
			}
		}

		return errorCode;
	}

	// IABObjectDefinition::AddSubElement() implementation
	iabError IABObjectDefinition::AddSubElement(IABElement* iSubElement)
	{
		iabError errorCode = kIABBadArgumentsError;
		IABElementIDType elementID;

		if (nullptr == iSubElement)
		{
			return errorCode;
		}

		// Check if element is of types allowed as BedDefinition sub element
		iSubElement->GetElementID(elementID);

		if ((kIABElementID_ObjectDefinition != elementID)
			&& (kIABElementID_ObjectZoneDefinition19 != elementID))
		{
			return errorCode;
		}

		for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
		{
			// Is iSubElement already in sub-element list?
			if (iSubElement == *iter)
			{
				// If yes, exit. Entry already in sub element list.
				return kIABDuplicateSubElementsError;
			}
		}

		// Not in sub element list, add.
		objectSubElements_.push_back(iSubElement);
		subElementCount_ += 1;						// Increment subElementCount_ to account for addition of 1 entry

		return kIABNoError;
	}

	// IABObjectDefinition::EnablePacking() implementation
	void IABObjectDefinition::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABObjectDefinition::DisablePacking() implementation
	void IABObjectDefinition::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABObjectDefinition::IsIncludedForPacking() implementation
	bool IABObjectDefinition::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABObjectDefinition::Serialize() implementation
	iabError IABObjectDefinition::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}
		else
		{
			// Yes, included. Update sub element count for sub element packing
			UpdatePackingEnabledSubElementCount();
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();


		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		// Calling method for writing Plex encoded field.
		write(elementPayloadWriter_, Plex<8>(metadataID_));
		write(elementPayloadWriter_, Plex<8>(audioDataID_));

		elementPayloadWriter_.write(conditionalObject_, 1);
		if (conditionalObject_ == 1)
		{
			elementPayloadWriter_.write(reserved1_, 1);						// A reserved 1-bit fired, always = 1
			elementPayloadWriter_.write(static_cast<uint8_t>(objectUseCase_), 8);
		}

		elementPayloadWriter_.write(reserved2_, 1);							// A reserved 1-bit fired, always = 0

		// numPanSubBlocks_ is not in the stream, but is determined by frame rate, per spec

		// Serialize all object pan sub blocks, in sequence, into payload writer

		// First block, skip "panInfoExists_" and always write panning parameters
		objectPanSubBlocks_[0]->SerializeSubBlock(elementPayloadWriter_);

		// For remaining sub-blocks, call serialization that packs the "panInfoExists_" at beginning
		for (uint8_t i = 1; i < numPanSubBlocks_; i++)
		{
			objectPanSubBlocks_[i]->Serialize(elementPayloadWriter_);
		}

		elementPayloadWriter_.align();										// Align to byte

		elementPayloadWriter_.write(static_cast<uint8_t>(audioDescription_.audioDescription_), 8);

		if (audioDescription_.audioDescription_ & 0x80)
		{
            // Write audio description text to bitstream as a null terminated string (i.e. character sequence)
            // writeStringAppendNull() will add a null byte after the last character of the string.
            writeStringAppendNull(elementPayloadWriter_, audioDescription_.audioDescriptionText_);
		}

		write(elementPayloadWriter_, Plex<8>(packedSubElementCount_));		// Note to write packedSubElementCount_ instead of subElementCount_
		// Serialize all sub-elements, in sequence
		// Append to end of IABObjectDefinition's "elementPayloadBuffer_"
		for (IABElementCountType i = 0; i < subElementCount_; i++)			// Still loop through all subElementCount_ of sub elements
		{
			// Temp StringStream to hold packed output from sub-elements (TODO: optimization?)
			std::stringstream subElementPayload(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			objectSubElements_[i]->Serialize(subElementPayload);
			const std::string payLoad = subElementPayload.str();
			uint32_t length = static_cast<uint32_t>(payLoad.size());
			elementPayloadWriter_.write((uint8_t*)payLoad.data(), length);
		}

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABObjectDefinition::DeSerialize() implementation
	iabError IABObjectDefinition::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;

		errorCode = DeSerializeHead(streamReader);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

        uint8_t	fixedLengthFieldMax8 = 0;
        Plex<8> plex8Field = 0;
        
        // Read meta data ID
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABObjectDefinitionError;
        }
        
        metadataID_ = static_cast<IABMetadataIDType>(plex8Field);
        
        // Read audio data ID
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABObjectDefinitionError;
        }

        audioDataID_ = static_cast<IABAudioDataIDType>(plex8Field);
        
        // Read 1-bit conditional object
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABObjectDefinitionError;
        }

        conditionalObject_ = static_cast<uint1_t>(fixedLengthFieldMax8);
        
        // Read 8-bit use case
        if (1 == conditionalObject_)
        {
            // Read 1-bit reserved field (reserved1_)
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
            {
                return kIABParserIABObjectDefinitionError;
            }

            // Read 8-bit object use case
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8))
            {
                return kIABParserIABObjectDefinitionError;
            }

            objectUseCase_ = static_cast<IABUseCaseType>(fixedLengthFieldMax8);
        }
        
        // Read 1-bit reserved field (reserved2_)
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABObjectDefinitionError;
        }

		// Clear any existent pan blocks
        if (objectPanSubBlocks_.size() > 0)
        {
            DeletePanSubBlocks();
        }

        // Read panning subblocks
        for (uint8_t i = 0; i < static_cast<uint8_t>(numPanSubBlocks_); i++)
        {
            IABObjectSubBlock* objectSubBlock = new IABObjectSubBlock();

            if (nullptr == objectSubBlock)
            {
                return kIABMemoryError;
            }
            
            if (0 == i)
            {
				errorCode = objectSubBlock->DeSerializeSubBlock(streamReader);
                objectSubBlock->SetPanInfoExists(1);
            }
            else
            {
				errorCode = objectSubBlock->DeSerialize(streamReader);
            }
            
			if (errorCode != kIABNoError)
			{
                delete objectSubBlock;
				return errorCode;
			}

			if (streamReader.good())
            {
                objectPanSubBlocks_.push_back(objectSubBlock);
            }
            else
            {
                delete objectSubBlock;
                return kIABParserIABObjectDefinitionError;
            }
        }
        
        if (CMNSTRM_OK != streamReader.align())  // byte alignment
        {
            return kIABParserIABObjectDefinitionError;
        }

        // Read audio description
        uint8_t audioDescription = 0;
        if (CMNSTRM_OK != streamReader.read(audioDescription))
        {
            return kIABParserIABObjectDefinitionError;
        }
        
        audioDescription_.audioDescription_ = static_cast<IABAudioDescriptionType>(audioDescription);

        if (audioDescription & 0x80)
        {
            // Read audio description text string from the bitstream and the string is expected to be
            // terminated by a null byte. The null byte is removed before saving into audioDescription_.audioDescriptionText_.
            if (CMNSTRM_OK != readToNull(streamReader, audioDescription_.audioDescriptionText_))
            {
                return kIABParserIABObjectDefinitionError;
            }
        }
        
        // Read object sub-element count
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABObjectDefinitionError;
        }

        subElementCount_ = static_cast<IABElementCountType>(plex8Field);
        
        for (uint32_t j = 0; j < static_cast<uint32_t>(subElementCount_); j++)
        {
			errorCode = ParseObjectSubElement(streamReader);

			if (errorCode != kIABNoError)
			{
				return errorCode;
			}
        }
        
		// If any sub elements have been skipped by the parser, deduct them from sub-element count read from the bitstream
		subElementCount_ -= numSkippedObjectSubElementsInParsing_;

		if (streamReader.good())
        {
            return errorCode;
        }
        else
        {
            return kIABParserIABObjectDefinitionError;
        }
	}
    
	// IABObjectDefinition::ParseObjectSubElement() implementation
	iabError IABObjectDefinition::ParseObjectSubElement(StreamReader& streamReader)
    {
		uint32_t parsedElementIDField;
		IABElementIDType elementID;

		// Peek the next (Plex<8> coded) element/sub-element ID field
		if (kIABNoError != PeekNextElementID(streamReader, parsedElementIDField))
		{
			return kIABParserParsingError;
		}

		elementID = static_cast<IABElementIDType>(parsedElementIDField);

		IABElement* objectDefinitionSubElement = nullptr;

		switch (elementID)
		{
			case kIABElementID_ObjectDefinition:
				objectDefinitionSubElement = new IABObjectDefinition(parentFrameRate_);
				break;
        	
			case kIABElementID_ObjectZoneDefinition19:
        	    objectDefinitionSubElement = new IABObjectZoneDefinition19(parentFrameRate_);
				break;
        	
			// Known type but invalid type as IAB object sub-element
			// Total number of unallowed element IDs for the object is tracked in numUnallowedObjectSubElements_.
			case kIABElementID_BedDefinition:
			case kIABElementID_BedRemap:
			case kIABElementID_AuthoringToolInfo:
			case kIABElementID_UserData:
			case kIABElementID_AudioDataDLC:
			case kIABElementID_AudioDataPCM:
			case kIABElementID_IAFrame:
				SkipDeSerializeCurrentElement(*elementReader_);
				numSkippedObjectSubElementsInParsing_++;
				numUnallowedObjectSubElements_++;
				return kIABNoError;										// Exit, as objectDefinitionSubElement == nullptr for this case
        	
			// Undefined/Unknown type as IAB ObjectDefinition sub-element
			// Total number of unallowed element IDs for the object is tracked in numUndefinedObjectSubElements_.
			default:
				SkipDeSerializeCurrentElement(*elementReader_);
				numSkippedObjectSubElementsInParsing_++;
				numUndefinedObjectSubElements_++;
				return kIABNoError;										// Exit, as objectDefinitionSubElement == nullptr for default case
		}

		// Check validity of objectDefinitionSubElement
		if (nullptr == objectDefinitionSubElement)
		{
			return kIABMemoryError;
		}

		// Parse sub-element
		iabError returnCode = kIABNoError;

		returnCode = objectDefinitionSubElement->DeSerialize(streamReader);

		if (returnCode != kIABNoError)
		{
			delete objectDefinitionSubElement;
			return returnCode;
		}

		objectSubElements_.push_back(objectDefinitionSubElement);				// Save to Object Definition sub element list

		return kIABNoError;
	}

	// IABObjectDefinition::UpdatePackEnabledSubElementCount() implementation
	void IABObjectDefinition::UpdatePackingEnabledSubElementCount()
	{
		IABElementCountType numPackingEnabledElement = 0;

		for (std::vector<IABElement*>::iterator iter = objectSubElements_.begin(); iter != objectSubElements_.end(); iter++)
		{
			if ((*iter)->IsIncludedForPacking())
			{
				numPackingEnabledElement += 1;
			}
		}

		packedSubElementCount_ = numPackingEnabledElement;
	}

	// IABObjectDefinition::GetNumUnallowedSubElements() implementation
	uint32_t IABObjectDefinition::GetNumUnallowedSubElements() const
	{
		return numUnallowedObjectSubElements_;
	}

	// IABObjectDefinition::GetNumUndefinedSubElements() implementation
	uint32_t IABObjectDefinition::GetNumUndefinedSubElements() const
	{
		return numUndefinedObjectSubElements_;
	}

    // IABObjectDefinition::Validate() implementation
    bool IABObjectDefinition::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// Update iValidationIssue.id_ so that any issues reported from this Validate()
		// function is for ObjectDefinition element with metadataID_ value.
		//
		iValidationIssue.id_ = metadataID_;

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// Object MetaID

		// Object AudioDataID

		// General count var
		size_t foundCount = 0;

		// objectUseCase_
		// Check only when conditionalObject_ is enabled
		if (conditionalObject_)
		{
			foundCount = ConstraintSet_ST2098_2_2018.validUseCases_.count(objectUseCase_);

			// If it is not uniquely found
			if (foundCount != 1)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionUndefinedUseCase;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

		// Pan sub-block count vs. pan sub-block list size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (numPanSubBlocks_ != objectPanSubBlocks_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionPanSubblockCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on all pan sub-blocks
		std::vector<IABObjectSubBlock*>::const_iterator iterPanSubblock;
		for (iterPanSubblock = objectPanSubBlocks_.begin(); iterPanSubblock != objectPanSubBlocks_.end(); iterPanSubblock++)
		{
			if (isToContinue = (*iterPanSubblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// AudioDescription code
		// Check if reserved code 0x40 (bit 6) is on, using AND (& 0x40)
		foundCount = ConstraintSet_ST2098_2_2018.reservedAudioDescriptionCodes_.count(audioDescription_.audioDescription_ & 0x40);

		// If it is on, issue warning
		if (foundCount == 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = WarningEvent;
			iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionUndefinedAudioDescription;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// AudioDescriptionText[n] length limit
		// Check this limit only if AudioDescription code has bit 7 on, by (& 0x80)
		if (audioDescription_.audioDescription_ & 0x80)
		{
			if (audioDescription_.audioDescriptionText_.length() > 64)
			{
				// Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = ErrorEvent;
				iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionAudioDescriptionTextExceeded;

				if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
				{
					return isToContinue;
				}
			}
		}

		// Sub-element count vs. sub-element size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (subElementCount_ != objectSubElements_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionSubElementCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on ObjectDefinition sub-elements
        // Due to sub-elements having their own validation for all constraint sets, this block will cover
        // IMF constraint sets as well.
		std::vector<IABElement*>::const_iterator iterObjectSubElement;
		IABElementIDType elementID;

		for (iterObjectSubElement = objectSubElements_.begin(); iterObjectSubElement != objectSubElements_.end(); iterObjectSubElement++)
		{
			// Retrieve elementID
			(*iterObjectSubElement)->GetElementID(elementID);

			// Validate sub-elements
			//
			switch (elementID)
			{
			case kIABElementID_ObjectDefinition:
				isToContinue = dynamic_cast<IABObjectDefinition*>(*iterObjectSubElement)->Validate(iEvenHandler, iValidationIssue);
                if (!isToContinue)
                {
                    return isToContinue;
                }
                
				break;

			case kIABElementID_ObjectZoneDefinition19:
				isToContinue = dynamic_cast<IABObjectZoneDefinition19*>(*iterObjectSubElement)->Validate(iEvenHandler, iValidationIssue);
                if (!isToContinue)
                {
                    return isToContinue;
                }
                    
                // ObjectZoneDefinition19 sub-element is not allowed for DbyCinema
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectZoneDefinition19NotAnAllowedSubElement;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }

				break;

			default:
				// Update issue variable and log for Cinema ST2098_2_201
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
				iValidationIssue.event_ = WarningEvent;
				iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionContainUnsupportedSubElement;
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                    
                // Sub-element processing can be run once only to avoid duplicate reporting
                // So, do sub-element IMF validation here as well
                // Update issue variable and log for IMF ST2098_2_2019
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = WarningEvent;
                iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionContainUnsupportedSubElement;
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                    
				break;
			}
            
			// Restore issue id_ here to parent object level, as some of the sub-element validation
			// may have set the issue id_ during element-internal validation
			//
			iValidationIssue.id_ = metadataID_;

			// Sub-element is not allowed for DbyCinema
            iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionSubElementsNotAllowed;
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }

            // Sub-element processing can be run once only to avoid duplicate reporting
            // So, do sub-element IMF validation here as well
            //
            if (elementID != kIABElementID_ObjectZoneDefinition19)
            {
                // For ST2067_201_2019, ObjectDefinition cannot contain sub-elements other than ObjectDefinitionZOne19
                // Update issue variable and log for IMF ST2098_2_2019
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionInvalidSubElementType;
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
		}

        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
		// Restore issue id_ to parent object
		//
		iValidationIssue.id_ = metadataID_;

		// ST429_18_2019 additional checks on IABObjectDefinition :
        // 1. Invalid use cases : 0x30 - 0xFE inclusive
        //
        if (conditionalObject_)
        {
            if ((objectUseCase_ >= ConstraintSet_ST429_18_2019.inValidUseCaseLowerBound_) &&
                (objectUseCase_ <= ConstraintSet_ST429_18_2019.inValidUseCaseUpperBound_))
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionInvalidUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }            
        }

        // Note that kConstraints_set_DbyCinema is a super set of ST-429-18-2019 and the preceding checks are required to validate
        // bitstream against DbyCinema
        
        // DbyCinema additional checks on IABObjectDefinition :
        // 1. Only "AlwaysUse" use case
        
        if (conditionalObject_)
        {
            foundCount = ConstraintSet_DbyCinema.validUseCases_.count(objectUseCase_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorDolCinObjectDefinitionInvalidUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // Update iValidationIssue.id_ so that any issues reported from this Validate()
        // function is for ObjectDefinition element with metadataID_ value.
        //
        iValidationIssue.id_ = metadataID_;
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Object MetaID
        
        // Object AudioDataID
        
        // objectUseCase_
        // Check only when conditionalObject_ is enabled
        if (conditionalObject_)
        {
            foundCount = ConstraintSet_IMF_ST2098_2_2019.validUseCases_.count(objectUseCase_);
            
            // If it is not uniquely found
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = WarningEvent;
                iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionUndefinedUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }
        
        // Pan sub-block count vs. pan sub-block list size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (numPanSubBlocks_ != objectPanSubBlocks_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionPanSubblockCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Run Validate() on all pan sub-blocks
        for (iterPanSubblock = objectPanSubBlocks_.begin(); iterPanSubblock != objectPanSubBlocks_.end(); iterPanSubblock++)
        {
            if (isToContinue = (*iterPanSubblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // AudioDescription code
        // Check if reserved code 0x40 (bit 6) is on, using AND (& 0x40)
        foundCount = ConstraintSet_IMF_ST2098_2_2019.reservedAudioDescriptionCodes_.count(audioDescription_.audioDescription_ & 0x40);
        
        // If it is on, issue warning
        if (foundCount == 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = WarningEvent;
            iValidationIssue.errorCode_ = kValidateWarningObjectDefinitionUndefinedAudioDescription;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // AudioDescriptionText[n] length limit
        // Check this limit only if AudioDescription code has bit 7 on, by (& 0x80)
        if (audioDescription_.audioDescription_ & 0x80)
        {
            if (audioDescription_.audioDescriptionText_.length() > 64)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionAudioDescriptionTextExceeded;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
                
            }
        }
        
        // Sub-element count vs. sub-element size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (subElementCount_ != objectSubElements_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionSubElementCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // ST2098_2_2019 checks completed, change validation constriant set to kConstraints_set_IMF_ST2067_201_2019
        // Note that ST2067_201_2019 is a super set of ST2098_2_2019 and the preceding ST2098_2_2019 checks are required
        // to validate bitstream against ST2067_201_2019

        if (conditionalObject_)
        {
            // Check use case against ST2067_201_2019
            foundCount = ConstraintSet_IMF_ST2067_201_2019.validUseCases_.count(objectUseCase_);
            if (foundCount != 1)
            {
                // Update issue variable and log
                iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
                iValidationIssue.event_ = ErrorEvent;
                iValidationIssue.errorCode_ = kValidateErrorObjectDefinitionInvalidUseCase;
                
                if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                {
                    return isToContinue;
                }
            }
        }

        // #################################################
        // IMF constraint sets validation ends
        // #################################################


		return isToContinue;
    }

	// ****************************************************************************
	// IABZone19SubBlock class implementation
	// ****************************************************************************

	// Constructor implementation
	IABZone19SubBlock::IABZone19SubBlock()
	{
		zone19Gain_.objectZone19InfoExists_ = 0;
	}

	// IABZone19SubBlock::GetObjectZoneGains19() implementation
	void IABZone19SubBlock::GetObjectZoneGains19(IABObjectZoneGain19 &oObjectZoneGains19) const
	{
		oObjectZoneGains19 = zone19Gain_;
	}

	// IABZone19SubBlock::SetObjectZoneGains19() implementation
	iabError IABZone19SubBlock::SetObjectZoneGains19(IABObjectZoneGain19 const& iObjectZoneGains19)
	{
		zone19Gain_ = iObjectZoneGains19;
		return kIABNoError;
	}

	// IABZone19SubBlock::Serialize() implementation
	iabError IABZone19SubBlock::Serialize(StreamWriter& streamWriter)
	{
		iabError returnCode = kIABNoError;

		// The payload stream writer for parent IABObjectZoneDefinition19 element is passed in.
		// Individual sub-clock instances are packed into the payload buffer/ostream of  
		// IABObjectZoneDefinition19 element, without alignment manipulation
		// ! Do not change alignment when writing individual sub blocks.

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		streamWriter.write(zone19Gain_.objectZone19InfoExists_, 1);

		if (zone19Gain_.objectZone19InfoExists_ == 1)
		{
			returnCode = SerializeSubBlock(streamWriter);
		}		// if (zone19Gain_.objectZone19InfoExists_ == 1)

		return returnCode;
	}

	// IABZone19SubBlock::SerializeSubBlock() implementation
	iabError IABZone19SubBlock::SerializeSubBlock(StreamWriter& streamWriter)
	{
		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")

		// For first zone19 sub block, zone19Gain_.objectZone19InfoExists_ is always 1 by specification. 
		// It is not in bitstream.

		// Writing zone19 gains specified for the block.
		for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
		{
			streamWriter.write(static_cast<uint8_t>(zone19Gain_.zoneGains_[i].getIABZoneGainPrefix()), 2);
			if (kIABZoneGainPrefix_InStream == zone19Gain_.zoneGains_[i].getIABZoneGainPrefix())
			{
				streamWriter.write(zone19Gain_.zoneGains_[i].getIABZoneGainInStreamValue(), 10);
			}
		}

		return kIABNoError;
	}

	// IABZone19SubBlock::DeSerialize() implementation
	iabError IABZone19SubBlock::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;
		uint8_t fixedLengthFieldMax8 = 0;

		// Read 1-bit ZoneInfoExists from input stream
		if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
        {
            return kIABParserIABZone19SubBlockError;
        }

		zone19Gain_.objectZone19InfoExists_ = static_cast<uint1_t>(fixedLengthFieldMax8);

		if (zone19Gain_.objectZone19InfoExists_ == 1)
		{
			errorCode = DeSerializeSubBlock(streamReader);
		}

		return errorCode;
	}

	// IABZone19SubBlock::DeSerializeSubBlock() implementation
	iabError IABZone19SubBlock::DeSerializeSubBlock(StreamReader& streamReader)
	{
		uint8_t fixedLengthFieldMax8 = 0;
		uint16_t fixedLengthFieldMax16 = 0;

		for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
		{
			// Read zone19 gain prefix
			if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
            {
                return kIABParserIABZone19SubBlockError;
            }

			if (fixedLengthFieldMax8 == 0)
			{
				// Silence gain
				zone19Gain_.zoneGains_[i].setIABZoneGain(0.0f);
			}
			else if (fixedLengthFieldMax8 == 1)
			{
				// unity gain
				zone19Gain_.zoneGains_[i].setIABZoneGain(1.0f);
			}
			else if (fixedLengthFieldMax8 == 2)
			{
				if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10)) // 10-bit gain in bitstream
                {
                    return kIABParserIABZone19SubBlockError;
                }
                
				zone19Gain_.zoneGains_[i].setIABZoneGainFromStreamValue(fixedLengthFieldMax16);
			}
			else
			{
				return kIABParserIABZone19SubBlockError;
			}
		}

		// Unless parsing error, the fact that this function is called means that
		// "objectZone19InfoExists_" must be set to 1
		zone19Gain_.objectZone19InfoExists_ = 1;

		return kIABNoError;
	}
    
    // IABZone19SubBlock::Validate() implementation
    bool IABZone19SubBlock::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// General count var
		size_t foundCount = 0;

		// Run zone19 sub-block validation only when zone19Gain_.objectZone19InfoExists_ is enabled
		if (zone19Gain_.objectZone19InfoExists_)
		{
			// Check zone19 gain prefixes
			for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
			{
				foundCount = ConstraintSet_ST2098_2_2018.validZoneGainPrefix_.count(zone19Gain_.zoneGains_[i].getIABZoneGainPrefix());

				// If it is not uniquely found
				if (foundCount != 1)
				{
					// Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
					iValidationIssue.event_ = ErrorEvent;
					iValidationIssue.errorCode_ = kValidateErrorObjectZoneDefinition19UnsupportedZoneGainPrefix;

					if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
					{
						return isToContinue;
					}
				}
			}
		}

        // Note: No additional checks for ST-429-18-2019 constraint set
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Run zone19 sub-block validation only when zone19Gain_.objectZone19InfoExists_ is enabled
        if (zone19Gain_.objectZone19InfoExists_)
        {
            // Check zone19 gain prefixes
            for (uint32_t i = 0; i < kIABObjectZoneCount19; i++)
            {
                foundCount = ConstraintSet_IMF_ST2098_2_2019.validZoneGainPrefix_.count(zone19Gain_.zoneGains_[i].getIABZoneGainPrefix());
                
                // If it is not uniquely found
                if (foundCount != 1)
                {
                    // Update issue variable and log
                    iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
                    iValidationIssue.event_ = ErrorEvent;
                    iValidationIssue.errorCode_ = kValidateErrorObjectZoneDefinition19UnsupportedZoneGainPrefix;
                    
                    if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
                    {
                        return isToContinue;
                    }
                }
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

        
		return isToContinue;
	}

	// ****************************************************************************
	// IABObjectZoneDefinition19 class implementation
	// ****************************************************************************

	// Constructor implementation
	IABObjectZoneDefinition19::IABObjectZoneDefinition19(IABFrameRateType iFrameRate) :
		IABElement(kIABElementID_ObjectZoneDefinition19)
    {
		numZone19SubBlocks_ = GetIABNumSubBlocks(iFrameRate);
	}

	IABObjectZoneDefinition19::~IABObjectZoneDefinition19()
	{
		DeleteZone19SubBlocks();
	}

	// IABObjectZoneDefinition19::GetNumZone19SubBlocks() implementation
	void IABObjectZoneDefinition19::GetNumZone19SubBlocks(uint8_t &oNumZone19SubBlocks) const
	{
		oNumZone19SubBlocks = numZone19SubBlocks_;
	}

	// IABObjectZoneDefinition19::GetZone19SubBlocks() implementation
	void IABObjectZoneDefinition19::GetZone19SubBlocks(std::vector<IABZone19SubBlock*> &oZone19SubBlocks) const
	{
		oZone19SubBlocks = zone19SubBlocks_;
	}

	// IABObjectZoneDefinition19::SetZone19SubBlocks() implementation
	iabError IABObjectZoneDefinition19::SetZone19SubBlocks(const std::vector<IABZone19SubBlock*> iZone19SubBlocks)
	{
		// Delete old entries first
		DeleteZone19SubBlocks();

		numZone19SubBlocks_ = static_cast<uint8_t>(iZone19SubBlocks.size());

		// Checking if any of the pointers is a nullptr
		for (uint32_t i = 0; i < numZone19SubBlocks_; i++)
		{
			if (iZone19SubBlocks[i] == nullptr)
			{
				return kIABBadArgumentsError;
			}
		}

		zone19SubBlocks_ = iZone19SubBlocks;

		return kIABNoError;
	}

	// IABObjectZoneDefinition19::DeleteZone19SubBlocks() implementation
	void IABObjectZoneDefinition19::DeleteZone19SubBlocks()
	{
		for (std::vector<IABZone19SubBlock*>::iterator iter = zone19SubBlocks_.begin(); iter != zone19SubBlocks_.end(); iter++)
		{
			delete *iter;
		}

		zone19SubBlocks_.clear();
	}

	// IABObjectZoneDefinition19::EnablePacking() implementation
	void IABObjectZoneDefinition19::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABObjectZoneDefinition19::DisablePacking() implementation
	void IABObjectZoneDefinition19::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABObjectZoneDefinition19::IsIncludedForPacking() implementation
	bool IABObjectZoneDefinition19::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABObjectZoneDefinition19::Serialize() implementation
	iabError IABObjectZoneDefinition19::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// numZone19SubBlocks_ is not in the stream, but is determined by frame rate, per spec

		// Serialize all zone19 sub blocks, in sequence, into payload writer
		// First block, call "SerializeSubBlock()" directly
		zone19SubBlocks_[0]->SerializeSubBlock(elementPayloadWriter_);

		// For remaining zone19 sub-blocks, call "Serialize()"
		for (uint8_t i = 1; i < numZone19SubBlocks_; i++)
		{
			zone19SubBlocks_[i]->Serialize(elementPayloadWriter_);
		}

		// Current specificaion v4.34 does not require alingment post Zone19?
		// Strongly suggest to add alignment to 25CSS group
		// I am adding alignment in anticipation, as not having it would
		// mean that DLC element may not be byte aligned, causing issues!
		elementPayloadWriter_.align();										// Align to byte

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABObjectZoneDefinition19::DeSerialize() implementation
	iabError IABObjectZoneDefinition19::DeSerialize(StreamReader& streamReader)
	{
        iabError errorCode = kIABNoError;
        
        errorCode = DeSerializeHead(streamReader);
        
        if (errorCode != kIABNoError)
        {
            return errorCode;
        }
        
		// Cear any existent zone19 sub blocks
		if (zone19SubBlocks_.size() > 0)
		{
			DeleteZone19SubBlocks();
		}

		// Read zone19 sub blocks
		for (uint8_t i = 0; i < numZone19SubBlocks_; i++)
		{
			iabError errorCode = kIABNoError;

			IABZone19SubBlock* zone19SubBlock = new IABZone19SubBlock();

			if (nullptr == zone19SubBlock)
			{
				return kIABMemoryError;
			}

			if (0 == i)
			{
				errorCode = zone19SubBlock->DeSerializeSubBlock(streamReader);
			}
			else
			{
				errorCode = zone19SubBlock->DeSerialize(streamReader);
			}

			if (errorCode != kIABNoError)
			{
                delete zone19SubBlock;
				return errorCode;
			}

			if (streamReader.good())
			{
				zone19SubBlocks_.push_back(zone19SubBlock);
			}
			else
			{
				delete zone19SubBlock;
				return kIABParserIABObjectZone19Error;
			}
		}

		// No alignment requirement per v4.34 specification (current).
		// I am adding alignment in anticipation that this will be corrected.
        if ((CMNSTRM_OK == streamReader.align()) && streamReader.good())
        {
            return kIABNoError;
        }
        else
        {
            return kIABParserIABObjectZone19Error;
        }
	}
    
    // IABObjectZoneDefinition19::Validate() implementation
    bool IABObjectZoneDefinition19::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// Zone19 sub-block count vs. zone19 sub-block list size consistency
		// This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib. 
		if (numZone19SubBlocks_ != zone19SubBlocks_.size())
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorObjectZoneDefinition19SubblockCountConflict;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// Run Validate() on all zone19 sub-blocks
		std::vector<IABZone19SubBlock*>::const_iterator iterZone19Subblock;
		for (iterZone19Subblock = zone19SubBlocks_.begin(); iterZone19Subblock != zone19SubBlocks_.end(); iterZone19Subblock++)
		{
			if (isToContinue = (*iterZone19Subblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}
        
        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // Note: No additional checks for ST-429-18-2019 constraint set
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // Zone19 sub-block count vs. zone19 sub-block list size consistency
        // This check finds use in packing. In parsing, the consistency is always guaranteed by IABParser lib.
        if (numZone19SubBlocks_ != zone19SubBlocks_.size())
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorObjectZoneDefinition19SubblockCountConflict;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // Run Validate() on all zone19 sub-blocks
        for (iterZone19Subblock = zone19SubBlocks_.begin(); iterZone19Subblock != zone19SubBlocks_.end(); iterZone19Subblock++)
        {
            if (isToContinue = (*iterZone19Subblock)->Validate(iEvenHandler, iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

                
		return isToContinue;
	}

	// ****************************************************************************
	// IABAuthoringToolInfo class implementation
	// ****************************************************************************

	// Constructor implementation
	IABAuthoringToolInfo::IABAuthoringToolInfo() : IABElement(kIABElementID_AuthoringToolInfo)
	{
		authoringToolURI_.clear();
	}

	// IABAuthoringToolInfo::GetAuthoringToolInfo() implementation
	void IABAuthoringToolInfo::GetAuthoringToolInfo(const char* &oAuthoringToolURI) const
	{
		oAuthoringToolURI = authoringToolURI_.c_str();
	}

	// IABAuthoringToolInfo::SetAuthoringToolInfo() implementation
	iabError IABAuthoringToolInfo::SetAuthoringToolInfo(const char* iAuthoringToolURI)
	{
		authoringToolURI_.assign(iAuthoringToolURI);
		return kIABNoError;
	}

	// IABAuthoringToolInfo::EnablePacking() implementation
	void IABAuthoringToolInfo::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABAuthoringToolInfo::DisablePacking() implementation
	void IABAuthoringToolInfo::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABAuthoringToolInfo::IsIncludedForPacking() implementation
	bool IABAuthoringToolInfo::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABAuthoringToolInfo::Serialize() implementation
	iabError IABAuthoringToolInfo::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

        // Write authoring tool info to bitstream as a null terminated string (i.e. character sequence)
        // writeStringAppendNull() will add a null byte after the last character of the string.
        writeStringAppendNull(elementPayloadWriter_, authoringToolURI_);

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABAuthoringToolInfo::DeSerialize() implementation
	iabError IABAuthoringToolInfo::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;
		std::vector<uint8_t> parsedBytes;

		errorCode = DeSerializeHead(streamReader);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

        // Read authoring tool info string from the bitstream and the string is expected to be
        // terminated by a null byte. The null byte is removed before saving into authoringToolURI_.
        if (CMNSTRM_OK == readToNull(streamReader, authoringToolURI_) && streamReader.good())
        {
            return kIABNoError;
        }
		else
		{
			return kIABParserIABAuthoringToolInfoError;
		}
	}

    // IABAuthoringToolInfo::Validate() implementation
    bool IABAuthoringToolInfo::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

        iValidationIssue.id_ = kIssueID_AuthoringToolInfo;
        
		bool isToContinue = true;

        // Note: No validation checks against ST2098_2_2018 limits and constraints

        // Note: No additional checks for ST-429-18-2019 limits and constraints
        
        // DbyCinema checks on IABAuthoringToolInfo :
        // 1. IABAuthoringToolInfo is not allowed
        
        // Update issue variable and log
        iValidationIssue.isBeingValidated_ = kConstraints_set_DbyCinema;
        iValidationIssue.event_ = ErrorEvent;
        iValidationIssue.errorCode_ = kValidateErrorDolCinAuthoringToolInfoNotAnAllowedSubElement;
        
        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
        {
            return isToContinue;
        }
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // Note: No validation checks against IMF ST2098_2_2019 limits and constraints
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################


		return isToContinue;
	}

	// ****************************************************************************
	// IABUserData class implementation
	// ****************************************************************************

	// Constructor implementation
	IABUserData::IABUserData() : IABElement(kIABElementID_UserData)
	{
		userDataBytes_.clear();								// Default: empty

		uint8_t* userID = userID_;

		for (uint8_t i = 0; i < 16; i++)
		{
			*userID++ = 0;
		}
	}

	// IABUserData::GetUserID() implementation
	void IABUserData::GetUserID(uint8_t (&oUserID)[16]) const
	{
		// Copy out UserID UL
		const uint8_t* srcUserID = userID_;
		uint8_t* destUserID = oUserID;

		for (uint8_t i = 0; i < 16; i++)
		{
			*destUserID++ = *srcUserID++;
		}
	}

	// IABUserData::SetUserID() implementation
	iabError IABUserData::SetUserID(const uint8_t(&iUserID)[16])
	{
		// Copy in UserID UL
		const uint8_t* srcUserID = iUserID;
		uint8_t* destUserID = userID_;

		for (uint8_t i = 0; i < 16; i++)
		{
			*destUserID++ = *srcUserID++;
		}

		return kIABNoError;
	}

	// IABUserData::GetUserDataBytes() implementation
	void IABUserData::GetUserDataBytes(std::vector<uint8_t> &oUserDataBytes) const
	{
		oUserDataBytes = userDataBytes_;
	}

	// IABUserData::SetUserDataBytes() implementation
	iabError IABUserData::SetUserDataBytes(std::vector<uint8_t> iUserDataBytes)
	{
		userDataBytes_ = iUserDataBytes;
		return kIABNoError;
	}

	// IABUserData::EnablePacking() implementation
	void IABUserData::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABUserData::DisablePacking() implementation
	void IABUserData::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABUserData::IsIncludedForPacking() implementation
	bool IABUserData::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABUserData::Serialize() implementation
	iabError IABUserData::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Write user ID
		elementPayloadWriter_.write(&userID_[0], 16);						// 16 bytes

		// Expect size of user data block does not exceed 32-bit range
		uint32_t userDataSize = static_cast<uint32_t>(userDataBytes_.size());
		elementPayloadWriter_.write(&userDataBytes_[0], userDataSize);		// write user data

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABUserData::DeSerialize() implementation
	iabError IABUserData::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;

		errorCode = DeSerializeHead(streamReader);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

		// Read User ID, 16-byte SMPTE label.
		if (CMNSTRM_OK != streamReader.read(&userID_[0], 16))
        {
            return kIABParserIABUserDataError;
        }

		// Calculating length of user data bytes (in byte) from parsed "elementSize_"
		uint32_t userDataBytesLength = static_cast<uint32_t>(elementSize_ - 16);			// length = element size - 16, latter being userID_

		userDataBytes_.clear();											// clear buffer first
		userDataBytes_.reserve(userDataBytesLength);					// pre-allocate expected number of bytes

		// Read user data bytes
		uint8_t* parsedUserDataBytes = new uint8_t[userDataBytesLength];
		if (CMNSTRM_OK != streamReader.read(parsedUserDataBytes, userDataBytesLength))
        {
            return kIABParserIABUserDataError;
        }
        
		userDataBytes_.assign(parsedUserDataBytes, (parsedUserDataBytes + userDataBytesLength));

		// Delete temp buffer
		delete[] parsedUserDataBytes;

		if (streamReader.good())
		{
			return kIABNoError;
		}
		else
		{
			return kIABParserIABUserDataError;
		}
	}
    
    // IABUserData::Validate() implementation
   bool IABUserData::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

        iValidationIssue.id_ = kIssueID_UserData;
        
		bool isToContinue = true;

        // Note: No validation checks against ST2098_2_2018 limits and constraints
        
        // ST429_18_2019 checks on IABUserData :
        // 1. UserData is not allowed
        
        if (!ConstraintSet_ST429_18_2019.allowUserData_)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorUserDataNotAnAllowedSubElement;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }

        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################

        // Note: No validation checks against IMF ST2098_2_2019 limits and constraints
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

		return isToContinue;
   }

	// ****************************************************************************
	// IABAudioDataDLC class implementation
	// ****************************************************************************

	// Constructor implementation, for 48kHz sample rate only
	IABAudioDataDLC::IABAudioDataDLC(IABFrameRateType iFrameRateCode,
		iabError& oConstructionReturnError) :
		IABElement(kIABElementID_AudioDataDLC)
    {
		oConstructionReturnError = kIABNoError;				// Init to no error for construction

		audioDataID_ = 0;
		DLCSize_ = 0;
		frameRateCode_ = iFrameRateCode;
		DLCSampleRate_ = dlc::eSampleRate_48000;
		decodedPCM_ = nullptr;

		// Set frame sample count per specification
		sampleCount_ = GetIABNumFrameSamples(iFrameRateCode, kIABSampleRate_48000Hz);

		// Set up DLC subblock
		if (SetupDLCSubblock() != kIABNoError)
		{
			oConstructionReturnError = kIABElementDLCMalConstructedError;
		}

		// Set up simple encoder
		if (dlcSimpleEncoder_.setup(DLCSampleRate_, static_cast<dlc::FrameRate>(frameRateCode_))
			!= dlc::SimpleEncoder::StatusCode_OK)
		{
			oConstructionReturnError = kIABElementDLCMalConstructedError;
		}
	}

	// Constructor implementation, allowing sample rate input, 48k or 96k
	IABAudioDataDLC::IABAudioDataDLC(IABFrameRateType iFrameRateCode
		, IABSampleRateType iSampleRate
		, iabError& oConstructionReturnError) :
		IABElement(kIABElementID_AudioDataDLC)
	{
		oConstructionReturnError = kIABNoError;				// Init to no error for construction

		audioDataID_ = 0;
		DLCSize_ = 0;
		frameRateCode_ = iFrameRateCode;
		decodedPCM_ = nullptr;
		
		// Set frame sample count per specification
		sampleCount_ = GetIABNumFrameSamples(iFrameRateCode, iSampleRate);

		// Set up DLC subblock
		if (SetupDLCSubblock() != kIABNoError)
		{
			oConstructionReturnError = kIABElementDLCMalConstructedError;
		}

		// Set up DLCSampleRate_. This call also sets up dlcSimpleEncoder_
		if (SetDLCSampleRate(iSampleRate) != kIABNoError)
		{
			oConstructionReturnError = kIABElementDLCMalConstructedError;
		}
	}

	// Destructor
	IABAudioDataDLC::~IABAudioDataDLC()
	{
		delete[] decodedPCM_;
	}

	// IABAudioDataDLC::GetAudioDataID() implementation
    void IABAudioDataDLC::GetAudioDataID(IABAudioDataIDType &oAudioDataID) const
    {
        oAudioDataID = audioDataID_;
    }
    
    // IABAudioDataDLC::SetAudioDataID() implementation
    iabError IABAudioDataDLC::SetAudioDataID(IABAudioDataIDType iAudioDataID)
    {
        audioDataID_ = iAudioDataID;
        return kIABNoError;
    }
    
    // IABAudioDataDLC::GetDLCSize() implementation
    void IABAudioDataDLC::GetDLCSize(uint16_t &oDLCSize) const
    {
        oDLCSize = DLCSize_;
    }
    
    // IABAudioDataDLC::SetDLCSize() implementation
    iabError IABAudioDataDLC::SetDLCSize(uint16_t iDLCSize)
    {
        DLCSize_ = iDLCSize;
        return kIABNoError;
    }

	// IABAudioDataDLC::GetDLCSampleRate() implementation
	iabError IABAudioDataDLC::GetDLCSampleRate(IABSampleRateType &oDLCSampleRate) const
	{
		if (DLCSampleRate_ == dlc::eSampleRate_48000)
		{
			oDLCSampleRate = kIABSampleRate_48000Hz;
		}
		else if (DLCSampleRate_ == dlc::eSampleRate_96000)
		{
			oDLCSampleRate = kIABSampleRate_96000Hz;
		}
		else
		{
			return kIABGeneralError;
		}

		return kIABNoError;
	}

	// IABAudioDataDLC::SetDLCSampleRate() implementation
	iabError IABAudioDataDLC::SetDLCSampleRate(IABSampleRateType iDLCSampleRate)
	{
		if (iDLCSampleRate == kIABSampleRate_48000Hz)
		{
			DLCSampleRate_ = dlc::eSampleRate_48000;
			
		}
		else if (iDLCSampleRate == kIABSampleRate_96000Hz)
		{
			DLCSampleRate_ = dlc::eSampleRate_96000;
		}
		else
		{
			return kIABGeneralError;
		}

		// Set up (reset) simple encoder to updated sample rate.
		if (dlcSimpleEncoder_.setup(DLCSampleRate_, static_cast<dlc::FrameRate>(frameRateCode_))
			!= dlc::SimpleEncoder::StatusCode_OK)
		{
			return kIABGeneralError;
		}

		return kIABNoError;
	}

	// IABAudioDataDLC::EncodeMonoPCMToDLC() implementation
	iabError IABAudioDataDLC::EncodeMonoPCMToDLC(const int32_t* iSamples, uint32_t iSampleCount)
	{
		dlc::SimpleEncoder::StatusCode encoderErrorCode = dlc::SimpleEncoder::StatusCode_OK;

		// Check input parameter for encoding samples into DLC
		if (iSamples == nullptr)
		{
			return kIABBadArgumentsError;
		}

		// Check frame sample count (ie frame size) against setup, per ST2098-2
		if (iSampleCount != sampleCount_)
		{
			return kIABArgumentIncorrectDLCSampleCount;
		}

		// Encode
		encoderErrorCode = dlcSimpleEncoder_.encode_noexcept(iSamples, iSampleCount, DLCSampleRate_, audioData_);

		if (encoderErrorCode != dlc::SimpleEncoder::StatusCode_OK)
		{
			return kIABPackerDLCError;
		}

		return kIABNoError;
	}

	// IABAudioDataDLC::EnablePacking() implementation
	void IABAudioDataDLC::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABAudioDataDLC::DisablePacking() implementation
	void IABAudioDataDLC::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABAudioDataDLC::IsIncludedForPacking() implementation
	bool IABAudioDataDLC::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABAudioDataDLC::Serialize() implementation
	iabError IABAudioDataDLC::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// uint32_t dlcOstreamPosition = 0;
		// uint32_t packedDLCSizeInBytes = 0;
		// dlcOstreamPosition = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());

		// since DLCSize can only be determined AFTER packing DLC payload, another internal 
		// bitstream writer is needed, to accommodate another instance of non-sequential
		// packing requirement.
		std::stringstream	dlcHeadBuffer(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		StreamWriter		dlcHeadWriter(dlcHeadBuffer);
		// Both writer and buffer are in init state.
        // TODO: Review XCode analyzer warning
		write(dlcHeadWriter, Plex<8>(audioDataID_));						// AudioDataID, identifying this DLC element

		// "DLCSize_" is to be determined by "payload" size, to be determined and written below.
		// (in the case of DLC element, it is actually "DLC payload" size".
		// The element payload is the sum of DLC payload size and DLC head size.

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		// Calling method for writing Plex encoded field.
		elementPayloadWriter_.write(audioData_.getSampleRate(), 2);
		elementPayloadWriter_.write(audioData_.getShiftBits(), 5);

		// Prediction Region for 48 kHz
		elementPayloadWriter_.write(audioData_.getNumPredRegions48(), 2);

		// 19/10/2017: getNumPredRegions48 probably == 0, for now.
		for (uint8_t n = 0; n < audioData_.getNumPredRegions48(); n++)
		{
			elementPayloadWriter_.write(audioData_.getPredRegion48(n).getRegionLength(), 4);
			elementPayloadWriter_.write(audioData_.getPredRegion48(n).getOrder(), 5);

			for (uint8_t m = 1; m <= audioData_.getPredRegion48(n).getOrder(); m++)				// Changed on Dec 11, 2017 (Currently following C indexing, but spec says "m = 1; m <= order; m++". TODO: Need confirming with Pierre.)
			{
				elementPayloadWriter_.write(audioData_.getPredRegion48(n).getKCoeff()[m], 10);
			}
		}

		// Coded residual samples for 48kHz
		// dlcNumDLCSubBlocks = audioData_.getNumDLCSubBlocks();

		for (uint8_t n = 0; n < audioData_.getNumDLCSubBlocks(); n++)
		{
			elementPayloadWriter_.write(audioData_.getDLCSubBlock48(n)->getCodeType(), 1);

			// If encoded as PCM
			if (audioData_.getDLCSubBlock48(n)->getCodeType() == dlc::eCodeType_PCM_CODE_TYPE)
			{
				dlc::PCMResidualsSubBlock* pcmResidualsSubBlock = dynamic_cast<dlc::PCMResidualsSubBlock*>(audioData_.getDLCSubBlock48(n));

				uint5_t residualBitDepth = pcmResidualsSubBlock->getBitDepth();
				elementPayloadWriter_.write(residualBitDepth, 5);

				if (residualBitDepth != 0)
				{
					uint32_t subBlockSize = pcmResidualsSubBlock->getSize();					// Get residual block size
					int32_t* pResiduals = pcmResidualsSubBlock->getResiduals();					// Get pointer to beginning of residual samples
					int32_t residualValue = 0;													// Residual value, as-is retrieved
					uint32_t residualMagnitude = 0;												// Stores residual magnitude
					uint1_t residualSign = 0;													// 1-bit sign, 0 == postive, 1 == negative

					for (uint32_t i = 0; i < subBlockSize; i++)
					{
						residualValue = *pResiduals++;
						if (residualValue >= 0)
						{
							residualMagnitude = static_cast<uint32_t>(residualValue);
							residualSign = 0;
						}
						else
						{
							residualMagnitude = static_cast<uint32_t>(-residualValue);
							residualSign = 1;
						}

						elementPayloadWriter_.write(residualMagnitude, residualBitDepth);		// Write the residual magnitude
						if (residualValue != 0)
						{
							elementPayloadWriter_.write(residualSign, 1);						// Only write sign if residual value is not 0
						}

					}
				}
			}
			// If encoded using Rice/Golomb entropy compression
			else
			{
				// TODO: not implemented for version 1
				return kIABNotImplementedError;
			}
		}

		// Further to write encoded data for the upper spectral part of 96kHz
		// Only if encoded DLC data indicates presence of 96kHz encoded data

		if (audioData_.getSampleRate() == dlc::eSampleRate_96000)
		{
			// Prediction Region for 96 kHz
			elementPayloadWriter_.write(audioData_.getNumPredRegions96(), 2);

			// 19/10/2017: getNumPredRegions96 probably == 0, for now.
			for (uint8_t n = 0; n < audioData_.getNumPredRegions96(); n++)
			{
				elementPayloadWriter_.write(audioData_.getPredRegion96(n).getRegionLength(), 4);
				elementPayloadWriter_.write(audioData_.getPredRegion96(n).getOrder(), 5);

				for (uint8_t m = 1; m <= audioData_.getPredRegion96(n).getOrder(); m++)				// Changed on Dec 11, 2017 (Currently following C indexing, but spec says "m = 1; m <= order; m++". TODO: Need confirming with Pierre.)
				{
					elementPayloadWriter_.write(audioData_.getPredRegion96(n).getKCoeff()[m], 10);
				}
			}

			// Coded residual samples for 96kHz
			for (uint8_t n = 0; n < audioData_.getNumDLCSubBlocks(); n++)
			{
				elementPayloadWriter_.write(audioData_.getDLCSubBlock96(n)->getCodeType(), 1);

				// If encoded as PCM
				if (audioData_.getDLCSubBlock96(n)->getCodeType() == dlc::eCodeType_PCM_CODE_TYPE)
				{
					dlc::PCMResidualsSubBlock* pcmResidualsSubBlock = dynamic_cast<dlc::PCMResidualsSubBlock*>(audioData_.getDLCSubBlock96(n));

					uint5_t residualBitDepth = pcmResidualsSubBlock->getBitDepth();
					elementPayloadWriter_.write(residualBitDepth, 5);

					if (residualBitDepth != 0)
					{
						uint32_t subBlockSize = pcmResidualsSubBlock->getSize();					// Get residual block size
						int32_t* pResiduals = pcmResidualsSubBlock->getResiduals();					// Get pointer to beginning of residual samples
						int32_t residualValue = 0;													// Residual value, as-is retrieved
						uint32_t residualMagnitude = 0;												// Stores residual magnitude
						uint1_t residualSign = 0;													// 1-bit sign, 0 == postive, 1 == negative

						for (uint32_t i = 0; i < subBlockSize; i++)
						{
							residualValue = *pResiduals++;
							if (residualValue >= 0)
							{
								residualMagnitude = static_cast<uint32_t>(residualValue);
								residualSign = 0;
							}
							else
							{
								residualMagnitude = static_cast<uint32_t>(-residualValue);
								residualSign = 1;
							}

							elementPayloadWriter_.write(residualMagnitude, residualBitDepth);		// Write the residual magnitude
							if (residualValue != 0)
							{
								elementPayloadWriter_.write(residualSign, 1);						// Only write sign if residual value is not 0
							}

						}
					}
				}
				// If encoded using Rice/Golomb entropy compression
				else
				{
					// TODO: not implemented for version 1
					return kIABNotImplementedError;
				}
			}
		}

		elementPayloadWriter_.align();														// Align to byte

		// Now ready to retrieve and set DLCSize_.
		DLCSize_ = static_cast<uint16_t>(elementPayloadBuffer_.tellp());
		dlcHeadWriter.write(DLCSize_, 16);

		// Retrieve DLC head length, part of Element payload length
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(dlcHeadBuffer.tellp());
		// Add element payload buffer length
		packedPayloadLength += static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		// The sum is the element payload length
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first,
		// followed then by dlcHeadBuffer
		// and finally followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << dlcHeadBuffer.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABAudioDataDLC::DecodeDLCToMonoPCMInternal() implementation
	iabError IABAudioDataDLC::DecodeDLCToMonoPCMInternal(uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate)
	{
		dlc::FullDecoder::StatusCode decoderErrorCode = dlc::FullDecoder::StatusCode_OK;

		// Check requested sampling rate
		if ((iDecodeSampleRate != kIABSampleRate_96000Hz)
			&& (iDecodeSampleRate != kIABSampleRate_48000Hz))
		{
			return kIABParserDLCDecodingError;
		}

		// Can't decode to 96k from a 48k DLC element
		if ((iDecodeSampleRate == kIABSampleRate_96000Hz)
			&& (DLCSampleRate_ != dlc::eSampleRate_96000))
		{
			return kIABParserDLCDecodingError;
		}

		// Check against number of samples contained in DLC element
		// (Note, this number is also equal to sampleCount_. )
		//
		uint32_t dlcNumPCMSamples = dlcNumDLCSubBlocks_ * dlcBlockLength_;

		// dlcBlockLength_ is initilized based on 48k sampling rate. As a result.
		// dlcNumPCMSamples is caculated based on 48k at this point.
		// 
		// For 96k DLC element, and when decoding to 96k output, do (dlcNumPCMSamples *= 2).
		// Otherwise, decoded sampling rate remains at 48k
		if ((DLCSampleRate_ == dlc::eSampleRate_96000)
			&& (iDecodeSampleRate == kIABSampleRate_96000Hz))
		{
			dlcNumPCMSamples *= 2;					// double up for 96k
		}

		// Check iSampleCount against
		// 1. ST2098-2, and also
		// 2. requested sample rate
		// Requested iSampleCount must be euqal to resolved decoded samples
		//
		if (iSampleCount != dlcNumPCMSamples)
		{
			return kIABArgumentIncorrectDLCSampleCount;
		}

		// Allocate memory to hold decoded PCM samples, if not already
		if (!decodedPCM_)
		{
			// Allocate
			decodedPCM_ = new int32_t[dlcNumPCMSamples];
		}

		// decode to decodedPCM_
		if (iDecodeSampleRate == kIABSampleRate_48000Hz)
		{
			decoderErrorCode = dlcFullDecoder_.decode_noexcept(decodedPCM_, dlcNumPCMSamples, dlc::eSampleRate_48000, audioData_);
		}
		else if (iDecodeSampleRate == kIABSampleRate_96000Hz)
		{
			decoderErrorCode = dlcFullDecoder_.decode_noexcept(decodedPCM_, dlcNumPCMSamples, dlc::eSampleRate_96000, audioData_);
		}
		else
		{
			return kIABParserDLCDecodingError;
		}

		if (decoderErrorCode != dlc::FullDecoder::StatusCode_OK)
		{
			return kIABParserDLCDecodingError;
		}
		

		return kIABNoError;
	}

	// IABAudioDataDLC::DecodeDLCToMonoPCM() implementation
	iabError IABAudioDataDLC::DecodeDLCToMonoPCM(int32_t* oSamples, uint32_t iSampleCount, IABSampleRateType iDecodeSampleRate)
	{
		// Check input parameter
		if (oSamples == nullptr)
		{
			return kIABBadArgumentsError;
		}

		iabError errorCode = kIABNoError;
		errorCode = DecodeDLCToMonoPCMInternal(iSampleCount, iDecodeSampleRate);

		if (errorCode)
		{
			return errorCode;
		}

		// Copy to client buffer
		int32_t* srcPointer = decodedPCM_;

		for (uint32_t i = 0; i < iSampleCount; i++)
		{
			oSamples[i] = srcPointer[i];
		}
        
        return kIABNoError;        
	}

	// IABAudioDataDLC::GetDecodedSampleBuffer() implementation
	int32_t* IABAudioDataDLC::GetDecodedSampleBuffer()
	{
		return decodedPCM_;
	}

	// IABAudioDataDLC::DeSerialize() implementation
	iabError IABAudioDataDLC::DeSerialize(StreamReader& streamReader)
	{
        if (kIABNoError != DeSerializeHead(streamReader))
        {
            return kIABParserIABDLCError;
        }
        
        // Ensure this element is an IAB DLC, do we want to check element size > 0 bytes?
        if (kIABElementID_AudioDataDLC != elementID_)
        {
            return kIABParserIABDLCError;
        }
 
        Plex<8> plex8Field = 0;
        uint16_t fixedLengthFieldMax16 = 0;
        uint8_t fixedLengthFieldMax8 = 0;
        
        // Read audio data ID
        if (CMNSTRM_OK != read(streamReader, plex8Field))
        {
            return kIABParserIABDLCError;
        }
        
        audioDataID_ = static_cast<IABAudioDataIDType>(plex8Field);
        
        // Read DLC payload size
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16))
        {
            return kIABParserIABDLCError;
        }

        DLCSize_ = fixedLengthFieldMax16;

        // Read sample rate
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABDLCError;
        }

        if (0 == fixedLengthFieldMax8)
        {
            DLCSampleRate_ = dlc::eSampleRate_48000;
        }
        else if (1 == fixedLengthFieldMax8)
        {
            DLCSampleRate_ = dlc::eSampleRate_96000;
        }
        else
        {
            // unsupported sample rate 
            return kIABParserIABDLCError;
        }
        
        // Set DLC subblock parameters
		if (SetupDLCSubblock() != kIABNoError)
		{
			return kIABParserIABDLCError;
		}

        uint8_t shiftBits = 0;
        
        // Read 5-bit shift bit field
        if (CMNSTRM_OK != streamReader.read(shiftBits, 5))
        {
            return kIABParserIABDLCError;
        }
        
        // Set DLC audio shift bits
        audioData_.setShiftBits(shiftBits);
       
        // ********************************
        // 48KHz predictor information
        // ********************************
        
        // Read 2-bit number of prediction regions for 48K
        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
        {
            return kIABParserIABDLCError;
        }

        dlcNumPredRegions48_ = static_cast<uint2_t>(fixedLengthFieldMax8);
        
        // Set DLC audio number of prediction regions
        audioData_.setNumPredRegions48(dlcNumPredRegions48_);
        
        uint10_t predCoeffs[32] = {0}; // maximum 32 coefficients
        
        for (uint32_t n = 0; n < static_cast<uint32_t>(dlcNumPredRegions48_); n++)
        {
            // Read 4-bit region length
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 4))
            {
                return kIABParserIABDLCError;
            }
            
            // Set region length
            audioData_.getPredRegion48(n).setRegionLength(fixedLengthFieldMax8);
            
            // Read 5-bit region order
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 5))
            {
                return kIABParserIABDLCError;
            }
            
            // Set region order
            audioData_.getPredRegion48(n).setOrder(fixedLengthFieldMax8);
            
            for (uint8_t m = 1; m <= fixedLengthFieldMax8; m++)
            {
                // Read 10-bit coefficient
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10))
                {
                    return kIABParserIABDLCError;
                }

                predCoeffs[m] = fixedLengthFieldMax16;
            }
            
            // Set coefficients
            audioData_.getPredRegion48(n).setKCoeff(&predCoeffs[0]);
            
        }

        // ********************************
        // 48KHz residuals
        // ********************************
                
        for (uint8_t n = 0; n < audioData_.getNumDLCSubBlocks(); n++)
        {
            // Read 1-bit code type
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
            {
                return kIABParserIABDLCError;
            }
            
            // If encoded as PCM
            if (fixedLengthFieldMax8 == dlc::eCodeType_PCM_CODE_TYPE)
            {
                // Initialise residual block to PCM type
                audioData_.initDLCSubBlock48(n, dlc::eCodeType_PCM_CODE_TYPE, dlcBlockLength_);

                dlc::PCMResidualsSubBlock* pcmResidualsSubBlock = dynamic_cast<dlc::PCMResidualsSubBlock*>(audioData_.getDLCSubBlock48(n));
                
                // Read 5-bit bit depth
                uint8_t bitDepth = 0;
                if (CMNSTRM_OK != streamReader.read(bitDepth, 5))
                {
                    return kIABParserIABDLCError;
                }

                // Set bit depth
                pcmResidualsSubBlock->setBitDepth(bitDepth);
                
                uint32_t subBlockSize = pcmResidualsSubBlock->getSize();					// Get residual block size
                int32_t* pResiduals = pcmResidualsSubBlock->getResiduals();					// Get pointer to beginning of residual samples
                int32_t residualValue = 0;													// Residual value, as-is retrieved
                uint8_t residualSign = 0;													// 1-bit sign, 0 == postive, 1 == negative
                
                for (uint32_t i = 0; i < subBlockSize; i++)
                {
					if (bitDepth == 0)
					{
						residualValue = 0;
					}
					else
                    {
                        // Read residual
                        if (CMNSTRM_OK != streamReader.read(residualValue, bitDepth))
                        {
                            return kIABParserIABDLCError;
                        }
                        
                        if (residualValue != 0)
                        {
                            // Read sign bit
                            if (CMNSTRM_OK != streamReader.read(residualSign, 1))
                            {
                                return kIABParserIABDLCError;
                            }

                            if (1 == residualSign)
                            {
                                // invert value
                                residualValue *= -1;
                            }
                        }
                    }

                    *pResiduals++ = residualValue;
                }
            }
            // If encoded using Rice/Golomb entropy compression
            else
            {
                // Initialise residual block to Rice type
                audioData_.initDLCSubBlock48(n, dlc::eCodeType_RICE_CODE_TYPE, dlcBlockLength_);

                dlc::RiceResidualsSubBlock* riceResidualsSubBlock = dynamic_cast<dlc::RiceResidualsSubBlock*>(audioData_.getDLCSubBlock48(n));
                
                // Read 5-bit rice remainder length
                uint8_t riceRemBits = 0;
                if (CMNSTRM_OK != streamReader.read(riceRemBits, 5))
                {
                    return kIABParserIABDLCError;
                }
                
                // Set remainder bits
                riceResidualsSubBlock->setRiceRemBits(riceRemBits);
                
                uint32_t subBlockSize = riceResidualsSubBlock->getSize();					// Get residual block size
                dlc::RiceResidual* pResiduals = riceResidualsSubBlock->getResiduals();		// Get pointer to beginning of residual samples
                
                for (uint32_t i = 0; i < subBlockSize; i++)
                {
                    int32_t remainder = 0;                          // Residual value, as-is retrieved
                    char residualSign = 1;							// 1-bit sign, 0 == postive (maps to 1 for DLC lib), 1 == negative (maps to -1 for DLC lib)
                    int32_t quotient = 0;
                    
                    if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                    {
                        return kIABParserIABDLCError;
                    }

                    while (1 == fixedLengthFieldMax8)
                    {
                        quotient++;
                        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                        {
                            return kIABParserIABDLCError;
                        }
                    }

					if (riceRemBits == 0)
					{
						remainder = 0;
					}
					else
                    {
                        // Read rice remainder
                        if (CMNSTRM_OK != streamReader.read(remainder, riceRemBits))
                        {
                            return kIABParserIABDLCError;
                        }
                    }
                    
                    if ((remainder != 0) || (quotient > 0))
                    {
                        // Read sign bit, 0 maps to +1, 1 maps to -1
                        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                        {
                            return kIABParserIABDLCError;
                        }

                        if (1 == fixedLengthFieldMax8)
                        {
                            residualSign = -1;
                        }
                    }
                    
                    // save residual
                    pResiduals->setQuotient(quotient);
                    pResiduals->setSign(residualSign);
                    pResiduals++->setRemainder(remainder);
                }
            }
        }
        
        // ********************************
        // 96KHz predictor information
        // ********************************
        
        if (DLCSampleRate_ == dlc::eSampleRate_96000)
        {
            // Read 2-bit number of prediction regions for 96K
            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 2))
            {
                return kIABParserIABDLCError;
            }

            dlcNumPredRegions96_ = static_cast<uint2_t>(fixedLengthFieldMax8);
            
            // Set DLC audio number of prediction regions for 96KHz
            audioData_.setNumPredRegions96(dlcNumPredRegions96_);
            
            for (uint32_t n = 0; n < static_cast<uint32_t>(dlcNumPredRegions96_); n++)
            {
                // Read 4-bit region length
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 4))
                {
                    return kIABParserIABDLCError;
                }
                
                // Set region length
                audioData_.getPredRegion96(n).setRegionLength(fixedLengthFieldMax8);
                
                // Read 5-bit region order
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 5))
                {
                    return kIABParserIABDLCError;
                }
                
                // Set region order
                audioData_.getPredRegion96(n).setOrder(fixedLengthFieldMax8);
                
                for (uint8_t m = 1; m <= fixedLengthFieldMax8; m++)
                {
                    // Read 10-bit coefficient
                    if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax16, 10))
                    {
                        return kIABParserIABDLCError;
                    }

                    predCoeffs[m] = fixedLengthFieldMax16;
                }
                
                // Set coefficients
                audioData_.getPredRegion96(n).setKCoeff(&predCoeffs[0]);
                
            }
            
            // ********************************
            // 96KHz residuals
            // ********************************
            
            for (uint8_t n = 0; n < audioData_.getNumDLCSubBlocks(); n++)
            {
                // Read 1-bit code type
                if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                {
                    return kIABParserIABDLCError;
                }
                
                // If encoded as PCM
                if (fixedLengthFieldMax8 == dlc::eCodeType_PCM_CODE_TYPE)
                {
                    // Initialise residual block to PCM type
                    audioData_.initDLCSubBlock96(n, dlc::eCodeType_PCM_CODE_TYPE, (dlcBlockLength_ * 2));

                    dlc::PCMResidualsSubBlock* pcmResidualsSubBlock = dynamic_cast<dlc::PCMResidualsSubBlock*>(audioData_.getDLCSubBlock96(n));
                    
                    // Read 5-bit bit depth
                    uint8_t bitDepth = 0;
                    if (CMNSTRM_OK != streamReader.read(bitDepth, 5))
                    {
                        return kIABParserIABDLCError;
                    }
                    
                    // Set bit depth
                    pcmResidualsSubBlock->setBitDepth(bitDepth);
                    
                    uint32_t subBlockSize = pcmResidualsSubBlock->getSize();					// Get residual block size
                    int32_t* pResiduals = pcmResidualsSubBlock->getResiduals();					// Get pointer to beginning of residual samples
                    int32_t residualValue = 0;													// Residual value, as-is retrieved
                    uint8_t residualSign = 0;													// 1-bit sign, 0 == postive, 1 == negative
                    
                    for (uint32_t i = 0; i < subBlockSize; i++)
                    {

						if (bitDepth == 0)
						{
							residualValue = 0;
						}
						else
						{
							// Read residual
							if (CMNSTRM_OK != streamReader.read(residualValue, bitDepth))
                            {
                                return kIABParserIABDLCError;
                            }

							if (residualValue != 0)
							{
								// Read sign bit
								if (CMNSTRM_OK != streamReader.read(residualSign, 1))
                                {
                                    return kIABParserIABDLCError;
                                }

                                if (1 == residualSign)
								{
									// invert value
									residualValue *= -1;
								}
							}
						}

                        *pResiduals++ = residualValue;
                    }
                }
                // If encoded using Rice/Golomb entropy compression
                else
                {
                    // Note that this block of 96KHz code has not beed tested since there is no 96KHz bitstream available
                    // Initialise residual block to Rice type
                    audioData_.initDLCSubBlock96(n, dlc::eCodeType_RICE_CODE_TYPE, (dlcBlockLength_ * 2));
                    
                    dlc::RiceResidualsSubBlock* riceResidualsSubBlock = dynamic_cast<dlc::RiceResidualsSubBlock*>(audioData_.getDLCSubBlock96(n));
                    
                    // Read 5-bit rice remainder length
                    uint8_t riceRemBits = 0;
                    if (CMNSTRM_OK != streamReader.read(riceRemBits, 5))
                    {
                        return kIABParserIABDLCError;
                    }
                    
                    // Set remainder bits
                    riceResidualsSubBlock->setRiceRemBits(riceRemBits);
                    
                    uint32_t subBlockSize = riceResidualsSubBlock->getSize();					// Get residual block size
                    dlc::RiceResidual* pResiduals = riceResidualsSubBlock->getResiduals();		// Get pointer to beginning of residual samples
                    
                    for (uint32_t i = 0; i < subBlockSize; i++)
                    {
                        int32_t remainder = 0;                          // Residual value, as-is retrieved
                        char residualSign = 1;							// 1-bit sign, 0 == postive (maps to 1 for DLC lib), 1 == negative (maps to -1 for DLC lib)
                        int32_t quotient = 0;
                        
                        if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                        {
                            return kIABParserIABDLCError;
                        }

                        while (1 == fixedLengthFieldMax8)
                        {
                            quotient++;
                            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                            {
                                return kIABParserIABDLCError;
                            }
                        }
                        
						if (riceRemBits == 0)
						{
							remainder = 0;
						}
						else
                        {
                            // Read rice remainder
                            if (CMNSTRM_OK != streamReader.read(remainder, riceRemBits))
                            {
                                return kIABParserIABDLCError;
                            }
                        }
                        
                        if ((remainder != 0) || (quotient > 0))
                        {
                            // Read sign bit, 0 maps to +1, 1 maps to -1
                            if (CMNSTRM_OK != streamReader.read(fixedLengthFieldMax8, 1))
                            {
                                return kIABParserIABDLCError;
                            }

                            if (1 == fixedLengthFieldMax8)
                            {
                                residualSign = -1;
                            }
                        }
                        
                        // save residual
                        pResiduals->setQuotient(quotient);
                        pResiduals->setSign(residualSign);
                        pResiduals++->setRemainder(remainder);
                    }
                }
            }
        }   // 96KHz
        
        if (CMNSTRM_OK == streamReader.align())   // byte alignment
        {
            return kIABNoError;
        }
		else
		{
			return kIABParserIABDLCError;
		}
	}
    
    iabError IABAudioDataDLC::SetupDLCSubblock()
    {
        audioData_.setSampleRate(DLCSampleRate_);

        switch (frameRateCode_)
        {
			case kIABFrameRate_24FPS:
                dlcNumDLCSubBlocks_ = 10;
                dlcBlockLength_ = 200;
				break;

            case kIABFrameRate_25FPS:
                dlcNumDLCSubBlocks_ = 10;
                dlcBlockLength_ = 192;
                break;
                
            case kIABFrameRate_30FPS:
                dlcNumDLCSubBlocks_ = 8;
                dlcBlockLength_ = 200;
                break;
                
            case kIABFrameRate_48FPS:
                dlcNumDLCSubBlocks_ = 5;
                dlcBlockLength_ = 200;
                break;

            case kIABFrameRate_50FPS:
                dlcNumDLCSubBlocks_ = 5;
                dlcBlockLength_ = 192;
                break;
                
            case kIABFrameRate_60FPS:
                dlcNumDLCSubBlocks_ = 4;
                dlcBlockLength_ = 200;
                break;
                
            case kIABFrameRate_96FPS:
                dlcNumDLCSubBlocks_ = 5;
                dlcBlockLength_ = 100;
                break;
                
            case kIABFrameRate_100FPS:
                dlcNumDLCSubBlocks_ = 4;
                dlcBlockLength_ = 120;
                break;
                
            case kIABFrameRate_120FPS:
                dlcNumDLCSubBlocks_ = 4;
                dlcBlockLength_ = 100;
                break;

			// DLC does not support kIABFrameRate_23_976FPS, or any other frame rate code
			case kIABFrameRate_23_976FPS:
			default:
				return kIABDataFieldInvalidDLC;
		}
        
        audioData_.setNumDLCSubBlocks(dlcNumDLCSubBlocks_);
        
        return kIABNoError;
    }

    // IABAudioDataDLC::Validate() implementation
   bool IABAudioDataDLC::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

		// Update iValidationIssue.id_ so that any issues reported from this Validate()
		// function is for AudioDataDLC element with audioDataID_ value.
		//
		iValidationIssue.id_ = audioDataID_;
        
        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// AudioDataID
		if (audioDataID_ == 0)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorAudioDataDLCAudioDataIDZero;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

		// General count var
		size_t foundCount = 0;

		// DLC sample rate
		IABSampleRateType dlcSampleRate;
		GetDLCSampleRate(dlcSampleRate);

		foundCount = ConstraintSet_ST2098_2_2018.validSampleRates_.count(dlcSampleRate);

		// If it is not uniquely found
		if (foundCount != 1)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorAudioDataDLCUnsupportedSampleRate;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}

        // #################################################
        // Cinema constraint sets validation ends
        // #################################################
        
        
        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // AudioDataID
        if (audioDataID_ == 0)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorAudioDataDLCAudioDataIDZero;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        foundCount = ConstraintSet_IMF_ST2098_2_2019.validSampleRates_.count(dlcSampleRate);
        
        // If it is not uniquely found
        if (foundCount != 1)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorAudioDataDLCUnsupportedSampleRate;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        // ST2098_2_2019 checks completed, change validation constriant set to kConstraints_set_IMF_ST2067_201_2019
        // Note that ST2067_201_2019 is a super set of ST2098_2_2019 and the preceding ST2098_2_2019 checks are required
        // to validate bitstream against ST2067_201_2019
        
        // AudioDataDLC is not an allowed element type for ST2067_201_2019
        // Update issue variable and log
        iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2067_201_2019;
        iValidationIssue.event_ = ErrorEvent;
        iValidationIssue.errorCode_ = kValidateErrorAudioDataDLCNotAnAllowedSubElement;
        
        if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
        {
            return isToContinue;
        }

        // #################################################
        // IMF constraint sets validation ends
        // #################################################

		return isToContinue;
    }

	// ****************************************************************************
	// IABAudioDataPCM class implementation
	// ****************************************************************************

	// Constructor implementation
	IABAudioDataPCM::IABAudioDataPCM(IABFrameRateType iFrameRateCode
		, IABSampleRateType iSampleRate
		, IABBitDepthType iBitDepth) : IABElement(kIABElementID_AudioDataPCM)
	{
		audioDataID_ = 0;
		frameRateCode_ = iFrameRateCode;
		sampleRateCode_ = iSampleRate;
		bitDepthCode_ = iBitDepth;
		unpackedPCM_ = nullptr;

		// Set per-sample byte count per iBitDepth
		numBytePerSample_ = 3;				// deafult to 24-bit / 3 bytes, unless ...
		if (bitDepthCode_ == kIABBitDepth_16Bit)
		{
			numBytePerSample_ = 2;
		}

		// Set frame sample count per specification
		sampleCount_ = GetIABNumFrameSamples(iFrameRateCode, iSampleRate);

		// Calculate buffer size
		totalByteCount_ = sampleCount_ * numBytePerSample_;

		// Allocate
		pcmBytes_ = new uint8_t[totalByteCount_];

		// Clear buffer
		memset(pcmBytes_, 0, totalByteCount_);
	}

	// Destructor
	IABAudioDataPCM::~IABAudioDataPCM()
	{
		delete[] pcmBytes_;
		delete[] unpackedPCM_;
	}

	// IABAudioDataPCM::GetAudioDataID() implementation
	void IABAudioDataPCM::GetAudioDataID(IABAudioDataIDType &oAudioDataID) const
	{
		oAudioDataID = audioDataID_;
	}

	// IABAudioDataPCM::SetAudioDataID() implementation
	iabError IABAudioDataPCM::SetAudioDataID(IABAudioDataIDType iAudioDataID)
	{
		audioDataID_ = iAudioDataID;
		return kIABNoError;
	}

	// IABAudioDataPCM::GetPCMFrameRate() implementation
	IABFrameRateType IABAudioDataPCM::GetPCMFrameRate() const
	{
		return frameRateCode_;
	}

	// IABAudioDataPCM::GetPCMSampleRate() implementation
	IABSampleRateType IABAudioDataPCM::GetPCMSampleRate() const
	{
		return sampleRateCode_;
	}

	// IABAudioDataPCM::GetPCMBitDepth() implementation
	IABBitDepthType IABAudioDataPCM::GetPCMBitDepth() const
	{
		return bitDepthCode_;
	}

	// IABAudioDataPCM::GetPCMSampleCount() implementation
	uint32_t IABAudioDataPCM::GetPCMSampleCount() const
	{
		return sampleCount_;
	}

	// IABAudioDataPCM::PackMonoSamplesToPCM() implementation
	iabError IABAudioDataPCM::PackMonoSamplesToPCM(const int32_t* iSamples, uint32_t iSampleCount)
	{
		// Check input parameter for packing PCM samples
		if (iSamples == nullptr)
		{
			return kIABBadArgumentsError;
		}

		// Check frame sample count (ie frame size) against setup, per ST2098-2
		if (iSampleCount != sampleCount_)
		{
			return kIABArgumentIncorrectPCMSampleCount;
		}

		// Packing PCM samples to sequence of bytes
		// Little endian format
		const int32_t* packSourceSample = iSamples;
		uint8_t *packDestination = pcmBytes_;

		// Clear pcmBytes_ buffer packing PCM samples into it
		memset(pcmBytes_, 0, totalByteCount_);

		// 24-bit packing
		if (bitDepthCode_ == kIABBitDepth_24Bit)
		{
			for (uint32_t i = 0; i < iSampleCount; i++)
			{
				*(packDestination++) = (*packSourceSample >> 8) & 0x000000FF;			// 1st: LSB of 24-bit sample
				*(packDestination++) = (*packSourceSample >> 16) & 0x000000FF;			// mid byte of 24-bit sample
				*(packDestination++) = (*packSourceSample >> 24) & 0x000000FF;			// last: MSB of 24-bit sample

				packSourceSample++;														// Next PCM sample
			}
		}
		else if (bitDepthCode_ == kIABBitDepth_16Bit)
		{
			// 16-bit
			for (uint32_t i = 0; i < iSampleCount; i++)
			{
				*(packDestination++) = (*packSourceSample >> 16) & 0x000000FF;			// 1st: LSB of 16-bit sample
				*(packDestination++) = (*packSourceSample >> 24) & 0x000000FF;			// last: MSB of 16-bit sample

				packSourceSample++;														// Next PCM sample
			}
		}
		else
		{
			return kIABPackerPCMError;
		}

		return kIABNoError;
	}

	// IABAudioDataPCM::UnpackPCMToMonoSamplesInternal() implementation
	iabError IABAudioDataPCM::UnpackPCMToMonoSamplesInternal(uint32_t iSampleCount)
	{
		// Check frame sample count (ie frame size) against setup, per ST2098-2
		if (iSampleCount != sampleCount_)
		{
			return kIABArgumentIncorrectPCMSampleCount;
		}

		// Allocate internal buffer and unpack, if not done already
		if (!unpackedPCM_)
		{
			unpackedPCM_ = new int32_t[sampleCount_];
        }
        // Unpacking little-endian byte sequence back to PCM samples
        uint8_t *unpackByteSource = pcmBytes_;
        int32_t* unpackedSampleDestination = unpackedPCM_;

        // 24-bit unpacking
        if (bitDepthCode_ == kIABBitDepth_24Bit)
        {
            for (uint32_t i = 0; i < iSampleCount; i++)
            {
                *unpackedSampleDestination = *(unpackByteSource++) << 8;					// 1st: save LSB of 24-bit sample
                *unpackedSampleDestination |= *(unpackByteSource++) << 16;					// OR (|) the mid byte of 24-bit sample
                *unpackedSampleDestination |= *(unpackByteSource++) << 24;					// OR (|) the MSB of 24-bit sample

                unpackedSampleDestination++;												// Increment pointer for next output PCM sample
            }
        }
        else if (bitDepthCode_ == kIABBitDepth_16Bit)
        {
            // 16-bit
            for (uint32_t i = 0; i < iSampleCount; i++)
            {
                *unpackedSampleDestination = *(unpackByteSource++) << 16;					// 1st: save LSB of 16-bit sample
                *unpackedSampleDestination |= *(unpackByteSource++) << 24;					// OR (|) the MSB of 16-bit sample

                unpackedSampleDestination++;												// Increment pointer for next output PCM sample
            }
        }
        else
        {
            return kIABParserPCMUnpackingError;
        }


		return kIABNoError;
	}

	// IABAudioDataPCM::UnpackPCMToMonoSamples() implementation
	iabError IABAudioDataPCM::UnpackPCMToMonoSamples(int32_t* oSamples, uint32_t iSampleCount)
	{
		// Check input parameter
		if (oSamples == nullptr)
		{
			return kIABBadArgumentsError;
		}

		iabError errorCode = kIABNoError;
		errorCode = UnpackPCMToMonoSamplesInternal(iSampleCount);

		if (errorCode)
		{
			return errorCode;
		}

		// Copy to client buffer
		int32_t* srcPointer = unpackedPCM_;

		for (uint32_t i = 0; i < iSampleCount; i++)
		{
			oSamples[i] = srcPointer[i];
		}

		return kIABNoError;
	}

	// IABAudioDataPCM::GetUnpackedSampleBuffer() implementation
	int32_t* IABAudioDataPCM::GetUnpackedSampleBuffer()
	{
		return unpackedPCM_;
	}

	// IABAudioDataPCM::EnablePacking() implementation
	void IABAudioDataPCM::EnablePacking()
	{
		// Call base class method
		IABElement::EnablePacking();
	}

	// IABAudioDataPCM::DisablePacking() implementation
	void IABAudioDataPCM::DisablePacking()
	{
		// Call base class method
		IABElement::DisablePacking();
	}

	// IABAudioDataPCM::IsIncludedForPacking() implementation
	bool IABAudioDataPCM::IsIncludedForPacking()
	{
		// Call base class method
		return IABElement::IsIncludedForPacking();
	}

	// IABAudioDataPCM::Serialize() implementation
	iabError IABAudioDataPCM::Serialize(std::ostream &outStream)
	{
		// Check if "this" element is included in packing. If not, simply return with no further processing
		if (!IsIncludedForPacking())
		{
			return kIABNoError;
		}

		// Set up payload writer and buffer
		elementPayloadWriter_.align();										// reset writer byte alignment
		elementPayloadBuffer_.seekp(0, std::ios::beg);						// reset internal payload buffer
		elementPayloadBuffer_.clear();
		elementHeadBuffer_.seekp(0, std::ios::beg);
		elementHeadBuffer_.clear();

		// Writing data fields (These are currently written into internal "elementPayloadBuffer_")
		// Calling method for writing Plex encoded field.
		write(elementPayloadWriter_, Plex<8>(audioDataID_));

		// Write PCM sample bytes, total number of bytes to pack = totalByteCount_
		// These are already arranged in little endian order during packing
		elementPayloadWriter_.write(pcmBytes_, totalByteCount_);			// totalByteCount_ bytes

		elementPayloadWriter_.align();										// Align to byte

		// Get buffer length as payload, and set element size
		IABElementSizeType packedPayloadLength = 0;
		packedPayloadLength = static_cast<IABElementSizeType>(elementPayloadBuffer_.tellp());
		SetElementSize(packedPayloadLength);

		// Serialize frame head
		SerializeHead();

		// Output elementHeadBuffer_ first, followed by elementPayloadBuffer_
		outStream << elementHeadBuffer_.rdbuf();
		outStream << elementPayloadBuffer_.rdbuf();

		// Clear stream contents after packing
		elementHeadBuffer_.str("");
		elementPayloadBuffer_.str("");

		return kIABNoError;
	}

	// IABAudioDataPCM::DeSerialize() implementation
	iabError IABAudioDataPCM::DeSerialize(StreamReader& streamReader)
	{
		iabError errorCode = kIABNoError;

		errorCode = DeSerializeHead(streamReader);

		if (errorCode != kIABNoError)
		{
			return errorCode;
		}

		Plex<8> plex8Field = 0;

		// Save stream position before reading for audioDataID_
		std::streampos positionAtStart;
		positionAtStart = streamReader.streamPosition();

		// Read audio data ID
		if (CMNSTRM_OK != read(streamReader, plex8Field))
		{
			return kIABParserIABPCMError;
		}

		audioDataID_ = static_cast<IABAudioDataIDType>(plex8Field);

		// Calculating number of PCM sample bytes from parsed "elementSize_"
		std::streampos positionCurrent = streamReader.streamPosition();
		uint64_t audioIDBytes = static_cast<uint64_t>(positionCurrent - positionAtStart);
		uint64_t pcmSampleBytesLength = static_cast<uint64_t>(elementSize_ - audioIDBytes);

		// Checking system/frame parameter congruency before retrieving PCM bytes
		if (pcmSampleBytesLength != totalByteCount_)
		{
			return kIABParserIABPCMError;
		}

		// Clear pcmBytes_ buffer parsing bytes into it
		memset(pcmBytes_, 0, totalByteCount_);

		// Read PCM sample data bytes
		if (CMNSTRM_OK != streamReader.read(pcmBytes_, totalByteCount_))
        {
			return kIABParserIABPCMError;
		}
        
		if (CMNSTRM_OK == streamReader.align())  // byte alignment
		{
			return kIABNoError;
		}
		else
		{
			return kIABParserIABPCMError;
		}
	}
    
    // IABAudioDataPCM::Validate() implementation
    bool IABAudioDataPCM::Validate(IABEventHandler &iEvenHandler, ValidationIssue &iValidationIssue) const
    {
		bool isToContinue = true;

		// Update iValidationIssue.id_ so that any issues reported from this Validate()
		// function is for AudioDataDLC element with audioDataID_ value.
		//
		iValidationIssue.id_ = audioDataID_;
        
        // #################################################
        // Cinema constraint sets validation starts
        // #################################################

		// **** Start validation against kConstraints_set_Cinema_ST2098_2_2018 limits and constraints
		//

		// AudioDataID
		if (audioDataID_ == 0)
		{
			// Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST2098_2_2018;
			iValidationIssue.event_ = ErrorEvent;
			iValidationIssue.errorCode_ = kValidateErrorAudioDataPCMAudioDataIDZero;

			if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
			{
				return isToContinue;
			}
		}
        
        // ST2098_2_2018 checks completed, change validation constriant set to kConstraints_set_Cinema_ST429_18_2019
        // Note that ST429_18_2019 is a super set of ST2098_2_2018 and the preceding checks are required to validate
        // bitstream against ST429_18_2019
        
        // ST429_18_2019 additional checks on IABAudioDataPCM :
        // 1. AudioDataPCM is not allowed

        if (!ConstraintSet_ST429_18_2019.allowAudioDataPCM_)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_Cinema_ST429_18_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorAudioDataPCMNotAnAllowedSubElement;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }

        // No additional checks for kConstraints_set_DbyCinema on AudioDataPCM
        
        // #################################################
        // Cinema constraint sets validation ends
        // #################################################

        // #################################################
        // IMF constraint sets validation starts
        // #################################################
        
        // **** Start validation against kConstraints_set_IMF_ST2098_2_2019 limits and constraints
        //
        
        // AudioDataID
        if (audioDataID_ == 0)
        {
            // Update issue variable and log
            iValidationIssue.isBeingValidated_ = kConstraints_set_IMF_ST2098_2_2019;
            iValidationIssue.event_ = ErrorEvent;
            iValidationIssue.errorCode_ = kValidateErrorAudioDataPCMAudioDataIDZero;
            
            if (isToContinue = iEvenHandler.Handle(iValidationIssue), isToContinue == false)
            {
                return isToContinue;
            }
        }
        
        
        // #################################################
        // IMF constraint sets validation ends
        // #################################################

		return isToContinue;
	}

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


