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

#ifndef COMMON_STREAM_SRC_UTILS_PCMUTILITIES_H_
#define COMMON_STREAM_SRC_UTILS_PCMUTILITIES_H_

#if defined(_MSC_VER)
#define CMNSTRM_INLINE __inline
#else
// could also be static __inline__ __attribute__((always_inline)) or just static __inline__
#define CMNSTRM_INLINE static inline
#endif

#define KNoError 0

#ifdef __cplusplus
extern "C" {
#endif

#include "commonstream/utils/Namespace.h"

namespace CommonStream
{
    /**************************************************************************
     *
     * PCM conversion utilities
     *
     * Utility functions to convert between integer and floating point PCM
     * representations.
     * <em> Big_Endian byte order is assumed, unless otherwise stated.</em>
     *
     * @defgroup pcmutilities MDA PCM Utilities/Helper functions
     *
     *****************************************************************************/

    /**
     * Converts 24-bit PCM samples to 32-bit native integer samples.
     *
     * @param pDest Pointer to a buffer of pCount int samples.
     * @param pSrc Pointer to a buffer of pCount PCM24 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM24ToInt32(int *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int i;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            *pDest  = *(lSrc++) << 24;
            *pDest |= *(lSrc++) << 16;
            *pDest |= *(lSrc++) << 8;

            pDest++;
        }

        return KNoError;
    }

    /**
     * Stores the value a single 24-bit PCM big-endian sample in a native int
     *
     * @param pSrc Pointer to a PCM24 big-endian sample.
     * @return pDest Native int sample.
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int OnePCM24ToInt32(unsigned char *pSrc)
    {
        int tmp = 0;

        tmp  = *(pSrc++) << 24;
        tmp |= *(pSrc++) << 16;
        tmp |= *(pSrc++) << 8;

        return tmp;
    }

    /**
     * Converts 24-bit PCM samples to 32-bit Int samples, stored specifically in <b>
     * Little-Endian </b> byte order.
     *
     * @param pDest Pointer to a buffer of pCount int samples, in <b>Little-Endian </b> byte order.
     * @param pSrc Pointer to a buffer of pCount PCM24 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM24ToInt32_LE(int *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            *pDest  = *(lSrc++) << 8;
            *pDest |= *(lSrc++) << 16;
            *pDest |= *(lSrc++) << 24;

            pDest++;
        }

        return KNoError;
    }


    /**
     * Converts 24-bit PCM samples to 32-bit Int samples, stored specifically in <b>
     * Little-Endian </b> byte order. Then right justify the samples, using
     * sign extension to pad. So a 24 bit sample
     * AA BB FF
     * is converted to
     * FF FF BB AA
     *
     * @param pDest Pointer to a buffer of pCount int samples, in <b>Little-Endian </b> byte order.
     * @param pSrc Pointer to a buffer of pCount PCM24 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM24ToInt32_LE_RJ_SE(int *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            *pDest  = *(lSrc++) << 8;
            *pDest |= *(lSrc++) << 16;
            *pDest |= *(lSrc++) << 24;

            *pDest >>= 8;

            pDest++;
        }

        return KNoError;
    }

    /**
     * Converts 32-bit PCM samples to 32-bit native integer samples.
     *
     * @param pDest Pointer to a buffer of pCount int samples.
     * @param pSrc Pointer to a buffer of pCount PCM32 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM32ToInt32(int *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            *pDest  = *(lSrc++) << 24;
            *pDest |= *(lSrc++) << 16;
            *pDest |= *(lSrc++) << 8;
            *pDest |= *(lSrc++) << 0;

            pDest++;
        }

        return KNoError;

    }

    /**
     * Converts 32-bit native interger samples to 32-bit PCM samples.
     *
     * @param pDest Pointer to a buffer of pCount PCM32 samples.
     * @param pSrc Pointer to a buffer of pCount int samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int Int32ToPCM32(char *pDest, int *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lDest = (unsigned char *) pDest;

        for(i = 0; i < pCount; i++)
        {
            *(lDest++) = (*pSrc >> 24) & 0x000000FF;
            *(lDest++) = (*pSrc >> 16) & 0x000000FF;
            *(lDest++) = (*pSrc >> 8)  & 0x000000FF;
            *(lDest++) = (*pSrc >> 0)  & 0x000000FF;

            pSrc++;
        }

        return KNoError;
    }


    /**
     * Converts 32-bit native interger samples to 24-bit PCM samples.
     *
     * @param pDest Pointer to a buffer of pCount PCM24 samples.
     * @param pSrc Pointer to a buffer of pCount int samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int Int32ToPCM24(char *pDest, int *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lDest = (unsigned char *) pDest;

        for(i = 0; i < pCount; i++)
        {
            *(lDest++) = (*pSrc >> 24) & 0x000000FF;
            *(lDest++) = (*pSrc >> 16) & 0x000000FF;
            *(lDest++) = (*pSrc >> 8)  & 0x000000FF;

            pSrc++;
        }

        return KNoError;
    }

    /**
     * Converts 32-bit PCM samples to 24-bit PCM samples.
     *
     * @param pDest Pointer to a buffer of pCount PCM24 samples.
     * @param pSrc Pointer to a buffer of pCount PCM32 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM32ToPCM24(char *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int i = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;
        unsigned char *lDest = (unsigned char *) pDest;

        for(i = 0; i < pCount; i++)
        {
            *lDest++  = *lSrc++;								// MSB
            *lDest++  = *lSrc++;								// Mid-SB
            *lDest++  = *lSrc++;								// LSB

            lSrc++;												// Lowest byte of 32-bit discarded, Upper 3 of 4 bytes used
        }

        return KNoError;

    }

    /**
     * Converts 24-bit PCM samples to float samples.
     *
     * @param pDest Pointer to a buffer of pCount float samples.
     * @param pSrc Pointer to a buffer of pCount PCM24 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM24ToFloat(float *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int    i = 0;

        int             val = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            val  = *(lSrc++) << 24;
            val |= *(lSrc++) << 16;
            val |= *(lSrc++) << 8;

            *(pDest++) = (float) val / 0x7fffffff;
        }

        return KNoError;

    }

    /**
     * Converts 32-bit PCM samples to float samples.
     *
     * @param pDest Pointer to a buffer of pCount float samples.
     * @param pSrc Pointer to a buffer of pCount PCM32 samples.
     * @param pCount Number of samples to convert.
     * @return KNoError if no errors occurred.
     *
     *
     * @ingroup pcmutilities
     */

    CMNSTRM_INLINE int PCM32ToFloat(float *pDest, char *pSrc, unsigned int pCount)
    {
        unsigned int    i = 0;

        int             val = 0;

        unsigned char *lSrc = (unsigned char *) pSrc;

        for(i = 0; i < pCount; i++)
        {
            val  = *(lSrc++) << 24;
            val |= *(lSrc++) << 16;
            val |= *(lSrc++) << 8;
            val |= *(lSrc++) << 0;

            *(pDest++) = (float) val / 0x7fffffff;
        }

        return KNoError;

    }

} // namespace CommonStream

#ifdef __cplusplus
}
#endif

#endif // COMMON_STREAM_SRC_UTILS_PCMUTILITIES_H_
