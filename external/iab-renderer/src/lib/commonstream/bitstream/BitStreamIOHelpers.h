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

#ifndef COMMON_STREAM_SRC_BITSTREAM_BITSTREAMIOHELPERS_H_
#define COMMON_STREAM_SRC_BITSTREAM_BITSTREAMIOHELPERS_H_

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
#include "commonstream/bitstream/BitStreamReaderT.h"
#include "commonstream/bitstream/BitStreamWriterT.h"
#include "commonstream/stream/StreamTypes.h"
#include "commonstream/utils/StringUtils.h"

#if defined(_WIN32) && _MSC_VER >= 1600
#include <codecvt>
#endif

namespace CommonStream
{
    template<typename StreamType>
    ReturnCode skip(BitStreamReaderT<StreamType> &bsr, uint8_t iNumBits)
    {
        return bsr.skip(iNumBits);
    }

    template<typename StreamType>
    ReturnCode align(BitStreamReaderT<StreamType> &bsr)
    {
        return bsr.align();
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, bool &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, bool &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint8_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint16_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint32_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint64_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint8_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint16_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint32_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, uint64_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int8_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int16_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int32_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int64_t &oVal)
    {
        return bsr.read(oVal);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int8_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int16_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int32_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, int64_t &oVal, uint8_t iNumBits)
    {
        return bsr.read(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode readAligned(BitStreamReaderT<StreamType> &bsr, uint8_t *oVal, uint32_t iNumBytes)
    {
        return bsr.readAligned(oVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint8_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint16_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint32_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint64_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint8_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint16_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint32_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, uint64_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int8_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int16_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int32_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int64_t &oVal)
    {
        return bsr.peek(oVal);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int8_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int16_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int32_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode peek(BitStreamReaderT<StreamType> &bsr, int64_t &oVal, uint8_t iNumBits)
    {
        return bsr.peek(oVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode sync(BitStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes, const uint8_t *iMask)
    {
        return bsr.sync(iVal, iNumBytes, iMask);
    }

    template<typename StreamType>
    ReturnCode sync(BitStreamReaderT<StreamType> &bsr, const uint8_t *iVal, uint8_t iNumBytes)
    {
        return bsr.sync(iVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, bool iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint8_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint16_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint32_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint64_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint8_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint16_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint32_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, uint64_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int8_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int16_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int32_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int64_t iVal)
    {
        return bsw.write(iVal);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int8_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int16_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int32_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, int64_t iVal, uint8_t iNumBits)
    {
        return bsw.write(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::wstring &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::string &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        const std::string s = iVal;
        
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
    ReturnCode writeStringAppendNull(BitStreamWriterT<StreamType> &bsw, const std::string &iVal)
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
    ReturnCode readToNull(BitStreamReaderT<StreamType>& bsr, std::string &oVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const uint8_t *iVal, uint8_t iNumBytes)
    {
        return bsw.write(iVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode readRestrictedURI(BitStreamReaderT<StreamType>& bsr, std::wstring &oVal)
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
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, std::wstring &oVal)
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

            oVal = StringUtils::utf8tow(s);
        }

        delete[] buffer;
        buffer = NULL;

        return rc;
    }

    template<typename StreamType>
    ReturnCode readRestrictedURI(BitStreamReaderT<StreamType>& bsr, std::string &oVal)
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
    
    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, std::string &oVal)
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
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const VarUInt32<I> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(iVal.value_, I);

        return rc;
    }

    template<typename StreamType, int I>
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, VarUInt32<I> &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsr.read(oVal.value_, I);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const PackedUInt32 &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(0x03, 2);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsw.write(iVal.value_, 32);

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, PackedUInt32 &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        uint32_t tmp;

        rc = bsr.read(tmp, 2);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsr.read(oVal.value_, 32);

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const PackedUInt64 &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        rc = bsw.write(0x07, 3);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsw.write(static_cast<uint32_t>(iVal.value_ >> 32) , 32);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsw.write(static_cast<uint32_t>(iVal.value_ & 0xffffffff), 32);

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, PackedUInt64 &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        uint32_t hi;
        uint32_t low;

        rc = bsr.read(hi, 3);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsr.read(hi, 32);

        if (rc != CMNSTRM_OK)
        {
            return rc;
        }

        rc = bsr.read(low, 32);

        oVal.value_ = hi;

        oVal.value_ = oVal.value_ << 32 | low;

        return rc;
    }

    template<typename StreamType>
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const PackedLength &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        if (iVal.length_ < 128)
        {
            rc = bsw.write(iVal.length_, 8);
        }
        else
        {
            /* push the length's length */

            rc = bsw.write(0x83, 8);

            if (rc != CMNSTRM_OK)
            {
                return rc;
            }

            /* push the length */

            rc = bsw.write(iVal.length_, 32);
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, PackedLength &oVal)
    {
        ReturnCode rc = CMNSTRM_OK;

        uint32_t l;

        rc = bsr.read(l, 8);

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
            rc = bsr.read(oVal.length_, ((l & 0x7F) + 1) << 3);
        }

        return rc;
    }

    template<typename StreamType, class T>
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, std::vector<T> &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::vector<T> &iVal)
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
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, std::vector<uint8_t> &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::vector<uint8_t> &iVal)
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
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, std::vector<int8_t> &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::vector<int8_t> &iVal)
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
    ReturnCode read(BitStreamReaderT<StreamType> &bsr, std::vector<char> &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType> &bsw, const std::vector<char> &iVal)
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
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const OptionalParameter<T> &iVal)
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
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, OptionalParameter<T> &oVal)
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
    ReturnCode write(BitStreamWriterT<StreamType>& bsw, const Plex<N> &iVal)
    {
        ReturnCode rc = CMNSTRM_OK;
        
        uint32_t escapeBits = 0;
        
        if (N == 4)
        {
            escapeBits = 0xF;
        }
        else
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
    ReturnCode read(BitStreamReaderT<StreamType>& bsr, Plex<N> &oVal)
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

#endif // COMMON_STREAM_SRC_BITSTREAM_BITSTREAMIOHELPERS_H_
