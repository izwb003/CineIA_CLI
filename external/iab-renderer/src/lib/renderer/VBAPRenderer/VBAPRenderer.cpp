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

 // Header files from "RenderUtils" library
#include "renderutils/Utils.h"

// Header files of this library
#include "renderer/VBAPRenderer/VBAPRenderer.h"

namespace IABVBAP
{
	// Constructor implementation
	VBAPRenderer::VBAPRenderer() :
		topVirtualSources_(NULL),
		rendererConfiguration_(NULL)
	{
	}

	// Destructor implementation
	VBAPRenderer::~VBAPRenderer()
	{
		if (topVirtualSources_)
		{
			delete topVirtualSources_;
			topVirtualSources_ = NULL;
		}
	}

	// VBAPRenderer::InitWithConfig() implementation
	vbapError VBAPRenderer::InitWithConfig(RenderUtils::IRendererConfiguration *iConfig)
	{
		if (iConfig == NULL)
			return kVBAPBadArgumentsError;

		// TODO - Review whether multiple calls to InitWithConfig() is allowed. (ie. re-config)
		//
		// Right now it's NOT allowed!
		//
		// (So if client need to re-configure, the recommended practice seems to
		// create another instance rather then re-use existing one.)
		//
		if (topVirtualSources_)
			return kVBAPAlreadyInitError;

		// Save renderer configuration
		// TODO: review ownership. The "save" seems to indicate that VBAPRenderer
		// 
		// instance owns the iConfig from this point on.
		// Yet, VBAPRenderer Destructor does NOT delete non-NULL rendererConfiguration_. This is rather confusing...
		// If this is intentional, Documentation need improving!
		rendererConfiguration_ = iConfig;

		// Config VBAP per rendererConfiguration_
		if (ConfigureVBAP(rendererConfiguration_, 128, 32) != true)
		{
			return kVBAPConfigurationFormatError;
		}
		else
		{
			// set member pointer to configured speakers per rendererConfiguration_
			rendererConfiguration_->GetSpeakers(renderSpeakers_);
			rendererConfiguration_->GetLFEIndex(speakerLFEIndex_);
		}

		return kVBAPNoError;
	}

	// VBAPRenderer::GetRendererConfiguration() implementation
	vbapError VBAPRenderer::GetRendererConfiguration(RenderUtils::IRendererConfiguration* &oConfig) const
	{
		oConfig = rendererConfiguration_;
		return kVBAPNoError;
	}

	// VBAPRenderer::RenderExtendedSource() implementation
	vbapError VBAPRenderer::RenderExtendedSource(vbapRendererExtendedSource* iSource)
	{
		vbapError rtnCode = kVBAPNoError;

		if (!iSource)
		{
			return kVBAPBadArgumentsError;
		}

		if (rendererConfiguration_ == NULL)
		{
			return kVBAPUnConfiguredError;
		}

		if ((iSource->renderedSpeakerGains_.size() == 0)
			|| (iSource->renderedSpeakerGains_.size() == 0))
		{
			return kVBAPBadArgumentsError;
		}

		// Checking range of extSourceGain_: [0.0f, 1.0f]
		if ((iSource->extSourceGain_ < 0.0f) || (iSource->extSourceGain_ > 1.0f))
		{
			return kVBAPParameterOutOfBoundsError;
		}

		// Clear renderedSpeakerGains_ gains
		//
		std::fill(iSource->renderedSpeakerGains_.begin(), iSource->renderedSpeakerGains_.end(), 0.0f);

		if (!ReusePreviouslyRendered(iSource))
		{
			// The input iSource has rendering parameter values that are different from any source entry 
			// in previouslyRenderedExtendedSources_. Call RenderExtent() to actually render gains.
			//
			rtnCode = RenderExtent(iSource->position_, iSource->aperture_, iSource->divergence_, iSource->renderedSpeakerGains_);

			if (rtnCode != kVBAPNoError)
			{
				return rtnCode;
			}

			// Downmix virtual speakers (gains) to channels/physical speakers (gains)
			rtnCode = SpeakerDownmix(iSource->renderedSpeakerGains_, iSource->renderedChannelGains_);

			if (rtnCode != kVBAPNoError)
			{
				return rtnCode;
			}

			// Add this source as a new source to previouslyRenderedExtendedSources_
			// by call AddToPreviouslyRendered()
			//
			AddToPreviouslyRendered(iSource);
		}

		return rtnCode;
	}

