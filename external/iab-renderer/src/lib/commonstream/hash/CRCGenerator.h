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

#ifndef COMMON_STREAM_SRC_HASH_CRCGENERATOR_H_
#define COMMON_STREAM_SRC_HASH_CRCGENERATOR_H_

// Inclusion of <stdint.h> (instead of <cstdin>) for C++98 backward compatibility.
#include <stdint.h>
#include "commonstream/utils/Namespace.h"

namespace CommonStream
{

    /**
     * @enum ECRCType
     *
     * @brief Available byte based CRC algorithms.
     *
     * @note Append only to the end and set eCRCType_Maximum as the last valid CRC.
     *
     */
    enum ECRCType
    {
        eCRCType_Minimum = 0,
        eCRCType_None = eCRCType_Minimum,
        eCRCType_CRC16 = 1,
        eCRCType_Maximum = eCRCType_CRC16
    };


    /**
     * @brief CRCGenerator is the interface for all CRC algorithms used by the BitStreamReader and BitStreamWriter objects.
     *
     *
     */
    class CRCGenerator
    {
    public:

        /**
         * @enum ECRCState
         *
         * @brief Represents the states of the crc class
         *
         * @note Valid state transitions:
         *          1) Stopped -> Stopped
         *          2) Stopped -> Started
         *          3) Started -> Started
         *          4) Started -> Paused
         *          5) Started -> Stopped
         *          6) Paused -> Paused
         *          7) Paused -> Started
         *          8) Paused -> Stopped
         *
         */
        enum ECRCState
        {
            eCRCState_Stopped = 0,
            eCRCState_Started,
            eCRCState_Paused
        };

        /**
         * @brief Destructor.
         *
         */
        virtual ~CRCGenerator() {}

        /**
         * @brief Returns the type of CRC.
         *
         */
        virtual ECRCType getCRCType(void) = 0;

        /**
         * @brief Returns the state of the hash class.
         *
         */
        virtual ECRCState getState(void) = 0;

        /**
         * @brief Returns if a CRC algorithm is currently being used (accumulated).
         *
         */
        virtual bool useCRC(void) = 0;

        /**
         * @brief Starts the acculuation of a CRC.
         *
         */
        virtual bool start(void) = 0;

        /**
         * @brief Ends the acculuation of a CRC.
         *
         * @note Potentially computes the final CRC with additional processing.
         *
         * @return true if CRC is stopped and successfully computed.
         */
        virtual bool stop(void) = 0;

        /**
         * @brief Pauses the acculuation of a hash.
         *
         * @return true if hash is stopped and successfully computed.
         */
        virtual bool pause(void) = 0;

        /**
         * @brief Resumes the acculuation of a hash.
         *
         * @return true if hash is stopped and successfully computed.
         */
        virtual bool resume(void) = 0;

        /**
         * @brief Adds a value to the CRC algorithm.
         *
         * @param iBits data to be CRCed.
         * @param iBitCount number of bytes stored in iValue
         * @return true if CRC is properly added.
         *
         */
        virtual bool accumulate(uint32_t iBits, uint8_t iBitCount) = 0;
        virtual bool accumulate(uint64_t iBits, uint8_t iBitCount) = 0;

        /**
         * @brief Size in bytes of final CRC.
         *
         */
        virtual uint32_t getCRCRegisterSize(void) = 0;

        /**
         * @brief Provides an interface for getting the contents of the computed CRC.
         *
         * @note oCRC must be large enough to store the CRC in.
         *       It must be the same or larger than getCRCSize.
         *
         * @param oCRC pointer to a buffer of data to be CRCed.
         * @return true if CRC is properly set to oCRC.
         *
         */
        virtual bool getCRCRegister(uint8_t *oCRC) = 0;

    };

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_HASH_CRCGENERATOR_H_
