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

#include "DLCCodec.h"
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace dlc
{
    AudioData::AudioData() : shiftBits_(0), sampleRate_(eSampleRate_48000) {}

    AudioData::~AudioData()
    {
        for (std::vector<ResidualSubBlock*>::iterator it = this->subBlocks48_.begin();
                it < this->subBlocks48_.end();
                it++)
        {
            delete *it;
        }

        for (std::vector<ResidualSubBlock*>::iterator it = this->subBlocks96_.begin();
                it < this->subBlocks96_.end();
                it++)
        {
            delete *it;
        }
    }

    void AudioData::setShiftBits(uint5_t iNum)
    {
        if (iNum > dlc::UINT5_MAX)
        {
            throw std::invalid_argument("Shift bits out of range.");
        }

        this->shiftBits_ = iNum;
    }

    uint5_t AudioData::getShiftBits() const
    {
        /* range is guaranteed by setShiftBits() */

        return static_cast<uint5_t>(this->shiftBits_);
    }

    void AudioData::setSampleRate(SampleRate iSampleRate)
    {
        this->sampleRate_ = iSampleRate;
    }

    SampleRate AudioData::getSampleRate() const
    {
        return this->sampleRate_;
    }

    uint2_t AudioData::getNumPredRegions48() const
    {
        return static_cast<uint2_t>(this->predRegions48_.size());
    }

    void AudioData::setNumPredRegions48(uint2_t count)
    {
        if (count > dlc::UINT2_MAX)
        {
            throw std::out_of_range("Maximum number of 3 prediction regions supported.");
        }

        this->predRegions48_.resize(count);
    }

    const PredRegion & AudioData::getPredRegion48(uint2_t iRegionIndex) const
    {
        return this->predRegions48_.at(iRegionIndex);
    }

    PredRegion & AudioData::getPredRegion48(uint2_t iRegionIndex)
    {
        return this->predRegions48_.at(iRegionIndex);
    }

    uint2_t AudioData::getNumPredRegions96() const
    {
        return static_cast<uint2_t>(this->predRegions96_.size());
    }

    void AudioData::setNumPredRegions96(uint2_t iCount)
    {
        if (iCount > dlc::UINT2_MAX)
        {
            throw std::out_of_range("Maximum number of 3 prediction regions supported.");
        }

        this->predRegions96_.resize(iCount);
    }

    const PredRegion & AudioData::getPredRegion96(uint2_t iRegionIndex) const
    {
        return this->predRegions96_.at(iRegionIndex);
    }

    PredRegion & AudioData::getPredRegion96(uint2_t iRegionIndex)
    {
        return this->predRegions96_.at(iRegionIndex);
    }

    uint8_t AudioData::getNumDLCSubBlocks() const
    {
        /* this cast is guaranteed by the range check in setNumDLCSubBlocks() */

        return static_cast<uint8_t>(this->subBlocks48_.size());
    }

    void AudioData::setNumDLCSubBlocks(uint8_t iCount)
    {
        if (iCount > std::numeric_limits<uint8_t>::max())
        {
            throw std::out_of_range("Number of sub blocks must be in the range [0, 255].");
        }

        std::vector<ResidualSubBlock*>::iterator it48 = this->subBlocks48_.end();
        std::vector<ResidualSubBlock*>::iterator it96 = this->subBlocks96_.end();

        for (uint8_t i = static_cast<uint8_t>(this->subBlocks48_.size()); i > iCount; i--)
        {
            delete *(--it48);
            delete *(--it96);
        }

        /* ST 2098-2 specifies that the number of 48 kHz and 96 kHz subblocks is identical */

        this->subBlocks48_.resize(iCount);
        this->subBlocks96_.resize(iCount);
    }

    void AudioData::initDLCSubBlock48(uint8_t iBlockIndex, CodeType iCodeType, uint32_t iResidualCount)
    {
        if (iResidualCount > std::numeric_limits<uint32_t>::max())
        {
            throw std::out_of_range("Sub block size must be in the range [0, 2^32-1].");
        }

        if (this->subBlocks48_.at(iBlockIndex) != NULL)
        {
            delete this->subBlocks48_.at(iBlockIndex);
        }

        switch (iCodeType)
        {
        case eCodeType_RICE_CODE_TYPE:
            this->subBlocks48_.at(iBlockIndex) = new RiceResidualsSubBlock(iResidualCount);
            break;
        case eCodeType_PCM_CODE_TYPE:
            this->subBlocks48_.at(iBlockIndex) = new PCMResidualsSubBlock(iResidualCount);
            break;
        }
    }

    ResidualSubBlock * AudioData::getDLCSubBlock48(uint8_t iBlockIndex)
    {
        return this->subBlocks48_.at(iBlockIndex);
    }

    const ResidualSubBlock * AudioData::getDLCSubBlock48(uint8_t iBlockIndex) const
    {
        return this->subBlocks48_.at(iBlockIndex);
    }

    void AudioData::initDLCSubBlock96(uint8_t iBlockIndex, CodeType iCodeType, uint32_t iResidualCount)
    {
        if (this->subBlocks96_.at(iBlockIndex) != NULL)
        {
            delete this->subBlocks96_.at(iBlockIndex);
        }

        switch (iCodeType)
        {
        case eCodeType_RICE_CODE_TYPE:
            this->subBlocks96_.at(iBlockIndex) = new RiceResidualsSubBlock(iResidualCount);
            break;
        case eCodeType_PCM_CODE_TYPE:
            this->subBlocks96_.at(iBlockIndex) = new PCMResidualsSubBlock(iResidualCount);
            break;
        }
    }

    ResidualSubBlock * AudioData::getDLCSubBlock96(uint8_t iBlockIndex)
    {
        return this->subBlocks96_.at(iBlockIndex);
    }

    const ResidualSubBlock * AudioData::getDLCSubBlock96(uint8_t iBlockIndex) const
    {
        return this->subBlocks96_.at(iBlockIndex);
    }

    uint32_t AudioData::getSampleCount48() const
    {
        uint32_t frameSize = 0;

        for (uint8_t i = 0; i < this->getNumDLCSubBlocks(); i++)
        {

            const ResidualSubBlock* sb48 = getDLCSubBlock48(i);

            if (sb48 == NULL)
            {
                throw std::runtime_error("At least one 48 kHz DLC Sub Block is undefined");
            }

            frameSize += sb48->getSize();

        }

        return frameSize;
    }


    uint32_t AudioData::getSampleCount96() const
    {
        if (this->getSampleRate() != eSampleRate_96000)
        {
            throw std::runtime_error("Sample rate of the Audio Data element is not 96 kHz.");
        }

        uint32_t frameSize = 0;

        for (uint8_t i = 0; i < this->getNumDLCSubBlocks(); i++)
        {

            const ResidualSubBlock* sb96 = getDLCSubBlock96(i);

            if (sb96 == NULL)
            {
                throw std::runtime_error("At least one 96 kHz DLC Sub Block is undefined");
            }

            frameSize += sb96->getSize();
        }

        return frameSize;
    }


    RiceResidualsSubBlock::RiceResidualsSubBlock(uint32_t iSize) : riceRemBits_(0), residuals_(iSize)
    {
        if (iSize > std::numeric_limits<uint32_t>::max())
        {
            throw std::out_of_range("Max sub block length exceeded.");
        }
        if (iSize == 0)
        {
            throw std::out_of_range("Sub block cannot have zero length.");
        }
    }

    RiceResidualsSubBlock::~RiceResidualsSubBlock()
    {
    }

    uint32_t RiceResidualsSubBlock::getSize() const
    {
        if (residuals_.size() > std::numeric_limits<uint32_t>::max())
        {
            throw std::runtime_error("DLC Sub Block Size is too large.");
        }

        return static_cast<uint32_t>(residuals_.size());
    }

    CodeType RiceResidualsSubBlock::getCodeType() const
    {
        return eCodeType_RICE_CODE_TYPE;
    }

    uint5_t RiceResidualsSubBlock::getRiceRemBits() const
    {
        return this->riceRemBits_;
    }

    void RiceResidualsSubBlock::setRiceRemBits(uint5_t iNum)
    {
        if (iNum > UINT5_MAX)
        {
            throw std::invalid_argument("RiceRemBits must be 31 or smaller.");
        }

        this->riceRemBits_ = iNum;
    }

    RiceResidual *RiceResidualsSubBlock::getResiduals()
    {
        return &this->residuals_[0];
    }

    const RiceResidual * RiceResidualsSubBlock::getResiduals() const
    {
        return &this->residuals_[0];
    }

    PCMResidualsSubBlock::PCMResidualsSubBlock(uint32_t iSize) : bitDepth_(0), residuals_(iSize)
    {
        if (iSize > std::numeric_limits<uint32_t>::max())
        {
            throw std::out_of_range("Max sub block length exceeded.");
        }
        if (iSize == 0)
        {
            throw std::out_of_range("Sub block cannot have zero length.");
        }
    }

    PCMResidualsSubBlock::~PCMResidualsSubBlock()
    {
    }

    uint32_t PCMResidualsSubBlock::getSize() const
    {
        if (residuals_.size() > std::numeric_limits<uint32_t>::max())
        {
            throw std::runtime_error("DLC Sub Block Size is too large.");
        }

        return (uint32_t)residuals_.size();
    }

    CodeType PCMResidualsSubBlock::getCodeType() const
    {
        return eCodeType_PCM_CODE_TYPE;
    }

    uint5_t PCMResidualsSubBlock::getBitDepth() const
    {
        return this->bitDepth_;
    }

    void PCMResidualsSubBlock::setBitDepth(uint5_t iNum)
    {
        if (iNum > UINT5_MAX)
        {
            throw std::out_of_range("Bit depth out of range.");
        }

        this->bitDepth_ = iNum;
    }

    int32_t * PCMResidualsSubBlock::getResiduals()
    {
        return &this->residuals_[0];
    }

    const int32_t * PCMResidualsSubBlock::getResiduals() const
    {
        return &this->residuals_[0];
    }

    RiceResidual::RiceResidual() : quotient_(0), remainder_(0), sign_(1)
    {
    }

    RiceResidual::~RiceResidual()
    {
    }

    int32_t RiceResidual::getQuotient() const
    {
        return this->quotient_;
    }

    void RiceResidual::setQuotient(int32_t iQuotient)
    {
        this->quotient_ = iQuotient;
    }

    int32_t RiceResidual::getRemainder() const
    {
        return this->remainder_;
    }

    void RiceResidual::setRemainder(int32_t iRemainder)
    {
        this->remainder_ = iRemainder;
    }

    char RiceResidual::getSign() const
    {
        return this->sign_;
    }

    void RiceResidual::setSign(char iSign)
    {
        if (iSign == 0)
        {
            throw std::out_of_range("The sign of the remainder must be set by a striclty positive or negative integer.");
        }

        this->sign_ = iSign > 0 ? 1 : -1;
    }

    PredRegion::PredRegion() : regionLength_(0), order_(0)
    {
        std::fill(kCoeff_, kCoeff_ + 32, (dlc::uint10_t) 0);
    }

    uint4_t PredRegion::getRegionLength() const
    {
        return this->regionLength_;
    }

    void PredRegion::setRegionLength(uint4_t iNum)
    {
        if (iNum > UINT4_MAX)
        {
            throw std::out_of_range("Region Length is out of range");
        }

        this->regionLength_ = iNum;
    }

    uint5_t PredRegion::getOrder() const
    {
        return this->order_;
    }

    void PredRegion::setOrder(uint5_t iNum)
    {
        if (iNum > UINT5_MAX)
        {
            throw std::out_of_range("Order out of range.");
        }

        this->order_ = iNum;
    }

    const uint10_t(&PredRegion::getKCoeff() const)[32]
    {
        return this->kCoeff_;
    }

    void PredRegion::setKCoeff(const uint10_t iCoeffs[32])
    {
        for (size_t i = 0; i < sizeof(this->kCoeff_)/sizeof(this->kCoeff_[0]); i++)
        {
            if (this->kCoeff_[i] > 0x3FF)
            {
                throw std::out_of_range("Coefficient out of range.");
            }
        }

        std::copy(iCoeffs, iCoeffs + sizeof(this->kCoeff_) / sizeof(this->kCoeff_[0]), this->kCoeff_);
    }

} // namespace dlc
