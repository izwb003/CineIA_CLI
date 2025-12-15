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

#pragma once

#include <iostream>
#include <vector>

namespace RenderUtils
{

    struct VirtualSource
    {
        int						fThetaIndex;
        float					fTheta;
        std::vector<float>		fSpeakerGains;
        int						fCount;
    };

    class VirtualSourceTree
    {

        void build(const std::vector<VirtualSource>::const_iterator &fStart,
                   const std::vector<VirtualSource>::const_iterator &fEnd,
                   std::vector<float> &pGains);


    public:

        VirtualSourceTree			*fLeftVirtualSources;
        VirtualSourceTree			*fRightVirtualSources;
        VirtualSource				fRoot;

        VirtualSourceTree(std::vector<VirtualSource>::size_type pGainCount);
        VirtualSourceTree();
        ~VirtualSourceTree();




        void build(const std::vector<VirtualSource>::const_iterator &fStart,
                   const std::vector<VirtualSource>::const_iterator &fEnd);

        int averageGainsOverRange(int pLow, int pHigh, int pMin, int pMax, std::vector<float> &pGains) const;

        friend std::ostream& operator<<(std::ostream& pStream, const VirtualSourceTree& pTree);

        void print(std::ostream& pStream, int pDepth = 0) const;
    };

    struct LongitudeVirtualSources
    {
        LongitudeVirtualSources()
        {
            fVirtualSources = NULL;
            fPhi = 0.0f;
            fPhiIndex = 0;
            fDeltaTheta = 0.0f;
            fMaxThetaIndex = 0;
        }

        ~LongitudeVirtualSources()
        {
        }

        VirtualSourceTree		*fVirtualSources;
        float					fPhi;
        int						fPhiIndex;
        float					fDeltaTheta;
        int						fMaxThetaIndex;

    };

    class HemisphereVirtualSources
    {
    public:
        std::vector<LongitudeVirtualSources>	fLongitudes;
        float								fDeltaPhi;

        std::vector<LongitudeVirtualSources>::const_iterator end() const;

        friend std::ostream& operator<<(std::ostream& pStream, const HemisphereVirtualSources& pSources);

        ~HemisphereVirtualSources()
        {
            for(std::vector<LongitudeVirtualSources>::const_iterator i = fLongitudes.begin(); i != fLongitudes.end(); i++)
            {
                if (i->fVirtualSources)
                {
                    delete i->fVirtualSources;
                }
            }

            fLongitudes.clear();
        }

    };

} // RenderUtils
