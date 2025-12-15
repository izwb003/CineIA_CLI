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

#ifndef COMMON_STREAM_SRC_BITSTREAM_BITSTREAMWRITERT_H_
#define COMMON_STREAM_SRC_BITSTREAM_BITSTREAMWRITERT_H_

#include <exception>
#include <sstream>
#include <istream>
#include <ios>
#include <climits>
#include <cstdio>

#include "commonstream/utils/Namespace.h"
#include "commonstream/bitstream/BitStreamStateT.h"

#ifdef CMNSTRM_USE_CHECK_SUM
#include "commonstream/hash/HashGenerator.h"
#include "commonstream/hash/CRCGenerator.h"
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

namespace CommonStream
{

    template<typename StreamType>
    class BitStreamWriterT : public BitStreamStateT<StreamType>
    {

        using BitStreamStateT<StreamType>::bitBufferFill_;
        using BitStreamStateT<StreamType>::bitBuffer_;
        using BitStreamStateT<StreamType>::bitCount_;
        using BitStreamStateT<StreamType>::error_;
        using BitStreamStateT<StreamType>::bufferLengthInBits_;

    public:

        BitStreamWriterT();

        BitStreamWriterT(const BitStreamWriterT<StreamType> &bs2); ///< copy constructor
        BitStreamWriterT(char *iBuffer, BitCount_t iBufferLength);
        BitStreamWriterT(unsigned char *iBuffer, BitCount_t iBufferLength);
        BitStreamWriterT(std::ofstream &iStream);
        BitStreamWriterT(std::ostream &iStream);

        BitStreamWriterT &operator=(const BitStreamWriterT<StreamType> &bs2);

        ReturnCode Init();
        ReturnCode Init(char *iBuffer, BitCount_t iBufferLength);
        ReturnCode Init(unsigned char *iBuffer, BitCount_t iBufferLength);
        ReturnCode Init(const BitStreamWriterT<StreamType> &bs2);

        ~BitStreamWriterT();

        std::streampos streamPosition() const
        {
            return stream_->tellp();
        }
        
        std::streampos streamPosition(std::streampos sp)
        {
            return stream_->seekp(sp);
        }

        ReturnCode write(bool iVal);

        ReturnCode write(uint8_t iVal, uint8_t iNumBits);
        ReturnCode write(uint16_t iVal, uint8_t iNumBits);
        ReturnCode write(uint32_t iVal, uint8_t iNumBits);
        ReturnCode write(uint64_t iVal, uint8_t iNumBits);

        ReturnCode write(int8_t iVal, uint8_t iNumBits);
        ReturnCode write(int16_t iVal, uint8_t iNumBits);
        ReturnCode write(int32_t iVal, uint8_t iNumBits);
        ReturnCode write(int64_t iVal, uint8_t iNumBits);

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
        ReturnCode writeAligned(const uint8_t *iVal, uint32_t iNumBytes);

        ReturnCode align();
        ReturnCode flush();

        BitCount_t getBitCount(void);

        ReturnCode ShrinkBuffer(BitCount_t new_buffer_size);
        ReturnCode IncrementBitCount(BitCount_t incr);
        ReturnCode skip(BitCount_t bits_to_skip);

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

        BitStreamStateT<StreamType> getState(void) const;
        BitStreamStateT<StreamType> setState(BitStreamStateT<StreamType> state);
        BitStreamStateT<StreamType> reset(void);

        // utilities, debugging
        void dump_buffer_ascii(FILE *fp);

#ifdef CMNSTRM_USE_CHECK_SUM
        bool                setHashGenerator(HashGenerator *iGenerator);
        HashGenerator*      getHashGenerator(void);

        bool                setCRCGenerator(CRCGenerator *iGenerator);
        CRCGenerator*       getCRCGenerator(void);
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

    private:

        ReturnCode write_unchecked(uint32_t iVal, uint8_t iNumBits);

        ReturnCode align_unchecked();
        ReturnCode flush_unchecked();

        StreamType          *stream_;

        static const uint32_t BITMASK[33];

#ifdef CMNSTRM_USE_CHECK_SUM
        CRCGenerator		*CRC_;
        HashGenerator		*hash_;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM
    };

    template<typename StreamType>
    const uint32_t BitStreamWriterT<StreamType>::BITMASK[33] =
    {
        0x00000000, 0x00000001, 0x00000003, 0x00000007,
        0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
        0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
        0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
        0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
        0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
        0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
        0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
        0xffffffff
    };

