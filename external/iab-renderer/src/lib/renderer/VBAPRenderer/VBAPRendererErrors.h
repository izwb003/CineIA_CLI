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
 * Header file for VBAP renderer errors.
 *
 * @file
 */

#ifndef __VBAPRENDERERERRORS_H__
#define	__VBAPRENDERERERRORS_H__

#include <wchar.h>
#include <math.h>

namespace IABVBAP
{
	/**
	*
	* Error Codes for IAB VBAP renderer.
	*
	* Error codes returned by IAB VBAP render module methods & functions.
	*
	*/
	enum VBAPErrorCodes {

		kVBAPNoError                               = 0,              /**< No error. */
		
        kVBAPObjectPositionNotInConvexHullError    = 1,              /**< Error: Object position is not covered by the VBAP hull. */
        kVBAPRendererConfigVersionUnspecifiedError = 2,              /**< Error: Unversioned cfg file not supported. */
        kVBAPObjectGainValueOutOfBoundsError       = 3,              /**< Error: Rendered object has out of bounds gain value. */
        kVBAPObjectRadiusValueOutOfBoundsError     = 4,              /**< Error: Rendered object has out of bounds radius value. */
        kVBAPObjectThetaValueOutOfBoundsError      = 5,              /**< Error: Rendered object has out of bounds azimuth value. */
        kVBAPObjectPhiValueOutOfBoundsError        = 6,              /**< Error: Rendered object has out of bounds elevation value. */
        kVBAPObjectDivergenceValueOutOfBoundsError = 7,              /**< Error: Rendered object has out of bounds divergence value. */
        kVBAPObjectApertureValueOutOfBoundsError   = 8,              /**< Error: Rendered object has out of bounds aperture value. */
        kVBAPNestedGroupsError                     = 9,              /**< Error: Renderer has encountered nested Groups. */
        kVBAPEndGroupError                         = 10,             /**< Error: Renderer has encountered Group end without matching begin. */
        kVBAPMathError                             = 11,             /**< Error: Mathematical error encountered. */
        kVBAPRenderExtendedSourceNoObjectError     = 12,             /**< Error: Renderer has encountered an extended source outside Object. */
        kVBAPBeginObjectFragmentNestedObjectError  = 13,             /**< Error: Renderer has encountered nested Objects. */
        kVBAPObjectRadiusValueNotEqualToOneError   = 14,             /**< Error: Renderer has encountered object with a radius value other than 1. */

        kVBAPParameterOutOfBoundsError             = 30,             /**< Error: Parameter out of bound. */
        kVBAPMemoryError                           = 31,             /**< Error: Memory error. */
        kVBAPBadArgumentsError                     = 32,             /**< Error: Bad argument error. */
        kVBAPNoLFEChannelError                     = 33,             /**< Error: LEF missing error. */
        kVBAPUnConfiguredError                     = 34,             /**< Error: Not configured error. */
        kVBAPAlreadyInitError                      = 35,             /**< Error: Duplication in initialization (must be inited only once). */
        kVBAPConfigurationFormatError              = 36,             /**< Error: Format error. */
        kVBAPNoExtendedSourceError                 = 37,             /**< Error: Extended source missing error. */
        kVBAPDivisionByZeroError                   = 38,             /**< Error: Division by 0 error. */

	};

}  // namespace IABVBAP

#endif // __VBAPRENDERERERRORS_H__
