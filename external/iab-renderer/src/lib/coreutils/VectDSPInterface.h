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

#ifndef __VECTDSPINTERFACE__
#define __VECTDSPINTERFACE__

namespace CoreUtils
{


    /**
     *
     * @brief Interface wrapper base class. Can be used as parent class for platform
	 * dependent/child VectDSP classes.
     *
     */
    class VectDSPInterface
    {
    public :

        /// Contructor
        VectDSPInterface()
        {
        }

        /**
         *
         * Constructor
         *
         * @param iMaxVectorSize is the size of the buffer needed to store intermediate double data when using VectDSPMacAccelerate
         *
         */
        VectDSPInterface(int iMaxVectorSize)
        {
        }

        /// Destructor
        virtual ~VectDSPInterface()
        {
        }

        /**
         *
         * Accelerated vector based add.
         *
         * @param iVectorA is the vector A, audio buffer
         * @param iVectorB is the vector B, second audio buffer
         * @param oVector is the output buffer
         * @param iLength is the length of addition
         *
         */
        virtual void add(const float *iVectorA,
                         const float *iVectorB,
                         float *oVector,
                         long iLength) = 0;

        /**
         *
         * Accelerated vector based multiply.
         *
         * @param iVectorA is the vector A, audio buffer
         * @param iVectorB is the vector B, second audio buffer
         * @param oVector is the output buffer
         * @param iLength is the length of multiplication
         *
         */
        virtual void mult(const float *iVectorA,
                          const float *iVectorB,
                          float *oVector,
                          long  iLength) = 0;


        /**
         *
         * Accelerated vector based ramp.
         *
         * @param iStartValue is the beginning value of ramp
         * @param iEndValue is the end value of ramp
         * @param oVector is the output buffer
         * @param iLength is the length of ramp
         *
         */
        virtual void ramp(const float iStartValue,
                          const float iEndValue,
                          float *oVector,
                          long iLength) = 0;

        /**
         *
         * Accelerated vector based fill.
         *
         * @param iFillValue is the fill value
         * @param oVector is the output buffer
         * @param iLength is the length to fill
         *
         */
        virtual void fill(const float iFillValue,
                          float *oVector,
                          long iLength) = 0;

    };

} // CoreUtils

#endif /* defined(__VECTDSPINTERFACE__) */
