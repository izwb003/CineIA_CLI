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
 * Header file for the IAB VBAP Renderer implementation.
 *
 * @file
 */

#ifndef __VBAPRENDERER_H__
#define __VBAPRENDERER_H__

#include <map>

// Header files from "CoreUtils" library
#include "coreutils/VectDSPInterface.h"

// Header files from "RenderUtils" library
#include "renderutils/VirtualSources.h"
#include "renderutils/IRendererConfiguration.h"

// Header files of this library
#include "renderer/VBAPRenderer/VBAPRendererDataStructures.h"

namespace IABVBAP
{
	/**
	*
	* Core VBAP Renderer class, for object rendering.
	*
	*/
	class VBAPRenderer
	{
	public:

		/// Constructor
		VBAPRenderer();

		/// Destructor
		~VBAPRenderer();

		/**
		* Initializing VBAP renderer to iConfig renderer configuration
		*
		* @param[in] iConfig renderer configuration for initializing VBAP renderer.
		*
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError InitWithConfig(RenderUtils::IRendererConfiguration *iConfig);

		/**
		* Get renderer configuration
		*
		* @param[out] oConfig output renderer configuration.
		*
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError GetRendererConfiguration(RenderUtils::IRendererConfiguration* &oConfig) const;

		/**
		* Render an extended source.
		*
		* Upon successful rendering (ie. no error), the rendered speaker gains and channel gains are
		* stored in (*iSource).renderedSpeakerGains_ and (*iSource).renderedChannelGains_ respectively.
		* Caller can retrieve them by directly accessing dot members.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iSource vbapRendererExtendedSource to be rendered
		*
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError RenderExtendedSource(vbapRendererExtendedSource* iSource);

		/**
		* Rendering an object.
		*
		* Upon successful rendering (ie. no error), the rendered channel gains are stored in
		* (*iObject).channelGains_. Caller can retrieve them by directly accessing the dot member.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iObject vbapRendererObject to be rendered
		*
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError RenderObject(vbapRendererObject* iObject);

		/**
		* Render an LFE source.
		* Note, although this API makes sense for generalized format (eg. MDA), it may not get used for IAB.
		* In IAB, no "LFE object" will be created. Rather, it's LFE channel in BedDefinition element.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iLFEChannel vbapRendererLFEChannel to be rendered
		*
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError RenderLFEChannel(vbapRendererLFEChannel* iLFEChannel);

		/**
		* Removes any "un-touched" extended source from from "previously rendered extended source" history list.
		*
		* The purpose of this function is to only permit the re-use of rendered gains from the point
		* of history clear/reset.
		*
		* @memberof VBAPRenderer
		*/
		void CleanupPreviouslyRendered();

		/**
		* Reset VBAP renderer cached extended source rendering history. ie.
		*     previouslyRenderedExtendedSources_
		*
		* Note: It does change state of render config etc.
		*/
		void ResetPreviouslyRendered();

		/**
		* Get size of VBAP cache of extended sources.
		*
		* Note: It does change state of render config etc.
		*/
		uint32_t GetVBAPCacheSize();

	private:

		// *****************************************************************************
		// Class private methods
		//

		/**
		* Mixdown from "speaker gains" to "channel gains". The "channel gains" is a sub-set of
		* "speaker gains" set. Both sets are defined in/from an valid rendererConfiguration_.
		* The "speaker gains" set include physical speakers present in the config file and also
		* virtual speakers not physically present, but necessary for VBAP rendering purposes.
		* The "channel gains" set include physical speakers only that are present and defined
		* in rendererConfiguration_. As output play-back is possible only on physical speakers,
		* outputs for virtual speakers must be "downmixed" to physical speakers, with pre-defined
		* downmix coefficients (also defined in rendererConfiguration_). This function performs
		* this downmix processing.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iSpeakerGains Speaker gains.
		* @param[out] oChannelGains Channel gains.
		* @return \link kIABNoError \endlink if no errors. Otherwise returns kIABRenderInvalidRadius error.
		*/
		vbapError SpeakerDownmix(const std::vector<float>& iSpeakerGains, std::vector<float>& oChannelGains);

		/**
		* Rendering an object that on the dome surface.
		*
		* (*iObject) contains 1 vbapRendererExtendedSource only.
		*
		* Upon successful rendering (ie. no error), the rendered channel gains are stored in
		* (*iObject).channelGains_. Caller can retrieve them by directly accessing the dot member.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iObject vbapRendererObject to be rendered
		* @return \link kIABNoError \endlink if no errors. Otherwise returns kIABRenderInvalidRadius error.
		*/
		vbapError RenderOnDomeObject(vbapRendererObject* iObject);