	// VBAPRenderer::RenderObject() implementation
	vbapError VBAPRenderer::RenderObject(vbapRendererObject* iObject)
	{
		vbapError err = kVBAPNoError;

		if (!iObject)
		{
			return kVBAPBadArgumentsError;
		}

		if (iObject->channelGains_.size() == 0)
		{
			return kVBAPBadArgumentsError;
		}

		// Clear channelGains_ gains. These are what client need.
		//
		std::fill(iObject->channelGains_.begin(), iObject->channelGains_.end(), 0.0f);

		// Retrive number of extended source in iObject
		uint32_t numExtendedSources = static_cast<uint32_t>(iObject->extendedSources_.size());

		// Render object, 1 on-dome or 3 interior
		if (numExtendedSources == 0)
		{
			// Error, object contains no extended source for rendering
			return kVBAPNoExtendedSourceError;
		}
		else if (numExtendedSources == 1)
		{
			// Object with single extended source: object on Dome surface
			//
			err = RenderOnDomeObject(iObject);
		}
		else
		{
			// Object with multiple extended sources.
			// Currently the likely use case is one with 3-extended sources, combinedly
			// emulating 1 interior object.
			//
			err = RenderInteriorObject(iObject);
		}

		return err;
	}

	// VBAPRenderer::RenderLFEChannel() implementation
	vbapError VBAPRenderer::RenderLFEChannel(vbapRendererLFEChannel* iLFEChannel)
	{
		if (!iLFEChannel)
		{
			return kVBAPBadArgumentsError;
		}

		// Renderer config must include an LFE speaker to support LFE content
		// Otherwise, it's an error condition.
		//
		if (speakerLFEIndex_ < 0)
		{
			return kVBAPNoLFEChannelError;
		}

		if ((iLFEChannel->lfeGain_ < 0.0f) || (iLFEChannel->lfeGain_ > 1.0f))
		{
			return kVBAPParameterOutOfBoundsError;
		}

		// Clear speaker gains
		//
		std::fill(iLFEChannel->speakerGains_.begin(), iLFEChannel->speakerGains_.end(), 0.0f);

		// Directly apply LFE fGain as LFE speaker gain. 
		// Note that LFE speaker can be virtual, and in which case, a follow-up downmix (map) 
		// is expected to happen, as below.
		//
		iLFEChannel->speakerGains_[speakerLFEIndex_] = iLFEChannel->lfeGain_;

		// "Downmix" to transfer LFE speaker gain to channel gain.
		//
		SpeakerDownmix(iLFEChannel->speakerGains_, iLFEChannel->channelGains_);

		return kVBAPNoError;
	}
	
	// VBAPRenderer::CleanupPreviouslyRendered() implementation
	void VBAPRenderer::CleanupPreviouslyRendered()
	{

		for (std::vector<vbapRendererExtendedSource>::iterator iter = previouslyRenderedExtendedSources_.begin(); iter != previouslyRenderedExtendedSources_.end();)
		{
			if (!(*iter).touched_) {

				std::vector<vbapRendererExtendedSource>::iterator  tmpIter = iter;
				iter = previouslyRenderedExtendedSources_.erase(tmpIter);
			}
			else {

				(*iter).touched_ = false;
				++iter;
			}
		}
	}

	// VBAPRenderer::ResetPreviouslyRendered() implementation
	void VBAPRenderer::ResetPreviouslyRendered()
	{
		// reset rendered extent source histories
		previouslyRenderedExtendedSources_.clear();
	}

	// VBAPRenderer::GetVBAPCacheSize() implementation
	uint32_t VBAPRenderer::GetVBAPCacheSize()
	{
		return static_cast<uint32_t>(previouslyRenderedExtendedSources_.size());
	}

	// *****************************************************************************
	// Private method implementation below
	//

