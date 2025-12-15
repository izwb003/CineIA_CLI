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

#ifndef DLC_AUDIODATA_H
#define DLC_AUDIODATA_H

#include <stdint.h>
#include <vector>

/**
* Defines the AudioData class and associated artefacts.
*
* The AudioData class represents the contents of a single IAB AudioDataDLC element, as defined in ST 2098-2.
*
* AudioData instances are returned by DLC encoders (dlc::Encoder), and provided as input to DLC decoders (dlc::Decoder).
*
* The AudioData base class supports the full capabilities of IAB AudioDataDLC element, and can be subclassed if an optimized version tailored
* to constrained forms of the IAB AudioDataDLC element, e.g. IAB AudioDataDLC elements that never use predictors, or addditional functionality are needed.
*
* The base class does not provide serialization to and from the bitstream format specified in ST 2098-2, and such functionality must be implemented
* separately, e.g. by subclassing or helper functions.
*
* <em>NOTE: This documenation assumes the reader has access to SMPTE ST 2098-2, and does not duplicate information therein. In particular,
* maximum and minimum permissible values are not repeated here unless undefined in SMPTE ST 2098-2.</em>
*
* @file
*/

/**
* Namespace for all exported symbols of the DLC library
*/
namespace dlc
{

	/**
	 * Represents a 2-bit unsigned integer
	 */
	typedef uint8_t uint2_t;

	/**
	* Maximum value of a uint2_t value
	*/
	const int32_t UINT2_MAX = (1 << 2) - 1;

	/**
	* Represents a 5-bit unsigned integer
	*/
	typedef uint8_t uint5_t;

	/**
	* Maximum value of a uint5_t value
	*/
	const int32_t UINT5_MAX = (1 << 5) - 1;

	/**
	* Represents a 4-bit unsigned integer
	*/
	typedef uint8_t uint4_t;

	/**
	* Maximum value of a uint4_t value
	*/
	const int32_t UINT4_MAX = (1 << 4) - 1;

	/**
	* Represents a 10-bit unsigned integer
	*/
	typedef uint16_t uint10_t;

	/**
	* Maximum value of a uint10_t value
	*/
	const int32_t UINT10_MAX = (1 << 10) - 1;

	/**
	* Values of the IAB CodeType field
	*/
	enum CodeType
    {
		eCodeType_PCM_CODE_TYPE = 0, ///< PCM Residual
		eCodeType_RICE_CODE_TYPE = 1 ///< Rice/Golomb Residual
	};

	/**
	* Values of the IAB DLCSampleRate field.
	* NOTE: FS stands for 'frequency sampling'.
	*/
	enum SampleRate
    {
		eSampleRate_48000 = 0, ///< 48 kHz sampling rate
		eSampleRate_96000 = 1  ///< 96 kHz sampling rate
	};

	/**
	* Values of the IAB Frame Rate for which a frame is represented 
	* by the DLCAudioData element.
	*/
	enum FrameRate
	{
		eFrameRate_24 = 0, ///< 24 frames per second
		eFrameRate_25 = 1, ///< 25 frames per second
		eFrameRate_30 = 2, ///< 30 frames per second
		eFrameRate_48 = 3, ///< 48 frames per second
		eFrameRate_50 = 4, ///< 50 frames per second
		eFrameRate_60 = 5, ///< 60 frames per second
		eFrameRate_96 = 6, ///< 96 frames per second
		eFrameRate_100 = 7, ///< 100 frames per second
		eFrameRate_120 = 8, ///< 120 frames per second
	};

	/**
	* Represents an IAB Predictor Region (either 48 kHz or 96 kHz)
	*/
	class PredRegion
    {
	public:
		PredRegion();

		/**
		* Retrieves the IAB RegionLength48 or RegionLength96 field associated with the predictor region.
		*
		* @return Value of the RegionLength48 or RegionLength96 field
		*/
		uint4_t getRegionLength() const;

		/**
		* Sets the IAB RegionLength48 or RegionLength96 field associated with the predictor region.
		*
		* @param[in] iLength Value of the RegionLength48 or RegionLength96 field.
		* @throws std::out_of_range if num is out of range
		*/
		void setRegionLength(uint4_t iLength);

		/**
		* Retrieves the IAB Order48 or Order96 field associated with the predictor region.
		*
		* @return Value of the Order48 or Order96 field
		*/
		uint5_t getOrder() const;

		/**
		* Sets the IAB Order48 or Order96 field associated with the predictor region.
		*
		* @param[in] iOrder Value of the Order48 or Order96 field.
		* @throws std::out_of_range if num is out of range
		*/
		void setOrder(uint5_t iOrder);

