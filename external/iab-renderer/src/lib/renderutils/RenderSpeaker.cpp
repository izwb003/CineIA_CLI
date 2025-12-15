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

#include "renderutils/RenderSpeaker.h"

#include <cmath>
#include <assert.h>

#include "renderutils/Utils.h"
#include "coreutils/CoreDefines.h"

namespace RenderUtils
{

    DownmixValue::DownmixValue(int32_t chan, float coef) :
        ch_(chan),
        coefficient_(coef)
    {

    }

    bool RenderSpeaker::hasDownmix() const
    {

        return (((ch_ == -1) && downmix_.size()>0) || ((ch_ >= 0) && (downmix_.size() == 1)));
    }

    RenderSpeaker::RenderSpeaker(const std::string pName, int32_t pChan, int32_t pOutputIndex, float pAzimuth, float pElevation, const std::string pUri) :
        name_(pName),
        ch_(pChan),
        outputIndex_(pOutputIndex),
        uri_(pUri),
        fAzimuth_(pAzimuth),
        fElevation_(pElevation)
    {
        if (pChan > -1)
        {
            downmix_.push_back(DownmixValue(ch_, 1.0));
            normalizedDownmix_.push_back(downmix_.back());
        }

        /*
         Developer notes:
         float theta = mdaThetaQToF(mdaThetaFToQ(pAzimuth * mdaKPI / 180.0f));
         float phi = mdaPhiQToF(mdaPhiFToQ(pElevation * mdaKPI / 180.0f));

         The above were the original code from MDACore. After discussions with Bob Brown, it was agreed to remove these MDA
         specific "round trip" quantisation steps which are specific to MDA

         */

        float theta = pAzimuth * CoreUtils::kPI / 180.0f;
        float phi = pElevation * CoreUtils::kPI / 180.0f;

        float x = std::sin(theta) * std::cos(phi);
        float y = std::cos(theta) * std::cos(phi);
        float z = std::sin(phi);

        pos_ = CoreUtils::Vector3(
                   clamp(x, -1.0f, 1.0f),
                   clamp(y, -1.0f, 1.0f),
                   clamp(z, -1.0f, 1.0f)
               );
    }

    void RenderSpeaker::addDownMixValue(const DownmixValue& iDownmix)
    {
        downmix_.push_back(iDownmix);
    }

    const std::vector<DownmixValue>& RenderSpeaker::getMixmaps() const
    {
        return downmix_;
    }

    float RenderSpeaker::getAzimuth() const
    {
        return fAzimuth_;
    }

    float RenderSpeaker::getElevation() const
    {
        return fElevation_;
    }

    void RenderSpeaker::normalizeDownMixValues()
    {
        assert(normalizedDownmix_.empty());

        std::vector<DownmixValue>::const_iterator iter;

        // Compute L^2 Norm
        float norm = 0.;
        for (iter = downmix_.begin(); iter != downmix_.end(); ++iter)
        {
            norm += iter->coefficient_ * iter->coefficient_;
        }
        norm = std::sqrt(norm);

        for (iter = downmix_.begin(); iter != downmix_.end(); ++iter)
        {
            normalizedDownmix_.push_back(*iter);
            normalizedDownmix_.back().coefficient_ /= norm;
        }
    }

} // RenderUtils
