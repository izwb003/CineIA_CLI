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

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <limits>

// Platform
#ifdef __APPLE__
#define USE_MAC_ACCELERATE
#endif

#ifdef USE_MAC_ACCELERATE
#include "coreutils/VectDSPMacAccelerate.h"
#else
#include "coreutils/VectDSP.h"
#endif

// Header files of this library
#include "renderer/ChannelGainsProcessor/ChannelGainsProcessor.h"

namespace IABGAINSPROC
{
	// =================================================================================
	// ChannelGainsProcessor implementation
	//

	// Constructor implementation
	ChannelGainsProcessor::ChannelGainsProcessor()
	{
	}

	// Destructor implementation
	ChannelGainsProcessor::~ChannelGainsProcessor()
	{
	}

	// ChannelGainsProcessor::ApplyChannelGains() implementation
	gainsProcError ChannelGainsProcessor::ApplyChannelGains(
		int32_t iObjectID
		, const float *iInputSamples
		, uint32_t iSampleCount
		, float **oOutputSamples
		, uint32_t iChannelCount
		, bool iInitializeOutputBuffers
		, const std::vector<float>& iTargetChannelGains
		, bool iEnableSmoothing
		)
	{
		// Check for null pointers, and size of iTargetChannelGains must agree with iChannelCount
		if (!iInputSamples
			|| !oOutputSamples
			|| (iSampleCount == 0)
			|| (iChannelCount == 0)
			|| (iTargetChannelGains.size() != iChannelCount))
		{
			return kGainsProcBadArgumentsError;
		}

		// Retrieve current channel gains from internally save history, per iObjectID
		// If not found, add a new entry to history
		//
		if (entityGainHistory_.find(iObjectID) == entityGainHistory_.end())
		{
			EntityPastChannelGains& newEntryInHistory = entityGainHistory_[iObjectID];

			if (newEntryInHistory.channelGains_.size() != iChannelCount)
			{
				newEntryInHistory.channelGains_.resize(iChannelCount);
			}

			// PACT-1940: Revert to old init 
			// If there is no existing history then initialise the
			// new history object with the current target.
			// If you were to initialise this to zero then the 
			// resulting ramp might cause undesired audio
			// artifacts.

			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				newEntryInHistory.channelGains_[i] = iTargetChannelGains[i];
			}
		}

		EntityPastChannelGains& currentChannelGains = entityGainHistory_[iObjectID];

		// If smoothing is enabled do extra checking
		if (iEnableSmoothing)
		{
			// size of iCurrentChannelGains must also agree with iChannelCount
			if (currentChannelGains.channelGains_.size() != iChannelCount)
			{
				return kGainsProcBadArgumentsError;
			}

			// Compare iCurrentChannelGains with iTargetChannelGains
			// to decide if smoothing is really nedded
			//
			// We first assume that smoothing may not be needed...
			iEnableSmoothing = false;

			// loop through channel gains - compare start with target values...
			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				// If difference found, revert back to smoothing
				if (iTargetChannelGains[i] != currentChannelGains.channelGains_[i])
				{
					iEnableSmoothing = true;
					break;
				}
			}
		}

		// Check output buffer pointer for each of output channels.
		// Initializing output buffer if requested
		for (uint32_t i = 0; i < iChannelCount; i++)
		{
			if (!oOutputSamples[i])
			{
				return kGainsProcBadArgumentsError;
			}

			// Reset output buffer samples to "0" only if (iInitializeOutputBuffers == true)
			if (iInitializeOutputBuffers)
			{
				memset(oOutputSamples[i], 0, sizeof(float) * iSampleCount);
			}
		}

		// ******************************************
		// VectDSP acceleration engine
		CoreUtils::VectDSPInterface *vectDSP;

#ifdef USE_MAC_ACCELERATE
		vectDSP = new CoreUtils::VectDSPMacAccelerate(MAX_RAMP_SAMPLES);
#else
		vectDSP = new CoreUtils::VectDSP();
