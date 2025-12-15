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

#include <iostream>
#include <sstream>

#include "IABMaxComplexityFrame.h"
#include "common/IABElements.h"
#include "common/IABConstants.h"
#include "IABUtilities.h"
#include "TestUtils.h"

IABMaxComplexityFrame::IABMaxComplexityFrame()
{
    sampleRate_ = kIABSampleRate_48000Hz;									// Max Complexity Frame (MCF) spec
    frameRate_ = kIABFrameRate_24FPS;										// MCF spec
    frameSampleCount_ = GetIABNumFrameSamples(frameRate_, sampleRate_);		// ST2098-2 spec
    numPanSubblocks_ = GetIABNumSubBlocks(frameRate_);
	maxRendered_ = 0;

	metaDataNextID_ = 1;
	audioDataNextID_ = 1;
	randomSampleSeed_ = 0;

	audioSamples_ = new int32_t[frameSampleCount_];							// Allocate memory for source sample buffer

    iabPacker_ = NULL;
}

IABMaxComplexityFrame::~IABMaxComplexityFrame()
{
	if (iabPacker_)
	{
		IABPackerInterface::Delete(iabPacker_);
	}

	delete audioSamples_;
	audioSamples_ = NULL;
}

uint32_t IABMaxComplexityFrame::GetFrameSampleCount()
{
	return frameSampleCount_;
}

void IABMaxComplexityFrame::Construct()
{
    // Create IAB packer and construct underlying frame
	//
    if (iabPacker_)
    {
		// Already created (avoid multiple creation)
        return;
    }

    iabPacker_ = IABPackerInterface::Create();

	// Config frame rate and sample rate for the frame
    iabPacker_->SetFrameRate(frameRate_);
    iabPacker_->SetSampleRate(sampleRate_);
    
    // ************************
    // Set up IAB Frame
    // ************************
    
    // Add a 9.1 bed definition to frame, per Max Complexity Frame specification
	// Associated DLC elements are added per specification
	Add91BedDefinition();
    
    // Add 118x object definitions to frame, per Max Complexity Frame specification
	// Associated DLC elements are added per specification
	Add118ObjectDefinitions();

	// Get Constructed Max Complexity Frame
	iabPacker_->GetIABFrame(maxComplexityFrame_);

	// Update maxRendered_ to frame
	maxComplexityFrame_->SetMaxRendered(maxRendered_);

	// Done
	return;
}

IABFrameInterface* IABMaxComplexityFrame::GetMaxComplexityFrame()
{
	return maxComplexityFrame_;
}


void IABMaxComplexityFrame::PackToStream()
{
	if (!iabPacker_)
	{
		// If not yet constructed, construct the frame first
		Construct();
	}

	// Serialise frame to packed data buffer
	//
	iabPacker_->PackIABFrame();

	// Get serialsed frame data
	iabPacker_->GetPackedBuffer(packedMaxComplexityFrame_, packedLengthInByte_);
}

std::vector<char>& IABMaxComplexityFrame::GetSerializedFrame()
{
	return packedMaxComplexityFrame_;
}

uint32_t IABMaxComplexityFrame::GetSerializedDataSize()
{
	return packedLengthInByte_;
}

