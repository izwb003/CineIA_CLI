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

#ifndef COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMWRITERT_H_
#define COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMWRITERT_H_

#include <exception>
#include <sstream>
#include <istream>
#include <iostream>
#include <climits>
#include <cstdio>

#include "commonstream/utils/Namespace.h"
#include "ByteStreamStateT.h"
#include "commonstream/utils/ByteSwap.h"

#ifdef CMNSTRM_USE_CHECK_SUM
#include "commonstream/hash/HashGenerator.h"
#include "commonstream/hash/CRCGenerator.h"
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

namespace CommonStream
{

    template<typename StreamType>
    class ByteStreamWriterT : public ByteStreamStateT<StreamType>
    {

        using ByteStreamStateT<StreamType>::error_;
        using ByteStreamStateT<StreamType>::byteCount_;
        using ByteStreamStateT<StreamType>::bufferLengthInBytes_;
    
    public:

        ByteStreamWriterT();

        ByteStreamWriterT(const ByteStreamWriterT<StreamType> &bs2); ///< copy constructor
        ByteStreamWriterT(char *iBuffer, ByteCount_t iBufferLength);
        ByteStreamWriterT(unsigned char *iBuffer, ByteCount_t iBufferLength);
        ByteStreamWriterT(std::ofstream &iStream);
        ByteStreamWriterT(std::ostream &iStream);

        ~ByteStreamWriterT();

        ByteStreamWriterT &operator=(const ByteStreamWriterT<StreamType> &bs2);

        ReturnCode Init();
        ReturnCode Init(char *iBuffer, ByteCount_t iBufferLength);
        ReturnCode Init(unsigned char *iBuffer, ByteCount_t iBufferLength);
        ReturnCode Init(const ByteStreamWriterT<StreamType> &bs2);

        std::streampos streamPosition() const
        {
            return stream_->tellp();
        }
        
        std::streampos streamPosition(std::streampos sp)
        {
            return stream_->seekp(sp);
        }

        ReturnCode write(bool iVal);

        ReturnCode write(uint8_t iVal);
        ReturnCode write(uint16_t iVal);
        ReturnCode write(uint32_t iVal);
        ReturnCode write(uint64_t iVal);

        ReturnCode write(char iVal);

        ReturnCode write(int8_t iVal);
        ReturnCode write(int16_t iVal);
        ReturnCode write(int32_t iVal);
        ReturnCode write(int64_t iVal);

        ReturnCode write(const uint8_t *iVal, uint32_t iNumBytes);

        ByteCount_t getByteCount(void);

        ReturnCode seek(uint64_t iPosition);

        ReturnCode ShrinkBuffer(ByteCount_t new_buffer_size);
        ReturnCode IncrementByteCount(ByteCount_t incr);
        ReturnCode skip(ByteCount_t bytes_to_skip);

        char *buffer() const;

        void clear(void);
        bool good(void);
        bool eof(void);
        bool fail(void);

        operator bool ()
        {
            return good();
        }

        ReturnCode status(void);

        ByteStreamStateT<StreamType> reset(void);

        // utilities, debugging
        void dump_buffer_ascii(FILE *fp);

#ifdef CMNSTRM_USE_CHECK_SUM
        bool                setHashGenerator(HashGenerator *iGenerator);
        HashGenerator*      getHashGenerator(void);

        bool                setCRCGenerator(CRCGenerator *iGenerator);
        CRCGenerator*       getCRCGenerator(void);
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

    private:

        ByteStreamStateT<StreamType> getState(void) const;
        ByteStreamStateT<StreamType> setState(ByteStreamStateT<StreamType> state);
        
        StreamType          *stream_;

#ifdef CMNSTRM_USE_CHECK_SUM
        CRCGenerator		*CRC_;
        HashGenerator		*hash_;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    };

    template<typename StreamType>
    ByteStreamWriterT<StreamType>::ByteStreamWriterT()
        :
        stream_(nullptr)
#ifdef CMNSTRM_USE_CHECK_SUM
        , hash_(nullptr)
        , CRC_(nullptr)
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    {
        Init();
    }

