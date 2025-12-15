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

#include "renderutils/VirtualSources.h"
#include <assert.h>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

namespace RenderUtils
{

    VirtualSourceTree::VirtualSourceTree(std::vector<VirtualSource>::size_type pSpeakerGainCount):
        fLeftVirtualSources(NULL),
        fRightVirtualSources(NULL)
    {
        fRoot.fSpeakerGains.resize(pSpeakerGainCount);
    }


    VirtualSourceTree::~VirtualSourceTree()
    {

        if (fLeftVirtualSources)
        {
            delete fLeftVirtualSources;
            fLeftVirtualSources = NULL;
        }

        /* search the right tree if the search interval intersects with the right tree interval  */

        if (fRightVirtualSources)
        {
            delete fRightVirtualSources;
            fRightVirtualSources = NULL;
        }
    }



    void VirtualSourceTree::build(const std::vector<VirtualSource>::const_iterator &fStart,
                                  const std::vector<VirtualSource>::const_iterator &fEnd)
    {
        std::vector<float> temp((*fStart).fSpeakerGains.size(), 0.0f);

        build(fStart, fEnd, temp);
    }


    void VirtualSourceTree::build(const std::vector<VirtualSource>::const_iterator &fStart,
                                  const std::vector<VirtualSource>::const_iterator &fEnd,
                                  std::vector<float> &pSpeakerGains)
    {

        assert(!(pSpeakerGains.size() == (*fStart).fSpeakerGains.size() == fRoot.fSpeakerGains.size()));

        std::vector<VirtualSource>::difference_type d = std::distance(fStart, fEnd);

        if (d == 1)
        {
            fRoot.fTheta = (*fStart).fTheta;
            fRoot.fThetaIndex = (*fStart).fThetaIndex;
            fRoot.fSpeakerGains = (*fStart).fSpeakerGains;
            fRoot.fCount = 1;

            std::transform(fRoot.fSpeakerGains.begin(), fRoot.fSpeakerGains.end(), pSpeakerGains.begin(),
                           pSpeakerGains.begin(), std::plus<float>());

        }
        else
        {

            std::vector<VirtualSource>::const_iterator mid = fStart + (std::vector<VirtualSource>::size_type) std::ceil((float) d / 2.0f);

            fRoot.fTheta = (*(mid - 1)).fTheta;
            fRoot.fThetaIndex = (*(mid - 1)).fThetaIndex;

            std::vector<float> tmp(pSpeakerGains.size(), 0.0f);

            fLeftVirtualSources = new VirtualSourceTree((*fStart).fSpeakerGains.size());

            fLeftVirtualSources->build(fStart, mid, tmp);

            std::transform(tmp.begin(), tmp.end(), pSpeakerGains.begin(), pSpeakerGains.begin(), std::plus<float>());

            std::fill(tmp.begin(), tmp.end(), 0.0f);

            fRightVirtualSources = new VirtualSourceTree((*fStart).fSpeakerGains.size());

            fRightVirtualSources->build(mid, fEnd, tmp);

            std::transform(tmp.begin(), tmp.end(), pSpeakerGains.begin(), pSpeakerGains.begin(), std::plus<float>());

            fRoot.fSpeakerGains = pSpeakerGains;

            fRoot.fCount = fRightVirtualSources->fRoot.fCount + fLeftVirtualSources->fRoot.fCount;
        }


    }


    int VirtualSourceTree::averageGainsOverRange(int pQueryLow, int pQueryHigh, int pMin, int pMax, std::vector<float> &pSpeakerGains) const
    {

        /* if node, is the tree interval is within the search interval */
        /* if leaf, is the leaf within the search interval */

        if (( pQueryLow <= pMin && pQueryHigh >= pMax ) ||
                (!fLeftVirtualSources && pQueryLow <= fRoot.fThetaIndex && pQueryHigh >= fRoot.fThetaIndex) )
        {
            std::transform(fRoot.fSpeakerGains.begin(), fRoot.fSpeakerGains.end(), pSpeakerGains.begin(), pSpeakerGains.begin(), std::plus<float>());
            return fRoot.fCount;
        }

        int rslt = 0;

        /* search the left tree if the search interval intersects with the left tree interval */

        if (pQueryLow <= fRoot.fThetaIndex && fLeftVirtualSources)
        {
            rslt += fLeftVirtualSources->averageGainsOverRange(pQueryLow, pQueryHigh, pMin, fRoot.fThetaIndex, pSpeakerGains) ;
        }

        /* search the right tree if the search interval intersects with the right tree interval */

        if (pQueryHigh > fRoot.fThetaIndex && fRightVirtualSources)
        {
            rslt += fRightVirtualSources->averageGainsOverRange(pQueryLow, pQueryHigh, fRoot.fThetaIndex, pMax, pSpeakerGains);
        }

        return rslt;
    }


    std::ostream& operator<<(std::ostream& pStream, const HemisphereVirtualSources& pSources)
    {
        for(std::vector<LongitudeVirtualSources>::const_iterator i = pSources.fLongitudes.begin(); i != pSources.fLongitudes.end(); i++)
        {
            pStream << "Phi: " << (*i).fPhi << "| Index:" << (*i).fPhiIndex << "| Theta Max:" << (*i).fMaxThetaIndex << std::endl;

            pStream << *((*i).fVirtualSources);

            pStream << std::endl;
        }

        return pStream;
    }


    std::ostream& operator<<(std::ostream& os, const VirtualSourceTree& q)
    {
        q.print(os);
        return os;
    }


    void VirtualSourceTree::print(std::ostream& os, int depth) const
    {
        std::string tabs(depth*4, ' ');

        if (!fLeftVirtualSources && !fRightVirtualSources)
        {
            os << tabs << "[" << std::setw(3) << fRoot.fTheta << "/" << fRoot.fThetaIndex << "]";
        }
        else
        {
            os << tabs << "<" << std::setw(3) << fRoot.fTheta << "/" << fRoot.fThetaIndex << ">";
        }

        os << " = ";

        for(std::vector<float>::const_iterator i = fRoot.fSpeakerGains.begin(); i != fRoot.fSpeakerGains.end(); i++)
        {
            os << std::setw(3) << (*i) << " ";
        }

        os << std::endl;

        if (fLeftVirtualSources)
        {
            fLeftVirtualSources->print(os, depth + 1);
        }


        if (fRightVirtualSources)
        {
            fRightVirtualSources->print(os, depth + 1);
        }

    }


    std::vector<LongitudeVirtualSources>::const_iterator HemisphereVirtualSources::end() const
    {
        return fLongitudes.end();
    }

} // RenderUtils