		/**
		* Rendering an interior object. An interior object is one inside the dome.
		*
		* (*iObject) contains multiple (>1, likely = 3) vbapRendererExtendedSources.
		*
		* Upon successful rendering (ie. no error), the rendered channel gains are stored in
		* (*iObject).channelGains_. Caller can retrieve them by directly accessing the dot member.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iObject vbapRendererObject to be rendered
		* @return \link kIABNoError \endlink if no errors. Otherwise returns kIABRenderInvalidRadius error.
		*/
		vbapError RenderInteriorObject(vbapRendererObject* iObject);

		/**
		* Normalize channel gains.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iNormGain total gain for normalization, an input parameter for channel gains normaliztion.
		* @param[in] ioChannelGains channel gains to be normalized.
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError NormalizeChannelGains(float iNormGain, std::vector<float> &ioChannelGains);

		/**
		* Apply object gain on top of rendered & normalized channel gains.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iObjectGain object gain, from clients/bitstream parsing, etc.
		* @param[in] ioChannelGains channel gains to be aggregated with iObjectGain.
		* @return \link vbapError \endlink kVBAPNoError if no errors. Other values indicate an error.
		*/
		vbapError ApplyObjectGainToChannelGains(float iObjectGain, std::vector<float> &ioChannelGains);

		/**
		* Adds pSource to VBAP renderer's cache memory for previously rendered extended sources.
		*
		* This function should be called after the extent source has been processed by the VBAP renderer.
		*
		* The VBAP-generated gain coefficients are stored such that they can be re-used later for extent source with
		* identical parameters. It is by calling ReusePreviouslyRendered().
		*
		* NOTE: This function internally creates its own copy from *pSource. Client remains the owner of original *pSource.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iSource
		*/
		void AddToPreviouslyRendered(vbapRendererExtendedSource* iSource);

		/**
		* Searches whether an extent source of identical position and extent is stored in the renderer's
		* cache memory of previously rendered extent source.
		*
		* If a match is found, the previously rendered per-speaker VBAP gain coefficients are reused.
		*
		* @memberof VBAPRenderer
		*
		* @param[in] iSource vbapRendererExtendedSource to be compared to previously/already rendered sources.
		* @return true if a match is found, false if no match is found.
		*/
		bool ReusePreviouslyRendered(vbapRendererExtendedSource* iSource);

		// *****************************************************************************
		// Internal - Core VBAP Algorithm processing functions
		//
		bool ConfigureVBAP(
			RenderUtils::IRendererConfiguration* iConfig
			, int32_t iThetaDivs
			, int32_t iPhiDivs
			);

		void BuildHemisphere(
			RenderUtils::HemisphereVirtualSources *oHemisphere
			, std::vector<float> &oTotalSpeakerGains
			, int32_t iThetaDivs
			, int32_t iPhiDivs
			);

		vbapError RenderExtent(
			const CoreUtils::Vector3& iSource
			, float iAperture
			, float iDivergence
			, std::vector<float> &oSpeakerGains
			);

		vbapError RenderHemisphere(
			float iTheta
			, float iPhi
			, float iAperture
			, float iDivergence
			, int32_t &oFoundSources
			, std::vector<float> &oSpeakerGains
			, const RenderUtils::HemisphereVirtualSources& iVirtualSources
			) const;

		// computeLatitudeSigma calculates angle sigma describing the portion of a latitude ring of virtual sources
		// that is within an object's aperture disk. This is used to select the virtual sources that contribute
		// to an object's extent.
		static void ComputeLatitudeSigma(
			float iPhi
			, float iAperture
			, float iLatitude
			, float &oSigma
			);

		/**
		* Renders a source against the VBAP hull.

		* @return The VBAP weight used to compute the gains. A number smaller than or equal
		* to zero indicates that the source is outside the convex hull formed by the loudspeakers.
		*/
		float RenderPatch(
			const CoreUtils::Vector3& iSource
			, std::vector<float> &oSpeakerGains
			);

		// *****************************************************************************
		// Class members

		/// VBAP internal variable for aggregating speaker gains from multiple active patches.
		std::vector<float>                      totalSpeakerGains_;

		/// Pointer to an array of Top (Hemisphere) virtual sources
		RenderUtils::HemisphereVirtualSources   *topVirtualSources_;

		/// Pointer to a renderer configuration
		RenderUtils::IRendererConfiguration     *rendererConfiguration_;

		// ================================================================
		// members related to renderer speakers, including virtual speakers
		//

		/// Pointer to as array of RenderSpeaker(s) that are set up according to rendererConfiguration_, including virtual speakers
		const std::vector<RenderUtils::RenderSpeaker> *renderSpeakers_;

		/// Store index for LFE speaker after VBAP config with rendererConfiguration_
		int32_t speakerLFEIndex_;

		// ================================================================
		// members related to cache'ing VBAP extended source rendering results
		// ie. speaker and channel gains.
		/// VBAP cache, store previously rendered extent sources from the point of last clearance (reset)
		std::vector<vbapRendererExtendedSource> previouslyRenderedExtendedSources_;
	};

} // namespace IABVBAP

#endif // __VBAPRENDERER_H__