#endif

		// smoothedGains points to start of working gain buffer. The buffer stores sample-by-sample
		// gains after smoothing processing.
		//
		float *smoothedGains;
		// Allocation to maximum size in sample (ie. MAX_RAMP_SAMPLES), as designed by algorithm
		// to cover all scenarios of realRampPeriod
		//
		smoothedGains = new float[MAX_RAMP_SAMPLES];

		// smoothedGains points to start of working buffer for storing gain processed sample.
		//
		float *gainAppliedSamples;
		gainAppliedSamples = new float[iSampleCount];

		// Apply channel gains
		//
		if (iEnableSmoothing)									// Smoothing enabled
		{
			// Calculate gain changing ramp period, slope, etc.
			// rampPeriod contains an initilialized value per iSampleCount
			//
			uint32_t initRampPeriod = (MAX_RAMP_SAMPLES < iSampleCount) ? MAX_RAMP_SAMPLES : iSampleCount;

			// realRampPeriod is the final ramp period value that is created when capping slope. ie gain change rate.
			// By design, slope (rate of gain change per sample period must be capped
			// to reduce transients - the goal of any smoothing algorithm
			// MAX_SLOPE is an empirical design thredshold for 48kHz audio
			//
			uint32_t realRampPeriod = 0;

			float currentGain = 0.0f;
			float targetGain = 0.0f;
			float gainDiff = 0.0f;
			float slope = 1.0f;

			// Calculate ramp rate for each of output channels, based on corresponding gain changes
			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				currentGain = currentChannelGains.channelGains_[i];			// current gain value
				targetGain = iTargetChannelGains[i];						// target gain to be reached in rampPeriod samples
				gainDiff = targetGain - currentGain;						// gain difference covered in smoothing ramp
				slope = 0.0;												// init

				// Make sure there is no divide by 0
				if (initRampPeriod != 0)
				{
					slope = gainDiff / initRampPeriod;						// slope of ramp, ie. gain changes for every next sample. (This algorithm implements a linear change for linear scale factor.)
				}
				else
				{
					// divide by 0 error
					delete vectDSP;
					delete[] smoothedGains;
					delete[] gainAppliedSamples;

					return kGainsProcDivisionByZeroError;
				}

				// Check slope against max limit. If it exceeds, cap it at max.
				// Revise ramp period based on set slope value and save it in realRampPeriod
				//
				if (slope > MAX_SLOPE)
				{
					// gain ramp up
					slope = MAX_SLOPE;
					realRampPeriod = RAMP_SAMPLE_MAX_SLOPE;
				}
				else if (slope < -MAX_SLOPE)
				{
					// gain ramp down
					slope = -MAX_SLOPE;
					realRampPeriod = RAMP_SAMPLE_MAX_SLOPE;
				}
				else if (slope == 0.0)
				{
					// No ramp needed
					realRampPeriod = 0;										// Don't use division if slope is 0. Value "0" means no ramp is needed.
				}
				else
				{
					// initRampPeriod already inside limit - simply use it.
					realRampPeriod = initRampPeriod;
				}

				// At this point, the resulting maximum value of realRampPeriod is MAX_RAMP_SAMPLES, currenlty 4800 samples.
				// the minimum value is either
				// 1) 1/MAX_SLOPE (which currently corresponds to 480 samples), or
				// 2) = iSampleCount, when (iSampleCount < 1/MAX_SLOPE), or
				// 3) = 0, when (slope == 0.0)
				// 

				// create smoothing ramp from slope
				//
				currentGain += slope;														// incrementing first gain one step beyond previously stored gain.
				vectDSP->ramp(currentGain, targetGain, smoothedGains, realRampPeriod);		// realRampPeriod is between 0 and 4800 samples.

																							// adding constant portion after ramp if iSampleCount is longer than realRampPeriod (rarely happens)
				if (realRampPeriod < iSampleCount)
				{
					vectDSP->fill(targetGain, smoothedGains + realRampPeriod, iSampleCount - realRampPeriod);
				}

				currentGain = smoothedGains[iSampleCount - 1];

				// Multiply input with smoothedGains ramp, store result in gainAppliedSamples
				vectDSP->mult(iInputSamples, smoothedGains, gainAppliedSamples, iSampleCount);

				// Find address of output samples for channel #[i]
				float *channelOutput = oOutputSamples[i];

				// Add (accummulate) gainAppliedSamples to output samples for channel.
				vectDSP->add(channelOutput, gainAppliedSamples, channelOutput, iSampleCount);

				// Storing gain value that has been reached on channel by end of iSampleCount
				currentChannelGains.channelGains_[i] = currentGain;
				currentChannelGains.touched_ = true;
			}
		}
		else
		{
			// Smoothing disabled. Apply gains in iTargetChannelGains uniformly

			// Apply gains for each output channels
			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				// Fill smoothedGains array with identical gain value (no smoothing applied here.)
				vectDSP->fill(iTargetChannelGains[i], smoothedGains, iSampleCount);

				// Multiply input with smoothedGains, store result in gainAppliedSamples
				vectDSP->mult(iInputSamples, smoothedGains, gainAppliedSamples, iSampleCount);

				// Find address of output samples for channel #[i]
				float *channelOutput = oOutputSamples[i];

				// Add (accummulate) gainAppliedSamples to output samples for channel.
				vectDSP->add(channelOutput, gainAppliedSamples, channelOutput, iSampleCount);

				// Storing gain value that has been reached on channel by end of iSampleCount
				currentChannelGains.channelGains_[i] = iTargetChannelGains[i];
				currentChannelGains.touched_ = true;
			}
		}

		// Delete vector engine
		delete vectDSP;

		// delete smoothedGains
		delete[] smoothedGains;

		// delete gainAppliedSamples
		delete[] gainAppliedSamples;

		return kGainsProcNoError;
	}

	// ChannelGainsProcessor::UpdateGainsHistory() implementation
	void ChannelGainsProcessor::UpdateGainsHistory(void)
	{
		// reset object gain history
		for (std::map<int32_t, EntityPastChannelGains>::iterator iter = entityGainHistory_.begin(); iter != entityGainHistory_.end();)
		{
			if (!(*iter).second.touched_)
			{
				std::map<int32_t, EntityPastChannelGains>::iterator tmpIter = iter;
				++iter;

				entityGainHistory_.erase(tmpIter);
			}
			else
			{
				(*iter).second.touched_ = false;
				++iter;
			}
		}
	}

	// ChannelGainsProcessor::ResetGainsHistory() implementation
	void ChannelGainsProcessor::ResetGainsHistory(void)
	{
		// reset smoothing gain histories
		entityGainHistory_.clear();
	}


} // namespace IABGAINSPROC