	// VBAPRenderer::SpeakerDownmix() implementation
	vbapError VBAPRenderer::SpeakerDownmix(const std::vector<float>& iSpeakerGains, std::vector<float>& oChannelGains)
	{
		uint32_t speakerCount = static_cast<uint32_t>(iSpeakerGains.size());
		uint32_t channelCount = static_cast<uint32_t>(oChannelGains.size());
        int32_t outputIndex = 0;

		if ((speakerCount == 0)
			|| (channelCount == 0))
		{
			return kVBAPBadArgumentsError;
		}

		// Clear oChannelGains gains
		//
		std::fill(oChannelGains.begin(), oChannelGains.end(), 0.0f);

		// Downmix processing from speakerGains to channelGains
		for (uint32_t speaker = 0; speaker < speakerCount; speaker++)
		{
			for (std::vector<RenderUtils::DownmixValue>::const_iterator iter = renderSpeakers_->at(speaker).downmix_.begin(); 
			                                                            iter != renderSpeakers_->at(speaker).downmix_.end();
																		iter++)
			{
                // Config file speaker and channel index notes:
                
                // The following is an example config file with non-sequential channel index and index gap, see the "chan" column
                // Note that there are only 7 output channels but the highest index is 8
                //
                // # Speaker definitions:
                // # s   name   chan   theta   phi   exception-uri
                // s   L        1   -30.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020101.00000000
                // s   C        0     0.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020103.00000000
                // s   R        6    30.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020102.00000000
                // s   LS       4  -110.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020105.00000000
                // s   RS       7   110.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020106.00000000
                // s   LFE      3     0.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020104.00000000
                // s   TSC1     8     0.00   90.00

                // To mix speaker gains to correct output channel, use RendererConfigurationFile::GetRendererOutputIndexbyChannel
                // to get renderer output index associated with the speaker's channel index "chan"
                // indicates channel position in the rendered output buffer block
                if (RenderUtils::kNoRendererConfigurationError != rendererConfiguration_->GetRendererOutputIndexbyChannel((*iter).ch_, outputIndex))
                {
                    // Error mapping config file speaker channel number
                    return kVBAPParameterOutOfBoundsError;
                }
                
				oChannelGains[outputIndex] += (*iter).coefficient_ * iSpeakerGains.at(speaker);
			}
		}

		return kVBAPNoError;
	}

	// VBAPRenderer::RenderOnDomeObject() implementation
	vbapError VBAPRenderer::RenderOnDomeObject(vbapRendererObject* iObject)
	{
		// Object with single extended source: object on Dome surface
		//
		vbapError err = kVBAPNoError;

		// Denfensive double check (these should have been checked at RenderObject() call
		if ((iObject->channelGains_.size() == 0)
			|| (iObject->extendedSources_.size() == 0))
		{
			return kVBAPBadArgumentsError;
		}

		// Render the one and only extended source iObject->extendedSources_[0]
		err = RenderExtendedSource(&(iObject->extendedSources_[0]));

		if (err != kVBAPNoError)
		{
			// Error found, early exit
			return err;
		}

		// Copy rendered channel gains from the single extended source to iObject
		iObject->channelGains_ = iObject->extendedSources_[0].renderedChannelGains_;

		// For single extended source, extSourceGain_ is always 1.
		// Set it to iObject->vbapNormGains_
		iObject->vbapNormGains_ = iObject->extendedSources_[0].extSourceGain_;

		// Normalize channel gains
		err = NormalizeChannelGains(iObject->vbapNormGains_, iObject->channelGains_);

		if (err != kVBAPNoError)
		{
			// Error found, early exit
			return err;
		}

		// apply object gain
		err = ApplyObjectGainToChannelGains(iObject->objectGain_, iObject->channelGains_);

		return err;
	}

