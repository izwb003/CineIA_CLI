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

#ifndef __RENDERSPEAKER_H__
#define __RENDERSPEAKER_H__

#include <string.h>
#include <vector>
#include <stdint.h>

#include "coreutils/Vector3.h"

namespace RenderUtils
{

    /**
     * Represents a downmix data structure
     *
     */

    struct DownmixValue
    {
        /** Speaker index of target channel */
        int32_t		ch_;

        /** Downmix coeffcient to scale downmix source channel to target channel */
        float       coefficient_;

        /**
         * Creates a downmix data structure
         *
         * @param[in] chan target channel for downmix
         * @param[in] coef downmix coefficient
         *
         */
        DownmixValue(int32_t chan, float coef);
    };

    /**
     * Represents a render speaker data structure class
     *
     */
    class RenderSpeaker
    {

    public:

        // Constructor
        RenderSpeaker(const std::string pName, int32_t pChan, int32_t pOutputIndex, float pAzimuth, float pElevation, const std::string pUri);

        virtual ~RenderSpeaker()
        {
        }

        /**
         * Indicates if speaker has downmix targets defined
         *
         * @return true if speaker has downmix. False means speaker has no downmix
         */
        bool hasDownmix() const;

        /**
         * Gets a reference to the normalized downmix values for this speaker
         *
         * @return constant reference to the normalizedDownmix vector
         */
        const std::vector<DownmixValue>& getNormalizedDownmixValues() const
        {
            return normalizedDownmix_;
        }

        /**
         * Gets the channel index associated with the speaker. This is "chan" in the configuration file.
         *
         * @returns the speaker's channel index.
         */
        int32_t getChannel() const
        {
            return ch_;
        }

        /**
         * Gets the renderer output index associated with the speaker.
         * This index indicates the order as the speaker appears in the configuration file.
         * The index takes into account of physical speakers only and should be aligned with the order in which
         * the renderer aranged its calculated VBAP gains. The renderer output channel order should also match the
         * VBAP gain order. The cleint may therefore use this API to get the corresponding renderer output index
         * for the speaker.
         *
         * @returns the speaker's renderer output index.
         */
        int32_t getRendererOutputIndex() const
        {
            return outputIndex_;
        }

        /**
         * Gets name associated with the speaker
         *
         * @returns constant reference to speaker's name string
         */
        const std::string& getName() const
        {
            return name_;
        }

        /**
         * Gets channel rendering exception URI associated with the speaker
         *
         * @returns constant reference to speaker's URI string
         */
        const std::string& getURI() const
        {
            return uri_;
        }

        /**
         * Gets position coordinate of the speaker
         *
         * @returns constant reference to speaker's position vector
         */
        const CoreUtils::Vector3& getPosition() const
        {
            return pos_;
        }

        /**
         * Adds a downmix value to the speaker
         *
         * @param[in] iDownmix constant reference to downmix value to be added
         */
        void addDownMixValue(const DownmixValue& iDownmix);

        /**
         * Normalize all of the downmix values
         *
         * Call this when all downmix values have been added
         */
        void normalizeDownMixValues();

        /**
         * Gets downmix map for the speaker
         *
         * @return constant reference to downmix vector
         */
        const std::vector<DownmixValue>& getMixmaps() const;

        /**
         * Gets the speaker's azimuth angle, in degrees
         *
         * @return azimuth angle
         */
        float getAzimuth() const;

        /**
         * Gets the speaker's elevation angle, in degrees
         *
         * @return elevation angle
         */
        float getElevation() const;

        /** Name of the speaker */
        std::string						name_;

        /** Index associated with the speaker */
        int32_t							ch_;

        /** Renderer output index associated with the speaker */
        int32_t							outputIndex_;


        /** Channel rendering exception (CRE) URI. Empty string indicates no CRE for this speaker */
        std::string                     uri_;

        /** Speaker xyz coordinate position vector. x, y,z range [-1.0, 1.0] */
        CoreUtils::Vector3							pos_;

        /** Downmix target vector for the speaker. Empty vector indicates no downmix specified for this speaker */
        std::vector<DownmixValue>       downmix_;

        /** Downmix target vector for the speaker with normalized coefficients. */
        std::vector<DownmixValue>       normalizedDownmix_;

    private:

        float                           fAzimuth_;
        float                           fElevation_;

    };

} // RenderUtils

#endif  // __RENDERSPEAKER_H__
