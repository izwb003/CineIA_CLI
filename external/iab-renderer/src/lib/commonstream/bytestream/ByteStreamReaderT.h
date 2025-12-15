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

#ifndef COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMREADERT_H_
#define COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMREADERT_H_

#include <string.h>
#include <istream>
#include <climits>

#include "commonstream/utils/Namespace.h"
#include "ByteStreamStateT.h"
#include "commonstream/utils/ByteSwap.h"

#ifdef CMNSTRM_USE_CHECK_SUM
#include "commonstream/hash/HashGenerator.h"
#include "commonstream/hash/CRCGenerator.h"
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

namespace CommonStream
{
    template<typename StreamType> class ByteStreamReaderT;

    template<typename StreamType>
    class ByteStreamStateRestorerT
    {
    public:
        ByteStreamStateRestorerT(ByteStreamReaderT<StreamType> *iBSR)
        {
            if (iBSR == nullptr)
            {
                return;
            }

            bsr_ = iBSR;

#ifdef CMNSTRM_USE_CHECK_SUM
            if (bsr_->getHashGenerator())
            {
                bsr_->getHashGenerator()->pause();
            }
            if (bsr_->getCRCGenerator())
            {
                bsr_->getCRCGenerator()->pause();
            }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

            savedState_ = bsr_->getState();
        }

        ~ByteStreamStateRestorerT()
        {
            if (bsr_ == nullptr)
            {
                return;
            }

#ifdef CMNSTRM_USE_CHECK_SUM
            if (bsr_->getHashGenerator())
            {
                bsr_->getHashGenerator()->resume();
            }
            if (bsr_->getCRCGenerator())
            {
                bsr_->getCRCGenerator()->resume();
            }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

            bsr_->setState(savedState_);
        }

    private:
        ByteStreamStateT<StreamType>      savedState_;
        ByteStreamReaderT<StreamType>     *bsr_;
    };


    template<typename StreamType>
    class ByteStreamReaderT : public ByteStreamStateT<StreamType>
    {

        using ByteStreamStateT<StreamType>::byteCount_;
        using ByteStreamStateT<StreamType>::error_;

        // Friend class for access to getState and setState
        //
        friend class ByteStreamStateRestorerT<StreamType>;

    public:

        ByteStreamReaderT();
        ByteStreamReaderT(const ByteStreamReaderT<StreamType> &bs2);
        ByteStreamReaderT(const char *iBuffer, ByteCount_t iBufferLength);
        ByteStreamReaderT(const unsigned char *iBuffer, ByteCount_t iBufferLength);
        ByteStreamReaderT(std::ifstream &iStream);
        ByteStreamReaderT(std::istream &iStream);

        ~ByteStreamReaderT();

        ReturnCode Init();
        ReturnCode Init(const char *iBuffer, ByteCount_t iBufferLength);
        ReturnCode Init(const unsigned char *iBuffer, ByteCount_t iBufferLength);
        ReturnCode Init(const ByteStreamReaderT<StreamType> &bs2);
        ByteStreamReaderT<StreamType> &operator=(const ByteStreamReaderT<StreamType> &bs2);

        std::streampos streamPosition() const
        {
            return stream_->tellg();
        }
        
        std::streampos streamPosition(std::streampos sp)
        {
            return stream_->seekg(sp);
        }
        
        ReturnCode read(bool &oval);
        ReturnCode read(char &oval);

        ReturnCode read(uint8_t &oVal);
        ReturnCode read(uint16_t &oVal);
        ReturnCode read(uint32_t &oVal);
        ReturnCode read(uint64_t &oVal);

        ReturnCode read(int8_t &oVal);
        ReturnCode read(int16_t &oVal);
        ReturnCode read(int32_t &oVal);
        ReturnCode read(int64_t &oVal);

        ReturnCode read(uint8_t *oVal, uint32_t iNumBytes);

        ReturnCode peek(bool &oVal);

        ReturnCode peek(uint8_t &oVal);
        ReturnCode peek(uint16_t &oVal);
        ReturnCode peek(uint32_t &oVal);
        ReturnCode peek(uint64_t &oVal);

        ReturnCode peek(int8_t &oVal);
        ReturnCode peek(int16_t &oVal);
        ReturnCode peek(int32_t &oVal);
        ReturnCode peek(int64_t &oVal);

        ReturnCode seek(ByteCount_t iPosition);

        ReturnCode skip(ByteCount_t iNumBytes);

        ReturnCode sync(const uint8_t *iVal, uint8_t iNumBytes, const uint8_t *iMask);
        ReturnCode sync(const uint8_t *iVal, uint8_t iNumBytes);