	// VBAPRenderer::RenderInteriorObject() implementation
	vbapError VBAPRenderer::RenderInteriorObject(vbapRendererObject* iObject)
	{
		// Object with multiple extended sources.
		// The processing routine support an object containing other numbers of multiple extended 
		// sources. The likely use case is one with 3-extended sources which in combination 
		// emulates 1 interior object.
		//

		vbapError err = kVBAPNoError;

		// Number of channel gains
		uint32_t numChannels = static_cast<uint32_t>(iObject->channelGains_.size());

		// Number of extended source in iObject
		uint32_t numExtendedSources = static_cast<uint32_t>(iObject->extendedSources_.size());

		// Denfensive double check (these should have been checked at RenderObject() call
		if ((numChannels == 0)
			|| (numExtendedSources == 0))
		{
			return kVBAPBadArgumentsError;
		}

		// Init vbapNormGains_ to 0
		iObject->vbapNormGains_ = 0.0;

		for (uint32_t i = 0; i < numExtendedSources; i++)
		{
			// Render extended source
			err = RenderExtendedSource(&(iObject->extendedSources_[i]));

			if (err != kVBAPNoError)
			{
				// Error found, early exit
				return err;
			}

			// Aggregate rendered channel gains from the extended source to iObject->channelGains_
			for (uint32_t j = 0; j < numChannels; j++)
			{
				iObject->channelGains_[j] += iObject->extendedSources_[i].renderedChannelGains_[j] * iObject->extendedSources_[i].extSourceGain_;
			}

			// Also add up metadata gains to determine group normalisation target value
			// ie. add up extended source gains for normalization
			iObject->vbapNormGains_ += iObject->extendedSources_[i].extSourceGain_;
		}

		// Normalize channel gains
		err = NormalizeChannelGains(iObject->vbapNormGains_, iObject->channelGains_);

		if (err != kVBAPNoError)
		{
			// Error found, early exit
			return err;
		}

		// apply object gain
		err = ApplyObjectGainToChannelGains(iObject->objectGain_, iObject->channelGains_);

		return err;
	}

	// VBAPRenderer::NormalizeChannelGains() implementation
	vbapError VBAPRenderer::NormalizeChannelGains(float iNormGain, std::vector<float> &ioChannelGains)
	{
		if (ioChannelGains.size() == 0)
		{
			return kVBAPBadArgumentsError;
		}

		float norm = std::sqrt(std::inner_product(ioChannelGains.begin()
			, ioChannelGains.end()
			, ioChannelGains.begin(), 0.0f));

		// perform normalisation only if norm and iObjectGain values are significant
		if (norm > CoreUtils::kEPSILON && iNormGain > 0.0f)
		{
			float gain = iNormGain / norm;			// gain: combining normalization and applying iObjectGain
			std::transform(ioChannelGains.begin()
				, ioChannelGains.end()
				, ioChannelGains.begin()
				, std::bind2nd(std::multiplies<float>(), gain));

		}
		else
		{
			// otherwise set the gains to zero
			std::fill(ioChannelGains.begin(), ioChannelGains.end(), 0.0f);
		}

		return kVBAPNoError;
	}

	// VBAPRenderer::ApplyObjectGainToChannelGains() implementation
	vbapError VBAPRenderer::ApplyObjectGainToChannelGains(float iObjectGain, std::vector<float> &ioChannelGains)
	{
		if (ioChannelGains.size() == 0)
		{
			return kVBAPBadArgumentsError;
		}

		// Aggregate iObjectGain to channel gains
		for (uint32_t i = 0; i < ioChannelGains.size(); i++)
		{
			ioChannelGains[i] *= iObjectGain;
		}

		return kVBAPNoError;
	}

	// VBAPRenderer::AddToPreviouslyRendered() implementation
	void VBAPRenderer::AddToPreviouslyRendered(vbapRendererExtendedSource* iSource)
	{
		iSource->touched_ = true;                                           // Set flag for source entry as being used (touched)
		previouslyRenderedExtendedSources_.push_back((*iSource));			// Copy pushed/added to vector
	}

	// VBAPRenderer::ReusePreviouslyRendered() implementation
	bool VBAPRenderer::ReusePreviouslyRendered(vbapRendererExtendedSource* iSource)
	{
		bool alreadyrendered = false;

		for (std::vector<vbapRendererExtendedSource>::iterator iter = previouslyRenderedExtendedSources_.begin(); iter != previouslyRenderedExtendedSources_.end(); iter++)
		{
			if (iter->HasSameRenderingParams(iSource))
			{
				iSource->renderedSpeakerGains_ = iter->renderedSpeakerGains_;    // Copy rendered speaker gains from cache
				iSource->renderedChannelGains_ = iter->renderedChannelGains_;    // Copy rendered channel gains from cache
				iter->touched_ = true;                                           // Mark source entry in previouslyRenderedExtendedSources_ as being used (touched)
				alreadyrendered = true;
				break;
			}
		}

		return alreadyrendered;
	}