    template<typename StreamType>
    ByteStreamWriterT<StreamType>::ByteStreamWriterT(char *iBuffer, ByteCount_t iBufferLength)
        :
        stream_(nullptr)
#ifdef CMNSTRM_USE_CHECK_SUM
        , hash_(nullptr)
        , CRC_(nullptr)
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    {
        ReturnCode rc = Init(iBuffer, iBufferLength);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    ByteStreamWriterT<StreamType>::ByteStreamWriterT(const ByteStreamWriterT<StreamType> &bs2)
        :
        stream_(nullptr)
#ifdef CMNSTRM_USE_CHECK_SUM
        , hash_(nullptr)
        , CRC_(nullptr)
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    {
        ReturnCode rc = Init(bs2);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    ByteStreamWriterT<StreamType>::ByteStreamWriterT(unsigned char *iBuffer, ByteCount_t iBufferLength)
        :
        stream_(nullptr)
#ifdef CMNSTRM_USE_CHECK_SUM
        , hash_(nullptr)
        , CRC_(nullptr)
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    {
        ReturnCode rc = Init(iBuffer, iBufferLength);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::Init()
    {
        byteCount_ = 0;
        bufferLengthInBytes_ = 0;
        if ( stream_ )
        {
            delete stream_;
        }
        stream_ = 0;

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;
        return CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::Init(unsigned char *iBuffer, ByteCount_t iBufferLength)
    {
        return Init(reinterpret_cast<char *>(iBuffer), iBufferLength);
    }

    /*
     * Retreiving the number of used bytes
     */
    template<typename StreamType>
    ByteCount_t ByteStreamWriterT<StreamType>::getByteCount(void)
    {
        return byteCount_;
    }

    template<typename StreamType>
    ReturnCode ByteStreamWriterT<StreamType>::seek(uint64_t iPosition)
    {
        if (!good())
        {
            return status();
        }

        stream_->seekp(iPosition);

        return status();
    }

    template<typename StreamType>
    void ByteStreamWriterT<StreamType>::clear()
    {
        error_ = CMNSTRM_OK;
        stream_->clear();
    }

    /// \brief Shrink the buffer to a given number of bytes
    /// \param new_buffer_size  new size in bytes
    /// \return OK/error code
    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::ShrinkBuffer(ByteCount_t new_buffer_size)
    {

        ByteCount_t tentative_byte_size = CHAR_BIT*new_buffer_size;

        // check that we are not enlarging the buffer, and that we are not cutting
        // already processed bytes
        if ( tentative_byte_size > bufferLengthInBytes_ || tentative_byte_size < byteCount_ )
        {
            return error_ = CMNSTRM_IO_FAIL;
        }
        bufferLengthInBytes_ = tentative_byte_size;
        return CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::IncrementByteCount(ByteCount_t incr)
    {
        ByteCount_t tentative = byteCount_ + incr;
        if ( tentative > bufferLengthInBytes_ )
        {
            byteCount_ = bufferLengthInBytes_;
        }
        else
        {
            byteCount_ = tentative;
        }
        return CMNSTRM_OK;
    }

    /**
     *
     */

    template<typename StreamType>
    bool  ByteStreamWriterT<StreamType>::good()
    {
        if (error_ != CMNSTRM_OK)
        {
            return  false;
        }

        return stream_->good();
    }

    /**
     *
     */

    template<typename StreamType>
    bool  ByteStreamWriterT<StreamType>::eof()
    {
        if (stream_->eof())
            error_ = CMNSTRM_IO_EOF;
        return error_ == CMNSTRM_IO_EOF;
    }

    /**
     *
     */

    template<typename StreamType>
    bool  ByteStreamWriterT<StreamType>::fail()
    {
        if (stream_->fail())
            error_ = CMNSTRM_IO_FAIL;
        return error_ == CMNSTRM_IO_FAIL;
    }

    /**
     *  Retrieving the error code from the byte stream state
     */

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::status(void)
    {
        if (error_ != CMNSTRM_OK)
        {
            return  error_;
        }

        if (eof())
        {
            return CMNSTRM_IO_EOF;
        }

        if (fail())
        {
            return CMNSTRM_IO_FAIL;
        }

        return CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode ByteStreamWriterT<StreamType>::skip(ByteCount_t iNumbytes)
    {
        if (!good())
        {
            return status();
        }

        uint8_t tmp = 0;

        ByteCount_t bytesLeftToWrite = iNumbytes;

        while (bytesLeftToWrite)
        {
            write(tmp);
            --bytesLeftToWrite;
        }

        return status();
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamWriterT<StreamType>::getState(void) const
    {

        ByteStreamStateT<StreamType> state;

        state.streamState(stream_->rdstate());
        state.streamPosition(stream_->tellp());
        state.byteCount(byteCount_);
        state.error(error_);
        state.bufferLengthInBytes(bufferLengthInBytes_);
        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamWriterT<StreamType>::setState(ByteStreamStateT<StreamType> state)
    {
        stream_->clear();
        stream_->setstate(state.streamState());
        stream_->seekp(state.streamPosition());

        byteCount_ = state.byteCount();
        error_ = state.error();
        bufferLengthInBytes_ = state.bufferLengthInBytes();
        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamWriterT<StreamType>::reset(void)
    {
        stream_->clear();
        stream_->seekp(0);

        byteCount_ = 0;

        error_ = CMNSTRM_OK;

        return getState();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(bool iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint8_t)iVal);

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(uint8_t iVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->write((char*) &iVal, 1);

        if (!good())
        {
            return status();
        }

        byteCount_ += 1;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 1);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(uint16_t iVal)
    {
        if (!good())
        {
            return status();
        }

        BYTESWAP::swapByteOrder(iVal);

        stream_->write((char*) &iVal, 2);

        if (!good())
        {
            return status();
        }

        byteCount_ += 2;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 2);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(uint32_t iVal)
    {
        if (!good())
        {
            return status();
        }

        BYTESWAP::swapByteOrder(iVal);

        stream_->write((char*) &iVal, 4);

        if (!good())
        {
            return status();
        }

        byteCount_ += 4;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 4);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(uint64_t iVal)
    {
        if (!good())
        {
            return status();
        }

        BYTESWAP::swapByteOrder(iVal);

        stream_->write((char*) &iVal, 8);

        if (!good())
        {
            return status();
        }

        byteCount_ += 8;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 8);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamWriterT<StreamType>::write(char iVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->write(&iVal, 1);

        if (!good())
        {
            return status();
        }

        byteCount_ += 1;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 1);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(int8_t iVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->write((char*) &iVal, 1);

        if (!good())
        {
            return status();
        }

        byteCount_ += 1;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 1);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(int16_t iVal)
    {
        if (!good())
        {
            return status();
        }

        uint16_t tmp = iVal;
        BYTESWAP::swapByteOrder(tmp);

        stream_->write((char*) &tmp, 2);

        if (!good())
        {
            return status();
        }

        byteCount_ += 2;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 2);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(int32_t iVal)
    {
        if (!good())
        {
            return status();
        }

        uint32_t tmp = iVal;
        BYTESWAP::swapByteOrder(tmp);

        stream_->write((char*) &tmp, 4);

        if (!good())
        {
            return status();
        }

        byteCount_ += 4;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 4);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(int64_t iVal)
    {
        if (!good())
        {
            return status();
        }

        uint64_t tmp = iVal;
        BYTESWAP::swapByteOrder(tmp);

        stream_->write((char*) &tmp, 8);

        if (!good())
        {
            return status();
        }

        byteCount_ += 8;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&iVal, 8);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode  ByteStreamWriterT<StreamType>::write(const uint8_t *iVal, uint32_t iNumBytes)
    {
        if (!good())
        {
            return status();
        }

        stream_->write((char*) iVal, iNumBytes);

        if (!good())
        {
            return status();
        }

        byteCount_ += iNumBytes;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)iVal, iNumBytes);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

#ifdef CMNSTRM_USE_CHECK_SUM
    template<typename StreamType>
    bool ByteStreamWriterT<StreamType>::setHashGenerator(HashGenerator *iGenerator)
    {
        if ((hash_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        hash_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    HashGenerator* ByteStreamWriterT<StreamType>::getHashGenerator(void)
    {
        return hash_;
    }

    template<typename StreamType>
    bool ByteStreamWriterT<StreamType>::setCRCGenerator(CRCGenerator *iGenerator)
    {
        if ((CRC_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        CRC_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    CRCGenerator* ByteStreamWriterT<StreamType>::getCRCGenerator(void)
    {
        return CRC_;
    }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMWRITERT_H_