        ByteCount_t getByteCount(void) const;
        ByteCount_t getPosition(void) const;

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

#ifdef CMNSTRM_USE_CHECK_SUM
        bool                setHashGenerator(HashGenerator *iGenerator);
        HashGenerator*      getHashGenerator(void);

        bool                setCRCGenerator(CRCGenerator *iGenerator);
        CRCGenerator*       getCRCGenerator(void);
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

    private:

        ByteStreamStateT<StreamType> getState(void) const;
        ByteStreamStateT<StreamType> setState(ByteStreamStateT<StreamType> state);

        StreamType		*stream_;

#ifdef CMNSTRM_USE_CHECK_SUM
        CRCGenerator		*CRC_;
        HashGenerator		*hash_;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    };

    template<typename StreamType>
    ByteStreamReaderT<StreamType>::ByteStreamReaderT():stream_(nullptr)
    {
        Init();
    }

    template<typename StreamType>
    ByteStreamReaderT<StreamType>::ByteStreamReaderT(const ByteStreamReaderT<StreamType> &bs2)
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
    ByteStreamReaderT<StreamType>::ByteStreamReaderT(const char *iBuffer, ByteCount_t iBufferLength)
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
    ByteStreamReaderT<StreamType>::ByteStreamReaderT(const unsigned char *iBuffer, ByteCount_t iBufferLength)
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
    ReturnCode ByteStreamReaderT<StreamType>::Init()
    {
        byteCount_ = 0;
        stream_ = 0;

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return error_ = CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::sync(const uint8_t *iVal, uint8_t iNumBytes, const uint8_t *iMask)
    {
        if (iNumBytes == 0 || iVal == nullptr)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        ByteStreamStateT<StreamType> state = getState();

        if (!good())
        {
            return status();
        }

        // Allocate enough memory to store our read data
        //
        uint8_t *currSyncBuf = new uint8_t[iNumBytes];

        // Allocate memory for temporary masked buffers
        //
        uint8_t *maskedSyncBuf = new uint8_t[iNumBytes];
        uint8_t *maskedValBuf = new uint8_t[iNumBytes];

        // Create temporary pointers for various copying
        //
        const uint8_t *maskPtr = iMask;
        uint8_t *insertionPoint = currSyncBuf;
        uint8_t *maskedSyncBufPtr = maskedSyncBuf;
        uint8_t *maskedValBufPtr = maskedValBuf;

        // If any of our allocations failed,
        // clean up and return early.
        if (currSyncBuf == nullptr || maskedSyncBuf == nullptr || maskedValBuf == nullptr)
        {
            if (currSyncBuf)
            {
                delete [] currSyncBuf;
            }

            if (maskedSyncBuf)
            {
                delete [] maskedSyncBuf;
            }

            if (maskedValBuf)
            {
                delete [] maskedValBuf;
            }

            return false;
        }

        // Don't start comparison until we have accumulated enough bytes
        //
        uint64_t accumulatedBytes = 0;
        bool foundVal = false;
        char charToRead;

        do
        {
            if (accumulatedBytes >= iNumBytes)
            {
                // Copy to tmp bufs
                //
                memcpy(maskedSyncBuf, currSyncBuf, iNumBytes);
                memcpy(maskedValBuf, iVal, iNumBytes);

                // Update tmp pointers
                //
                maskPtr = iMask;
                maskedSyncBufPtr = maskedSyncBuf;
                maskedValBufPtr = maskedValBuf;

                // Apply the mask
                //
                for (int32_t i = 0; i < iNumBytes; i++)
                {
                    *maskedSyncBufPtr = *maskedSyncBufPtr & *maskPtr;
                    *maskedValBufPtr = *maskedValBufPtr & *maskPtr;

                    ++maskedSyncBufPtr;
                    ++maskedValBufPtr;
                    ++maskPtr;
                }

                // Check to see if we have a match
                //
                int32_t success = memcmp(maskedSyncBuf, maskedValBuf, iNumBytes);

                if (success == 0)
                {
                    foundVal = true;
                    break;
                }

                stream_->read(&charToRead, 1);
                if (stream_->good())
                {

                    memmove(currSyncBuf, currSyncBuf + 1, iNumBytes - 1);
                    memcpy(currSyncBuf + iNumBytes - 1, &charToRead, 1);

                    byteCount_ += 1;
                }
            }
            else
            {
                stream_->read(&charToRead, 1);
                if (stream_->good())
                {

                    memcpy(insertionPoint, &charToRead, 1);

                    ++insertionPoint;
                    ++accumulatedBytes;

                    byteCount_ += 1;

                }
            }
        }
        while (stream_->good());

        delete [] maskedValBuf;
        delete [] maskedSyncBuf;
        delete [] currSyncBuf;

        if (foundVal)
        {
            // Seek back to the beginning of the byte sequence
            // we were looking for
            //
            stream_->seekg(-iNumBytes, std::ios::cur);

            byteCount_ -= iNumBytes;

            return status();

        }
        else
        {

            ReturnCode rc = status();
            setState(state);

            return rc;
        }
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::sync(const uint8_t *iVal, uint8_t iNumBytes)
    {
        if (iNumBytes == 0 || iVal == nullptr)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        ByteStreamStateT<StreamType> state = getState();

        if (!good())
        {
            return status();
        }

        // Allocate enough memory to store our read data
        //
        uint8_t *currSyncBuf = new uint8_t[iNumBytes];
        uint8_t *insertionPoint = currSyncBuf;

        if (currSyncBuf == nullptr)
        {
            return false;
        }

        // Don't start comparison until we have accumulated enough bytes
        //
        uint64_t accumulatedBytes = 0;
        bool foundVal = false;
        char charToRead;

        do
        {
            if (accumulatedBytes >= iNumBytes)
            {
                // Check to see if we have a match
                int32_t success = memcmp(currSyncBuf, iVal, iNumBytes);

                if (success == 0)
                {
                    foundVal = true;
                    break;
                }

                stream_->read(&charToRead, 1);
                if (stream_->good())
                {

                    memmove(currSyncBuf, currSyncBuf + 1, iNumBytes - 1);
                    memcpy(currSyncBuf + iNumBytes - 1, &charToRead, 1);

                    byteCount_ += 1;

                }
            }
            else
            {
                stream_->read(&charToRead, 1);
                if (stream_->good())
                {

                    memcpy(insertionPoint, &charToRead, 1);

                    ++insertionPoint;
                    ++accumulatedBytes;

                    byteCount_ += 1;
                }
            }
        }
        while (stream_->good());

        delete [] currSyncBuf;

        if (foundVal)
        {
            // Seek back to the beginning of the byte sequence
            // we were looking for
            //
            stream_->seekg(-iNumBytes, std::ios::cur);

            byteCount_ -= iNumBytes;

            return status();
        }
        else
        {

            ReturnCode rc = status();
            setState(state);

            return rc;
        }
    }

    /*
     * Retreiving the number of used bytes
     */

    template<typename StreamType>
    ByteCount_t ByteStreamReaderT<StreamType>::getByteCount(void) const
    {
        return byteCount_;
    }

    template<typename StreamType>
    ByteCount_t ByteStreamReaderT<StreamType>::getPosition(void) const
    {
        return static_cast<ByteCount_t>(stream_->tellg());
    }

    template<typename StreamType>
    void ByteStreamReaderT<StreamType>::clear()
    {
        error_ = CMNSTRM_OK;
        stream_->clear();
    }

    /**
     * Is the byte stream good?
     */

    template<typename StreamType>
    bool ByteStreamReaderT<StreamType>::good()
    {
        if (error_ != CMNSTRM_OK)
        {
            return  false;
        }

        return stream_->good();
    }

    /**
     * Is the byte stream in EOF state?
     */

    template<typename StreamType>
    bool ByteStreamReaderT<StreamType>::eof()
    {
        if (stream_->eof())
            error_ = CMNSTRM_IO_EOF;
        return error_ == CMNSTRM_IO_EOF;
    }

    /**
     * Is the byte stream in FAIL state?
     */

    template<typename StreamType>
    bool ByteStreamReaderT<StreamType>::fail()
    {
        if (stream_->fail())
            error_ = CMNSTRM_IO_FAIL;
        return error_ == CMNSTRM_IO_FAIL;
    }

    /**
     *  Retrieving the error code from the byte stream state
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::status(void)
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

    /**
     *
     */

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamReaderT<StreamType>::getState(void) const
    {
        ByteStreamStateT<StreamType> state;

        state.streamState(stream_->rdstate());
        state.streamPosition(stream_->tellg());

        state.byteCount(byteCount_);
        state.error(error_);

        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamReaderT<StreamType>::setState(ByteStreamStateT<StreamType> state)
    {
        stream_->clear();
        stream_->setstate(state.streamState());
        stream_->seekg(state.streamPosition());

        byteCount_ = state.byteCount();
        error_ = state.error();

        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    ByteStreamStateT<StreamType> ByteStreamReaderT<StreamType>::reset(void)
    {
        stream_->clear();
        stream_->seekg(0);

        byteCount_ = 0;

        error_ = CMNSTRM_OK;

        return getState();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(uint8_t *oVal, uint32_t iNumBytes)
    {
        if (iNumBytes == 0 || oVal == nullptr)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        if (!good())
        {
            return status();
        }

        stream_->read((char *) oVal, iNumBytes);
        if (stream_->good())
        {
#ifdef CMNSTRM_USE_CHECK_SUM
            if (hash_ && hash_->useHash())
            {
                hash_->accumulate(oVal, iNumBytes);
            }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

            return status();

        }
        else
        {

            return status();
        }
    }

    /*
     * Reading and peeking booleans
     */
    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(bool &oVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->read((char*) &oVal, 1);

        if (!good())
        {
            return status();
        }

        byteCount_++;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&oVal, 1);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(bool &oVal)
    {
        if (!good())
        {
            return status();
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 8-bit unsigned integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(uint8_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->read((char*) &oVal, 1);

        if (!good())
        {
            return status();
        }

        byteCount_++;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&oVal, 1);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(uint8_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 16-bit unsigned integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(uint16_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->read((char*) &oVal, 2);

        if (!good())
        {
            return status();
        }

        BYTESWAP::swapByteOrder(oVal);

        byteCount_ += 2;

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&oVal, 2);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        return status();
    }


    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(uint16_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 32-bit unsigned integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(uint32_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->read((char*) &oVal, 4);

        if (!good())
        {
            return status();
        }

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&oVal, 4);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        BYTESWAP::swapByteOrder(oVal);

        byteCount_ += 4;

        return status();
    }


    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(uint32_t &oVal)
    {
        if (!good())
        {
            return false;
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 64-bit unsigned integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(uint64_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        stream_->read((char*) &oVal, 8);

        if (!good())
        {
            return status();
        }

#ifdef CMNSTRM_USE_CHECK_SUM
        if (hash_ && hash_->useHash())
        {
            hash_->accumulate((uint8_t*)&oVal, 8);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        BYTESWAP::swapByteOrder(oVal);

        byteCount_ += 8;

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(uint64_t &oVal)
    {
        if (!good())
        {
            return false;
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 8-bit integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(char &oVal)
    {
        int8_t tmp;
        read(tmp);
        oVal = tmp;

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(int8_t &oVal)
    {
        uint8_t tmp;
        read(tmp);
        oVal = tmp;

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(int8_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 16-bit integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(int16_t &oVal)
    {
        uint16_t tmp;
        read(tmp);
        oVal = tmp;

        return status();
    }
    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(int16_t &oVal)
    {
        if (!good())
        {
            return status();
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 32-bit integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(int32_t &oVal)
    {
        uint32_t tmp;
        read(tmp);
        oVal = tmp;

        return status();
    }
    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(int32_t &oVal)
    {
        if (!good())
        {
            return false;
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Reading and peeking 64-bit integers
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::read(int64_t &oVal)
    {
        uint64_t tmp;
        read(tmp);
        oVal = tmp;

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::peek(int64_t &oVal)
    {
        if (!good())
        {
            return false;
        }

        ByteStreamStateRestorerT<StreamType> s(this);

        return read(oVal);
    }

    /*
     * Skipping
     */

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::skip(ByteCount_t iNumBytes)
    {
        if (!good())
        {
            return status();
        }

        char *tmp = new char[iNumBytes];
        stream_->read(tmp, static_cast<int32_t>(iNumBytes));
        delete [] tmp;

        if (!good())
        {
            return status();
        }

        byteCount_ += iNumBytes;

        return status();
    }

    template<typename StreamType>
    ReturnCode ByteStreamReaderT<StreamType>::seek(ByteCount_t iPosition)
    {
        if (!good())
        {
            return status();
        }

        stream_->seekg(iPosition);

        return status();
    }

#ifdef CMNSTRM_USE_CHECK_SUM
    template<typename StreamType>
    bool ByteStreamReaderT<StreamType>::setHashGenerator(HashGenerator *iGenerator)
    {
        if ((hash_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        hash_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    HashGenerator* ByteStreamReaderT<StreamType>::getHashGenerator(void)
    {
        return hash_;
    }

    template<typename StreamType>
    bool ByteStreamReaderT<StreamType>::setCRCGenerator(CRCGenerator *iGenerator)
    {
        if ((CRC_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        CRC_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    CRCGenerator* ByteStreamReaderT<StreamType>::getCRCGenerator(void)
    {
        return CRC_;
    }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

} //namespace CommonStream

#endif // COMMON_STREAM_SRC_BYTESTREAM_BYTESTREAMREADERT_H_