void IABMaxComplexityFrame::Add91BedDefinition()
{
	// Get access to IABFrame inside iabPacker_ for adding the bed and DLC elements
	IABFrameInterface* iabFrame = NULL;
	iabPacker_->GetIABFrame(iabFrame);

	// Add 9.1OH bed, per MCF spec
	//
	// Channel commposition of 9.1OH bed
	IABChannelIDType channelComposition[10] = {
		kIABChannelID_Center,
		kIABChannelID_Left,
		kIABChannelID_Right,
		kIABChannelID_LeftSideSurround,
		kIABChannelID_RightSideSurround,
		kIABChannelID_LeftRearSurround,
		kIABChannelID_RightRearSurround,
		kIABChannelID_LFE,
		kIABChannelID_LeftTopSurround,
		kIABChannelID_RightTopSurround
	};

	// Channel tone freqencies, per MCF spec. Order matching channelComposition
	float channelFrequenct[10] = {
		1000.0f,
		2000.0f,
		3000.0f,
		4000.0f,
		5000.0f,
		6000.0f,
		7000.0f,
		96.0f,
		8000.0f,
		9000.0f
	};
	
	IABBedDefinitionInterface* Bed91OH = IABBedDefinitionInterface::Create(frameRate_);

	// Set metadata ID to a unique number. Post increment meta ID
	Bed91OH->SetMetadataID(metaDataNextID_++);

	// Set to un-conditional bed (default bed)
	Bed91OH->SetConditionalBed(0);

	// Variables for generating tones, per MCF spec
	float sampleRate = 48000.0f;										// 48kHz
	float SamplePeriod = 1.0f / sampleRate;								// sample period for sampleRate

	float toneLevelDb = -25.0f;											// @-25dB
	float scaleFor32bit = kInt32BitMaxValue * pow(10.0f, (toneLevelDb / 20.0f));

	// Create 10x bed channels
	std::vector<IABChannel*> bedChannels;

	for (uint32_t i = 0; i < 10; i++)
	{
		IABChannelInterface* channel = IABChannelInterface::Create();
		channel->SetChannelID(channelComposition[i]);				// ID
		channel->SetChannelGain(IABGain());							// default gain, 1.0
		channel->SetDecorInfoExists(0);								// no decor

		// Create DLC element for the channel
		// Pre-calculate step size in radian for the ith tone frequency
		float stepRadian = 2 * iabKPI * SamplePeriod * channelFrequenct[i];

		// Generate tone samples for 1 frame
		for (uint32_t j = 0; j < frameSampleCount_; j++)
		{
			audioSamples_[j] = static_cast<int32_t>(scaleFor32bit * sin(stepRadian * j));
		}

		// Create a DLC for the channel
		IABAudioDataDLCInterface* dlcElement = IABAudioDataDLCInterface::Create(frameRate_,sampleRate_);
		dlcElement->SetDLCSampleRate(sampleRate_);					// Set sample rate
		dlcElement->SetAudioDataID(audioDataNextID_);				// Set ID from pool

		// Associate the audio ID to the channel. Post increment audio ID.
		channel->SetAudioDataID(audioDataNextID_++);

		// encode PCM samples into the DLC element
		dlcElement->EncodeMonoPCMToDLC(audioSamples_, frameSampleCount_);

		// DLC element is ready to be added as sub-element to frame
		// (we don't really care about the order of frame sub-element).
		iabFrame->AddSubElement(dynamic_cast<IABAudioDataDLC*>(dlcElement));

		// Add channel to Bed
		bedChannels.push_back(dynamic_cast<IABChannel*>(channel));
	}

	Bed91OH->SetBedChannels(bedChannels);

	// Set audio description
	IABAudioDescription audioDescription;
	audioDescription.audioDescription_ = kIABAudioDescription_Music;
	audioDescription.audioDescriptionText_.clear();
	Bed91OH->SetAudioDescription(audioDescription);

	iabFrame->AddSubElement(dynamic_cast<IABBedDefinition*>(Bed91OH));

	// Increment maxRendered_ by 10 for 9.1OH
	maxRendered_ += 10;

	return;
}