	// =================================================================================
	// Core VBAP Algorithm
	//

	// The following two inline functions are used to circumvent cross-platform differences
	// of std::atan2()
	inline float _renderer_rtz(float value)
	{
		return (float)(value >= 0 ? -std::floor(-value + 0.5) : std::floor(value + 0.5));
	}

	inline float _renderer_atan2(float value1, float value2)
	{
		return 0.001f * _renderer_rtz((float)std::atan2(value1, value2) / 0.001f);
	}

	// VBAPRenderer::ConfigureVBAP() implementation
	bool VBAPRenderer::ConfigureVBAP(
		RenderUtils::IRendererConfiguration* iConfig
		, int32_t iThetaDivs
		, int32_t iPhiDivs
		)
	{
		const std::vector<RenderUtils::RenderSpeaker>* speakersVBAP = NULL;

		iConfig->GetSpeakers(speakersVBAP);
		if (speakersVBAP == NULL)
			return false;

		totalSpeakerGains_.clear();
		totalSpeakerGains_.resize(speakersVBAP->size(), 0);

		if (!topVirtualSources_) {
			topVirtualSources_ = new RenderUtils::HemisphereVirtualSources();
		}

		const int32_t phiDivs = iPhiDivs;
		const int32_t thetaDivs = iThetaDivs;

		BuildHemisphere(topVirtualSources_, totalSpeakerGains_, thetaDivs, phiDivs);

		return true;
	}

	// VBAPRenderer::BuildHemisphere() implementation
	void VBAPRenderer::BuildHemisphere(
		RenderUtils::HemisphereVirtualSources *oHemisphere
		, std::vector<float> &oTotalSpeakerGains
		, int32_t iThetaDivs
		, int32_t iPhiDivs
		)
	{
		oHemisphere->fDeltaPhi = CoreUtils::kPI / 2.0f / iPhiDivs;
		oHemisphere->fLongitudes.clear();

		int32_t count = iPhiDivs;

		for (int32_t i = 0; i <= count; i++)
		{
			float phi = i * oHemisphere->fDeltaPhi;
			int32_t n = i == 0 ? 1 : (int32_t)std::floor(iThetaDivs * std::sin(phi));

			RenderUtils::LongitudeVirtualSources rendererVirtualSource;
			RenderUtils::VirtualSourceTree* vst = new RenderUtils::VirtualSourceTree(oTotalSpeakerGains.size());

			rendererVirtualSource.fMaxThetaIndex = n - 1;
			rendererVirtualSource.fDeltaTheta = 2 * CoreUtils::kPI / (float(n));
			rendererVirtualSource.fPhi = phi;
			rendererVirtualSource.fPhiIndex = i;
			rendererVirtualSource.fVirtualSources = vst;

			std::vector<RenderUtils::VirtualSource> vsv(n);

			for (int32_t j = 0; j < n; j++)
			{
				float theta = rendererVirtualSource.fDeltaTheta*((float)j);

				vsv[j].fThetaIndex = j;
				vsv[j].fTheta = theta;
				vsv[j].fSpeakerGains.resize(totalSpeakerGains_.size(), 0.0f);


				float x = std::sin(theta) * std::sin(phi);
				float y = std::cos(theta) * std::sin(phi);
				float z = std::cos(phi);

				CoreUtils::Vector3 src(
					RenderUtils::clamp(x, -1.0, 1.0),
					RenderUtils::clamp(y, -1.0, 1.0),
					RenderUtils::clamp(z, -1.0, 1.0)
					);

				RenderPatch(src, vsv[j].fSpeakerGains);

				// TODO: report kVBAPObjectPositionNotInConvexHullError here
				std::transform(vsv[j].fSpeakerGains.begin(), vsv[j].fSpeakerGains.end(), oTotalSpeakerGains.begin(), oTotalSpeakerGains.begin(), std::plus<float>());
			}

			rendererVirtualSource.fVirtualSources->build(vsv.begin(), vsv.end());

			oHemisphere->fLongitudes.push_back(rendererVirtualSource);
		}

		return;
	}