    template<typename StreamType>
    BitStreamWriterT<StreamType>::BitStreamWriterT()
    {
        Init();
    }

    template<typename StreamType>
    BitStreamWriterT<StreamType>::BitStreamWriterT(char *iBuffer, BitCount_t iBufferLength)
    {
        ReturnCode rc = Init(iBuffer, iBufferLength);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    BitStreamWriterT<StreamType>::BitStreamWriterT(const BitStreamWriterT<StreamType> &bs2)
    {
        ReturnCode rc = Init(bs2);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    BitStreamWriterT<StreamType>::BitStreamWriterT(unsigned char *iBuffer, BitCount_t iBufferLength)
    {
        ReturnCode rc = Init(iBuffer, iBufferLength);
        if ( rc != CMNSTRM_OK )
        {
            // throw something appropriate
        }
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::Init()
    {
        bitBufferFill_ = 0;
        bitBuffer_ = 0;
        bitCount_ = 0;
        bufferLengthInBits_ = 0;
        if ( stream_ )
        {
            delete stream_;
        }

        stream_ = nullptr;

#ifdef CMNSTRM_USE_CHECK_SUM
        hash_ = nullptr;
        CRC_ = nullptr;
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        error_ = CMNSTRM_OK;

        return CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::Init(unsigned char *iBuffer, BitCount_t iBufferLength)
    {
        return Init(reinterpret_cast<char *>(iBuffer), iBufferLength);
    }

    /*
     * Retreiving the number of used bits
     */
    template<typename StreamType>
    BitCount_t BitStreamWriterT<StreamType>::getBitCount(void)
    {
        return bitCount_;
    }

    template<typename StreamType>
    void BitStreamWriterT<StreamType>::clear()
    {
        error_ = CMNSTRM_OK;
        stream_->clear();
    }

    /// \brief Shrink the buffer to a given number of bytes
    /// \param new_buffer_size  new size in bytes
    /// \return OK/error code
    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::ShrinkBuffer(BitCount_t new_buffer_size)
    {
        BitCount_t tentative_bit_size = CHAR_BIT*new_buffer_size;

        // check that we are not enlarging the buffer, and that we are not cutting
        // already processed bits
        if ( tentative_bit_size > bufferLengthInBits_ || tentative_bit_size < bitCount_ )
        {
            return error_ = CMNSTRM_IO_FAIL;
        }
        
        bufferLengthInBits_ = tentative_bit_size;
        return CMNSTRM_OK;
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::IncrementBitCount(BitCount_t incr)
    {
        BitCount_t tentative = bitCount_ + incr;
        if ( tentative > bufferLengthInBits_ )
        {
            bitCount_ = bufferLengthInBits_;
        }
        else
        {
            bitCount_ = tentative;
        }
        
        return CMNSTRM_OK;
    }

    /**
     *
     */

    template<typename StreamType>
    bool  BitStreamWriterT<StreamType>::good()
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
    bool  BitStreamWriterT<StreamType>::eof()
    {
        if (stream_->eof())
            error_ = CMNSTRM_IO_EOF;
        return error_ == CMNSTRM_IO_EOF;
    }

    /**
     *
     */

    template<typename StreamType>
    bool  BitStreamWriterT<StreamType>::fail()
    {
        if (stream_->fail())
            error_ = CMNSTRM_IO_FAIL;
        return error_ == CMNSTRM_IO_FAIL;
    }

    /**
     *  Retrieving the error code from the byte stream state
     */

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::status(void)
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
    ReturnCode BitStreamWriterT<StreamType>::skip(BitCount_t iNumbits)
    {
        if (!good())
        {
            return status();
        }

        uint8_t tmp = 0;

        BitCount_t bytesLeftToWrite = iNumbits / 8;

        while (bytesLeftToWrite)
        {
            write(tmp, 8);
            --bytesLeftToWrite;
        }

        BitCount_t bitsLeftToWrite = iNumbits % 8;
        if (bitsLeftToWrite)
        {
            write(tmp, bitsLeftToWrite);
        }

        return status();
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    BitStreamStateT<StreamType> BitStreamWriterT<StreamType>::getState(void) const
    {
        BitStreamStateT<StreamType> state;

        state.streamState(stream_->rdstate());
        state.streamPosition(stream_->tellp());
        state.bitBuffer(bitBuffer_);
        state.bitBufferFill(bitBufferFill_);
        state.bitCount(bitCount_);
        state.error(error_);
        state.bufferLengthInBits(bufferLengthInBits_);
        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    BitStreamStateT<StreamType> BitStreamWriterT<StreamType>::setState(BitStreamStateT<StreamType> state)
    {
        stream_->clear();
        stream_->setstate(state.streamState());
        stream_->seekp(state.streamPosition());
        bitBuffer_ = state.bitBuffer();
        bitBufferFill_ = state.bitBufferFill();
        bitCount_ = state.bitCount();
        error_ = state.error();
        bufferLengthInBits_ = state.bufferLengthInBits();
        
        return state;
    }

    // Note that this functions may cause stream failures

    template<typename StreamType>
    BitStreamStateT<StreamType> BitStreamWriterT<StreamType>::reset(void)
    {
        stream_->clear();
        stream_->seekp(0);

        bitBuffer_ = 0;
        bitBufferFill_ = 0;
        bitCount_ = 0;

        error_ = CMNSTRM_OK;

        return getState();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint8_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if ( iNumBits == 0 )
        {
            return status();    // do nothing
        }

        if ( iNumBits > 8 )
        {
            return CMNSTRM_PARAMS_BAD;
        }

        this->write_unchecked((uint32_t)iVal, iNumBits);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint16_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }

        if (iNumBits > 16)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        this->write_unchecked((uint32_t)iVal, iNumBits);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint32_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 32)
        {
            return CMNSTRM_PARAMS_BAD;
        }
        return write_unchecked(iVal, iNumBits);
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write_unchecked(uint32_t iVal, uint8_t iNumBits)
    {
        uint32_t bits = (iVal & BITMASK[iNumBits]) << (32 - iNumBits);

        int32_t pBitsLeft = iNumBits;

        BitCount_t totalBitCount = bitCount_ + iNumBits;

        // We will hit a buffer overflow
        // Make sure we flush the last bits followed by 0
        // then return CMNSTRM_IO_EOF
        //
        if (totalBitCount > bufferLengthInBits_)
        {
            error_ = CMNSTRM_IO_EOF;
            // flush bits that were guaranteed good and accounted for
            flush_unchecked();
            return CMNSTRM_IO_EOF;
        }

        while (pBitsLeft > 0)
        {
            // Take the first 8 - bitBufferFill_ bits
            // and add them to the bitBuffer_ for writing
            //
            // If there are no extra bits to write,
            // we use all 8 bits
            //
            bitBuffer_ |= bits >> (24 + bitBufferFill_);

            if (pBitsLeft + bitBufferFill_ >= 8)
            {
                // Write a single byte/8-bits
                //
                stream_->write((char*) &bitBuffer_, 1);

                // Check the status and only update the various pieces of data
                // if the write was successful
                //
                if (!good())
                {
                    return status();
                }

#ifdef CMNSTRM_USE_CHECK_SUM
                if (hash_ && hash_->useHash())
                {
                    hash_->accumulate(&bitBuffer_, 1);
                }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

                // Compute the number of bits left to write
                //
                pBitsLeft -= 8 - bitBufferFill_;

                // Shift over to the next set of bits we need to write
                //
                bits = bits << (8 - bitBufferFill_);

                // Clear our bit buffer and fill
                //
                bitBuffer_ = 0;
                bitBufferFill_ = 0;
            }
            else
            {
                // Since we do not have a full 8 bits to write to the file,
                // save off the number of bits left to write.
                // On the next call we will write these bits first
                //
                bitBufferFill_ = bitBufferFill_ + pBitsLeft;
                pBitsLeft = 0;
            }
        }

#ifdef CMNSTRM_USE_CHECK_SUM
        if (CRC_ && CRC_->useCRC())
        {
            CRC_->accumulate(iVal, iNumBits);
        }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

        ReturnCode rc = status();
        if ( rc == CMNSTRM_OK )
        {
            bitCount_ = totalBitCount;
        }

        return rc;
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint64_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 64)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        BitCount_t totalBitCount = bitCount_ + iNumBits;

        // We will hit a buffer overflow
        // Make sure we flush the last bits followed by 0
        // then return CMNSTRM_IO_EOF
        //
        if (totalBitCount > bufferLengthInBits_)
        {
            error_ = CMNSTRM_IO_EOF;
            // flush bits that were guaranteed good and accounted for
            flush_unchecked();
            return CMNSTRM_IO_EOF;
        }

        uint32_t tmp = 0;
        ReturnCode rc;
        if ( iNumBits > 32 )
        {
            tmp = static_cast<uint32_t>(iVal >> 32);
            rc = write_unchecked(tmp, iNumBits-32);
            if ( rc )
            {
                return rc;
            }
            iNumBits=32;
        }
        
        tmp = static_cast<uint32_t>(iVal);
        rc = write_unchecked(tmp, iNumBits);
        
        if ( rc )
        {
            return rc;
        }
        
        bitCount_ = totalBitCount;
        
        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int8_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 8)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        this->write((uint32_t)iVal, iNumBits);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int16_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 16)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        this->write((uint32_t)iVal, iNumBits);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int32_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 32)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        this->write((uint32_t)iVal, iNumBits);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int64_t iVal, uint8_t iNumBits)
    {
        if (!good())
        {
            return status();
        }

        if (iNumBits == 0)
        {
            return status();
        }
        if (iNumBits > 64)
        {
            return CMNSTRM_PARAMS_BAD;
        }

        uint8_t bitsWritten = 0;
        uint32_t tmp = 0;
        if (iNumBits > 32)
        {
            bitsWritten = 32;
            uint64_t tmp64 = iVal >> 32;

            tmp = static_cast<uint32_t>(tmp64);
            this->write((uint32_t)tmp, 32);

            if (!good())
            {
                return status();
            }
        }

        tmp = static_cast<uint32_t>(iVal);
        this->write((uint32_t)tmp, iNumBits - bitsWritten);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(bool iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 1);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint8_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 8);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint16_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 16);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint32_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write(iVal, 32);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(uint64_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write(iVal, 64);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(char iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 8);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int8_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 8);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int16_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write((uint32_t)iVal, 16);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int32_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write(iVal, 32);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(int64_t iVal)
    {
        if (!good())
        {
            return status();
        }

        this->write(iVal, 64);

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::write(const uint8_t *iVal, uint32_t iNumBytes)
    {
        if (!good())
        {
            return status();
        }

        uint8_t *end = (uint8_t *) iVal + iNumBytes;
        uint8_t *buf = (uint8_t *) iVal;

        while (buf != end)
        {
            write(*(buf++), 8);

            if (!good())
            {
                return status();
            }
        }

        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::writeAligned(const uint8_t *iVal, uint32_t iNumBytes)
    {
        if (!good())
        {
            return status();
        }

        align();

        return this->write(iVal, iNumBytes);
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::align()
    {
        if (!good())
        {
            return status();
        }
        
        return align_unchecked();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::align_unchecked()
    {
        if (bitBufferFill_ != 0)
        {
            // Write a single byte/8-bits
            //
            stream_->write((char*) &bitBuffer_, 1);

#ifdef CMNSTRM_USE_CHECK_SUM
            if (hash_ && hash_->useHash())
            {
                hash_->accumulate(&bitBuffer_, 1);
            }

            if (CRC_ && CRC_->useCRC())
            {
                CRC_->accumulate(static_cast<uint32_t>(bitBuffer_), 8);
            }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

            // Account for the additional 0 fill bits of the align
            //
            bitCount_ += (8 - bitBufferFill_);
            bitBufferFill_ = 0;
            bitBuffer_ = 0;
        }
        return status();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::flush_unchecked()
    {
        return align_unchecked();
    }

    template<typename StreamType>
    ReturnCode  BitStreamWriterT<StreamType>::flush()
    {
        return align();
    }

#ifdef CMNSTRM_USE_CHECK_SUM
    template<typename StreamType>
    bool BitStreamWriterT<StreamType>::setHashGenerator(HashGenerator *iGenerator)
    {
        if ((hash_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        hash_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    HashGenerator* BitStreamWriterT<StreamType>::getHashGenerator(void)
    {
        return hash_;
    }

    template<typename StreamType>
    bool BitStreamWriterT<StreamType>::setCRCGenerator(CRCGenerator *iGenerator)
    {
        if ((CRC_ != nullptr) && (iGenerator != nullptr))
        {
            return false;
        }

        CRC_ = iGenerator;
        return true;
    }

    template<typename StreamType>
    CRCGenerator* BitStreamWriterT<StreamType>::getCRCGenerator(void)
    {
        return CRC_;
    }
#endif // #ifdef CMNSTRM_USE_CHECK_SUM

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_BITSTREAM_BITSTREAMWRITERT_H_
