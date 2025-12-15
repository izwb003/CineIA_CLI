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

#ifndef IABTRANSFORMDATATYPE_H_INCLUDED
#define IABTRANSFORMDATATYPE_H_INCLUDED

#include "IABDataTypes.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    
    // ---------------------------------------------------------------------------------------------
    // IAB object coordinates [x, y, z] in float
	// C~ and D~ (intermediate) are possible. May define separately for each system. (TO-DO)
	// Range for C~:
	//               x: [0, 1]
	//               y: [0, 1]
	//               z: [0, 1]
	// 
	// Range for D~:
	//               x: [-1, 1]
	//               y: [-1, 1]
	//               z: [0, 1]
	// 
	typedef float IABValueX;
	typedef float IABValueY;
	typedef float IABValueZ;

	// ---------------------------------------------------------------------------------------------
	// MDA VBAP object polar coordinates [azimuth, elevation, radius] in float
	// Range: (TO-DO)
	//
	typedef float VBAPValueAzimuth;
	typedef float VBAPValueElevation;
	typedef float VBAPValueRadius;
    
	// MDA VBAP can also be specified as Cartesian within a normalized room mode, and based on Pyra Mesa
	typedef float PyraMesaValueX;
	typedef float PyraMesaValueY;
	typedef float PyraMesaValueZ;

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
#endif