	// VBAPRenderer::RenderExtent() implementation
	vbapError VBAPRenderer::RenderExtent(
		const CoreUtils::Vector3& iSource
		, float iAperture
		, float iDivergence
		, std::vector<float> &oSpeakerGains
		)
	{
		assert(iAperture <= CoreUtils::kPI && iAperture >= 0.0f);
		assert(iDivergence <= CoreUtils::kPI && iDivergence >= 0.0f);

		float norm = iSource.norm();										// "iSource.norm()" mathematically calulates the vector length (or magnitude) of vector "iSource".
		CoreUtils::Vector3 center3 = iSource / norm;						// == "iSource * (1 / norm)". The factor "1 / norm" is used to normalize vector "iSource" to radius of "1".

		float phi = std::acos(center3.getZ());
		float theta = _renderer_atan2(center3.getX(), center3.getY());

		std::vector<float> tmpSpeakerGains(oSpeakerGains.size(), 0.0f);		// working buffer to save results from RenderHemisphere() or RenderPatch() below
		int32_t foundVirtualSources = 0;
		vbapError err;

		// die if the source is in an hemisphere not covered by speakers
		if ((center3.getZ() < 0) || (center3.getZ() >= 0 && !topVirtualSources_))
		{
			return kVBAPObjectPositionNotInConvexHullError;
		}

		// For iSource with non-zero extent parameters, call RenderHemisphere() for extent rendering
		if (iAperture != 0.0f || iDivergence != 0.0f)
		{
			err = RenderHemisphere(theta, phi, iAperture, iDivergence, foundVirtualSources, tmpSpeakerGains, *topVirtualSources_);

			if (err != kVBAPNoError)
			{
				return err;
			}
		}

		// If foundVirtualSources < 2, (this includes the point-source rendering as foundVirtualSources = 0)
		// call RenderPatch() to render instead. This is pure point-source rendering (iSource being the only
		// input parameter), with both aperture and divergence set to 0.
		//
		// Note, any partial results from RenderHemisphere(), if called, is cleared and not used.
		//
		if (foundVirtualSources < 2)
		{
			std::fill(tmpSpeakerGains.begin(), tmpSpeakerGains.end(), 0.0f);

			// rendering fails if the iSource is outside the convex hull formed by the loudspeakers specified
			// in the config file, e.g. if the iSource is in the lower hemisphere, and/or no speakers are present
			// there.
			//
			if (RenderPatch(iSource, tmpSpeakerGains) <= 0)
			{
				return kVBAPObjectPositionNotInConvexHullError;
			}
		}

		// Aggregate rendered speaker gains to oSpeakerGains. (not overwrite).
		std::transform(tmpSpeakerGains.begin(), tmpSpeakerGains.end(), oSpeakerGains.begin(), oSpeakerGains.begin(), std::plus<float>());

		return kVBAPNoError;
	}

