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

#ifndef COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMIOHELPERS_H_
#define COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMIOHELPERS_H_

#include <assert.h>
#include <exception>
#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <map>

// Inclusion of <stdint.h> (instead of <cstdin>) for C++98 backward compatibility.
#include <stdint.h>

#include "commonstream/utils/Namespace.h"
#include "commonstream/bytestream/ByteStreamReaderT.h"
#include "commonstream/bytestream/ByteStreamWriterT.h"
#include "commonstream/stream/StreamTypes.h"
#include "commonstream/utils/StringUtils.h"

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

namespace CommonStream
{
    template<typename StreamType>
    ReturnCode skip(ByteStreamReaderT<StreamType> &bsr, uint8_t iNumBytes)
    {
        return bsr.skip(iNumBytes);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, bool &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, bool &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, uint8_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, uint16_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, uint32_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, uint64_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, int8_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, int16_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, int32_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, int64_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, uint8_t *oVal, uint32_t iNumBytes)
    {
        return bsr.read(oVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, uint8_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, uint16_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, uint32_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, uint64_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, int8_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, int16_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, int32_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(ByteStreamReaderT<StreamType> &bsr, int64_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode sync(ByteStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes, const uint8_t *iMask)
    {
        return bsr.sync(iVal, iNumBytes, iMask);
    }

    template<typename StreamType>
    ReturnCode sync(ByteStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes)
    {
        return bsr.sync(iVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, bool iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, uint8_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, uint16_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, uint32_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, uint64_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, int8_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, int16_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, int32_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, int64_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const std::wstring &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        const std::string s = StringUtils::wtoutf8(iVal);

        PackedLength len;

        // PackedLength is UInt32

        assert(s.size() <= 0xFFFFFFFF);

        len.length_ = static_cast<uint32_t>(s.size());

        rc = write(bsw, len);

        if (rc != CMNSTRM_OK )
        {
            return rc;
        }

        rc = bsw.write((uint8_t*) s.data(), len.length_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const std::string &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        // PackedLength is UInt32

        assert(iVal.size() <= 0xFFFFFFFF);

        len.length_ = static_cast<uint32_t>(iVal.size());

        rc = write(bsw, len);

        if (rc != CMNSTRM_OK )
        {
            return rc;
        }

        rc = bsw.write((uint8_t*) iVal.data(), len.length_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode writeStringAppendNull(ByteStreamWriterT<StreamType> &bsw, const std::string &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        const std::string s = iVal;
        size_t stringLength = s.size();
        
        if (stringLength > 0)
        {
            // Write the string
            rc = bsw.write((uint8_t*) s.data(), static_cast<uint32_t>(stringLength));
            
            if (rc != CMNSTRM_OK )
            {
                return rc;
            }
        }
        
        // Write null byte
        uint8_t nullByte = 0;
        return bsw.write(nullByte);
    }
    
    template<typename StreamType>
    ReturnCode readToNull(ByteStreamReaderT<StreamType>& bsr, std::string &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        uint8_t val = 0;
        std::vector<uint8_t> parsedBytes;
        
        rc = bsr.read(val);
        
        if (rc != CMNSTRM_OK )
        {
            return rc;
        }
        
        while ((val != 0) && (rc == CMNSTRM_OK))
        {
            parsedBytes.push_back(val);
            rc = bsr.read(val);
        }
        
        if (rc != CMNSTRM_OK )
        {
            return rc;
        }
        
        oVal.assign(parsedBytes.begin(), parsedBytes.end());
        
        return rc;
    }

    template<typename StreamType>
    ReturnCode sizeOf(ByteStreamWriterT<StreamType>& bsw, const std::string &iVal, int32_t &oSize)
    {
        ReturnCode rc = CMNSTRM_OK;

        oSize = 0;

        if (iVal.length() < 128)
        {
            oSize += sizeof(uint8_t);
        }
        else
        {
            oSize += sizeof(uint8_t);
            oSize += sizeof(uint32_t);
        }

        oSize += iVal.length();

        return rc;
    }

    template<typename StreamType>
    ReturnCode readRestrictedURI(ByteStreamReaderT<StreamType>& bsr, std::wstring &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK )
        {
            return rc;
        }

        char *buffer = new char[len.length_];

        rc = bsr.read((uint8_t*) buffer, len.length_);

        if (rc == CMNSTRM_OK )
        {
            std::string s(buffer, len.length_);

            oVal = std::wstring(s.begin(), s.end());
        }

        delete[] buffer;
        buffer = NULL;

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, std::wstring &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK )
        {
            return rc;
        }

        char *buffer = new char[len.length_];

        rc = bsr.read((uint8_t*) buffer, len.length_);

        if (rc == CMNSTRM_OK )
        {
            std::string s(buffer, len.length_);

            oVal = std::wstring(s.begin(), s.end());
        }

        delete[] buffer;
        buffer = NULL;

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, std::string &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK )
        {
            return rc;
        }

        char *buffer = new char[len.length_];

        rc = bsr.read((uint8_t*) buffer, len.length_);

        if (rc == CMNSTRM_OK )
        {
            std::string s(buffer, len.length_);

            oVal = s;
        }

        delete[] buffer;
        buffer = NULL;

        return rc;
    }

    template<typename StreamType, int I>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const VarUInt32<I> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(iVal.value_);

        return rc;
    }

    template<typename StreamType, int I>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, VarUInt32<I> &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsr.read(oVal.value_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const PackedUInt32 &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(iVal.value_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, PackedUInt32 &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsr.read(oVal.value_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const PackedUInt64 &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(iVal.value_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, PackedUInt64 &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsr.read(oVal.value_);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const PackedLength &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        if (iVal.length_ < 128)
        {
            rc = bsw.write((uint8_t)iVal.length_);
        }
        else
        {
            /* push the length's length */

            rc = bsw.write((uint8_t)0x83);

            if (rc != CMNSTRM_OK)
            {
                return rc;
            }

            /* push the length */

            rc = bsw.write(iVal.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, PackedLength &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        uint8_t l;

        rc = bsr.read(l);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        if (l < 128)
        {
            oVal.length_ = l;
        }
        else
        {
            rc = bsr.read(oVal.length_);
        }

        return rc;
    }

    template<typename StreamType, class T>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, std::vector<T> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        iVal.resize(len.length_);

        for(typename std::vector<T>::iterator iter = iVal.begin(); iter != iVal.end(); iter++)
        {
            rc = read(bsr, (*iter));
        }

        return rc;
    }

    template<typename StreamType, class T>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, const std::vector<T> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        // PackedLength is UInt32
        assert(iVal.size() <= 0xFFFFFFFF);

        len.length_ = static_cast<uint32_t>(iVal.size());

        rc = write(bsw, len);

        for(typename std::vector<T>::const_iterator iter = iVal.begin(); iter != iVal.end(); iter++)
        {
            rc = write(bsw, (*iter));
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, std::vector<uint8_t> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        iVal.resize(len.length_);

        if (len.length_ > 0)
        {
            rc = bsr.read((uint8_t*)&iVal.front(), len.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, const std::vector<uint8_t> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength pl;

        /* PackedLength is UInt32 */

        assert(iVal.size() <= 0xFFFFFFFF);


        pl.length_ = (unsigned int)iVal.size();

        rc = write(bsw, pl);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        if (pl.length_ > 0)
        {
            rc = bsw.write(&iVal.front(), pl.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, std::vector<int8_t> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        iVal.resize(len.length_);

        if (len.length_ > 0)
        {
            rc = bsr.read((uint8_t*)&iVal.front(), len.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, const std::vector<int8_t> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength pl;

        /* PackedLength is UInt32 */

        assert(iVal.size() <= 0xFFFFFFFF);

        pl.length_ = (unsigned int)iVal.size();

        rc = write(bsw, pl);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        if (pl.length_ > 0)
        {
            rc = bsw.write((uint8_t*)&iVal.front(), pl.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(ByteStreamReaderT<StreamType> &bsr, std::vector<char> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength len;

        rc = read(bsr, len);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        iVal.resize(len.length_);

        if (len.length_ > 0)
        {
            rc = bsr.read((uint8_t*)&iVal.front(), len.length_);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(ByteStreamWriterT<StreamType> &bsw, const std::vector<char> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        PackedLength pl;

        /* PackedLength is UInt32 */

        assert(iVal.size() <= 0xFFFFFFFF);

        pl.length_ = (unsigned int)iVal.size();

        rc = write(bsw, pl);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        if (pl.length_ > 0)
        {
            rc = bsw.write((uint8_t*)&iVal.front(), pl.length_);
        }

        return rc;
    }

    template<typename StreamType, class T>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const OptionalParameter<T> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = write(bsw, iVal.getPresent());

        if (iVal.getPresent())
        {
            rc = write(bsw, iVal.getValue());
        }

        return rc;
    }

    template<typename StreamType, class T>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, OptionalParameter<T> &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        bool present;
        T value;

        rc = bsr.read(present);

        oVal.setPresent(present);

        if (present)
        {
            rc = read(bsr, value);
            oVal.setValue(value);
        }

        return rc;
    }

    template<typename StreamType, uint32_t N>
    ReturnCode write(ByteStreamWriterT<StreamType>& bsw, const Plex<N> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        uint32_t escapeBits = 0;
        
        if (N == 8)
        {
            escapeBits = 0xFF;
        }
        else
        {
            return CMNSTRM_PARAMS_BAD;
        }
        
        uint32_t bitsForValue = N;
        uint32_t value = iVal;
        
        // Compute the escape bits
        //
        while (true)
        {
            uint64_t maxValue = 1;
            maxValue = (maxValue << bitsForValue) - 1;
            
            if (bitsForValue > 32)
            {
                return CMNSTRM_PARAMS_BAD;
            }
            else
            if (value < maxValue)
            {
                break;
            }
            else
            {
                bitsForValue = bitsForValue << 1;
            }
        }
        
        // Write the escape bits
        //
        uint32_t escapeBitsToWrite = (bitsForValue / N) - 1;
        
        while (escapeBitsToWrite > 0)
        {
            escapeBitsToWrite--;
            
            rc = bsw.write(escapeBits, N);
            
            if (rc != CMNSTRM_OK)
            {
                return rc;
            }
        }
        
        // Write the value
        //
        rc = bsw.write(value, bitsForValue);
        
        return rc;
    }

    template <typename StreamType, uint32_t N>
    ReturnCode read(ByteStreamReaderT<StreamType>& bsr, Plex<N> &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        oVal = 0;
        uint32_t bitsToRead = N;
        uint32_t value = 0;
        
        while (bitsToRead <= 32)
        {
            rc = bsr.read(value, bitsToRead);
            
            if (rc == CMNSTRM_OK)
            {
                uint64_t maxValue = 1;
                maxValue = (maxValue << bitsToRead) - 1;
                
                if (value < maxValue)
                {
                    oVal = value;
                    
                    return CMNSTRM_OK;
                }
                else
                {
                    bitsToRead = bitsToRead << 1;
                }
            }
            else
            {
                return rc;
            }
        }
        
        // We have 32 or more bits to read
        // Not supported
        //
        return CMNSTRM_PARAMS_BAD;
    }

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMIOHELPERS_H_
