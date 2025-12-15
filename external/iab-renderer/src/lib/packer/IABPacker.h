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

/**
 * Header file for the IAB Packer implementation.
 *
 * @file
 */


#ifndef __IABPACKER_H__
#define	__IABPACKER_H__

#include "common/IABElements.h"
#include "IABPackerAPI.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /**
     *
     * IAB Packer class to pack IAB frame elements into a packed buffer.
     *
     */
    
    class IABPacker : public IABPackerInterface
    {
    public:

		// Constructor
        IABPacker();

		// Destructor
		~IABPacker();

        // Returns the API version of the Packer.
        void GetAPIVersion(IABAPIVersionType& oVersion);

		// Set the frame rate for IAB packer
		iabError SetFrameRate(IABFrameRateType iFrameRate);

		// Set the audio sample rate for IAB packer
		iabError SetSampleRate(IABSampleRateType iSampleRate);

		// Get the (pointer to) internal IABFrame.
		iabError GetIABFrame(IABFrameInterface* &oIABFrame);

		// Packs an IAB frame into a stream.
        iabError PackIABFrame();
        
        // Get packed frame buffer.
        iabError GetPackedBuffer(std::vector<char>& oProgramBuffer, uint32_t& oProgramBufferLength);
        
		// *** IAB Packer helper methods
		//

		// Get next unique meta ID.
		IABMetadataIDType GetNextUniqueMetaID();

		// Get next unique audio ID.
		IABAudioDataIDType GetNextUniqueAudioID();

		// Create, set up an IAB bed definiton element, and add it to internal IABFrame (iabPackerFrame_)
		iabError AddBedDefinition(IABMetadataIDType iBedID, 
			IABUseCaseType iBedLayout, 
			const IABBedMappingInfo& iAudioIDs,
			IABBedDefinitionInterface* &oBedDefinition);
        
		// Create, set up an IAB object definiton element, and add it to internal IABFrame (iabPackerFrame_)
		iabError AddObjectDefinition(IABMetadataIDType objectID, 
			IABAudioDataIDType iAudioDataID, 
			IABObjectDefinitionInterface* &oObjectDefinition);
        
		// Update audio samples for current IAB frame
		iabError UpdateAudioSamples(std::map<IABAudioDataIDType, int32_t*> iAudioSources);
        
		// Update pan/position change meta data for an IAB object for current frame
        iabError UpdateObjectMetaData(IABMetadataIDType iObjectID, std::vector<IABObjectPanningParameters> iSubblockPanParameters);
        
		// Add a list of AudioDataDLC elements to internal IABFrame (iabPackerFrame_)
		iabError AddDLCElements(std::vector<IABAudioDataIDType> iAudioDataIDList);
        
        // Add an AuthoringToolInfo element to internal IABFrame (iabPackerFrame_)
        iabError AddAuthoringToolInfo(const std::string &iAuthoringToolInfoString);
        
        // Add a UserData element to internal IABFrame (iabPackerFrame_)
        iabError AddUserData(const uint8_t(&iUserID)[16], const std::vector<uint8_t> iUserDataBytes);
        
    private:

		// Create, set up a 5.1 IAB object definiton element, and add it to internal IABFrame (iabPackerFrame_)
		iabError AddBedDefinition51(IABMetadataIDType iBedID, 
			const IABBedMappingInfo& iAudioIDs, 
			IABBedDefinitionInterface* &oBedDefinition);
        
		// Create, set up a 7.1DS IAB object definiton element, and add it to internal IABFrame (iabPackerFrame_)
		iabError AddBedDefinition71DS(IABMetadataIDType iBedID, 
			const IABBedMappingInfo& iAudioIDs, 
			IABBedDefinitionInterface* &oBedDefinition);
        
		// Create, set up a 9.1OH IAB object definiton element, and add it to internal IABFrame (iabPackerFrame_)
		iabError AddBedDefinition91OH(IABMetadataIDType iBedID, 
			const IABBedMappingInfo& iAudioIDs, 
			IABBedDefinitionInterface* &oBedDefinition);
        
		// Update bed channel gain
		iabError UpdateBedChannelGain(IABChannelIDType chanId, IABGain iChannelGain);

		// Check if an element is present anywhere in the IABFrame sub-element hierarchy.
		// (Internal & not implemented. The usefulness of this potential API is under evaluation. Subject to change.)
		bool IsElementInFrame(IABElement* iElement);

		// Pointer to IABFrame instance
        IABFrameInterface*          iabPackerFrame_;
		uint8_t                     numSubBlocks_;
		IABFrameRateType            frameRate_;
        IABSampleRateType           sampleRate_;
        std::vector<char>           fProgramBuffer_;			// output buffer

		IABMaxRenderedRangeType     maxRendered_;

		// Variables for managing meta ID and audio ID pools
		IABMetadataIDType           currentMetaID_;
		IABAudioDataIDType          currentAudioID_;
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABPACKER_H__
