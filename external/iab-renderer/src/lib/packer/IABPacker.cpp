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
 * MDA Packer Implementation
 *
 * @file
 */

#include <assert.h>
#include <stack>
#include <vector>
#include <stdlib.h>

#include "IABDataTypes.h"
#include "common/IABElements.h"
#include "IABUtilities.h"
#include "packer/IABPacker.h"
#include "IABVersion.h"
#include "commonstream/utils/opbacksbuf.h"

#define IAB_PACKERLIB_EMBEDDED_STRFILEVER "IAB Packer Lib Version " IAB_VERSION_STRING

#define IABPackerAPIHighVersion     0
#define IABPackerAPILowVersion      0


namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    const char *gpEmbeddedPckrVersionString = IAB_PACKERLIB_EMBEDDED_STRFILEVER;

	// ****************************************************************************
	// Factory methods for creating instances of IABPacker
	// ****************************************************************************

	// Creates an IABPacker instance
	IABPackerInterface* IABPackerInterface::Create()
	{
		IABPacker* iabPacker = nullptr;
		iabPacker = new IABPacker();
		return iabPacker;
	}

    // Deletes an IABPacker instance
    void IABPackerInterface::Delete(IABPackerInterface* iInstance)
    {
        delete iInstance;
    }

    /****************************************************************************
    *
    * IABPacker
    *
    *****************************************************************************/

	// IABPacker constructor implementation
	IABPacker::IABPacker()
    {
        iabPackerFrame_ = IABFrameInterface::Create(nullptr);
        maxRendered_ = 0;
		frameRate_ = kIABFrameRate_24FPS;				// Default: 24 fps
		sampleRate_ = kIABSampleRate_48000Hz;			// Default: 48 kHz
		numSubBlocks_ = GetIABNumSubBlocks(frameRate_);

		currentMetaID_ = 0;
		currentAudioID_ = 0;
	}

	// IABPacker destructor implementation
	IABPacker::~IABPacker()
    {
		delete iabPackerFrame_;
		iabPackerFrame_ = nullptr;
    }

	// IABPacker::GetAPIVersion() implementation
	void IABPacker::GetAPIVersion(IABAPIVersionType& oVersion)
    {
        oVersion.fHigh_ = IABPackerAPIHighVersion;
        oVersion.fLow_  = IABPackerAPILowVersion;
    }

	// IABPacker::PackIABFrame() implementation
	// Pack an IAB frame
    iabError IABPacker::PackIABFrame()
    {
		iabError errorCode = kIABNoError;

		if (nullptr == iabPackerFrame_)
        {
            return kIABBadArgumentsError;
        }

		// Calculate MaxRendered before packing
		std::vector<IABElement*> frameSbElements;
		iabPackerFrame_->GetSubElements(frameSbElements);

		// Calculate MaxRendered and update to IABPacker::maxRendered_
		errorCode = CalculateFrameMaxRendered(frameSbElements, maxRendered_);

		// Also update the same value to underlying iabPackerFrame_->maxRendered_
		iabPackerFrame_->SetMaxRendered(maxRendered_);

		// Exit if error
		if (errorCode)
		{
			return errorCode;
		}

		// Validate frame before packing
		// errorCode = iabPackerFrame_->Validate();

		// Exit if error
		if (errorCode)
		{
			return errorCode;
		}
        
        // Clear packed frame buffer
        fProgramBuffer_.clear();

		opbacksbuf<std::vector<char> > outStreamBuf(fProgramBuffer_);
		std::ostream outputStream(&outStreamBuf);

		errorCode = iabPackerFrame_->Serialize(outputStream);

		return errorCode;
	}
    
	// IABPacker::GetPackedBuffer() implementation
	// Get IAB frame packed buffer
    iabError IABPacker::GetPackedBuffer(std::vector<char>& oProgramBuffer, uint32_t& oProgramBufferLength)
    {        
        oProgramBufferLength = (uint32_t) fProgramBuffer_.size();
        
        oProgramBuffer = fProgramBuffer_;
        
        if (0 == oProgramBufferLength)
        {
            return kIABPackerPackedStreamEmptyError;
        }

        return kIABNoError;
    }
    
	// IABPacker::SetFrameRate() implementation
	iabError IABPacker::SetFrameRate(IABFrameRateType iFrameRate)
    {
        switch (iFrameRate)
        {
            case kIABFrameRate_24FPS:
            case kIABFrameRate_25FPS:
            case kIABFrameRate_30FPS:
            case kIABFrameRate_48FPS:
            case kIABFrameRate_50FPS:
            case kIABFrameRate_60FPS:
            case kIABFrameRate_96FPS:
            case kIABFrameRate_100FPS:
            case kIABFrameRate_120FPS:
			case kIABFrameRate_23_976FPS:
				break;
                
            default:
                return kIABBadArgumentsError;
        }
        
        frameRate_ = iFrameRate;
    
        if (kIABNoError != iabPackerFrame_->SetFrameRate(frameRate_))
        {
            return kIABBadArgumentsError;
        }
        
		numSubBlocks_ = GetIABNumSubBlocks(frameRate_);

        return kIABNoError;
    }
    
	// IABPacker::SetSampleRate() implementation
	iabError IABPacker::SetSampleRate(IABSampleRateType iSampleRate)
    {
        sampleRate_ = iSampleRate;
        iabPackerFrame_->SetSampleRate(iSampleRate);
        return kIABNoError;
    }

	// IABPacker::GetIABFrame() implementation
	iabError IABPacker::GetIABFrame(IABFrameInterface* &oIABFrame)
	{
		oIABFrame = iabPackerFrame_;
		return kIABNoError;
	}

	// IABPacker::GetNextUniqueMetaID() implementation
	IABMetadataIDType IABPacker::GetNextUniqueMetaID()
	{
		currentMetaID_++;
		return currentMetaID_;
	}
		
	// IABPacker::GetNextUniqueAudioID() implementation
	IABAudioDataIDType IABPacker::GetNextUniqueAudioID()
	{
		currentAudioID_++;
		return currentAudioID_;
	}

	// IABPacker::IsElementInFrame() implementation
	bool IABPacker::IsElementInFrame(IABElement* iElement)
	{
		return true;
	}
	// IABPacker::AddBedDefinition() implementation
	iabError IABPacker::AddBedDefinition(IABMetadataIDType iBedID, 
		IABUseCaseType iBedLayout, 
		const IABBedMappingInfo& iAudioIDs, 
		IABBedDefinitionInterface* &oBedDefinition)
    {

        if (0 == iAudioIDs.lookupMap_.size())
        {
            return kIABBadArgumentsError;   // Need audio data vector to created bed definition
        }
        
        if ((kIABUseCase_5_1 == iBedLayout) && (6 == iAudioIDs.lookupMap_.size()))
        {
            // maxRendered includes bed channels, consult spec on how to account for conditional use cases
            maxRendered_ += 6;
            return AddBedDefinition51(iBedID, iAudioIDs, oBedDefinition);
        }
        else if ((kIABUseCase_7_1_DS == iBedLayout) && (8 == iAudioIDs.lookupMap_.size()))
        {
            maxRendered_ += 8;
            return AddBedDefinition71DS(iBedID, iAudioIDs, oBedDefinition);
        }
        else if ((kIABUseCase_9_1_OH == iBedLayout) && (10 == iAudioIDs.lookupMap_.size()))
        {
            maxRendered_ += 10;
            return AddBedDefinition91OH(iBedID, iAudioIDs, oBedDefinition);
        }
        else
        {
            return kIABNotImplementedError;
        }
    }
    
	// IABPacker::AddBedDefinition51() implementation
	iabError IABPacker::AddBedDefinition51(IABMetadataIDType iBedID, 
		const IABBedMappingInfo& iAudioIDs, 
		IABBedDefinitionInterface* &oBedDefinition)
    {
        IABBedDefinition* packerBedDefinition = dynamic_cast<IABBedDefinition*>(IABBedDefinitionInterface::Create(frameRate_));
        
        if (nullptr == packerBedDefinition)
        {
            return kIABBadArgumentsError;
        }

        if ((iAudioIDs.lookupMap_.size() != 6) || (iAudioIDs.getUseCase() != kIABUseCase_5_1))
        {
            return kIABBadArgumentsError;
        }
        
        // !!! won't need iBedID arg if it can be passed in the IABBedMappingInfo...
        if (iBedID != iAudioIDs.getMetadataID())
        {
            return kIABBadArgumentsError;
        }

        // Set metadata ID to a unique number
        packerBedDefinition->SetMetadataID(iBedID);
        
        // Ste conditional bed = 0
        packerBedDefinition->SetConditionalBed(0);
        
        // Set bed use case for consistency but this will not go into bitstream when conditional bed = 0
        packerBedDefinition->SetBedUseCase(kIABUseCase_5_1);
        
        // Create 5.1 bed channels
        std::vector<IABChannel*> bedChannels;
        
        std::map<IABChannelIDType, IABAudioDataIDType>::const_iterator iterIDs;
        
        for (iterIDs = iAudioIDs.lookupMap_.begin(); iterIDs != iAudioIDs.lookupMap_.end(); iterIDs++)
        {

            IABChannelInterface* newChannel = dynamic_cast<IABChannelInterface*>(IABChannelInterface::Create());
            newChannel->SetChannelID((*iterIDs).first);
            newChannel->SetAudioDataID((*iterIDs).second);
            newChannel->SetChannelGain(IABGain());
            newChannel->SetDecorInfoExists(0);
            bedChannels.push_back(dynamic_cast<IABChannel*>(newChannel));
            
        }
        
        packerBedDefinition->SetBedChannels(bedChannels);
        
        // Set audio description
        IABAudioDescription audioDescription;
        audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription.audioDescriptionText_.clear();
        packerBedDefinition->SetAudioDescription(audioDescription);

		iabPackerFrame_->AddSubElement(packerBedDefinition);

		oBedDefinition = packerBedDefinition;
        
        return kIABNoError;
    }
    
	// IABPacker::AddBedDefinition71DS() implementation
	iabError IABPacker::AddBedDefinition71DS(IABMetadataIDType iBedID, 
		const IABBedMappingInfo& iAudioIDs, 
		IABBedDefinitionInterface* &oBedDefinition)
    {
        IABBedDefinition* packerBedDefinition = dynamic_cast<IABBedDefinition*>(IABBedDefinitionInterface::Create(frameRate_));
        
        if (nullptr == packerBedDefinition)
        {
            return kIABBadArgumentsError;
        }

        if ((iAudioIDs.lookupMap_.size() != 8) || (iAudioIDs.getUseCase() != kIABUseCase_7_1_DS))
        {
            return kIABBadArgumentsError;
        }

        // !!! won't need iBedID arg if it can be passed in the IABBedMappingInfo...
        if (iBedID != iAudioIDs.getMetadataID())
        {
            return kIABBadArgumentsError;
        }
        
        // Set metadata ID to a unique number
        packerBedDefinition->SetMetadataID(iBedID);
        
        // Ste conditional bed = 0
        packerBedDefinition->SetConditionalBed(0);
        
        // Set bed use case for consistency but this will not go into bitstream when conditional bed = 0
        packerBedDefinition->SetBedUseCase(kIABUseCase_7_1_DS);
        
        // Create 7.1 bed channels
        std::vector<IABChannel*> bedChannels;
        std::map<IABChannelIDType, IABAudioDataIDType>::const_iterator iterIDs;
        
        for (iterIDs = iAudioIDs.lookupMap_.begin(); iterIDs != iAudioIDs.lookupMap_.end(); iterIDs++)
        {
            IABChannelInterface* newChannel = dynamic_cast<IABChannelInterface*>(IABChannelInterface::Create());
            newChannel->SetChannelID((*iterIDs).first);
            newChannel->SetAudioDataID((*iterIDs).second);
            newChannel->SetChannelGain(IABGain());
            newChannel->SetDecorInfoExists(0);
            bedChannels.push_back(dynamic_cast<IABChannel*>(newChannel));
            
        }
        
        packerBedDefinition->SetBedChannels(bedChannels);
        
        // Set audio description
        IABAudioDescription audioDescription;
        audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription.audioDescriptionText_.clear();
        packerBedDefinition->SetAudioDescription(audioDescription);
        
		iabPackerFrame_->AddSubElement(packerBedDefinition);
        
		oBedDefinition = packerBedDefinition;
	
		return kIABNoError;
    }
    
	// IABPacker::AddBedDefinition91OH() implementation
	iabError IABPacker::AddBedDefinition91OH(IABMetadataIDType iBedID, 
		const IABBedMappingInfo& iAudioIDs, 
		IABBedDefinitionInterface* &oBedDefinition)
    {
        IABBedDefinition* packerBedDefinition = dynamic_cast<IABBedDefinition*>(IABBedDefinitionInterface::Create(frameRate_));
        
        if (nullptr == packerBedDefinition)
        {
            return kIABBadArgumentsError;
        }
        
        if ((iAudioIDs.lookupMap_.size() != 10) || (iAudioIDs.getUseCase() != kIABUseCase_9_1_OH))
        {
            return kIABBadArgumentsError;
        }
        
        // !!! won't need iBedID arg if it can be passed in the IABBedMappingInfo...
        if (iBedID != iAudioIDs.getMetadataID())
        {
            return kIABBadArgumentsError;
        }
        
        // Set metadata ID to a unique number
        packerBedDefinition->SetMetadataID(iBedID);
        
        // Ste conditional bed = 0
        packerBedDefinition->SetConditionalBed(0);
        
        // Set bed use case for consistency but this will not go into bitstream when conditional bed = 0
        packerBedDefinition->SetBedUseCase(kIABUseCase_9_1_OH);
        
        // Create 7.1 bed channels
        std::vector<IABChannel*> bedChannels;
        
        std::map<IABChannelIDType, IABAudioDataIDType>::const_iterator iterIDs;
        
        for (iterIDs = iAudioIDs.lookupMap_.begin(); iterIDs != iAudioIDs.lookupMap_.end(); iterIDs++)
        {
            IABChannelInterface* newChannel = dynamic_cast<IABChannelInterface*>(IABChannelInterface::Create());
            
            newChannel->SetChannelID((*iterIDs).first);
            newChannel->SetAudioDataID((*iterIDs).second);
            newChannel->SetChannelGain(IABGain());
            newChannel->SetDecorInfoExists(0);
            bedChannels.push_back(dynamic_cast<IABChannel*>(newChannel));
        }
        
        packerBedDefinition->SetBedChannels(bedChannels);
        
        // Set audio description
        IABAudioDescription audioDescription;
        audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription.audioDescriptionText_.clear();
        packerBedDefinition->SetAudioDescription(audioDescription);
        
		iabPackerFrame_->AddSubElement(packerBedDefinition);
        
		oBedDefinition = packerBedDefinition;
	
		return kIABNoError;
    }

	// IABPacker::AddObjectDefinition() implementation
	iabError IABPacker::AddObjectDefinition(IABMetadataIDType iObjectID, 
		IABAudioDataIDType iAudioDataID, 
		IABObjectDefinitionInterface* &oObjectDefinition)
    {
        IABObjectDefinition* packerObjectDefinition = dynamic_cast<IABObjectDefinition*>(IABObjectDefinitionInterface::Create(frameRate_));
        
        if (nullptr == packerObjectDefinition)
        {
            return kIABBadArgumentsError;
        }
        
        // Set metadata ID
        packerObjectDefinition->SetMetadataID(iObjectID);
        
        // Set audio data ID
        packerObjectDefinition->SetAudioDataID(iAudioDataID);
        
        // Set conditional object, 1-bit
        packerObjectDefinition->SetConditionalObject(1);
        packerObjectDefinition->SetObjectUseCase(kIABUseCase_Always);
        
        // Set audio description
        IABAudioDescription audioDescription;
        audioDescription.audioDescription_ = kIABAudioDescription_NotIndicated;
        audioDescription.audioDescriptionText_.clear();
        packerObjectDefinition->SetAudioDescription(audioDescription);
        
        std::vector<IABObjectSubBlock*> panSubBlocks;

        for (uint8_t i = 0; i < numSubBlocks_; i++)
        {
            IABObjectSubBlock* packerObjectSubBlock = dynamic_cast<IABObjectSubBlock*>(IABObjectSubBlockInterface::Create());
            
            if (nullptr == packerObjectSubBlock)
            {
                return kIABBadArgumentsError;
            }
            
            uint1_t panInfoExists = 0;

            if (0 == i)
            {
                // For sub block 0, this flag is not written to bitstream and parser will set it to 1, i.e. pan info always exists in bitstream for subblock 0
                panInfoExists = 1;
            }
            
            packerObjectSubBlock->SetPanInfoExists(panInfoExists);
            
            // Preset to defaults, They will be updated in every frame
            IABGain objectGain = IABGain();

            IABObjectSnap objectSnap;
            objectSnap.objectSnapPresent_ = 0;
            objectSnap.objectSnapTolExists_ = 0;
            objectSnap.objectSnapTolerance_ = 0;
            objectSnap.reservedBit_ = 0;

            IABObjectZoneGain9 objectZoneGains9;
            objectZoneGains9.objectZoneControl_ = 0;    // no zone gain info in bitstream
            
            // Set to low res mode, only spreadXYZ_ is used, set it to zero for zero spread (check this value)
            IABObjectSpread objectSpread;
            objectSpread.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 0.0f, 0.0f, 0.0f);
            
            // NO decorrelation
            IABDecorCoeff decorCoef;
            decorCoef.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
            decorCoef.decorCoef_ = 0;
            
            // Set position to zero during construction. It will be updated for every frame
            CartesianPosInUnitCube objectPositionUC = CartesianPosInUnitCube();
            packerObjectSubBlock->SetObjectGain(objectGain);
            packerObjectSubBlock->SetObjectPositionFromUnitCube(objectPositionUC);
            packerObjectSubBlock->SetObjectSnap(objectSnap);
            packerObjectSubBlock->SetObjectZoneGains9(objectZoneGains9);
            packerObjectSubBlock->SetObjectSpread(objectSpread);
            packerObjectSubBlock->SetDecorCoef(decorCoef);
            panSubBlocks.push_back(dynamic_cast<IABObjectSubBlock*>(packerObjectSubBlock));
        }   // for (uint8_t i = 0; i < numSubBlocks_; i++)
        
        packerObjectDefinition->SetPanSubBlocks(panSubBlocks);

		iabPackerFrame_->AddSubElement(packerObjectDefinition);
        
        maxRendered_++;

		oObjectDefinition = packerObjectDefinition;
        
        return kIABNoError;
    }
    
	// IABPacker::AddDLCElements() implementation
	iabError IABPacker::AddDLCElements(std::vector<IABAudioDataIDType> iAudioDataIDList)
    {
        std::vector<IABAudioDataIDType>::const_iterator iterADL;
        for (iterADL = iAudioDataIDList.begin(); iterADL != iAudioDataIDList.end(); iterADL++)
        {
            IABAudioDataDLC* dlcElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(frameRate_, sampleRate_));
            
            if (nullptr != dlcElement)
            {
                // Set audio data ID
                dlcElement->SetAudioDataID(*iterADL);
                
                // Set DLC sample rate
                dlcElement->SetDLCSampleRate(sampleRate_);
                
				iabPackerFrame_->AddSubElement(dlcElement);
            }
            else
            {
                return kIABBadArgumentsError;
            }
        }
        
        return kIABNoError;
    }
    
	// IABPacker::UpdateAudioSamples() implementation
	iabError IABPacker::UpdateAudioSamples(std::map<IABAudioDataIDType, int32_t*> iAudioSources)
    {
		std::vector<IABElement*> frameSbElements;
		iabPackerFrame_->GetSubElements(frameSbElements);

        std::map<IABAudioDataIDType, int32_t*>::iterator iter;
        std::vector<IABElement*> ::const_iterator iterFSE;
        
        for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)
        {
            IABAudioDataIDType audioID;
            IABAudioDataDLC* dlcElement = dynamic_cast<IABAudioDataDLC*>(*iterFSE);
            
            if (dlcElement != nullptr)
            {
                dlcElement->GetAudioDataID(audioID);
                iter = iAudioSources.find(audioID);
                if (iter != iAudioSources.end())
                {
                    // encode audio samples and replace encoded samples
                    dlcElement->EncodeMonoPCMToDLC(iter->second, 2000);
                }
                else
                {
                    return kIABBadArgumentsError;
                }
            }
        }
        
        return kIABNoError;
    }

	// IABPacker::UpdateObjectMetaData() implementation
	iabError IABPacker::UpdateObjectMetaData(IABMetadataIDType iObjectID, std::vector<IABObjectPanningParameters> iSubblockPanParameters)
    {
        if (iSubblockPanParameters.size() != numSubBlocks_)
        {
            return kIABBadArgumentsError;
        }
        
		std::vector<IABElement*> frameSbElements;
		iabPackerFrame_->GetSubElements(frameSbElements);
		// For an object, update position and audio data for each frame
        std::vector<IABElement*> ::const_iterator iterFSE;
        
        for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)
        {
            IABMetadataIDType objectMetaID;
            IABObjectDefinition* objectElement = dynamic_cast<IABObjectDefinition*>(*iterFSE);
            
            if (objectElement != nullptr)
            {
                objectElement->GetMetadataID(objectMetaID);
                if (objectMetaID == iObjectID)
                {
                    std::vector<IABObjectSubBlock*> panSubBlocks;
                    objectElement->GetPanSubBlocks(panSubBlocks);
                    
                    uint1_t panInfoExists;

                    for (uint8_t i = 0; i < 8; i++)
                    {
                        if (0 == i)
                        {
                            // pan exsit flag is not required for first subbock, set to 1 for consistency
                            panInfoExists = 1;
                        }
                        else
                        {
                            panInfoExists = iSubblockPanParameters[i].panInfoExists_;
                        }
                        
                        panSubBlocks[i]->SetPanInfoExists(panInfoExists);
                        
                        if (panInfoExists)
                        {
                            panSubBlocks[i]->SetObjectPositionFromUnitCube(iSubblockPanParameters[i].position_);
                            panSubBlocks[i]->SetObjectSpread(iSubblockPanParameters[i].spread_);
                            panSubBlocks[i]->SetObjectGain(iSubblockPanParameters[i].objectGain_);
                        }
                    }
                }
            }
        }
        
        return kIABNoError;
        
    }

	// IABPacker::UpdateBedChannelGain() implementation
	iabError IABPacker::UpdateBedChannelGain(IABChannelIDType chanId, IABGain iChannelGain)
    {
        IABBedDefinition* bedElement = nullptr;
        std::vector<IABChannel*> bedChannels;
        std::vector<IABChannel*>::const_iterator iterBC;
        IABChannelIDType channelID;
        bool found = false;
        
		std::vector<IABElement*> frameSbElements;
		iabPackerFrame_->GetSubElements(frameSbElements);
		std::vector<IABElement*> ::const_iterator iterFSE;
        
        for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)
        {
            // For version 1 btstream, we only have a single bed definition.
            // To do: add code to match meta data ID to find the correct bed definition
            
            bedElement = dynamic_cast<IABBedDefinition*>(*iterFSE);
            
            if (bedElement != nullptr)
            {
                bedElement->GetBedChannels(bedChannels);
                
                // Find correct bed channel to set gain
                for (iterBC = bedChannels.begin(); iterBC != bedChannels.end(); iterBC++)
                {
                    (*iterBC)->GetChannelID(channelID);
                    if (chanId == channelID)
                    {
                        (*iterBC)->SetChannelGain(iChannelGain);
                        break;
                    }
                }
            }
        }
        
        if (!found)
        {
            return kIABBadArgumentsError;
        }
        
        return kIABNoError;
    }

    // IABPacker::AddAuthoringToolInfo() implementation
    iabError IABPacker::AddAuthoringToolInfo(const std::string &iAuthoringToolInfoString)
    {
        iabError returnCode = kIABNoError;
        
        if (iAuthoringToolInfoString.size() == 0)
        {
            return kIABBadArgumentsError;
        }
        
        IABAuthoringToolInfo* authoringToolElement = nullptr;
        std::vector<IABElement*> frameSbElements;
        iabPackerFrame_->GetSubElements(frameSbElements);
        std::vector<IABElement*> ::const_iterator iterFSE;
        
        // See if an AuthoringToolInfo element is already in the packer frame
        for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)
        {
            authoringToolElement = dynamic_cast<IABAuthoringToolInfo*>(*iterFSE);
            
            if (authoringToolElement != nullptr)
            {
                // Already exists, update content
                returnCode = authoringToolElement->SetAuthoringToolInfo(iAuthoringToolInfoString.c_str());
                return returnCode;
            }
        }
        
        // Not found, add one to frame
        authoringToolElement = dynamic_cast<IABAuthoringToolInfo*>(IABAuthoringToolInfoInterface::Create());
        
        if (nullptr == authoringToolElement)
        {
            return kIABMemoryError;
        }

        returnCode = authoringToolElement->SetAuthoringToolInfo(iAuthoringToolInfoString.c_str());
        returnCode |= iabPackerFrame_->AddSubElement(authoringToolElement);
        return returnCode;
    }
    
    // IABPacker::AddUserData() implementation
    iabError IABPacker::AddUserData(const uint8_t (&iUserID)[16], const std::vector<uint8_t> iUserDataBytes)
    {
        iabError returnCode = kIABNoError;

        if (iUserDataBytes.size() == 0)
        {
            return kIABBadArgumentsError;
        }
        
        IABUserData* userDataElement = dynamic_cast<IABUserData*>(IABUserDataInterface::Create());
        
        if (nullptr == userDataElement)
        {
            return kIABMemoryError;
        }
        
        returnCode = userDataElement->SetUserID(iUserID);
        returnCode |= userDataElement->SetUserDataBytes(iUserDataBytes);
        returnCode |= iabPackerFrame_->AddSubElement(userDataElement);
        return returnCode;
    }


} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
