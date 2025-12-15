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
 * RendererConfigErrors.h
 *
 * @file
 */

#ifndef __RENDERERCONFIGURATIONERRORS_H__
#define	__RENDERERCONFIGURATIONERRORS_H__


#include <wchar.h>
#include <math.h>

namespace RenderUtils
{

    /**
     * Data type for error codes returned by IRendererConfiguration API calls. It is \c int32_t.
     */

    typedef int32_t rendererConfigError;

    /**
    *
    * Error Codes for Render configuration library.
    *
    * Error codes returned by render configuration library methods & functions.
    *
    */
    enum RenderConfigErrorCodes
    {

        kNoRendererConfigurationError       = 0,             /**< No error. */

        kDeprecatedCfgFileError             = 1,             /**< Error: Renderer configuration initialised with a deprecated configuration file or stream. */
        kInvalidParameterError              = 2,             /**< Error: Input parameter in a function call is invalid. */
        kDuplicateSpeakerParametersError    = 3,             /**< Error: Speaker parameter is already defined in the renderer configuration. */
        kInvalidPatchError                  = 4,             /**< Error: Invalid render patch has been found in the renderer configuration. */
        kNoSuchSpeakerError                 = 5,             /**< Error: Referred speaker not found in the renderer configuration. */
        kInvalidCfgLineError                = 6,             /**< Error: Invalid line has been found in the configuration file or stream. */
        kNoLFESpeakerError                  = 7,             /**< Error: Referred LFE speaker not defined in the renderer configuration. */
        kNoDownMixSpeakerError              = 8,             /**< Error: Referred speaker downmix map not defined in the renderer configuration. */

    };

} // RenderUtils

#endif // __RENDERERCONFIGURATIONERRORS_H__
