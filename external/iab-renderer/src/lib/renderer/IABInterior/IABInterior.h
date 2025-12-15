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
 * IABInterior.h
 *
 * @file
 */

#ifndef __IABINTERIOR_H__
#define __IABINTERIOR_H__

#include "renderer/VBAPRenderer/VBAPRendererDataStructures.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    class IABInterior
    {
    public:
        
        // Constructor
        IABInterior();
        
        // Destructor
        ~IABInterior();

        /**
         * @enum InteriorNormObjects
         *
         * @brief interior normalised objects.
         *
         * @note interior object is converted into number of norm objects.
         *        Left, right and projected interior object onto the dome surface.
         *        Here the list of norm objects defined.
         *        Currently this enums are used only in IABInteriorTests (Unit tests).
         *
         */
        enum InteriorNormObjects
        {
            eLeftNormObject = 0,
            eRightNormObject,
            eProjectedNormObject,
            eMaxNumNormObjects
        };
        
        // Number of entries in linear gain table - based on PT x resolution of +100 to -100
        // Set to odd number to ensure single center value
#define IAB_GAIN_TABLE_SIZE              201
        
        /**
         * This function returns the vbap extended sources for given the extended source's coordinates.
         * Step 1: converts the extend source into 1 or 3 (depends upon the radius) MDA normalised objects.
         * Step 2: creates the vbap extended sources for converted normalised object co-ordinates and gain.
         * Step 3: fills the vbap extended source data structure with aperture, divergence and gain.
         *
         * This method may return 1 or 3 vbap extended sources depends upon the extended source positions.
         * If extended source position falls over the VBAP dome or exterior to dome, returns 1 vbap extended source, otherwise returns 3 objects.
         *
         * IMPORTANT NOTE : The returned vbap extended source's object(MEMORY) ownership is transferred to client.
         * then it is client's responsibility to free up the vbap extended sources, once the rendering is done.
         *
         * NOTE : floating point input arguments are passed by const reference for optimisation purpose
         *        and to avoid unneccessary floating point precision error while copying.
         *
         * @param[in] iAzimuth - extend source's azimuth
         * @param[in] iElevation - extend source's elevation
         * @param[in] iRadius - extend source's radius. (Range: radius > 0.0, including interior, on surface, and exterior)
         * @param[in] iAperture - extend source's aperture
         * @param[in] iDivergence - extend source's divergence
         * @param[out] oVBAPExtendedSources - vector of  pointers to vbap extend source
         * @return \link kIABNoError \endlink if no errors. Otherwise returns error.
         */
        iabError MapExtendedSourceToVBAPExtendedSources(const float& iAzimuth,
                                                        const float& iElevation,
                                                        const float& iRadius,
                                                        const float& iAperture,
                                                        const float& iDivergence,
                                                        std::vector<IABVBAP::vbapRendererExtendedSource>& oVBAPExtendedSources);
    private:
        
        // Table to store preset gains.
        float *gainTable_;
        
        /**
         *
         * Function to intialise the gain table.
         *
         */
        void initGainTable();
        
        /**
         * This function creates the vbap extended sources and returns it.
         *
         * @param[in] iElevation - input VBAP object's elevation
         * @param[in] iAzimuth - input VBAP object's azimuth
         * @param[in] iGain - input VBAP object's gain
         * @param[in] iAperture - input VBAP object's aperture
         * @param[in] iDivergence - input VBAP object's divergence
		 * @param[out] oExtendedSource - output VBAP extended source onject
		 * @return \link kIABNoError \endlink if no errors. Otherwise returns kIABRenderInvalidRadius error.
		 *
         */
		iabError CreateExtendSource(const float& iElevation,
                                    const float& iAzimuth,
                                    const float& iGain,
                                    const float& iAperture,
                                    const float& iDivergence,
									IABVBAP::vbapRendererExtendedSource& oExtendedSource);

    };

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABINTERIOR_H__
