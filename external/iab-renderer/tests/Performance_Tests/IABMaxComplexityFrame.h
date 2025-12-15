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

#ifndef __IABMAXCOMPLEXITYFRAME_H__
#define __IABMAXCOMPLEXITYFRAME_H__

#include "IABElementsAPI.h"
#include "IABPackerAPI.h"

using namespace SMPTE::ImmersiveAudioBitstream;

class IABElement;

/**
 * Class representing a bespoke Maximum Complexity Frame, for
 *     - 48kHz Sample Rate, and
 *     - 24fps Frame Rate.
 *
 */

class IABMaxComplexityFrame
{
public:
    IABMaxComplexityFrame();
    ~IABMaxComplexityFrame();
    
	// Get frame sample count for  Max Complexity Frame
	uint32_t GetFrameSampleCount();

	// To construct the frame
    void Construct();
    
	// Get a constructed Max Complexiy Frame
	IABFrameInterface* GetMaxComplexityFrame();

	// To construct the frame
	void PackToStream();

	// Get serialized Max Complexity Frame data
	std::vector<char>& GetSerializedFrame();

	// Get size for the serialized Max Complexity Frame data
	uint32_t GetSerializedDataSize();

private:
    
	// Add a 9.1OH bed definition, including associated DLC essence elements
	void Add91BedDefinition();

	// Add 118x object definitions, including associated DLC essence elements
    void Add118ObjectDefinitions();
    
    // Clean up test and delete allocated memory
    void CleanUp();
    
    // IAB packer to serialise IAB frame into stream for measurement
    IABPackerInterface*     iabPacker_;
    
	// metadata ID for the next object/bed definition to be added
	IABMetadataIDType       metaDataNextID_;

	// audiodata ID for the next object/bed definition to be added
    IABAudioDataIDType      audioDataNextID_;
    
    // Frame rate for the measurement
    IABFrameRateType        frameRate_;
    
    // Sample rate for the measurement
    IABSampleRateType       sampleRate_;

	// Count MaxRendered
	IABMaxRenderedRangeType maxRendered_;
    
    // Number of audio samples in frame
    uint32_t                frameSampleCount_;
    
    // Number of pan subblocks in object
    uint8_t                 numPanSubblocks_;

	// Constructed Max Complexity Frame, as data structure. For serialized data, see packedMaxComplexityFrame_
	IABFrameInterface*      maxComplexityFrame_;

	// Packed (serialized) data for the Max Complexity Frame
	std::vector<char>       packedMaxComplexityFrame_;

	// Size of packed (serialized) Max Complexity Frame data, in byte
	uint32_t                packedLengthInByte_;

	// Pointer to a frame asset sample buffer. A work buffer shared for creating DLC elements.
	int32_t*                audioSamples_;

	// ****
	// Utility variables
    
    // Seed to initialise the random audio sample generator, use a fixed value for each object
    // Starts with 0 and increment by 1 for each subsequent object/bed channel
    uint32_t                randomSampleSeed_;
};
#endif // __IABMAXCOMPLEXITYFRAME_H__