void IABMaxComplexityFrame::Add118ObjectDefinitions()
{
	// Get access to IABFrame inside iabPacker_ for adding object and DLC elements
	IABFrameInterface* iabFrame = NULL;
	iabPacker_->GetIABFrame(iabFrame);

	// Generate random noise samples for 1 frame.
	// The same audio samples are used for all DLC for simplicity.
	IABTestUtils::GenerateRandomSamples(audioSamples_, frameSampleCount_, 0);

	// Scale down by 8 bit from 32-bit full range, to avoid rendering clipping
	for (uint32_t j = 0; j < frameSampleCount_; j++)
	{
		audioSamples_[j] >>= 8;
	}

	// Object gain of 1.0 throughout (defaul is 1.0)
	IABGain gain = IABGain();

	// Set up snap parameters that can be used for sub blocks. 2 possibilities, 1 on, and 1 off
	// For snapOn. default tolerance is used.
	IABObjectSnap snapOn;
	snapOn.objectSnapPresent_ = 1;
	snapOn.objectSnapTolExists_ = 0;
	snapOn.objectSnapTolerance_ = 0;
	snapOn.reservedBit_ = 0;

	IABObjectSnap snapOff;
	snapOff.objectSnapPresent_ = 0;
	snapOff.objectSnapTolExists_ = 0;
	snapOff.objectSnapTolerance_ = 0;
	snapOff.reservedBit_ = 0;

	// Set up spread parameters that can be used for sub blocks. 2 possibilities, 1 on, and 1 off (point source)
	// For spreadOff = default state
	IABObjectSpread spreadOff;

	// For spreadOn, set to 1D with 0.5
	IABObjectSpread spreadOn;
	spreadOn.setIABObjectSpread(kIABSpreadMode_HighResolution_1D, 0.5f, 0.0f, 0.0f);

	// Set up zone parameter (off) common to all objects and their sub blcoks
	IABObjectZoneGain9 zone9;
	zone9.objectZoneControl_ = 0;

	// Set up decorrelation = none, common to all objects ans sub blocks
	IABDecorCoeff decor;
	decor.decorCoefPrefix_ = kIABDecorCoeffPrefix_NoDecor;
	decor.decorCoef_ = 0;

	// Set up audio description common to all objects
	IABAudioDescription audioDescription;
	audioDescription.audioDescription_ = kIABAudioDescription_Ambience;
	audioDescription.audioDescriptionText_.clear();

	// ***
	// Add 118x objects and associated DLC elements, per MCF spec
	//

	// For constructing 8x sub block for each and every object
	std::vector<IABObjectSubBlock*> panSubBlocks;
	uint1_t panInfoExists = 0;

	// Position variables, commonly used.
	CartesianPosInUnitCube position = CartesianPosInUnitCube();
	float x, y, z;

	for (uint32_t i = 0; i < 118; i++)
	{
		// Create an object
		IABObjectDefinitionInterface* object = IABObjectDefinitionInterface::Create(frameRate_);

		// Set metadata ID
		object->SetMetadataID(metaDataNextID_++);

		// Set to un-conditional
		object->SetConditionalObject(0);

		// Set audio description
		object->SetAudioDescription(audioDescription);

		// Reset any SB pointers from last pass
		panSubBlocks.clear();

		for (uint32_t j = 0; j < numPanSubblocks_; j++)
		{
			// Create a subblock instance
			IABObjectSubBlockInterface* panSubBlock = IABObjectSubBlockInterface::Create();

			// Sub blocks 1 (indexed 0) and 5 (indexed 4) contain panning data, per MCF spec
			if (j == 0 || j == 4)
			{
				panInfoExists = 1;
			}
			else
			{
				panInfoExists = 0;
			}

			// Set parameters per MCF spec
			panSubBlock->SetPanInfoExists(panInfoExists);
			panSubBlock->SetObjectGain(gain);
			panSubBlock->SetObjectZoneGains9(zone9);
			panSubBlock->SetDecorCoef(decor);

			// First 59 objects, snapOff & spreadON
			if (i < 59)
			{
				panSubBlock->SetObjectSnap(snapOff);
				panSubBlock->SetObjectSpread(spreadOn);
			}
			else
			{
				// Others, snapOn & spreadOff
				panSubBlock->SetObjectSnap(snapOn);
				panSubBlock->SetObjectSpread(spreadOff);
			}

			// Create unique object positions for sub blocks 1 & 5, per MCF
			// Init to (0.0f, 0.0f, 0.0f) for all other sub blocks.
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;

			if (j == 0)
			{
				// Increment z by 0.2 for every 25 positions
				z = (i / 25) * 0.2f;
				// Remainder modulus 1.0 (cube limit)
				z = fmod(z, 1.0f);

				// Increment y by 0.2 for every 5 positions
				y = (i / 5) * 0.2f;
				// Remainder modulus 1.0 (cube limit)
				y = fmod(y, 1.0f);

				// Increment x by 0.2 for every positions
				x = i * 0.2f;
				// Remainder modulus 1.0 (cube limit)
				x = fmod(x, 1.0f);

				// std::cout << "\t" << x << "\t" << y << "\t" << z << std::endl;
			}
			else if (j == 4)
			{
				// Similar to case "j == 0", with shift of +0.1
				z = (i / 25) * 0.2f;
				z = fmod(z, 1.0f) + 0.1f;

				y = (i / 5) * 0.2f;
				y = fmod(y, 1.0f) + 0.1f;

				x = i * 0.2f;
				x = fmod(x, 1.0f) + 0.1f;

				// std::cout << "\t" << x << "\t" << y << "\t" << z << std::endl;
			}
			else ;

			// Set position to subblock
			position.setIABObjectPosition(x, y, z);
			panSubBlock->SetObjectPositionFromUnitCube(position);

			// Add to panSubBlocks
			panSubBlocks.push_back(dynamic_cast<IABObjectSubBlock*>(panSubBlock));
		}

		// Set object sub block vector
		object->SetPanSubBlocks(panSubBlocks);

		// Create a DLC for the object
		IABAudioDataDLCInterface* dlcElement = IABAudioDataDLCInterface::Create(frameRate_, sampleRate_);
		dlcElement->SetDLCSampleRate(sampleRate_);					// Set sample rate
		dlcElement->SetAudioDataID(audioDataNextID_);				// Set ID from pool

		// Associate the audio ID to the object. Post increment audio ID.
		object->SetAudioDataID(audioDataNextID_++);

		// encode PCM samples into the DLC element
		dlcElement->EncodeMonoPCMToDLC(audioSamples_, frameSampleCount_);

		// DLC element is ready to be added as sub-element to frame
		// (we don't really care about the order of frame sub-element).
		iabFrame->AddSubElement(dynamic_cast<IABAudioDataDLC*>(dlcElement));

		// Also add constructed object to IABFrame
		iabFrame->AddSubElement(dynamic_cast<IABObjectDefinition*>(object));
	}

	// Increment maxRendered_ by 118 for 118 objects
	maxRendered_ += 118;

	return;
}

void IABMaxComplexityFrame::CleanUp()
{
    IABPackerInterface::Delete(iabPacker_);
    iabPacker_ = NULL;

	delete audioSamples_;
	audioSamples_ = NULL;
}