		/**
		* Retrieve the IAB KCoeff48 or KCoeff96 values associated with the predictor region.
		*
		* @return Reference to an array of 32 values. The values should not be modified. The first coefficient is not used.
		*/
		const uint10_t(&getKCoeff() const)[32];

		/**
		* Sets the IAB KCoeff48 or KCoeff96 values associated with the predictor region.
		*
		* @param[in] iCoeffs Array of 32 values. The first coefficient is not used.
		* @throws std::out_of_range if any coefficient is out of range
		*/
		void setKCoeff(const uint10_t iCoeffs[32]);

	private:
		uint4_t regionLength_;
		uint5_t order_;
		uint10_t kCoeff_[32];
	};

	/**
	* Represents an IAB Residual Sub Block (either 48 kHz or 96 kHz)
	*/
	class ResidualSubBlock
    {
	public:

		/**
		* Retrieves the IAB CodeType associated with the sub block. The information can be used to cast
		* the ResidualSubBlock to a concrete subclass.
		*
		* @return IAB CodeType associated with the sub block.
		*/
		virtual CodeType getCodeType() const = 0;

		/**
		* Retrieves the IAB DLCSubBlockSize48 or DLCSubBlockSiz96 associated with the sub block. In contrast with ST 2098-2
		* the library allows each sub block to have a different size for future extensions.
		*
		* @return Number of residuals associated with the sub block.
		*/
		virtual uint32_t getSize() const = 0;

		virtual ~ResidualSubBlock() {}
	};

	/**
	* Represents a single IAB Rice/Golomb Residual
	*/
	class RiceResidual
    {
	public:

		RiceResidual();
		~RiceResidual();

		/**
		* Retrieves the IAB quotient of the residual.
		*
		* @return Quotient of the residual in binary form.
		*/
		int32_t getQuotient() const;

		/**
		* Sets the IAB quotient of the residual.
		*
		* @param[in] iQuotient Quotient in binary form.
		*/
		void setQuotient(int32_t iQuotient);

		/**
		* Retrieves the IAB Residual48 or Residual96 of the residual.
		*
		* @return Residual48 or Residual96 in binary form.
		*/
		int32_t getRemainder() const;

		/**
		* Sets the IAB Residual48 or Residual96 of the residual.
		*
		* @param[in] iRemainder Residual48 or Residual96 in binary form.
		*/
		void setRemainder(int32_t iRemainder);

		/**
		* Retrieves the sign of the residual.
		*
		* @return -1 if remainder is negative, or +1 if the residual is positive.
		*/
		char getSign() const;

		/**
		* Sets the sign of the residual to the sign of the sign parameter.
		*
		* @param[in] iSign Strictly positive or negative integer.
		* @throws std::out_of_range if sign is zero
		*/
		void setSign(char iSign);

	private:
		int32_t quotient_;
		int32_t remainder_;
		char sign_;
	};

	/**
	* Represents a sub block consisting of IAB Rice/Golomb residuals
	* The size of the subblock (in samples) is immutable.
	*/
	class RiceResidualsSubBlock : public ResidualSubBlock
    {
	public:

		/**
		* @param[in] iSize Strictly positive size of the subblock.
		* @throws std::out_of_range if size is zero
		*/
		RiceResidualsSubBlock(uint32_t iSize);

		~RiceResidualsSubBlock();

		uint32_t getSize() const;

		CodeType getCodeType() const;

		/**
		* Retrieves the IAB RiceRemBits field
		*
		* @return IAB RiceRemBits field value.
		*/
		uint5_t getRiceRemBits() const;

		/**
		* Sets the IAB RiceRemBits
		*
		* @param[in] iNum IAB RiceRemBits value.
		* @throws std::out_of_range if num is out of range
		*/
		void setRiceRemBits(uint5_t iNum);

		/**
		* Retrieves the residuals associated with the sub block, which can be modified in-place.
		*
		* @return Pointer to an array of residual values of size getSize().
		*/
		RiceResidual* getResiduals();

		/**
		* Retrieves a read-only array of residuals associated with the sub block.
		*
		* @return Pointer to a constant array of residual values of size getSize().
		*/
		const RiceResidual * getResiduals() const;

	private:

		uint5_t riceRemBits_;
		std::vector<RiceResidual> residuals_;
	};

	/**
	* Represents a sub block consisting of IAB PCM residuals
	* The size of the subblock (in samples) is immutable.
	*/
	class PCMResidualsSubBlock : public ResidualSubBlock
    {
	public:

		/**
		* @param[in] iSize Strictly positive size of the subblock.
		* @throws std::out_of_range if size is zero
		*/
		PCMResidualsSubBlock(uint32_t iSize);
		~PCMResidualsSubBlock();

