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

#ifndef __VBAPRENDERERDATASTRUCTURES_H__
#define __VBAPRENDERERDATASTRUCTURES_H__

#include <stdint.h>
#include <vector>

#include "coreutils/Vector3.h"
#include "coreutils/CoreDefines.h"
#include "renderer/VBAPRenderer/VBAPRendererErrors.h"

namespace IABVBAP
{
	/*****************************************************************************
	*
	* Types
	*
	*****************************************************************************/

	/**
	* Data type for error codes returned by API calls. It is \c int32_t.
	*/
	typedef int32_t vbapError;

	/**
	* Data type for VBAP object position (Cartesian coordinates).
	* x    Range [-1, 1]
	* y    Range [-1, 1]
	* z    Range [0, 1]
	*/
	typedef CoreUtils::Vector3 vbapPosition;

	/**
	* Represents an extended source for VBAP rendering
	*
	*/
	struct vbapRendererExtendedSource
	{
		vbapRendererExtendedSource(uint32_t iSpeakerCount, uint32_t iChannelCount) :
			position_(0, 1, 0),
			extSourceGain_(1.0),
			aperture_(0),
			divergence_(0),
			renderedSpeakerGains_(iSpeakerCount, 0.0f),
			renderedChannelGains_(iChannelCount, 0.0f),
			touched_(false)
		{
		}

		virtual ~vbapRendererExtendedSource()
		{
		}

		/**
		* Returns true if iSource has the same rendering input values as "this".
		* Rendering parameters inlcude those below that are used by VBAP renderer.
		*   position_ (x, y, z)
		*   aperture_
		*   divergence_
		*   extSourceGain_
		*
		* Note that Rendered output variables are NOT considered, eg. renderedSpeakerGains_
		* and renderedChannelGains_
		*
		* @memberof vbapRendererExtendedSource
		*
		* @param[in] iSource Pointer to extended source for comparison (right hand side)
		*/
		bool HasSameRenderingParams(vbapRendererExtendedSource* iSource)
		{
			return	((position_.x == iSource->position_.x) &&
				(position_.y == iSource->position_.y) &&
				(position_.z == iSource->position_.z) &&
				(aperture_ == iSource->aperture_) &&
				(divergence_ == iSource->divergence_) &&
				(extSourceGain_ == iSource->extSourceGain_));
		}

		// The set functions offer range checking.
		// The variables can otherwise be directly accessed, as this is a structure.
		// Getter functions are not really needed.

		float GetGain() const
		{
			return extSourceGain_;
		}

		vbapError SetGain(float iGain)
		{
			if ((iGain > 1.0f) || (iGain < 0.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			extSourceGain_ = iGain;
			return kVBAPNoError;
		}

		vbapPosition GetPosition() const
		{
			vbapPosition position;
			position.x = position_.x;
			position.y = position_.y;
			position.z = position_.z;
			return position;
		}

		vbapError SetPosition(const vbapPosition &iPosition)
		{
			if ((iPosition.x < -1.0f) || (iPosition.x > 1.0f) ||
				(iPosition.y < -1.0f) || (iPosition.y > 1.0f) ||
				(iPosition.z < 0.0f) || (iPosition.z > 1.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			position_.x = iPosition.x;
			position_.y = iPosition.y;
			position_.z = iPosition.z;

			return kVBAPNoError;
		}

		float GetAperture() const
		{
			return aperture_;
		}

		vbapError SetAperture(float iAperture)
		{
			if ((iAperture > CoreUtils::kPI) || (iAperture < 0.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			aperture_ = iAperture;
			return kVBAPNoError;
		}

		float GetDivergence() const
		{
			return divergence_;
		}

		vbapError SetDivergence(float iDivergence)
		{
			if ((iDivergence > CoreUtils::kPI / 2) || (iDivergence < 0.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			divergence_ = iDivergence;
			return kVBAPNoError;
		}

		// Member variables
		//
		CoreUtils::Vector3 position_;
		float extSourceGain_;
		float aperture_;
		float divergence_;

		// To save rendered speaker gains
		// Speaker gains include virtual speakers. In IAB-revised VBAP design,
		// rendered speaker gains are only retained in vbapRendererExtendedSource
		// as internal. VBAPRenderer clients do not need to know speaker gains.
		//
		std::vector<float> renderedSpeakerGains_;

		// To save rendered channel gains
		// Channel gains exactly correspond to each and all output channel speakers
		// that are physically present in target configuration.
		// Channel gains are the only gains that clients need from vbapRendererExtendedSource
		// in order to apply them to PCM assets to generate rendered outputs.
		//
		std::vector<float> renderedChannelGains_;

		// This flag is used in the memory for parameter re-use between two successive rendering time segment calls.
		// It is set to true for memorized objects that have been newly added or have been re-used in the current time segment.
		bool touched_;
	};

	/**
	* Represents an object for VBAP rendering. Can contain 1 single extended source (on-dome-surface)
	* or multiple extended sources that in combination emulate an interior object.
	*
	*/
	struct vbapRendererObject
	{
		vbapRendererObject(uint32_t iChannelCount) :
			objectGain_(1.0),
			id_(0),
			vbapNormGains_(0.0),
			channelGains_(iChannelCount, 0.0f),
			extendedSources_()
		{
		}

		~vbapRendererObject()
		{
		}

		uint32_t GetId() const
		{
			return id_;
		}

		vbapError SetId(uint32_t iID)
		{
			id_ = iID;
			return kVBAPNoError;
		}

		float GetGain() const
		{
			return objectGain_;
		}

		vbapError SetGain(float iGain)
		{
			if ((iGain > 1.0f) || (iGain < 0.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			objectGain_ = iGain;
			return kVBAPNoError;
		}

		/**
		* Reset everything except number of channels (channel gains)
		* The values of channel gains are set to 0.0.
		*
		*/
		vbapError ResetState()
		{
			objectGain_ = 1.0f;
			id_ = 0;
			vbapNormGains_ = 0.0f;
			extendedSources_.clear();

			std::vector<float>::iterator iter;

			for (iter = channelGains_.begin(); iter != channelGains_.end(); iter++)
			{
				// reset channel gains
				*iter = 0.0f;
			}

			return kVBAPNoError;
		}

		float objectGain_;
		uint32_t id_;

		float vbapNormGains_;
		std::vector<float> channelGains_;
		std::vector<vbapRendererExtendedSource> extendedSources_;
	};

	/**
	* Represents an LFE entity for VBAP rendering
	*
	*/
	struct vbapRendererLFEChannel
	{
		vbapRendererLFEChannel(uint32_t iSpeakerCount, uint32_t pChannelCount) :
			lfeGain_(1.0),
			id_(0),
			speakerGains_(iSpeakerCount, 0.0f),
			channelGains_(pChannelCount, 0.0f)
		{
		}

		~vbapRendererLFEChannel()
		{
		}

		float GetGain() const
		{
			return lfeGain_;
		}

		vbapError SetGain(float iGain)
		{
			if ((iGain > 1.0f) || (iGain < 0.0f))
			{
				return kVBAPParameterOutOfBoundsError;
			}

			lfeGain_ = iGain;
			return kVBAPNoError;
		}

		uint32_t GetId() const
		{
			return id_;
		}

		vbapError SetId(uint32_t iID)
		{
			id_ = iID;
			return kVBAPNoError;
		}

		float lfeGain_;
		uint32_t id_;

		std::vector<float> speakerGains_;
		std::vector<float> channelGains_;
	};

} // namespace IABVBAP

#endif // __VBAPRENDERERDATASTRUCTURES_H__
