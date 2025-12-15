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

/* Implementation of common IAB utility functions */

#include "IABUtilities.h"
#include "common/IABConstants.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    // GetIABNumSubBlocks() implementation
    uint8_t GetIABNumSubBlocks(IABFrameRateType iFrameRate)
    {
        std::map<IABFrameRateType, uint8_t> iabNumSubBlocksLookupTable;

         iabNumSubBlocksLookupTable[kIABFrameRate_24FPS] = 8;
         iabNumSubBlocksLookupTable[kIABFrameRate_25FPS] = 8;
         iabNumSubBlocksLookupTable[kIABFrameRate_30FPS] = 8;
         iabNumSubBlocksLookupTable[kIABFrameRate_48FPS] = 4;
         iabNumSubBlocksLookupTable[kIABFrameRate_50FPS] = 4;
         iabNumSubBlocksLookupTable[kIABFrameRate_60FPS] = 4;
         iabNumSubBlocksLookupTable[kIABFrameRate_96FPS] = 2;
         iabNumSubBlocksLookupTable[kIABFrameRate_100FPS] = 2;
         iabNumSubBlocksLookupTable[kIABFrameRate_120FPS] = 2;
		 iabNumSubBlocksLookupTable[kIABFrameRate_23_976FPS] = 8;

        std::map<IABFrameRateType, uint8_t>::iterator iter;
        iter = iabNumSubBlocksLookupTable.find(iFrameRate);
        
        if (iter != iabNumSubBlocksLookupTable.end())
        {
            return iter->second;
        }
        else
        {
            return 0;
        }
    }
    
    // GetIABNumFrameSamples() implementation
    uint32_t GetIABNumFrameSamples(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate)
    {
       	std::map<IABFrameRateType, uint32_t> iabNumFrameSamplesLookupTable48k;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_24FPS] = 2000;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_25FPS] = 1920;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_30FPS] = 1600;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_48FPS] = 1000;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_50FPS] = 960;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_60FPS] = 800;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_96FPS] = 500;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_100FPS] = 480;
        iabNumFrameSamplesLookupTable48k[kIABFrameRate_120FPS] = 400;
		iabNumFrameSamplesLookupTable48k[kIABFrameRate_23_976FPS] = 2002;

        std::map<IABFrameRateType, uint32_t> iabNumFrameSamplesLookupTable96k;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_24FPS] = 4000;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_25FPS] = 3840;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_30FPS] = 3200;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_48FPS] = 2000;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_50FPS] = 1920;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_60FPS] = 1600;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_96FPS] = 1000;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_100FPS] = 960;
        iabNumFrameSamplesLookupTable96k[kIABFrameRate_120FPS] = 800;
		iabNumFrameSamplesLookupTable96k[kIABFrameRate_23_976FPS] = 4004;

        std::map<IABFrameRateType, uint32_t>* tableToSearch = NULL;

        if (kIABSampleRate_48000Hz == iSampleRate)
        {
            tableToSearch = &iabNumFrameSamplesLookupTable48k;
        }
        else if (kIABSampleRate_96000Hz == iSampleRate)
        {
            tableToSearch = &iabNumFrameSamplesLookupTable96k;
        }
        else
        {
            return 0;
        }
        
        std::map<IABFrameRateType, uint32_t>::iterator iter;
        
        iter = tableToSearch->find(iFrameRate);
        
        if (iter != tableToSearch->end())
        {
            return iter->second;
        }
        else
        {
            return 0;
        }
    }

	// CalculateFrameMaxRendered() implementation
	iabError CalculateFrameMaxRendered(const std::vector<IABElement*> iFrameSubElements, IABMaxRenderedRangeType &oMaxRendered)
	{
		// Calculate MaxRendered parameter value from a list of frame sub-elements, per ST2098-2 spec
		// definition of MaxRendered.
		const std::vector<IABElement*> frameSbElements = iFrameSubElements;
		std::vector<IABElement*> ::const_iterator iterFSE;

		IABMaxRenderedRangeType maxRenderedCount = 0;
		IABElementIDType elementID = kIABElementID_IAFrame;				// init to kIABElementID_IAFrame

		// Map variable to track cumulative combined channel and object counts for conditional
		// beds and objects at frame (root) sub element level.
		// The maximum (worst-case) across all found use cases is added to maxRenderedCount, 
		// as only 1 use case can be active at 1 time.
		//
		uint1_t conditional = 0;
		IABUseCaseType useCase = kIABUseCase_NoUseCase;
		std::map<IABUseCaseType, uint32_t> rootCondObjectAndChannelCounts;

		// Loop through frame (root-level) sub elements
		for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)
		{
			// Get element ID / type.
			(*iterFSE)->GetElementID(elementID);

			// Increment maxRenderedCount per 2 element types, object or bed
			if (elementID == kIABElementID_ObjectDefinition)
			{
				IABObjectDefinition* objectElement = dynamic_cast<IABObjectDefinition*>(*iterFSE);

				if (objectElement->IsIncludedForPacking())
				{
					objectElement->GetConditionalObject(conditional);
					useCase = kIABUseCase_NoUseCase;			// reset useCase before get
					if (conditional != 0)
					{
						// Get use case
						objectElement->GetObjectUseCase(useCase);
					}

					if ((conditional == 0) || (useCase == kIABUseCase_Always))
					{
						// For object, increment by 1 if non-conditional, or "always"
						// case.  This is regardless if it contains further sub-element or not.
						maxRenderedCount += 1;
					}
					else
					{
						// Accumulate object numbers for each of found use cases
						if (rootCondObjectAndChannelCounts.find(useCase) == rootCondObjectAndChannelCounts.end())
						{
							rootCondObjectAndChannelCounts[useCase] = 1;		// First object for the useCase
						}
						else
						{
							rootCondObjectAndChannelCounts[useCase] += 1;		// Add 1 for the useCase
						}
					}  // ((conditional == 0) || (useCase == kIABUseCase_Always))
				}      // if (objectElement->IsIncludedForPacking())
			}
			else if (elementID == kIABElementID_BedDefinition)
			{
				IABBedDefinition* bedElement = dynamic_cast<IABBedDefinition*>(*iterFSE);
				IABChannelCountType numChannels = 0;

				if (bedElement->IsIncludedForPacking())
				{
					bedElement->GetConditionalBed(conditional);
					useCase = kIABUseCase_NoUseCase;			// reset useCase before get
					if (conditional != 0)
					{
						bedElement->GetBedUseCase(useCase);
					}

					// Get number of channels for this bed definition element, Never return error
					bedElement->GetChannelCount(numChannels);

					// For bed, increment by number of channels if included for packing
					// and non-conditional or always use
					if ((conditional == 0) || (useCase == kIABUseCase_Always))
					{
						// Check 1-level down of sub-element to see if it contains a sub-element of 
						// BedDefinition type, which might contain higher number of channels
						// (Interpretation of MAXRendered per ST2098-2 spec)
						std::vector<IABElement*> bedSbElements;
						bedElement->GetSubElements(bedSbElements);
						std::vector<IABElement*> ::const_iterator iterBSE;

						IABChannelCountType numChannelsSubElement = 0;
						for (iterBSE = bedSbElements.begin(); iterBSE != bedSbElements.end(); iterBSE++)
						{
							bedElement = dynamic_cast<IABBedDefinition*>(*iterBSE);			// Pointer now points to sub-element

							if (bedElement != nullptr)
							{
								// If this sub-element is included in packing
								if (bedElement->IsIncludedForPacking())
								{
									bedElement->GetChannelCount(numChannelsSubElement);			// Retrieve channel count of this sub-(BedDefinition)-element

																								// For maxRenderedCount, record the higher channel count as MAX
									if (numChannels < numChannelsSubElement)
									{
										numChannels = numChannelsSubElement;
									}
								}
							}
						}  // for (iterBSE = bedSbElements.begin(); iterBSE != bedSbElements.end(); iterBSE++)

						maxRenderedCount += numChannels;						// Increment maxRenderedCount by numChannels
					}
					else
					{
						// Accumulate channel numbers for each of found bed use cases
						if (rootCondObjectAndChannelCounts.find(useCase) == rootCondObjectAndChannelCounts.end())
						{
							rootCondObjectAndChannelCounts[useCase] = numChannels;		// First bed for the useCase
						}
						else
						{
							rootCondObjectAndChannelCounts[useCase] += numChannels;		// Add bed channel number for the useCase
						}
					}      // if ( (conditional == 0) || (useCase == kIABUseCase_Always) )
				}          // if (bedElement->IsIncludedForPacking())
			}  // if (elementID == kIABElementID_ObjectDefinition)
		}  // End of loop for (iterFSE = frameSbElements.begin(); iterFSE != frameSbElements.end(); iterFSE++)

		   // Find the maximum conditional object/channel number from frame/root level
		   // conditional bed/object elements.
		uint32_t maxConditionalCount = 0;

		for (std::map<IABUseCaseType, uint32_t>::iterator iter = rootCondObjectAndChannelCounts.begin();
		iter != rootCondObjectAndChannelCounts.end(); iter++)
		{
			// Find the maximum among found frame/root level conditional/use cases elements
			if (maxConditionalCount < iter->second)
			{
				maxConditionalCount = iter->second;
			}
		}

		// Update to maxRenderedCount
		maxRenderedCount += maxConditionalCount;

		// Update maxRendered_
		oMaxRendered = maxRenderedCount;

		return kIABNoError;
	}


} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