		uint32_t getSize() const;

		CodeType getCodeType() const;

		/**
		* Retrieves the IAB BitDepth field
		*
		* @return IAB BitDepth field value.
		*/
		uint5_t getBitDepth() const;

		/**
		* Sets the IAB BitDepth
		*
		* @param[in] iNum IAB BitDepth value.
		* @throws std::out_of_range if num is out of range
		*/
		void setBitDepth(uint5_t iNum);

		/**
		* Retrieves the residuals associated with the sub block, which can be modified in-place.
		*
		* @return Pointer to an array of residual values of size getSize().
		*/
		int32_t * getResiduals();

		/**
		* Retrieves a read-only array of residuals associated with the sub block.
		*
		* @return Pointer to a constant array of residual values of size getSize().
		*/
		const int32_t * getResiduals() const;

	private:

		uint5_t bitDepth_;
		std::vector<int32_t> residuals_;
	};

	/**
	* Represents the contents of a single IAB AudioDataDLC element.
	* Serialization to an actual IAB AudioDataDLC element (or other representation) is not included.
	* This implementation supports all variations of IAB AudioDataDLC element permitted in ST 2098-2, 
	* and subclasses tailored to specific use cases can be defined.
	*/
	class AudioData
    {
	public:

		AudioData();
		virtual ~AudioData();

		/****************
		* ShiftBits
		*
		*/

		/**
		* Sets the IAB ShiftBits field
		*
		* @param[in] iNum IAB ShiftBits value.
		* @throws std::out_of_range if num is out of range
		*/
		virtual void setShiftBits(uint5_t iNum);

		/**
		* Retrieves the IAB BitDepth field
		*
		* @return IAB BitDepth field value.
		*/
		virtual uint5_t getShiftBits() const;

		/****************
		* DLCSampleRate
		*
		*/

		/**
		* Sets the IAB DLCSampleRate field
		*
		* @param[in] iSampleRate IAB DLCSampleRate value.
		*/
		virtual void setSampleRate(SampleRate iSampleRate);

		/**
		* Retrieves the IAB DLCSampleRate field
		*
		* @return IAB DLCSampleRate field value.
		*/
		virtual SampleRate getSampleRate() const;


		/****************
		* 48 kHz prediction regions
		*
		*/

		/**
		* Retrieves the IAB NumPredRegions48 field
		*
		* @return IAB NumPredRegions48 field value.
		*/
		virtual uint2_t getNumPredRegions48() const;

		/**
		* Sets the IAB NumPredRegions48 field. The contents of any existing
		* region whose index is lower than or equal to count are preserved.
		*
		* @param[in] iCount IAB NumPredRegions48 value.
		* @throws std::out_of_range if count is out of range
		*/
		virtual void setNumPredRegions48(uint2_t iCount);

		/**
		* Retrieves a read-only 48 kHz Predictor Region
		*
		* @param iRegionIndex index of the region in the range [0, getNumPredRegions48() )
		* @return Reference to a Predictor Region.
		* @throws std::out_of_range if regionIndex is out of range
		*/
		virtual const PredRegion& getPredRegion48(uint2_t iRegionIndex) const;

		/**
		* Retrieves a 48 kHz Predictor Region, which can be modified in place
		*
		* @param[in] iRegionIndex index of the region in the range [0, getNumPredRegions48() )
		* @return Reference to a Predictor Region.
		* @throws std::out_of_range if regionIndex is out-of-range
		*/
		virtual PredRegion& getPredRegion48(uint2_t iRegionIndex);

		/****************
		* 96 kHz prediction regions
		*
		*/

		/**
		* Retrieves the IAB NumPredRegions96 field
		*
		* @return IAB NumPredRegions96 field value.
		*/
		virtual uint2_t getNumPredRegions96() const;

		/**
		* Sets the IAB NumPredRegions96 field. The contents of any existing
		* region whose index is lower than or equal to count are preserved.
		*
		* @param[in] iCount IAB NumPredRegions96 value.
		* @throws std::out_of_range if count is out of range
		*/
		virtual void setNumPredRegions96(uint2_t iCount);

		/**
		* Retrieves a 96 kHz Predictor Region, which can be modified in place
		*
		* @param[in] iRegionIndex index of the region in the range [0, getNumPredRegions96() )
		* @return Reference to a Predictor Region.
		* @throws std::out_of_range if regionIndex is out of range 
		*/
		virtual PredRegion& getPredRegion96(uint2_t iRegionIndex);

		/**
		* Retrieves a read-only 96 kHz Predictor Region
		*
		* @param[in] iRegionIndex index of the region in the range [0, getNumPredRegions96() )
		* @return Reference to a Predictor Region.
		* @throws std::out_of_range if iRegionIndex is out of range
		*/
		virtual const PredRegion& getPredRegion96(uint2_t iRegionIndex) const;