	// VBAPRenderer::RenderHemisphere() implementation
	vbapError VBAPRenderer::RenderHemisphere(
		float iTheta
		, float iPhi
		, float iAperture
		, float iDivergence
		, int32_t &oFoundSources
		, std::vector<float> &oSpeakerGains
		, const RenderUtils::HemisphereVirtualSources& iVirtualSources
		) const
	{
		if (iTheta < 0)
		{
			iTheta = iTheta + 2.0f * CoreUtils::kPI;
		}

		// Rounds phi to the nearest virtual source ring.
		// This ensures that virtual sources are selected when aperture == 0
		// and divergence > 0.
		//
		iPhi = iVirtualSources.fDeltaPhi * (float)floor(iPhi / iVirtualSources.fDeltaPhi + 0.5);

		// integrate over longitudes
		std::vector<RenderUtils::LongitudeVirtualSources>::const_iterator iter = iVirtualSources.fLongitudes.begin();

		while (iter != iVirtualSources.fLongitudes.end())
		{
			float sigma;

			// check error
			ComputeLatitudeSigma(iPhi, iAperture, (*iter).fPhi, sigma);

			if (sigma != -1)
			{
				float arc = RenderUtils::clamp(iDivergence + sigma, 0, CoreUtils::kPI);

				float thetamax = iTheta + arc;
				float thetamin = iTheta - arc;

				int32_t thetamin_i = (int32_t)floor(thetamin / (*iter).fDeltaTheta + 0.5f);
				int32_t thetamax_i = (int32_t)floor(thetamax / (*iter).fDeltaTheta + 0.5f);

				if (thetamax_i - thetamin_i > (*iter).fMaxThetaIndex)
				{
					thetamin_i = 0;
					thetamax_i = (*iter).fMaxThetaIndex;
				}

				std::vector<float> tmpSpeakerGains(oSpeakerGains.size(), 0.0f);

				if (thetamax_i > (*iter).fMaxThetaIndex)
				{
					oFoundSources += (*iter).fVirtualSources->averageGainsOverRange(0,
						thetamax_i - (*iter).fMaxThetaIndex - 1,
						0,
						(*iter).fMaxThetaIndex,
						tmpSpeakerGains);

					oFoundSources += (*iter).fVirtualSources->averageGainsOverRange(thetamin_i,
						(*iter).fMaxThetaIndex,
						0,
						(*iter).fMaxThetaIndex,
						tmpSpeakerGains);
				}
				else if (thetamin_i < 0)
				{
					oFoundSources += (*iter).fVirtualSources->averageGainsOverRange(0,
						thetamax_i,
						0,
						(*iter).fMaxThetaIndex,
						tmpSpeakerGains);

					oFoundSources += (*iter).fVirtualSources->averageGainsOverRange((*iter).fMaxThetaIndex + thetamin_i + 1,
						(*iter).fMaxThetaIndex,
						0,
						(*iter).fMaxThetaIndex,
						tmpSpeakerGains);
				}
				else
				{
					oFoundSources += (*iter).fVirtualSources->averageGainsOverRange(thetamin_i, thetamax_i, 0, (*iter).fMaxThetaIndex, tmpSpeakerGains);
				}

				std::transform(tmpSpeakerGains.begin(), tmpSpeakerGains.end(), oSpeakerGains.begin(), oSpeakerGains.begin(), std::plus<float>());
			}

			iter++;
		}

		return kVBAPNoError;
	}

	// VBAPRenderer::ComputeLatitudeSigma() implementation
	void VBAPRenderer::ComputeLatitudeSigma(
		float iPhi
		, float iAperture
		, float iLatitude
		, float &oSigma
		)
	{
		// ===============================================================================================
		// Function calculates angle sigma describing the portion of a latitude ring of virtual sources that
		// is within an object's aperture disk. This is used to select the virtual sources that contribute to an object's extent.
        // 
		// iPhi: 0 zenith, pi at bottom
		// iAperture: 0 - pi
		// iLatitude: elevation of aperture disk under investigation. 0 - pi, pi/2 horizontal
        // 
		// Result: oSigma is either set to -1 (no arc on this aperture disk) or a value between 0 and pi

		float ca = std::cos(iAperture);
		float cp = std::cos(iPhi + iLatitude);
		float cm = std::cos(iPhi - iLatitude);

		float en = 2.0f*ca - (cp + cm);
		float dn = cm - cp;

		oSigma = -1;

		if (ca <= cp)
		{
			oSigma = CoreUtils::kPI;
		}
		else if (cm > cp)											// dn > 0 : with phi and latitude between 0 and pi, dn can not get < 0.
		{
			if (ca == cm)											//  en == dn
			{
				oSigma = 0;
			}
			else if (ca < cm)										//  en < dn
			{
				oSigma = std::acos(en / dn);
			}
		}
	}

