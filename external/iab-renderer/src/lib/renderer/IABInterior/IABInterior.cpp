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
 * IABInterior.cpp
 *
 * @file
 */

#include <cstring>

#include "common/IABConstants.h"
#include "renderer/IABInterior/IABInterior.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
   
    // Noramlized room side angles
    const float Norm_side_front_left = -45.0f;
    const float Norm_side_front_right = 45.0f;
    const float Norm_side_rear_left = -135.0f;
    const float Norm_side_rear_right = 135.0f;
    
    // Radius where gain of third object goes to zero for inward panning (range 0 - 100, where 100 = surface of dome, and 0 = listener)
    // The value of c_inwardPanObjectZeroGain was derived from listening tests, to provide the best directionality when inward panning.
    const float c_inwardPanObjectZeroGain = 75.0f;
    
    // Constructor
    IABInterior::IABInterior():
    gainTable_(NULL)
    {
        // Initialise the gain table
        initGainTable();
    }
    
    // Destructor
    IABInterior::~IABInterior()
    {
        if (gainTable_)
        {
            delete[] gainTable_;
        }
        gainTable_ = NULL;
    }
    
    void IABInterior::initGainTable()
    {
        if (gainTable_ == NULL)
        {
            gainTable_ = new float[IAB_GAIN_TABLE_SIZE];
            // Init gain table for mapping 2d panner x coordinate to linear gain
            // ProTools uses a sin/cos profile
            for (int32_t i = 0; i < IAB_GAIN_TABLE_SIZE; i++)
            {
                gainTable_[i] = sin(((float)i / IAB_GAIN_TABLE_SIZE) * iabKPI / 2.0f);
            }
        }
    }
    
    // Maps extended source positions into VBAP extended source positions, creates the VBAP extended sources and returns them.
    // NOTE : input floating point arguments are passed by const reference so as to avoid floating point precision error while copying.
    // Compute the postion and gain for a 3d inward panned normalized group, returning the positions, gains and elevations
    // for each object
    //
    // iAzimuth - Azimuth of object for inward panning (radian)
    // iElevation - Elevation of object for inward panning  (radian)
    // iRadius - Radius of object for inward panning
    // iAperture - aperture of object for inward panning
    // iDivergence - divergence of object for inward panning
	// oVBAPExtendedSources - vector of pointers to vbap extend source
	// iabError - returns the error if any error occurs during the conversion.
	iabError IABInterior::MapExtendedSourceToVBAPExtendedSources(const float& iAzimuth,
		const float& iElevation,
		const float& iRadius,
		const float& iAperture,
		const float& iDivergence,
		std::vector<IABVBAP::vbapRendererExtendedSource>& oVBAPExtendedSources)
	{
        float           x1;
        float           xRight;
        float           y1;
        float           gainSum;
        float           height;
        float           rCircle;
        float           rObj;
        float           lAzimuth, rAzimuth, pAzimuth;
        float           lElevation, rElevation, pElevation;
        float           lGain, rGain, pGain;
        int32_t         gainTableIndex;
        
        // if gain table is not setup yet, return error.
        if (gainTable_ == NULL)
        {
            return kIABGeneralError;
        }

		// Object for store any created instances
		IABVBAP::vbapRendererExtendedSource vbapExtendedSourceObject(0, 0);

        oVBAPExtendedSources.clear();
        
        // If radius is close to surface of dome, just return single object
        if (iRadius >= 1.0 - kEPSILON)
        {
            lAzimuth = iAzimuth;
            lElevation = iElevation;
            lGain = 1.0;

			// Generate vbap extended object
			CreateExtendSource(lElevation, lAzimuth, lGain, iAperture, iDivergence, vbapExtendedSourceObject);

            oVBAPExtendedSources.push_back(vbapExtendedSourceObject);

        } else if( iRadius < 0.0)
        {
            return kIABRendererInvalidRadiusError;
            
        }else
        {
            // Compute radius of 2D circle at height of object
            height = iRadius * sin(iElevation);
            rCircle = sqrt(1 - height * height);
            
            // Project object onto 2D circle at height of object to compute x and y
            rObj = iRadius * cos(iElevation);
            
            // Determine y value from projected of circle, 1.0 front to -1.0 back
            y1 = rObj * cos(iAzimuth);
            
            // x value of object location
            x1 = rObj * sin(iAzimuth);
            
            // Range check object position
            if (x1 > rCircle)
                x1 = rCircle;
            else if (x1 < -rCircle)
                x1 = -rCircle;
            if (y1 > rCircle)
                y1 = rCircle;
            else if (y1 < -rCircle)
                y1 = -rCircle;
            
            // Calculate intersecion of horizontal line at y with circle, use right intersection
            xRight = sqrt((rCircle * rCircle) - (y1 * y1));
            
            // Right azimuth is angle of right intersection
            rAzimuth = atan2(xRight, y1);
            
            // Left azimuth is mirror of right azimuth
            lAzimuth = -rAzimuth;
            
            // Elevation
            lElevation = atan2(height, rCircle);
            if (lElevation > iabKPI / 2.0f)
                lElevation = iabKPI / 2.0f;
            rElevation = lElevation;
            
            // Right gain is distance from object x coordinate to right intersection, scaled by length of horizontal line
            if (xRight > kEPSILON)
                gainTableIndex = static_cast<int32_t>(((xRight + x1) / (2.0 * xRight)) * IAB_GAIN_TABLE_SIZE);
            else
                gainTableIndex = IAB_GAIN_TABLE_SIZE / 2;
            if (gainTableIndex < 0)
                gainTableIndex = 0;
            else if (gainTableIndex >= IAB_GAIN_TABLE_SIZE)
                gainTableIndex = IAB_GAIN_TABLE_SIZE - 1;
            
            rGain = gainTable_[gainTableIndex];
            lGain = gainTable_[IAB_GAIN_TABLE_SIZE - 1 - gainTableIndex];

            // Normalize gains to 1.0
            gainSum = lGain + rGain;
            rGain *= 1.0f / gainSum;
            lGain *= 1.0f / gainSum;

            // Add 3rd object for triple-balanced panning
            // Place the object at the original azimuth and elevation, with radius of 1.0
            pAzimuth = iAzimuth;
            pElevation = iElevation;
            
            // Linearly scale gain based on radius of object and constant indicating where gain goes to zero
            pGain     = ((100.0f * iRadius) - c_inwardPanObjectZeroGain) / (100.0f - c_inwardPanObjectZeroGain);
            if (pGain < 0.0)
                pGain = 0.0;
            
            // Normaized room side speakers
            float nsfl = Norm_side_front_left * iabKPI / 180.0f;
            float nsfr = Norm_side_front_right * iabKPI / 180.0f;
            float nsrl = Norm_side_rear_left * iabKPI / 180.0f;
            float nsrr = Norm_side_rear_right * iabKPI / 180.0f;
            
            // Restrict side objects to norm room side speaker locations
            if (rAzimuth < nsfr)
            {
                rGain *= rAzimuth / nsfr;
                lGain *= rAzimuth / nsfr;

                rAzimuth = nsfr;
                lAzimuth = nsfl;
            } else if (rAzimuth > nsrr)
            {
                rGain *= (iabKPI - rAzimuth) / (iabKPI - nsrr);
                lGain *= (iabKPI - rAzimuth) / (iabKPI - nsrr);

                rAzimuth = nsrr;
                lAzimuth = nsrl;
            }
            
            // Recompute normalized gain with third object
            gainSum = lGain + rGain + pGain;
            pGain *= 1.0f / gainSum;
            lGain *= 1.0f / gainSum;
            rGain *= 1.0f / gainSum;
            
            // check for negative gain, as negative is not allowed in extended source
            if (lGain < 0.0)
            {
                lGain = 0.0;
            }
            
            if (rGain < 0.0)
            {
                rGain = 0.0;
            }
            
            // Create extended source object and insert them in the vector at appropriate position.
            // The order of push_back (left, right, projected) into the vector is important,
            // because the unit tests will expect the position and gains of the nomralised vbap objects at fixed location in the vector(array)

			// Generate vbap extended objects and add to oVBAPExtendedSources
			CreateExtendSource(lElevation, lAzimuth, lGain, iAperture, iDivergence, vbapExtendedSourceObject);
			oVBAPExtendedSources.push_back(vbapExtendedSourceObject);

			CreateExtendSource(rElevation, rAzimuth, rGain, iAperture, iDivergence, vbapExtendedSourceObject);
			oVBAPExtendedSources.push_back(vbapExtendedSourceObject);

			CreateExtendSource(pElevation, pAzimuth, pGain, iAperture, iDivergence, vbapExtendedSourceObject);
			oVBAPExtendedSources.push_back(vbapExtendedSourceObject);
        }

        return kIABNoError;
    }
    
	iabError IABInterior::CreateExtendSource(const float& iElevation,
		const float& iAzimuth,
		const float& iGain,
		const float& iAperture,
		const float& iDivergence,
		IABVBAP::vbapRendererExtendedSource& oExtendedSource)
    {
        // Convert Polar back to Cartesian
        float oX = 1.0f * cos(iElevation) * sin(iAzimuth);
        float oY = 1.0f * cos(iElevation) * cos(iAzimuth);
        float oZ = 1.0f * sin(iElevation);
        IABVBAP::vbapPosition vbapPosition(oX, oY, oZ);
        
        // Create a VBAP extended source
        // NOTE: freeing up this extendedSource is the responsibiltiy of the client who is going to use it.
		// Note: Neither SpeakerCount nor ChannelCount is known to within IABInterior module. Init to 0 for both.
		//
        IABVBAP::vbapRendererExtendedSource extendedSource = IABVBAP::vbapRendererExtendedSource(0, 0);
        
        // Set parameters.
        IABVBAP::vbapError vbapReturnCode = extendedSource.SetGain(iGain);
        vbapReturnCode |= extendedSource.SetPosition(vbapPosition);
        vbapReturnCode |= extendedSource.SetAperture(iAperture);
        vbapReturnCode |= extendedSource.SetDivergence(iDivergence);

        if (vbapReturnCode != IABVBAP::kVBAPNoError)
        {
            return vbapReturnCode;
        }

		oExtendedSource = extendedSource;

		return kIABNoError;
	}

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