		/****************
		* Sub Block Count
		*
		*/

		/**
		* Retrieves the number of sub blocks (== IAB NumDLCSubBlocks48 == IAB NumDLCSubBlocks96)
		*
		* @return Number of sub blocks
		*/
		virtual uint8_t getNumDLCSubBlocks() const;

		/**
		* Sets the number of sub blocks (== IAB NumDLCSubBlocks48 == IAB NumDLCSubBlocks96).
		* The contents of any existing sub blocks whose index is lower than or equal to count are preserved.
		*
		* @param[in] iCount Number of 48 kHz and 96 kHz sub blocks
		* @throws std::out_of_range if regionIndex is outside [0, 255]
		*/
		virtual void setNumDLCSubBlocks(uint8_t iCount);


		/****************
		* 48 kHz sub blocks
		*
		*/

		/**
		* Initializes a 48 kHz sub block, overwriting any existing contents, if any.
		*
		* @param[in] iBlockIndex Index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @param[in] iCodeType CodeType of the sub block
		* @param[in] iResidualCount Size (in residuals) of the sub block
		* @throws std::out_of_range if blockIndex is out of range
		*/
		virtual void initDLCSubBlock48(uint8_t iBlockIndex, CodeType iCodeType, uint32_t iResidualCount);

		/**
		* Retrieves a 48 kHz sub block, which can be modified in place
		*
		* @param[in] iBlockIndex index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @return Pointer to a sub block, or NULL is the sub block has not been initialized using initDLCSubBlock48()
		* @throws std::out_of_range if blockIndex is out of range
		*/
		virtual ResidualSubBlock* getDLCSubBlock48(uint8_t iBlockIndex);

		/**
		* Retrieves a read-only 48 kHz sub block
		*
		* @param[in] iBlockIndex index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @return Pointer to a sub block, or NULL is the sub block has not been initialized using initDLCSubBlock48()
		* @throws std::out_of_range if iBlockIndex is out of range
		*/
		virtual const ResidualSubBlock* getDLCSubBlock48(uint8_t iBlockIndex) const;

		/****************
		* 96 kHz sub blocks
		*
		*/

		/**
		* Initializes a 96 kHz sub block, overwriting any existing contents, if any.
		*
		* @param[in] iBlockIndex Index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @param[in] iCodeType CodeType of the sub block
		* @param[in] iSize Size (in residuals) of the sub block
		* @throws std::out_of_range if iBlockIndex is out of range
		*/
		virtual void initDLCSubBlock96(uint8_t iBlockIndex, CodeType iCodeType, uint32_t iSize);

		/**
		* Retrieves a 96 kHz sub block, which can be modified in place
		*
		* @param[in] iBlockIndex index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @return Pointer to a sub block, or NULL is the sub block has not been initialized using initDLCSubBlock96()
		* @throws std::out_of_range if iBlockIndex is out of range
		*/
		virtual ResidualSubBlock* getDLCSubBlock96(uint8_t iBlockIndex);

		/**
		* Retrieves a read-only 96 kHz sub block
		*
		* @param[in] iBlockIndex index of the sub block in the range [0, getNumDLCSubBlocks() )
		* @return Pointer to a sub block, or NULL is the sub block has not been initialized using initDLCSubBlock96()
		* @throws std::out_of_range if iBlockIndex is out of range
		*/
		virtual const ResidualSubBlock* getDLCSubBlock96(uint8_t iBlockIndex) const;

		/****************
		* 96 kHz sub blocks
		*
		*/

		/**
		* Retrieves SampleCount48 for the frame represented by the DLCAudioData element
		*
		* @return SampleCount48 (the number of 48 kHz samples in the frame)
		* @throws runtime_error if any 48 kHz sub block is undefined
		*/
		virtual uint32_t getSampleCount48() const;

		/**
		* Retrieves SampleCount96 for the frame represented by the DLCAudioData element
		*
		* @return SampleCount96 (the number of 96 kHz samples in the frame)
		* @throws runtime_error if any 96 kHz sub block is undefined or if getSampleRate() != FS_96000
		*/
		virtual uint32_t getSampleCount96() const;

		private:

		int32_t	shiftBits_;
		SampleRate	sampleRate_;
		std::vector<PredRegion> predRegions48_;
		std::vector<PredRegion> predRegions96_;
		std::vector<ResidualSubBlock*> subBlocks48_;
		std::vector<ResidualSubBlock*> subBlocks96_;
	};

} // namespace dlc

#endif // DLC_AUDIODATA_H