	// VBAPRenderer::RenderPatch() implementation
	float VBAPRenderer::RenderPatch(
		const CoreUtils::Vector3& iSource
		, std::vector<float> &oSpeakerGains
		)
	{
		if (rendererConfiguration_ == NULL)
			return 0;

		const std::vector<RenderUtils::RenderSpeaker>* speakersVBAP = NULL;

		rendererConfiguration_->GetSpeakers(speakersVBAP);

		// TODO: this test should be done at init */
		if (speakersVBAP == NULL)
			return 0;

		// TODO: this test should be done at init */
		if (oSpeakerGains.size() != speakersVBAP->size())
			return 0;

		// TODO: this test should be done at init
		const std::vector<RenderUtils::RenderPatch>* patchesVBAP = NULL;
		rendererConfiguration_->GetPatches(patchesVBAP);
		if (patchesVBAP == NULL)
			return 0;

		std::vector<RenderUtils::RenderPatch>::size_type numPatches = patchesVBAP->size();

		if (!numPatches)
			return 0;

		// TODO: this test should be done at init
		if (speakersVBAP->size() < totalSpeakerGains_.size())
		{
			totalSpeakerGains_.resize(speakersVBAP->size());
		}

		std::fill(totalSpeakerGains_.begin(), totalSpeakerGains_.end(), 0.0f);
		CoreUtils::Vector3 coefs(0, 0, 0);

		float countActivePatches = 0;

		// "iSource.norm()" mathematically calulates the vector length (or magnitude) of vector "iSource".
		// This is equivalent to "nsource = iSource * (1 / iSource.norm()". The factor "1 / iSource.norm()" 
		// is used to normalize vector "iSource" to radius of "1".
		//
		// We are looping over the configured VBAP triangle patches, calculating the gains for the three 
		// contributing speakers. Depending on the coefficients we get from each patch, they are added 
		// differently to the total speaker gains.
		//
		// We are distinguishing between sources that are 
		// 1. inside the triangle, 
		// 2. on the border of a triangle, or
		// 3. coincident with one of the triangle corners.
		//
		CoreUtils::Vector3 normalizedSource = iSource / iSource.norm();

		for (uint32_t i = 0; i < numPatches; i++)
		{
			coefs = (*patchesVBAP)[i].basis_ * normalizedSource;

			// First we determine if iSource is rendered at all by this triangle patch.
			if (coefs.getX() < -CoreUtils::kEPSILON
				|| coefs.getY() < -CoreUtils::kEPSILON
				|| coefs.getZ() < -CoreUtils::kEPSILON)
			{
				continue;
			}

			// Only gains greater than kEPSILON add to the number of significant gains (nsg).
			// Gains between -kEPSILON and kEPSILON thereby constitute the 'border' of the triangle.
			int32_t numSignificantGains = ((coefs.getX() > CoreUtils::kEPSILON) 
				+ (coefs.getY() > CoreUtils::kEPSILON) 
				+ (coefs.getZ() > CoreUtils::kEPSILON));

			if (numSignificantGains == 2)
			{
				// We have 2 significant gains. This means we are on a triangle border that will
				// be rendered twice. We have to divide the gains by two and increase the count only by 0.5
				// in order to not skew the coefficients when a 3rd or more triangles overlap with this border.
				totalSpeakerGains_.at((*patchesVBAP)[i].s1_) += 0.5f * coefs.getX();
				totalSpeakerGains_.at((*patchesVBAP)[i].s2_) += 0.5f * coefs.getY();
				totalSpeakerGains_.at((*patchesVBAP)[i].s3_) += 0.5f * coefs.getZ();

				countActivePatches += 0.5;
			}
			else
			{
				// We have either 1 or 3 significant gains.
				// iSource is either in the triangle or directly coincident with a VBAP speaker.
				totalSpeakerGains_.at((*patchesVBAP)[i].s1_) += coefs.getX();
				totalSpeakerGains_.at((*patchesVBAP)[i].s2_) += coefs.getY();
				totalSpeakerGains_.at((*patchesVBAP)[i].s3_) += coefs.getZ();

				countActivePatches++;
			}

			// If only one gain was significant, the source coincides with the VBAP speaker at a triangle corner.
			// We can stop looking at more patches, since no other significant contribution is to be expected.
			if (numSignificantGains == 1)
			{
				break;
			}
		}

		// Speaker gains are now averaged over the number of contributing triangle patches.
		// Triangle borders only count half as they are rendered twice.
		if (countActivePatches != 0)
		{
			for (uint32_t i = 0; i < totalSpeakerGains_.size(); ++i) {
				totalSpeakerGains_[i] /= countActivePatches;

				// Write coefficients to output array (speaker downmix now occurs in Renderer.cpp)
				oSpeakerGains[i] += totalSpeakerGains_[i];
			}
		}

		return countActivePatches;
	}

} // namespace IABVBAP
