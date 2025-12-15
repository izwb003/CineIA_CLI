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
 * IABDecorrelation.cpp
 *
 * @file
 */

#include <math.h>

#include "renderer/IABDecorrelation/IABDecorrelation.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	static const L2APFFilterParamers IABDecorrelatorFilters[4] = {
		{ 0.32f,  107,  0.10f, 29 },
		{ -0.32f, 107, -0.10f, 29 },
		{ 0.32f,  73,   0.10f, 41 },
		{ -0.32f, 73,  -0.10f, 41 }
	};

	// ==================================================
	// NestedAllPassFilter class implementation

	// Constructor
	NestedAllPassFilter::NestedAllPassFilter()
	{
		gainCoeff_ = 0.32f;								// Default to 0.32. Range: (-1.0..+1.0). 1.0 on unit circle.
		delayLength_ = 113;								// Default 113 tap
		readWritePointer_ = 0;
		innerAPF_ = NULL;
		isEnabled_ = false;								// Default: "Not activated".
	}

	// Parameterized constructor
	NestedAllPassFilter::NestedAllPassFilter(float iGainCoeff
                                             , int32_t iDelayLength
                                             , NestedAllPassFilter* iChildAPF)
	{
		gainCoeff_ = iGainCoeff;
		delayLength_ = iDelayLength;
		readWritePointer_ = 0;
		innerAPF_ = iChildAPF;
		isEnabled_ = true;								// Activated if constructed with specific filter params.
	}

	// Destructor
	NestedAllPassFilter::~NestedAllPassFilter()
	{
		// Delete inner APF if it exists
		if (innerAPF_)
		{
			delete innerAPF_;
		}
	}

	// NestedAllPassFilter::Reset() implementation
	void NestedAllPassFilter::Reset()
	{
		// Clear delay line/beffer to 0s
		for (int32_t i = 0; i < kNestedAllpassL2LengthMax; i++)
		{
			delayBuffer_[i] = 0;
		}

		// Reset read/write pointer to 0
		readWritePointer_ = 0;

		// If child APF is present, reset it.
		if (innerAPF_)
		{
			innerAPF_->Reset();
		}

		// Note Reset() does not modify anything else
	}

	// NestedAllPassFilter::Decorrelate() implementation
	// (Implementation code from DTS pps-common-dsp lib, with minor refactoring.)
	void NestedAllPassFilter::Decorrelate(IABSampleType* ioPCMSample, IABRenderedOutputSampleCountType iSampleCount)
	{
		uint32_t i;
		float delayEndOuter, delayEndInner;
		float temp;												// temp: work variable for holding any temporary internal state

		// Check if we should process decorrelation
		// 1. isEnabled_ = true, activated
		// 2. delayLength_ > 0, must contain non-zero delay
		// 3. innerAPF_ != NULL, must contain a nested APF (we are using 2-level nested APF.) 
		if (isEnabled_
			&& (delayLength_ > 0)
			&& (innerAPF_ != NULL))
		{
			for (i = 0; i < iSampleCount; i++)
			{
				// Read the end of the delay line
				delayEndOuter = delayBuffer_[readWritePointer_];

				// Process the inner APF
				{
					delayEndInner = innerAPF_->delayBuffer_[innerAPF_->readWritePointer_];
					temp = delayEndOuter + innerAPF_->gainCoeff_ * delayEndInner;
					innerAPF_->delayBuffer_[innerAPF_->readWritePointer_] = temp;

					delayEndOuter = delayEndInner - temp * innerAPF_->gainCoeff_;
					innerAPF_->readWritePointer_++;
					innerAPF_->readWritePointer_ %= innerAPF_->delayLength_;
				}

				// Calculate the new delay line input
				temp = ioPCMSample[i] + gainCoeff_ * delayEndOuter;
				delayBuffer_[readWritePointer_] = temp;

				// Calculate the output
				ioPCMSample[i] = delayEndOuter - temp * gainCoeff_;

				// Advance the delay line
				readWritePointer_++;
				readWritePointer_ %= delayLength_;
			}
		}
	}

    // ==================================================
    // IABDecorrelation class implementation

	// Constructor
	IABDecorrelation::IABDecorrelation()
	{
		decorrelators_.clear();
		numOutputChannels_ = 0;
	}

	// Destructor
    IABDecorrelation::~IABDecorrelation()
    {
		// Delete all decorrelator instances
		for (std::vector<NestedAllPassFilter*>::iterator iter = decorrelators_.begin(); iter != decorrelators_.end(); iter++)
		{
			delete *iter;
		}

		decorrelators_.clear();
	}
    
    // IABDecorrelation::Setup() implementation
    iabError IABDecorrelation::Setup(const RenderUtils::IRendererConfiguration &iConfig)
    {
        const std::vector<RenderUtils::RenderSpeaker> *configSpeakers;
        iConfig.GetPhysicalSpeakers(configSpeakers);
        
        const std::vector<RenderUtils::RenderSpeaker> &configSpeakerList = *configSpeakers;

        // Report error if iConfigSpeakerList is empty.
        if (configSpeakerList.empty())
        {
            return kIABBadArgumentsError;
        }

        // numOutputChannels_ is the size of configSpeakerList
        numOutputChannels_ = static_cast<uint32_t>(configSpeakerList.size());
 
		// Instantiate a NestedAllPassFilter decorrelator for each of speakers (channels)
		// 
		// Dev note: as decorr is supported for object rendering only, and in binary ON/OFF mode
		// only, an enabled 2-level nested APF is created for each of the VBAP speakers.
		//
		// For non-VBAP speakers, a disabled APF decorrelator is also created, but not used (disabled).
		// It's a place holder. These place holders are still needed, as an iConfig can contain a random
		// of non-VBAP speakers. Their index and sequence is also random, determined by iConfig.
		//
        std::vector<RenderUtils::RenderSpeaker>::const_iterator iter;
        
        // Search the speaker list and instantiate decorr instances.
		//
		// After listening tests and comparisons, decorr filters are from 
		// the 4x filters in IABDecorrelatorFilters. Filters starting from 1 & onwards are created in the 
		// sequence of VBAP speakers as from iConfig configuration file.
		//
		uint32_t decorrFilterIndex = 0;

        for (iter = configSpeakerList.begin(); iter != configSpeakerList.end(); iter++)
        {
            // Is it a VBAP speaker?
            if (iConfig.IsVBAPSpeaker(iter->getName()))
            {
				// VBAP speakers, used in object rendering
				// Create an enabled 2-level nested APF. Decorr processed for these speakers/channels.

				// Inner APF first
				NestedAllPassFilter* anInnerAPF = new NestedAllPassFilter(
					  IABDecorrelatorFilters[decorrFilterIndex].innerGain_
					, IABDecorrelatorFilters[decorrFilterIndex].innerDelayLength_
					, NULL);

				if (!anInnerAPF)
				{
					return kIABMemoryError;
				}

				// Outer of L2 APF, full L2 nested with anInnerAPF as child APF
				NestedAllPassFilter* aL2NestedAPF = new NestedAllPassFilter(
					IABDecorrelatorFilters[decorrFilterIndex].outerGain_
					, IABDecorrelatorFilters[decorrFilterIndex].outerDelayLength_
					, anInnerAPF);

				if (!aL2NestedAPF)
				{
					return kIABMemoryError;
				}

				// Reset to ensure delay buffer and pointer init state
				aL2NestedAPF->Reset();

				// Add to decorrelators_
				decorrelators_.push_back(aL2NestedAPF);

				// Increment filter index to select next filter
				decorrFilterIndex++;

				// Wrap back if we run out of 4 filters
				//
				if (decorrFilterIndex >= 4)
				{
					decorrFilterIndex = 0;
				}
			}
			else
			{
				// Non-VBAP speakers. Create a disabled 1-level APF as place holder. 
				// These will be bypassed in decorr processing.
				//
				NestedAllPassFilter* aL1APFPlaceholder = new NestedAllPassFilter();

				if (!aL1APFPlaceholder)
				{
					return kIABMemoryError;
				}

				// Reset to ensure delay buffer and pointer init state
				aL1APFPlaceholder->Reset();

				// Add to decorrelators_
				decorrelators_.push_back(aL1APFPlaceholder);
			}
        }
        
        return kIABNoError;
    }

	// IABDecorrelation::Reset() implementation
	void IABDecorrelation::Reset()
	{
		// Reset delay line buffers and read/write pointers (only) for all decorrelators.
		//
		for (std::vector<NestedAllPassFilter*>::iterator iter = decorrelators_.begin(); iter != decorrelators_.end(); iter++)
		{
			(*iter)->Reset();
		}
	}

	// IABDecorrelation::DecorrelateDecorOutputs() implementation
    iabError IABDecorrelation::DecorrelateDecorOutputs(IABSampleType **ioOutputChannels
                                                      , IABRenderedOutputChannelCountType iOutputChannelCount
                                                      , IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		// Check if iOutputChannelCount is as expected.
		// This is the only check by decorrelation instance.
		//
		if (iOutputChannelCount != numOutputChannels_)
		{
			return kIABBadArgumentsError;
		}

		// Deccorelate each of the decorr-enabled speaker/channel outputs
		//
		for (uint32_t i = 0; i < iOutputChannelCount; i++)
		{
			// If this a VBAP channel for decorr?
			if (decorrelators_[i]->isEnabled_)
			{
				// Yes, decorrelation processing
				// In-place processing. ioOutputChannels[i] contains input. It is then replaced by output
				// after processing.
				decorrelators_[i]->Decorrelate(ioOutputChannels[i], iOutputSampleBufferCount);
			}
			else
			{
				// No, simply skip without processing
				// Any PCM samples in input buffers are unchanged.
				// Likely, these are all 0s.
				continue;
			}
		}

		return kIABNoError;
	}
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
