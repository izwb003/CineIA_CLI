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

#ifndef __RENDERPATCH_H__
#define __RENDERPATCH_H__

#include <string.h>
#include <stdint.h>

#include "coreutils/Matrix3.h"

namespace RenderUtils
{

    /**
     * Represents a Render Patch, for example, a patch derived from a patch line in renderer configuration file:
     * p   TFR   R     C
     *
     * TFR, R, C are names of the 3 speakers in the patch
     * @sa Matrix3
     */

    struct RenderPatch
    {
        /** channel index associated with its coresponding speaker */
        int32_t		s1_, s2_, s3_;

        /** 3x3 matrix with each row specifying xyz coordinate of a speaker in the patch */
        CoreUtils::Matrix3		basis_;

        /**
         * Creates a render patch
         *
         * @param[in] speaker1 index for s1_
         * @param[in] speaker2 index for s2_
         * @param[in] speaker3 index for s3_
         * @param[in] basis 3x3 matrix
         * Row 1 specifies xyz coordinate of speaker1
         * Row 2 specifies xyz coordinate of speaker2
         * Row 3 specifies xyz coordinate of speaker3
         *
         */
        RenderPatch(int32_t speaker1, int32_t speaker2, int32_t speaker3, CoreUtils::Matrix3 basis);
    };

} // RenderUtils

#endif  // __RENDERPATCH_H__
